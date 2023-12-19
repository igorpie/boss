
//  * @file           : usbd_cdc_if.c




#include "usbd_midi_if.h"
#include "jr_usart_103_hal.h"
#include "ProjectMain.h"
#define NEXTBYTE(idx, mask) (mask & (idx + 1))

tUsbMidiCable usbmidicable1 = { .curidx=0, .rdidx=0, .buf={0,} };
//tUsbMidiCable usbmidicable2;

extern PCD_HandleTypeDef hpcd_USB_OTG_FS;
extern USBD_HandleTypeDef hUsbDeviceFS;

// basic midi rx/tx functions
static uint16_t MIDI_DataRx(uint8_t *msg, uint16_t length);
static uint16_t MIDI_DataTx(uint8_t *msg, uint16_t length);

USBD_MIDI_ItfTypeDef USBD_Interface_fops_FS = {  MIDI_DataRx /*,  MIDI_DataTx*/};

static uint8_t g_SysEx = 0;
void USBD_AddEvent( uint8_t cable ){

	uint8_t 		txbuf[4]= {0,};
	uint8_t			byte	= 0;
	tUsbMidiCable* 	pcable 	= NULL;

	if (hUsbDeviceFS.dev_state != USBD_STATE_CONFIGURED) return;
	if ( 0 == cable ) pcable = &usbmidicable1;
	if ( NULL == pcable ) return;
	if ( pcable->curidx == pcable->rdidx ){ return;}
	byte = pcable->buf[ pcable->rdidx ];					// get byte
	pcable->rdidx = NEXTBYTE(pcable->rdidx, USBMIDIMASK);	// move read index
	cable <<= 4;

	if ( ((byte >> 4) & 0x0f) == 0x08 ){ // note off
		txbuf[0] = cable + 0x08;
		txbuf[1] = byte;
		txbuf[2] = pcable->buf[ pcable->rdidx ];
		pcable->rdidx = NEXTBYTE(pcable->rdidx, USBMIDIMASK);
		txbuf[3] = pcable->buf[ pcable->rdidx ];
		pcable->rdidx = NEXTBYTE(pcable->rdidx, USBMIDIMASK);
	}
	else if (((byte >> 4) & 0x0f) == 0x09 ){ // note on
		txbuf[0] = cable + 0x09;
		txbuf[1] = byte;
		txbuf[2] = pcable->buf[ pcable->rdidx ];
		pcable->rdidx = NEXTBYTE(pcable->rdidx, USBMIDIMASK);
		txbuf[3] = pcable->buf[ pcable->rdidx ];
		pcable->rdidx = NEXTBYTE(pcable->rdidx, USBMIDIMASK);
	}
	else if (((byte >> 4) & 0x0f) == 0x0A){ //Polyphonic Key Pressure
		txbuf[0] = cable + 0x0A;
		txbuf[1] = byte;
		txbuf[2] = pcable->buf[ pcable->rdidx ];
		pcable->rdidx = NEXTBYTE(pcable->rdidx, USBMIDIMASK);
		txbuf[3] = pcable->buf[ pcable->rdidx ];
		pcable->rdidx = NEXTBYTE(pcable->rdidx, USBMIDIMASK);
	}
	else if (((byte >> 4) & 0x0f) == 0x0B){ //Control Change
		txbuf[0] = cable + 0x0B;
		txbuf[1] = byte;
		txbuf[2] = pcable->buf[ pcable->rdidx ];
		pcable->rdidx = NEXTBYTE(pcable->rdidx, USBMIDIMASK);
		txbuf[3] = pcable->buf[ pcable->rdidx ];
		pcable->rdidx = NEXTBYTE(pcable->rdidx, USBMIDIMASK);
	}
	else if (((byte >> 4) & 0x0f) == 0x0C){ //Program Change
		txbuf[0] = cable + 0x0C;
		txbuf[1] = byte;
		txbuf[2] = pcable->buf[ pcable->rdidx ];
		pcable->rdidx = NEXTBYTE(pcable->rdidx, USBMIDIMASK);
	}
	else if (((byte >> 4) & 0x0f) == 0x0D){ //Channel Pressure
		txbuf[0] = cable + 0x0D;
		txbuf[1] = byte;
		txbuf[2] = pcable->buf[ pcable->rdidx ];
		pcable->rdidx = NEXTBYTE(pcable->rdidx, USBMIDIMASK);
	}
	else if (((byte >> 4) & 0x0f) == 0x0E){ //Pitch Bend Change
		txbuf[0] = cable + 0x0E;
		txbuf[1] = byte;
		txbuf[2] = pcable->buf[ pcable->rdidx ];
		pcable->rdidx = NEXTBYTE(pcable->rdidx, USBMIDIMASK);
		txbuf[3] = pcable->buf[ pcable->rdidx ];
		pcable->rdidx = NEXTBYTE(pcable->rdidx, USBMIDIMASK);
	}
	else if ((byte == 0xF1)||(byte == 0xF3)){ //MIDI Time Code Quarter Frame || Song Select.
		txbuf[0] = cable + 0x02;
		txbuf[1] = byte;
		txbuf[2] = pcable->buf[ pcable->rdidx ];
		pcable->rdidx = NEXTBYTE(pcable->rdidx, USBMIDIMASK);
	}
	else if ((byte == 0xF6) // Tune Request
			||(byte>= 0xF8)){// single byte messages
		txbuf[0] = cable + 0x05;
		txbuf[1] = byte;
	}
	else if (byte == 0xF2){ //Song Position Pointer
		txbuf[0] = cable + 0x03;
		txbuf[1] = byte;
		txbuf[2] = pcable->buf[ pcable->rdidx ];
		pcable->rdidx = NEXTBYTE(pcable->rdidx, USBMIDIMASK);
		txbuf[3] = pcable->buf[ pcable->rdidx ];
		pcable->rdidx = NEXTBYTE(pcable->rdidx, USBMIDIMASK);
	}
	else if((0==g_SysEx)&&(byte=0xF0) ){// start SysEx
		txbuf[0] = cable + 0x04;
		txbuf[1] = byte;
		txbuf[2] = pcable->buf[ pcable->rdidx ];
		pcable->rdidx = NEXTBYTE(pcable->rdidx, USBMIDIMASK);
		txbuf[3] = pcable->buf[ pcable->rdidx ];
		pcable->rdidx = NEXTBYTE(pcable->rdidx, USBMIDIMASK);
		g_SysEx = 1;
	}
	else if (1==g_SysEx){
		for(uint8_t i=1; i<=3; i++ ){
			txbuf[i] = byte;
			if (0xF7==byte){
				switch (i){
				case 1:
					txbuf[0] = cable + 0x05;
					break;
				case 2:
					txbuf[0] = cable + 0x06;
					break;
				case 3:
					txbuf[0] = cable + 0x07;
					break;
				}
				g_SysEx = 0;
				break;
			}
			if ( i<3 ){
				byte = pcable->buf[ pcable->rdidx ];
				pcable->rdidx = NEXTBYTE(pcable->rdidx, USBMIDIMASK);
			}
			if ((3==i)&&(1==g_SysEx)) txbuf[0] = cable + 0x04;
		}
	}
	else return;

	MIDI_DataTx(txbuf, 4);
}

//Start transfer
void USBD_SendMidiMessages(void){
	if (hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED)  {
		if (!USB_Tx_State) USBD_MIDI_SendPacket();
		else USB_Tx_State = USB_TX_CONTINUE;
	}
	else USB_Tx_State = USB_TX_READY;
}


//fill midi tx buffer
static uint16_t MIDI_DataTx(uint8_t *msg, uint16_t length){
	uint16_t i = 0;
	for(i = 0; i < length ; i++) {
		APP_Rx_Buffer[APP_Rx_ptr_in] = *(msg + i);
		APP_Rx_ptr_in++;
		if (APP_Rx_ptr_in == APP_RX_DATA_SIZE) APP_Rx_ptr_in = 0;
	}
	return USBD_OK;
}


//process recived midi data
static uint16_t MIDI_DataRx(uint8_t* msg, uint16_t length){
  uint16_t cnt;
  tUsbMidiCable* pcable;

  int byte1 = 0 , byte2 =0 , byte3 =0;

  for (cnt = 0; cnt < length; cnt += 4)  {
    switch ( msg[cnt] >> 4 ) {
    case 0:
      pcable = &usbmidicable1;
      break;
    /*case 1:
      pcable = &usbmidicable2;
      break;*/
    default:
      continue;
    };

    switch ( msg[cnt] & 0x0F ) {
    case 0x0:
    case 0x1:
      continue;
    case 0x5:
    case 0xF:
      pcable->buf[ pcable->curidx ] = msg[ cnt+1 ];
      pcable->curidx = NEXTBYTE(pcable->curidx, USBMIDIMASK);
      break;
    case 0x2:
    case 0x6:
    case 0xD:
      byte1 = pcable->buf[ pcable->curidx ] = msg[ cnt+1 ];
      pcable->curidx = NEXTBYTE(pcable->curidx, USBMIDIMASK);
      byte2 = pcable->buf[ pcable->curidx ] = msg[ cnt+2 ];
      pcable->curidx = NEXTBYTE(pcable->curidx, USBMIDIMASK);
      debug2("usb midi1 " , byte1);//
      debug2("usb midi2 " , byte2);
      break;

    case 0xB:													// MIDI CC command , 2 bytes
    	byte1 = pcable->buf[ pcable->curidx ] = msg[ cnt+1 ];
        pcable->curidx = NEXTBYTE(pcable->curidx, USBMIDIMASK);
        byte2 = pcable->buf[ pcable->curidx ] = msg[ cnt+2 ];
        pcable->curidx = NEXTBYTE(pcable->curidx, USBMIDIMASK);
        byte3= pcable->buf[ pcable->curidx ] = msg[ cnt+3 ];
        pcable->curidx = NEXTBYTE(pcable->curidx, USBMIDIMASK);
        UsbReceivedMidiCC(byte1 , byte2 , byte3 ); 				// Send data to program
      break;

    case 0xC:													//MIDI PC command , 2 bytes
      byte1 = pcable->buf[ pcable->curidx ] = msg[ cnt+1 ];
      pcable->curidx = NEXTBYTE(pcable->curidx, USBMIDIMASK);
      byte2 = pcable->buf[ pcable->curidx ] = msg[ cnt+2 ];
      pcable->curidx = NEXTBYTE(pcable->curidx, USBMIDIMASK);
      UsbReceivedMidiPC(byte1 , byte2); 						// Send data to program
      break;


    default:													// 3bytes  Note On/Off etc
    byte1 = pcable->buf[ pcable->curidx ] = msg[ cnt+1 ];
      pcable->curidx = NEXTBYTE(pcable->curidx, USBMIDIMASK);
      byte2 = pcable->buf[ pcable->curidx ] = msg[ cnt+2 ];
      pcable->curidx = NEXTBYTE(pcable->curidx, USBMIDIMASK);
      pcable->buf[ pcable->curidx ] = msg[ cnt+3 ];
      pcable->curidx = NEXTBYTE(pcable->curidx, USBMIDIMASK);
      break;
    };
    //dump_hex8((char *)pcable->buf , 512);
//    debug2("b1 " , byte1);
//    debug2("b2 " , byte2);
//    crlf();
  };

  return 0;
}



// **********************************
// set message for send to PC
void USBD_MIDI_Message(uint8_t byte1, uint8_t byte2, uint8_t byte3) {
	uint8_t cable = 0;
	uint8_t txbuf[4]= {0,0,0,0};
	tUsbMidiCable * pcable 	= NULL;

	//if (hUsbDeviceFS.dev_state != USBD_STATE_CONFIGURED) return;
	if ( 0 == cable ) pcable = &usbmidicable1;
	if ( NULL == pcable ) return;
	pcable->rdidx = NEXTBYTE(pcable->rdidx, USBMIDIMASK);		// move read index
	cable <<= 4;
	pcable->rdidx = NEXTBYTE(pcable->rdidx, USBMIDIMASK);
	pcable->rdidx = NEXTBYTE(pcable->rdidx, USBMIDIMASK);

	txbuf[0] = cable + 0x0B;	// xxx
	txbuf[1] = byte1;
	txbuf[2] = byte2;
	txbuf[3] = byte3;

	MIDI_DataTx(txbuf, 4);
}



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
