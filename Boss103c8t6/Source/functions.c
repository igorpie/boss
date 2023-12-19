/*
 * functions.c
 *
 *  Created on: Apr 29, 2023
 *      Author: shift
 */

#include "functions.h"
#include <stdio.h>

#include "ssd1306.h"

//put ADC value in X Y coordinate
void printADC(int ADC, int x, int y){

   char Text[] = "    ";
//   sprintf(Text, "%hu\r\n", ADC);

   if (ADC==100){
	  ssd1306_SetCursor(x+14, y);
         ssd1306_WriteChar(Text[2], Font_7x10,Black);
      ssd1306_SetCursor(x+7, y);
         ssd1306_WriteChar(Text[1], Font_7x10,Black);
      ssd1306_SetCursor(x, y);
         ssd1306_WriteChar(Text[0], Font_7x10,Black);
      } else if (ADC>10){
    	  ssd1306_SetCursor(x+14, y);
    	           ssd1306_WriteChar(Text[1], Font_7x10,Black);
    	  ssd1306_SetCursor(x+7, y);
    	           ssd1306_WriteChar(Text[0], Font_7x10,Black);
    	  ssd1306_SetCursor(x, y);
    	           ssd1306_WriteChar(' ', Font_7x10,Black);
      } else if (ADC>=0){
          	  ssd1306_SetCursor(x+14, y);
          	           ssd1306_WriteChar(Text[0], Font_7x10,Black);
          	  ssd1306_SetCursor(x+7, y);
          	           ssd1306_WriteChar(' ', Font_7x10,Black);
          	  ssd1306_SetCursor(x, y);
          	           ssd1306_WriteChar(' ', Font_7x10,Black);
      } else {
    	  	  ssd1306_SetCursor(x+14, y);
    	            	           ssd1306_WriteChar('R', Font_7x10,Black);
    	            	  ssd1306_SetCursor(x+7, y);
    	            	           ssd1306_WriteChar('R', Font_7x10,Black);
    	            	  ssd1306_SetCursor(x, y);
    	            	           ssd1306_WriteChar('E', Font_7x10,Black);
      }
   ssd1306_UpdateScreen();
}


//sprintf(Text3, "%hu\r\n", raw);
//           ssd1306_SetCursor(10, 15);
//                    ssd1306_WriteString(Text3, Font_7x10,Black);
//         //
//                    sprintf(Text3, "%hu\r\n", raw1);
//                    ssd1306_SetCursor(40, 15);
//                             ssd1306_WriteString(Text3, Font_7x10,Black);
//
//                             sprintf(Text3, "%hu\r\n", raw2);
//                             ssd1306_SetCursor(70, 15);
//                                      ssd1306_WriteString(Text3, Font_7x10,Black);
//
//                                      sprintf(Text3, "%hu\r\n", raw3);
//                                      ssd1306_SetCursor(100, 15);
//                                               ssd1306_WriteString(Text3, Font_7x10,Black);


////	    Считать пот 0 и вывести на экран
//
//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
//HAL_ADC_Start(&hadc1);
//HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
//raw = HAL_ADC_GetValue(&hadc1);
//sprintf(myText, "%hu\r\n", raw>>4);
//
//   ssd1306_SetCursor(5, 5);
//   ssd1306_WriteString(myText, Font_7x10,Black);
//   //ssd1306_UpdateScreen();



//	  /*-[ I2C Bus Scanning ]-*/
//	 	       HAL_UART_Transmit(&huart2, StartMSG, sizeof(StartMSG), 10000);
//	 	       for(i=1; i<128; i++)
//	 	       {
//	 	           ret = HAL_I2C_IsDeviceReady(&hi2c2, (uint16_t)(i<<1), 3, 5);
//	 	           if (ret != HAL_OK) /* No ACK Received At That Address */
//	 	           {
//	 	               HAL_UART_Transmit(&huart2, Space, sizeof(Space), 10000);
//
//
//	 	           }
//	 	           else if(ret == HAL_OK)
//	 	           {
//	 	               sprintf(Buffer, "0x%X", i);
//	 	               HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), 10000);
//	 	              ssd1306_SetCursor(5, 5);
//	 	              	 	                    ssd1306_WriteString(Buffer, Font_7x10,Black);
//	 	              	 	                    ssd1306_UpdateScreen();
//	 	           }
//	 	       }
//	 	       HAL_UART_Transmit(&huart2, EndMSG, sizeof(EndMSG), 10000);
//	 	       /*--[ Scanning Done ]--*/
//
//	 	       HAL_Delay(1000);
