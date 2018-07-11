
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"

#include  "stdio.h"
#include  "user.h"

#include  "tools.h"
#include "HX711.h"
#include "sim900a.h"
#define USART_REC_LEN  			200  	//�����������ֽ��� 200
/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/
u8 USART3_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART3_RX_STA=0;//����״̬���	
u16 ReceiveState=0;

extern char RxBuffer2[17];
extern char RxCounter2;
extern char RxBuffer3;
u8 a=0;
u8 b=0;

/**
 * @brief  This function handles NMI exception.
 * @param  None
 * @retval None
 */
void NMI_Handler(void)
{
}

/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval None
 */
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {}
}

/**
 * @brief  This function handles Memory Manage exception.
 * @param  None
 * @retval None
 */
void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {}
}

/**
 * @brief  This function handles Bus Fault exception.
 * @param  None
 * @retval None
 */
void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {}
}

/**
 * @brief  This function handles Usage Fault exception.
 * @param  None
 * @retval None
 */
void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {}
}

/**
 * @brief  This function handles SVCall exception.
 * @param  None
 * @retval None
 */
void SVC_Handler(void)
{
}

/**
 * @brief  This function handles Debug Monitor exception.
 * @param  None
 * @retval None
 */
void DebugMon_Handler(void)
{
}

/**
 * @brief  This function handles PendSVC exception.
 * @param  None
 * @retval None
 */
//void PendSV_Handler(void)
//{
//}
/**
 * @brief  This function handles SysTick Handler.
 * @param  None
 * @retval None
 */



/************************************************
*������ ��        USART1_IRQHandler

*�������� ��      ��

*��������ֵ :     ��

*������������ ��  ����1���жϺ���
*************************************************/
void USART1_IRQHandler()
{


    if(USART_GetITStatus(USART1,
                         USART_IT_RXNE) != RESET)
    {

        USART_ClearITPendingBit(USART1, USART_IT_RXNE);


    }
}

void USART2_IRQHandler()
{


    if(USART_GetITStatus(USART2,
                         USART_IT_RXNE) != RESET)
    {

        USART_ClearITPendingBit(USART2, USART_IT_RXNE);

        GSM_recvBuff[recvDataCnt] = USART2->DR; // 252

        recvDataCnt++;
			   if (FindChar(GSM_recvBuff,
                             "hello")) //��GSM_recvBuff[]�����������û�к��� ref��ô���ַ�
                {
                  LCD_ShowString(0, 160, 209, 24, 24, " OK  hello");//17*12=204
                }else return;
    }
}





void USART3_IRQHandler()
{		
		u8 Temp_Clear_IDLE=Temp_Clear_IDLE; 

	if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)  //���������ж�
	{
		b=1;
		Temp_Clear_IDLE=USART3->SR;
		Temp_Clear_IDLE=USART3->DR;//���USART_IT_IDLEλ
		USART3_RX_STA|=0x8000;
	}
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		{	a++;
			USART3_RX_BUF[USART3_RX_STA&0X7FFF]=USART_ReceiveData(USART3);//(USART1->DR);	//��ȡ���յ�������
			USART3_RX_STA++;
		}
	
		//SendStringBy_USART1(USART3_RX_BUF);
//	//////////////////////////////////////////////
//	u8 Res;
//	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
//		{
//		Res =USART_ReceiveData(USART3);	//��ȡ���յ�������
//		
//		if((USART3_RX_STA&0x8000)==0)//����δ���
//			{
//			if(USART3_RX_STA&0x4000)//���յ���0x0d
//				{
//				if(Res!=0x0a)USART3_RX_STA=0;//���մ���,���¿�ʼ
//				else USART3_RX_STA|=0x8000;	//��������� 
//				}
//			else //��û�յ�0X0D
//				{	
//				if(Res==0x0d)USART3_RX_STA|=0x4000;
//				else
//					{
//					USART3_RX_BUF[USART3_RX_STA&0X3FFF]=Res ;
//					USART3_RX_STA++;
//						SendStringBy_USART1(USART3_RX_BUF);
//						SendStringBy_USART1("\r\n");
//						SendDataBy_USART1(USART3_RX_BUF);
//					if(USART3_RX_STA>(USART_REC_LEN-1))USART3_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
//					}		 
//				}
//			}   		 
//     } 
//		        SendStringBy_USART1(USART3_RX_BUF);
//						SendStringBy_USART1("\r\n");
//						SendDataBy_USART1(USART3_RX_BUF);
/////////////////////////////////////////
//			if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
//			{
//				USART3_RX_BUF[USART3_RX_STA++]=USART3->DR  ;
//			}
//			else if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)
//			{
//				Temp_Clear_IDLE=USART3->SR;
//		    Temp_Clear_IDLE=USART3->DR;//���USART_IT_IDLEλ
//				ReceiveState=1;
//			}
//			//SendStringBy_USART1("USAR2T");
//			SendStringBy_USART1(USART3_RX_BUF);
//			 LCD_ShowString(0, 160, 209, 24, 24, " OK  Data");//17*12=204
//			//memset( USART3_RX_BUF, 0x00, sizeof(USART3_RX_BUF) ); //�������
	
	
	
}

/************************************************
*������ ��        EXTI3_IRQHandler

*�������� ��      ��

*��������ֵ :     ��

*������������ ��  EXTI��3�ϵ��ж�
*************************************************/
void EXTI3_IRQHandler()
{
    static uint8_t u8ExtiCnt = 0;
    if (EXTI_GetITStatus(EXTI_Line3) !=
        RESET) //�����ⲿ�ж����Ϸ�����ѡ��ı����¼�����λ���� 1����λ�������0
    {
        EXTI_ClearITPendingBit(
            EXTI_Line3); //����������жϱ�־λ�����㣬���һֱ���жϣ���
        u8ExtiCnt++;
        printf("�����ж�ʽ��� ���óɹ���%d", u8ExtiCnt);
    }
}


/************************************************
*������ ��        TIM4_IRQHandler

*�������� ��      ��

*��������ֵ :     ��

*������������ ��  ϵͳ���ʱ�����жϺ�����2.5ms�����ж�һ��
*************************************************/
void  TIM4_IRQHandler()
{

    if (TIM_GetITStatus(TIM4,
                        TIM_IT_Update) != RESET) //�����������¼�ʱ����Ϊ��Ӳ����1�����������0
    {
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);

	      KeyPadScan();
			  
			
			  if(getWeightService==0)
				{
				getWeightCnt++;
				 if(getWeightCnt>=100)//250ms����һ��
				{
					getWeightCnt=0;
				  getWeightService=1;
//					Weight_Shiwu++;
//				 
//					if(Weight_Shiwu>=149)
//					{
//						 
//					Weight_Shiwu=0;
//					}
				}
				}
			  
			 if (overTimeCnt > 0)
        {
            overTimeCnt--;
        }
			
			
        DS0_TimeCnt++;
        if(DS0_TimeCnt >= 0xffff)
        {
            DS0_TimeCnt = 0;
        }
				
				if(cunnectFlag)
				{
				heartbeat++;
					if(heartbeat>=2400)
					{
					heartbeatService=1;
						heartbeat=0;
					}
				
				}
    }
}

void EXTI9_5_IRQHandler()
{

    if (EXTI_GetITStatus(EXTI_Line8) !=
        RESET) //�����ⲿ�ж����Ϸ�����ѡ��ı����¼�����λ���� 1����λ�������0
    {
        EXTI_ClearITPendingBit(
            EXTI_Line8); //����������жϱ�־λ�����㣬���һֱ���жϣ���

        //  u8W5100_Service=1;

    }
}



/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
