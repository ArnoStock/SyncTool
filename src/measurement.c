/*
 * measurement.c
 *
 *  Created on: 14.09.2014
 *      Author: Arno Stock
 */

#include "measurement.h"

uint32_t	__attribute__ ((aligned)) volatile mbuff1 [MEASUREMENT_BLOCK_LENGTH];
uint32_t	__attribute__ ((aligned)) volatile mbuff2 [MEASUREMENT_BLOCK_LENGTH];
uint8_t		active_buffer;
uint8_t		evaluate_buffer;

void DMA1_Channel1_IRQHandler(void);
void enable_dma (uint8_t);


void enable_dma (uint8_t b) {

	b &= 1;
	DMA_Cmd(DMA1_Channel1, DISABLE); //Disable the DMA1 - Channel1
	if (b) {
		DMA1_Channel1->CMAR = (uintptr_t)mbuff2;
	}
	else {
		DMA1_Channel1->CMAR = (uintptr_t)mbuff1;
	}
	active_buffer = b;
	DMA1_Channel1->CNDTR = MEASUREMENT_BLOCK_LENGTH;
	DMA_Cmd(DMA1_Channel1, ENABLE); //Enable the DMA1 - Channel1

}

void init_measurement_subsystem () {

TIM_TimeBaseInitTypeDef TIM_TimeBase_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
GPIO_InitTypeDef GPIO_InitStructure;
ADC_InitTypeDef ADC_InitStructure;
DMA_InitTypeDef DMA_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	/* initialize DMAC channel for ADC result transfer */
	DMA_DeInit(DMA1_Channel1); //Set DMA registers to default values
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADC1->DR); //Address of peripheral the DMA must map to
	DMA_InitStructure.DMA_MemoryBaseAddr = (uintptr_t)mbuff1; //Variable to which ADC values will be stored
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = MEASUREMENT_BLOCK_LENGTH; //Buffer size
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	active_buffer = 0;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure); //Initialize the DMA
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE); // Enable transfer complete interrupt

	/* Enable DMA1 channel1 IRQ Channel */
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	DMA_Cmd(DMA1_Channel1, ENABLE); //Enable the DMA1 - Channel1


	/* initialize ADC channels */
	RCC_ADCCLKConfig(RCC_PCLK2_Div6); /* 12MHz */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
	// Enable GPIO Peripheral clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	/* ADC 1 (Channel 10) */
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;
	ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_Init(ADC1, &ADC_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_7Cycles5);
	ADC_Cmd(ADC1, ENABLE);
	ADC_DMACmd(ADC1, ENABLE);
	ADC_ExternalTrigConvCmd(ADC1, ENABLE);

	/* ADC 2 (Channel 11) */
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_Init(ADC2, &ADC_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	ADC_RegularChannelConfig(ADC2, ADC_Channel_11, 1, ADC_SampleTime_7Cycles5);
	ADC_Cmd(ADC2, ENABLE);
	ADC_ExternalTrigConvCmd(ADC2, ENABLE);

	/* calibrate ADCs */
	ADC_ResetCalibration(ADC1);
	ADC_ResetCalibration(ADC2);
	while(ADC_GetResetCalibrationStatus(ADC1) || ADC_GetResetCalibrationStatus(ADC2));

	ADC_StartCalibration(ADC1);
	ADC_StartCalibration(ADC2);
	while(ADC_GetCalibrationStatus(ADC1) || ADC_GetCalibrationStatus(ADC2));

	/* initialize timer tick */
	/* Timer 3 overflow event in master mode */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	TIM_TimeBase_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBase_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBase_InitStructure.TIM_Period = 71;
	TIM_TimeBase_InitStructure.TIM_Prescaler = 99;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBase_InitStructure);
	TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);

	TIM_Cmd(TIM3, ENABLE);

}

void DMA1_Channel1_IRQHandler(void)
{
  //Test on DMA1 Channel1 Transfer Complete interrupt
  if(DMA_GetITStatus(DMA1_IT_TC1))
  {
	  //Clear DMA1 Channel1 Half Transfer, Transfer Complete and Global interrupt pending bits
	  DMA_ClearITPendingBit(DMA1_IT_GL1);

	  evaluate_buffer = active_buffer | MEASUREMENT_BUFFER_FULL_FLAG;
	  enable_dma ((uint8_t)(active_buffer+1));
  }
}

/*******************************************************************************
* Function Name  : get_buffer_value
* Description    : returns raw value from measurement result buffer
* Input          : buf: buffer #
* 				   ch:  analog input channel#
* 				   offs: offset of value in buffer
* Output         : measurement value
* Return         : measurement value
* Attention		 : None
*******************************************************************************/
uint16_t get_buffer_value (uint8_t buf, uint8_t ch, uint16_t offs) {
//FIXME: need to adapt for > 2 channels

uint32_t volatile *p;
uint32_t val;

	if (offs >= MEASUREMENT_BLOCK_LENGTH)
		return 0;

	p = buf ? mbuff2 : mbuff1;
	val = ch ? (*(p+offs) >> 16) : *(p+offs);
	return (uint16_t)val & 0xffff;
}


/*******************************************************************************
* Function Name  : get_buffer_average
* Description    : returns raw value from measurement result buffer
* Input          : buf: buffer #
* 				   ch:  analog input channel#
* 				   offs: offset of value in buffer
* 				   samp: amount of samples
* Output         : measurement value
* Return         : measurement value
* Attention		 : None
*******************************************************************************/
uint16_t get_buffer_average (uint8_t buf, uint8_t ch, uint16_t offs, uint16_t samp) {

uint64_t	s = 0;
uint16_t i;

	for (i = 0; i < samp; i++) {
		s += get_buffer_value (buf, ch, (uint16_t)(offs+i));
	}
	return (uint16_t) s/samp;
}

/*******************************************************************************
* Function Name  : get_buffer_pointer
* Description    : returns pointer to measurement result buffer
* Input          : buf: buffer #
* Output         : pointer to buffer
* Return         : measurement value
* Attention		 : None
*******************************************************************************/
volatile void* get_buffer_pointer (uint8_t buf) {
	void volatile *p;
	p = buf ? mbuff2 : mbuff1;
	return p;
}
