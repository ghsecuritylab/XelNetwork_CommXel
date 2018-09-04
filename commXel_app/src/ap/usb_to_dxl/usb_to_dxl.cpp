/*
 * usb_to_dxl.cpp
 *
 *  Created on: Sep 3, 2018
 *      Author: kei
 */

#include "usb_to_dxl.hpp"

void u2dInit(void)
{
  dxlportInit();
  dxlportOpen(_DEF_DXL1, 57600);
}

void u2dRun(void)
{
  uint32_t length, i;

  if(vcpGetBaud() != uartGetBaud(_DEF_UART2))
  {
    dxlportOpen(_DEF_DXL1, vcpGetBaud());
  }

  // USB to DXL
  length = vcpAvailable();
  if( length > 0 )
  {
    uint8_t ch;
    dxlportTxEnable(_DEF_DXL1);
    for(i=0; i<length; i++ )
    {
      ch = vcpRead();
      dxlportWrite(_DEF_DXL1, &ch, 1);
    }
    dxlportTxDisable(_DEF_DXL1);
  }

  // DXL to USB
  length = dxlportAvailable(_DEF_DXL1);
  if( length > 0 )
  {
    uint8_t tx_buffer[256];
    if(length > (uint32_t)256)
    {
      length = 256;
    }

    for(i=0; i<length; i++ )
    {
      tx_buffer[i] = dxlportRead(_DEF_DXL1);
    }
    vcpWrite(tx_buffer, length);
  }
}
