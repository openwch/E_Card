/********************************** (C) COPYRIGHT *******************************
 * File Name          : usb_desc.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/08/08
 * Description        : Descriptors for Virtual Com Port Demo.
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/ 
#include "usb_lib.h"
#include "usb_desc.h"

/* USB Device Descriptors */
const uint8_t  USBD_DeviceDescriptor[] = {
//	0x12, 0x01, 0x10, 0x01,0xFF, 0x80, 0x55, 0x40,
//	0x86, 0x1A, 0x37, 0x55,
//	0x00, 0x01, 0x01, 0x02, 0x03, 0x01,

    0x12,
    0x01,
    0x10,
    0x01,
    0x02,
    0x00,
    0x00,
    0x40,
    0x86, 0x1a,
    0x40, 0x80,
    0x00, 0x30,
    0x01,                 //�����ߵ��ַ���������������ֵ
    0x02,                 //��Ʒ���ַ���������������ֵ
    0x03,                 //��ŵ��ַ���������������ֵ
    0x01                  //�������õ���Ŀ
};

/* USB Configration Descriptors */
const uint8_t  USBD_ConfigDescriptor[] = { 
//	0x09, 0x02, 0x2E, 0x00, 0x01, 0x01, 0x00, 0x80, 0x32,
//	0x09, 0x04, 0x00, 0x00, 0x04, 0xFF, 0x80, 0x55, 0x00,
//	0x07, 0x05, 0x82, 0x02, 0x40, 0x00, 0x00,
//	0x07, 0x05, 0x02, 0x02, 0x40, 0x00, 0x00,
//	0x07, 0x05, 0x81, 0x02, 0x40, 0x00, 0x00,
//	0x07, 0x05, 0x01, 0x02, 0x40, 0x00, 0x00,

    0x09,0x02,0x43,0x00,0x02,0x01,0x00,0x80,0x30,

    //����Ϊ�ӿ�0��CDC�ӿڣ�������
    0x09, 0x04,0x00,0x00,0x01,0x02,0x02,0x01,0x00,

    0x05,0x24,0x00,0x10,0x01,
    0x04,0x24,0x02,0x02,
    0x05,0x24,0x06,0x00,0x01,
    0x05,0x24,0x01,0x01,0x00,

    0x07,0x05,0x84,0x03,0x08,0x00,0x01,                       //�ж��ϴ��˵�������  �˵�4

    //����Ϊ�ӿ�1�����ݽӿڣ�������
    0x09,0x04,0x01,0x00,0x02,0x0a,0x00,0x00,0x00,

    0x07,0x05,0x01,0x02,0x40,0x00,0x00,                       //�˵������� �˵�1
    0x07,0x05,0x81,0x02,0x40,0x00,0x00,                       //�˵������� �˵�1

};

/* USB String Descriptors */
const uint8_t USBD_StringLangID[USBD_SIZE_STRING_LANGID] = {
	USBD_SIZE_STRING_LANGID,
	USB_STRING_DESCRIPTOR_TYPE,
	0x09,
	0x04 
};

/* USB Device String Vendor */
const uint8_t USBD_StringVendor[USBD_SIZE_STRING_VENDOR] = {
	USBD_SIZE_STRING_VENDOR,    
	USB_STRING_DESCRIPTOR_TYPE,           
	'W', 0, 'C', 0, 'H', 0, 'I', 0, 'C', 0, 'r', 0, 'o', 0, 'e', 0,
	'l', 0, 'e', 0, 'c', 0, 't', 0, 'r', 0, 'o', 0, 'n', 0, 'i', 0,
	'c', 0, 's', 0
};

/* USB Device String Product */
const uint8_t USBD_StringProduct[USBD_SIZE_STRING_PRODUCT] = {
	USBD_SIZE_STRING_PRODUCT,         
	USB_STRING_DESCRIPTOR_TYPE,        
	'W', 0, 'C', 0, 'H', 0, '3', 0, '2', 0, ' ', 0, 'V', 0, 'i', 0,
	'r', 0, 't', 0, 'u', 0, 'a', 0, 'l', 0, ' ', 0, 'C', 0, 'O', 0,
	'M', 0, ' ', 0, 'P', 0, 'o', 0, 'r', 0, 't', 0, ' ', 0, ' ', 0
};

/* USB Device String Serial */
const uint8_t USBD_StringSerial[USBD_SIZE_STRING_SERIAL] = {
	USBD_SIZE_STRING_SERIAL,          
	USB_STRING_DESCRIPTOR_TYPE,                   
	'W', 0, 'C', 0, 'H', 0, '3', 0, '2', 0
};






