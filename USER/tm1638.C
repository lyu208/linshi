/*******************(C) COPYRIGHT �����Ƽ����޹�˾**************************/

#include "tm1638.h"
#include "delay.h"



/************************************************
*                �����Ķ���                       *
*************************************************/
uchar           ucTm1638_2FreeFlag = 1; /* TM1638���б�־ �����д��ʾ��ʱ�� �ڶ���tm1638оƬ���ܶ�ȡ���� ��������������л��� �˱������⿪�� */

uchar           ucKeyTouch = 0; /*�����������  ��ʾ��Ӧ�����ĸ�����(1~9)  �˱������⿪�� ��user.c ʹ��*/


uchar           ucKeyLock_1 = 0;  //��������  ȫ�ֱ���Ĭ��Ϊ0
uchar           ucKeyLock_2 = 0;
uchar           ucKeyLock_3 = 0;
uchar           ucKeyLock_4 = 0;
uchar           ucKeyLock_5 = 0;
uchar           ucKeyLock_6 = 0;
uchar           ucKeyLock_7 = 0;
uchar           ucKeyLock_8 = 0;


uchar ucKeyPressCnt1 = 0; /*�������ʱ��ļ�ʱ����*/
uchar ucKeyShortTuchFlag1 = 0;

uchar ucKeyPressCnt3 = 0;
uchar ucKeyShortTuchFlag3 = 0;

uchar ucKeyPressCnt4 = 0;
uchar ucKeyShortTuchFlag4 = 0;

uchar ucKeyPressCnt5 = 0;
uchar ucKeyShortTuchFlag5 = 0;

uchar ucKeyPressCnt6 = 0;
uchar ucKeySequenceFlag6 = 0;
uchar ucKeyShortTuchFlag6 = 0;

uchar  ucKeyPressCnt7 = 0;
uchar ucKeySequenceFlag7 = 0;
uchar ucKeyShortTuchFlag7 = 0;


/*0    1     2      3     4     5    6     7     8      9    ��    -    c */
uchar  const  table[13] = { 0x3f, 0x06, 0x5B, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x00, 0x40, 0x58 };

/*оƬ�ڲ�ת���롣��оƬ����д�ľ�����д������ */
volatile uchar  dat1[16] = { 0 }; //��һƬtm1638оƬ����
volatile uchar  dat2[16] = { 0 }; //�ڶ�Ƭtm1638оƬ����








/************************************************
*              ˽�к�������                       *
*************************************************/
uchar ReadByteFro_TM1638(void); //��ȡ�����ģ�������ֻд��T2оƬ��(���������T2���Ƶģ�)
void  WriteByteTo_TM1638_SAME(uchar byte); //�жϵ��е��� ��ȡ�����õ�

void WriteByteTo_TM1638(uchar select, uchar byte);
void WriteCmdTo_TM1638(uchar select, uchar cmd);


void delay(u32 nCount);






/************************************************
*������ ��     allotData

*�������� �� tm1638x :��ʾ�ڼ���tm1638оƬ ;  tubex :��ʾ��оƬ���Ƶĵڼ�������� ;tubeData: Ҫ��ʾ������

*��������ֵ :   ��

*������������ ��  ת�뺯�� ��������
�������ƣ�ÿ�������й��ɵĳ��򣬶��ǿ��Լ򻯵�
    ���ǣ������2511�У�̫TM�����ˣ��ĳ����һ�£��Ǻ�~~
    �����ռ任ȡʱ���������ʱ�任ȡ�ռ� �����������������
*************************************************/
void allotData(uchar tm1638x, uchar tubex, uchar tubeData)
{
    uchar i;

    switch (tm1638x) //�ڼ���tm1638оƬ ����Ŀ����3Ƭ��
    {
    case 1:
        if (tubex <=
                8) //���if else �ж���� ��������ȷ���forѭ����ã�Ч��Ӧ�øߵ㣬������أ���
        {
            for (i = 0; i < 8; i++)
            {
                if ((tubeData & 0x01) == 1) dat1[2 * i] |= (0x01 << (tubex - 1));
                else dat1[2 * i] &= ~(0x01 << (tubex - 1));
                tubeData >>= 1;
            }
        } else  //��������ھŸ� ��ʮ������ܣ����õ����������
        {
            for (i = 0; i < 8; i++)
            {
                if ((tubeData & 0x01) == 1) dat1[2 * i + 1] |= (0x01 << (tubex - 9));
                else dat1[2 * i + 1] &= ~(0x01 << (tubex - 9));
                tubeData >>= 1;
            }
        }
        break;
    case 2:
        if (tubex <= 8)
        {
            for (i = 0; i < 8; i++)
            {
                if ((tubeData & 0x01) == 1) dat2[2 * i] |= (0x01 << (tubex - 1));
                else dat2[2 * i] &= ~(0x01 << (tubex - 1));
                tubeData >>= 1;
            }
        } else
        {
            for (i = 0; i < 8; i++)
            {
                if ((tubeData & 0x01) == 1) dat2[2 * i + 1] |= (0x01 << (tubex - 9));
                else dat2[2 * i + 1] &= ~(0x01 << (tubex - 9));
                tubeData >>= 1;
            }
        }
        break;
    }

}

/************************************************
*������ :       TM1638_Config

*�������� :      ��

*��������ֵ :     ��

*������������ :    ��������tm1638оƬ������ģʽ

*ע�� :    
*************************************************/
void  TM1638_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //������Ϊʲô��©����
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_SetBits(GPIOC, GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//������Ϊʲô��©����
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_SetBits(GPIOA, GPIO_Pin_8 | GPIO_Pin_9);
}



/************************************************
*������ ��TM1638_Init

*�������� �� ��

*��������ֵ :  ��

*������������ �� �ϵ����ʾ�Ĵ��� ����һ���������
*************************************************/
void TM1638_Init()       //���ٷ��ֲ�����ͼд����
{
    uchar i;

    WriteCmdTo_TM1638(1, AUTO_ADDR_MODE); //��������
    WriteCmdTo_TM1638(1, ADDR_0); //�@ʾ��ַ
    for (i = 0; i < 16; i++)
    {
        WriteByteTo_TM1638(1, 0x00); //��һ��tm1638���Ƶ���������ܣ�ʲôҲ����ʾ
    }
    WriteCmdTo_TM1638(1, LIGHT_LEVE_6); //���ȵ���  �Ժ�Ͳ���ÿ��д���ݶ����ˡ�����
    STB1 = 1;



    WriteCmdTo_TM1638(2, AUTO_ADDR_MODE);
    WriteCmdTo_TM1638(2, ADDR_0);
    for (i = 0; i < 16; i++)
    {
        WriteByteTo_TM1638(2, 0x00); //��2��tm1638���Ƶ���������ܣ�ʲôҲ����ʾ
    }
    WriteCmdTo_TM1638(2, LIGHT_LEVE_6); //���ȵ���   �Ժ�Ͳ���ÿ��д���ݶ����ˡ�����
    STB2 = 1;
}




/************************************************
*������ ��    ReadByteFro_TM1638

*�������� ��

*��������ֵ :

*������������ :   ��ȡ������Ҫ�õ��˺���
*************************************************/
uchar ReadByteFro_TM1638(void)                  //�����ݺ���
{
    unsigned char i;
    unsigned char temp = 0;
    DIO2 = 1; //����������
              delay(12);//��
    for (i = 0; i < 8; i++)
    {
        CLK2 = 0;
         delay(12);	 //��
        temp >>= 1;
        CLK2 = 1;
        if (DIO2_IN == 1)
        {
            temp |= 0x80;
        }
         delay(12);//Delay_us(1); 
    }
    return temp;
}


/************************************************
*������ ��     WriteByteTo_TM1638_SAME

*�������� ��

*��������ֵ :

*������������ ��  ��TM1638�����ݻ�����д������

*ע��: �ж�������� �ж�����������ﲻ��ͬʱ����ͬһ������  �����Ҹ�����һ����ͬ�ĺ��� 
      ���ж϶�ȡ����ʹ��
*************************************************/
void WriteByteTo_TM1638_SAME(uchar byte) //
{
    uchar i;
    STB2 = 1;
    delay(12);//Delay_us(2);
    STB2 = 0;

    for (i = 0; i < 8; i++)
    {
        CLK2 = 0;
       delay(12);// Delay_us(1); //tSETUP  ��С100ns
        if ((byte & 0x01) == 1)
        {
            DIO2 = 1;
        } else
        {
            DIO2 = 0;
        }
        CLK2 = 1;
        delay(12);//Delay_us(1); //tHOLD   ��С100ns
        byte >>= 1;
    }
    delay(24);//Delay_us(2); //tCLKSTB    ��С1us
}



/************************************************
*������ ��     WriteDatasToTm1638

*�������� ��

*��������ֵ :

*������������ ��  ��TM1638�����ݻ�����д������
*************************************************/
void WriteDatasTo_TM1638(uchar select)
{
    uchar i;
    switch (select)
    {
        /* ��1Ƭtm1638оƬ   ����10������� */
    case 1:

        WriteCmdTo_TM1638(1, AUTO_ADDR_MODE);
        WriteCmdTo_TM1638(1, ADDR_0); //����ʾ��ַ����0xc0��ַ�� ��ʼ�Զ���1
        for (i = 0; i < 16; i++)
        {
            WriteByteTo_TM1638(1, dat1[i]); //��һ��tm1638���Ƶ���������ܣ�ʲôҲ����ʾ
        }
        WriteCmdTo_TM1638(1, LIGHT_LEVE_6); //���ȵ���
        STB1 = 1;
        //	Delay2us();//new
        break;
        /* �ڶ�Ƭtm1638оƬ   ����10������� */
    case 2:

        ucTm1638_2FreeFlag = 0; /* �ڶ�������ܼ��ɨ�谴���Ĺ��� ��д����ܵ�ʱ�� ţ�к�æ ûʱ���ȡ���� */


        WriteCmdTo_TM1638(2, AUTO_ADDR_MODE); //������������
        WriteCmdTo_TM1638(2, ADDR_0); //����ʾ��ַ����0xc0��ַ�� ��ʼ�Զ���1   ������ʾ��ַ
        for (i = 0; i < 16; i++)
        {
            WriteByteTo_TM1638(2, dat2[i]); //��һ��tm1638���Ƶ���������ܣ�ʲôҲ����ʾ
        }
        WriteCmdTo_TM1638(2, LIGHT_LEVE_6); //���ȵ���    ��ʾ����ָ��
        STB2 = 1;



        ucTm1638_2FreeFlag = 1;  /*�ڶ�������ܿ����� ���Խ��ж�ȡ���� */
        break;

    default :
        break;
    }
}

/**
 *6000000  500ms  ֻ�ǲ²� �����ϵ�
 * 12000   1ms
 * 12      1us
 */
void delay(u32 nCount)
{
	for(;nCount!=0;nCount--);
}

/************************************************
*������ :          ReadKeyValue

*�������� :        ��

*��������ֵ :       ��

*������������ :    ��ⴥ�������ĸ�����

*ע��  :    ��Ȼ����һ����8�� ���Ǹ��ݲ�ͬ�İ�����ʽ �����̰� �� �ж��ֲ�ͬ�İ�����ʽ������
*************************************************/
void ReadKeyValue()
{
    uchar keyBuff[4], i;

    WriteByteTo_TM1638_SAME(
        READ_KEY_MODE); //���������ж��ﲻ�ܳ���ͬʱ����һ�����������
   delay(12);
 
    for (i = 0; i < 4; i++)
    {
        keyBuff[i] = ReadByteFro_TM1638(); //��ȡ������4��BYTE
    }
    STB2 = 1;

    /**
    *   �������ĸ�����������? ���TM1638�����ֲ� 4ҳ��13ҳ?���õ��ǹ�����ʽ?
    * ��ǳǳ��Լ����:
    *  S1://�ֶ��Զ�
    *  S2://���繦��
    *  S3://1�ŷ翪��
    *  S4://2�ŷ翪��
    *  S5://���ð���
    *  S6://�Ӱ���
    *  S7://������
    *  S8://ȷ��
    * @author ���� (2015��8��29��)
    */
    /*
    *        ��������ͼ
    *        SW1(K1-KS1)  SW2(K1-KS2)   SW3(K1-KS3)  SW4(K1-KS4)  SW5(K1-KS5)
    *        SW6(K1-KS6)  SW7(K1-KS7)   SW8(K1-KS8)
    */
    for (i = 0; i < 4; i++)
    {
        switch (i)
        {
        case 0:
            if (keyBuff[i] != 0x04)
            {
                ucKeyLock_1 = 0;
                ucKeyPressCnt1 = 0;
                if (ucKeyShortTuchFlag1 == 1)
                {
                    ucKeyShortTuchFlag1 = 0;
                    ucKeyTouch = 1;
                }
            } else if (ucKeyLock_1 == 0)
            {
                ucKeyPressCnt1++;
                ucKeyShortTuchFlag1 = 1;
                if (ucKeyPressCnt1 >= 200)
                {
                    ucKeyShortTuchFlag1 = 0;
                    ucKeyLock_1 = 1;
                    ucKeyPressCnt1 = 0;
                    ucKeyTouch = 12;   //��ϵͳʱ һ���ָ����Զ�ģʽ
                }
            }





            /************************************************/
            //�ų��L����
            if (keyBuff[i] == 0x40 &&
                ucKeyLock_2 == 0) //2�ż����������ǵ�һ�α����� KS2 & K1
            {
                ucKeyTouch = 2;
                ucKeyLock_2 = 1; //2�ż����� ��ֹһֱ����

            } else if (keyBuff[i] != 0x40)
            {
                ucKeyLock_2 = 0; //2�ż�����
            }
            break;
        case 1:
            //3�Ű���?1̖���L��
  
            if (keyBuff[i] != 0x04)
            {
                ucKeyLock_3 = 0;
                ucKeyPressCnt3 = 0;
                if (ucKeyShortTuchFlag3 == 1)
                {
                    ucKeyShortTuchFlag3 = 0;
                    ucKeyTouch = 3;
                }
            } else if (ucKeyLock_3 == 0)
            {
                ucKeyPressCnt3++;
                ucKeyShortTuchFlag3 = 1;
                if (ucKeyPressCnt3 >= 200)
                {
                    ucKeyShortTuchFlag3 = 0;
                    ucKeyLock_3 = 1;
                    ucKeyPressCnt3 = 0;
                    ucKeyTouch = 10;   //��ϵͳʱ һ���򿪷ֿ�
                }
            }



            /************************************************/


            //4�Ű��� 1̖���L��
            if (keyBuff[i] != 0x40)
            {
                ucKeyLock_4 = 0;
                ucKeyPressCnt4 = 0;
                if (ucKeyShortTuchFlag4 == 1)
                {
                    ucKeyShortTuchFlag4 = 0;
                    ucKeyTouch = 4;
                }
            } else if (ucKeyLock_4 == 0)
            {
                ucKeyPressCnt4++;
                ucKeyShortTuchFlag4 = 1;
                if (ucKeyPressCnt4 >= 200)
                {
                    ucKeyShortTuchFlag4 = 0;
                    ucKeyLock_4 = 1;
                    ucKeyPressCnt4 = 0;
                    ucKeyTouch = 11;   //��ϵͳʱ һ���رշֿ�
                }
            }


            break;
        case 2:
            // 5�Ű���  ����
            if (keyBuff[i] != 0x04)
            {
                ucKeyLock_5 = 0;
                ucKeyPressCnt5 = 0;
                if (ucKeyShortTuchFlag5 == 1)
                {
                    ucKeyShortTuchFlag5 = 0;
                    ucKeyTouch = 5;
                }
            } else if (ucKeyLock_5 == 0)
            {
                ucKeyPressCnt5++;
                ucKeyShortTuchFlag5 = 1;
                if (ucKeyPressCnt5 >= 200)
                {
                    ucKeyShortTuchFlag5 = 0;
                    ucKeyLock_5 = 1;
                    ucKeyPressCnt5 = 0;
                    ucKeyTouch = 9;   //��̨����
                }
            }



            /************************************************/

            // 6�Ű���  ��   ���� 2����ڿ�

            if (keyBuff[i] != 0x40)
            {
                ucKeyLock_6 = 0;
                ucKeyPressCnt6 = 0;
                ucKeySequenceFlag6 = 0;
                if (ucKeyShortTuchFlag6 == 1) //?????
                {
                    ucKeyShortTuchFlag6 = 0;
                    ucKeyTouch = 6;
                }
            } else if (keyBuff[i] == 0x40 && ucKeyLock_6 == 0)
            {
                ucKeyPressCnt6++;
                ucKeyShortTuchFlag6 = 1;

                if (ucKeyPressCnt6 > 220 || ucKeySequenceFlag6 == 1)
                {
                    ucKeySequenceFlag6 = 1;
                    ucKeyPressCnt6++;
                    if (ucKeyPressCnt6 > 15) //25
                    {
                        ucKeyPressCnt6 = 0;
                        ucKeyTouch = 6;
                    }
                }
            }

            break;
        case 3:
            //7�Ű���?��  ���� 2����ڹ�

            if (keyBuff[i] != 0x04)
            {
                ucKeyLock_7 = 0;
                ucKeyPressCnt7 = 0;
                ucKeySequenceFlag7 = 0;
                if (ucKeyShortTuchFlag7 == 1) //?????
                {
                    ucKeyShortTuchFlag7 = 0;
                    ucKeyTouch = 7;
                }
            } else if (keyBuff[i] == 0x04 && ucKeyLock_7 == 0)
            {
                ucKeyPressCnt7++;
                ucKeyShortTuchFlag7 = 1;

                if (ucKeyPressCnt7 > 220 || ucKeySequenceFlag7 == 1)
                {
                    ucKeySequenceFlag7 = 1;
                    ucKeyPressCnt7++;
                    if (ucKeyPressCnt7 > 15) //25
                    {
                        ucKeyPressCnt7 = 0;
                        ucKeyTouch = 7;
                    }
                }
            }


            /************************************************/
            //8�ż�  ȷ��

            if (keyBuff[i] == 0x40 &&
                ucKeyLock_8 == 0) //8�ż����������ǵ�һ�α����� KS8 & K1
            {
                ucKeyTouch = 8;
                ucKeyLock_8 = 1; //8�ż����� ��ֹһֱ����
            } else if (keyBuff[i] != 0x40)   //û������
            {
                ucKeyLock_8 = 0; //8�ż�����
            }


            break;
        }
    }

}









/***************************************
       ������Ϊ�ײ㺯����
****************************************/


/************************************************
*������ ��    WriteByteTo_TM1638

*�������� ��  select: �ڼ�ƬTM1638 ����оƬ
          cmd:  ���͵�����ָ��

*��������ֵ :   ��

*������������ ��  ʵ��һ���ֽڵ�д��
*************************************************/
void WriteByteTo_TM1638(uchar select, uchar byte)
{
    uchar i;
    switch (select)
    {
    case 1:
        for (i = 0; i < 8; i++)
        {
            CLK1 = 0;
            Delay_us(1); //tSETUP  ��С100ns
            if ((byte & 0x01) == 1)
            {
                DIO1 = 1;
            } else
            {
                DIO1 = 0;
            }

            CLK1 = 1;
            Delay_us(1); //tHOLD   ��С100ns
            byte >>= 1;
        }

        break;
    case 2:
        for (i = 0; i < 8; i++)
        {
            CLK2 = 0;
            Delay_us(1); //tSETUP  ��С100ns
            if ((byte & 0x01) == 1)
            {
                DIO2 = 1;
            } else
            {
                DIO2 = 0;
            }
            CLK2 = 1;
            Delay_us(1); //tHOLD   ��С100ns
            byte >>= 1;
        }
        break;
    default :
        break;
    }
    Delay_us(2); //tCLKSTB    ��С1us
}

/************************************************
*������ ��     WriteCmdTo_TM1638

*�������� ��    select: �ڼ�ƬTM1638 ����оƬ
          cmd:  ���͵�����ָ��
*��������ֵ :   ��

*������������ �� ��TM1638��������ĺ���   
*************************************************/
void WriteCmdTo_TM1638(uchar select, uchar cmd)
{
    switch (select)
    {
    case 1:
        STB1 = 1;
        Delay_us(2);
        STB1 = 0;
        // Delay1us();//new
        WriteByteTo_TM1638(1, cmd);

        break;
    case 2:
        STB2 = 1;
        Delay_us(2); //  PWSTB  ��С1us
        STB2 = 0;
        // Delay1us();//new
        WriteByteTo_TM1638(2, cmd);

        break;
    default :
        break;

    }
}


/************************************************
*������ ��     WriteDataToAddr

*�������� ��

*��������ֵ :

*������������ ��  дһ���ֽ����ݵ�ָ���ĵ�ַ
*************************************************/
//void WriteDataToAddr(uchar select, uchar addr, uchar dat) //ָ����ַд������
//{
//    switch (select)
//    {
//    case 1:
//        WriteCmdTo_TM1638(1, addr);
//        WriteByteTo_TM1638(1, dat);

//        break;
//    case 2:
//        WriteCmdTo_TM1638(2, addr);
//        WriteByteTo_TM1638(2, dat);

//        break;
//
//    }
//}

