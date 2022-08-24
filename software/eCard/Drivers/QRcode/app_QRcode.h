/*
 * app_QRcode.h
 *
 *  Created on: Jul 20, 2022
 *      Author: TECH66
 */

#ifndef QRCODE_APP_QRCODE_H_
#define QRCODE_APP_QRCODE_H_
#include "qrencode.h"
#include "qr_encode.h"

#define QRCODE_Y 	172

void DISPLAY_RENCODE_TO_TFT(uint8_t *qrcode_data);

#endif /* QRCODE_APP_QRCODE_H_ */
