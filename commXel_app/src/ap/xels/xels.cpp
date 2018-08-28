/*
 * xels.cpp
 *
 *  Created on: 2018. 8. 24.
 *      Author: HanCheol Cho
 */




#include "xels.h"
#include "dxl/dxl.h"

#define _XELS_DEBUG



#ifdef _XELS_DEBUG
static XelNetwork::XelInfo_t xelinfo_tbl[8];

int xelsCmdif(int argc, char **argv);
#endif


static dxl_t dxl_cmd;


err_code_t err_code = OK;

dxlcmd_param_t        param;
dxlcmd_resp_t         resp;
dxlcmd_resp_read_t    resp_read;



void xelsInit(void)
{
  dxlInit(&dxl_cmd, DXL_PACKET_VER_2_0);

#ifdef _XELS_DEBUG
  cmdifAdd((char *)"xels", xelsCmdif);
#endif
}

bool xelsOpen(uint8_t ch, uint32_t baud)
{
  return dxlOpenPort(&dxl_cmd, ch, baud);
}

bool xelsIsOpen(void)
{
  return dxlIsOpen(&dxl_cmd);
}

uint32_t xelsPings(XelNetwork::XelInfo_t *p_xel_infos, uint32_t max_xels)
{
  uint16_t xels_count = 0;
  dxl_error_t dxl_ret;
  dxl_t    *p_dxl_node;
  uint16_t xel_index;

  p_dxl_node = &dxl_cmd;

  if (dxlIsOpen(p_dxl_node) == false)
  {
    err_code = ERR_DXL_NOT_OPEN;
    return 0;
  }

  xel_index = 0;

  dxl_ret = dxlcmdPing(p_dxl_node, DXL_GLOBAL_ID, &resp.ping, 128*3);
  if (dxl_ret == DXL_RET_RX_RESP)
  {
    xels_count = resp.ping.id_count;

    if (xels_count > max_xels)
    {
      xels_count = max_xels;
    }

    for (int i=0; i<xels_count; i++)
    {
      p_xel_infos[xel_index].init_data.xel_id = resp.ping.p_node[i]->id;

      if (xelsReadHeader(&p_xel_infos[xel_index]) == true)
      {
        xel_index++;
      }
    }
  }


  return xel_index;
}

bool xelsPing(XelNetwork::XelInfo_t *p_xel_info)
{
  bool ret = true;
  dxl_error_t dxl_ret;
  dxl_t    *p_dxl_node;

  p_dxl_node = &dxl_cmd;


  dxl_ret = dxlcmdPing(p_dxl_node, p_xel_info->init_data.xel_id, &resp.ping, 100);
  if (dxl_ret == DXL_RET_RX_RESP)
  {
    ret = true;
  }
  else
  {
    ret = false;
  }

  return ret;
}

bool xelsReadHeader(XelNetwork::XelInfo_t *p_xel_info)
{
  bool ret = true;
  dxl_error_t dxl_ret;
  dxl_t    *p_dxl_node;
  xels_header_t xel_header;

  p_dxl_node = &dxl_cmd;


  dxl_ret = dxlcmdRead(p_dxl_node, p_xel_info->init_data.xel_id, 32, sizeof(xels_header_t), &resp_read, 100);
  if (dxl_ret == DXL_RET_RX_RESP)
  {
    memcpy(&xel_header, resp_read.p_node[0]->p_data, sizeof(xels_header_t));

    p_xel_info->init_data.data_type = (XelNetwork::DataType)xel_header.data_type;
    p_xel_info->init_data.data_get_interval_hz = xel_header.data_get_interval_hz;
    memcpy(p_xel_info->init_data.data_name, xel_header.data_name, 32);
    p_xel_info->init_data.msg_type = (ros2::MessagePrefix)xel_header.msg_type;
    p_xel_info->init_data.data_addr = xel_header.data_addr;
    p_xel_info->init_data.data_lenght = xel_header.data_lenght;
  }
  else
  {
    ret = false;
  }


  return ret;
}

bool xelsReadData(XelNetwork::XelInfo_t *p_xel_info)
{
  bool ret = true;
  dxl_error_t dxl_ret;
  dxl_t    *p_dxl_node;
  uint16_t data_addr;
  uint16_t data_length;

  p_dxl_node = &dxl_cmd;

  data_addr   = p_xel_info->init_data.data_addr;
  data_length = p_xel_info->init_data.data_lenght;

  dxl_ret = dxlcmdRead(p_dxl_node, p_xel_info->init_data.xel_id, data_addr, data_length, &resp_read, 100);
  if (dxl_ret == DXL_RET_RX_RESP)
  {
    memcpy(p_xel_info->data, resp_read.p_node[0]->p_data, data_length);
  }
  else
  {
    ret = false;
  }

  return ret;
}


#ifdef _XELS_DEBUG
int xelsCmdif(int argc, char **argv)
{
  bool ret = true;
  uint32_t number;
  static uint16_t xelinfo_count = 0;


  if (argc == 2)
  {
    if(strcmp("ping", argv[1]) == 0 && xelsIsOpen() == true)
    {
      xelinfo_count = xelsPings(xelinfo_tbl, 8);

      for (int i=0; i<xelinfo_count; i++)
      {
        xelinfo_tbl[i].init_data.data_name[31] = 0;

        cmdifPrintf("xelinfo     \t: %d\n", i);
        cmdifPrintf("xel_id      \t: %d\n", xelinfo_tbl[i].init_data.xel_id);
        cmdifPrintf("data_type   \t: %d\n", xelinfo_tbl[i].init_data.data_type);
        cmdifPrintf("interval_hz \t: %d\n", xelinfo_tbl[i].init_data.data_get_interval_hz);
        cmdifPrintf("data_name   \t: %s\n", xelinfo_tbl[i].init_data.data_name);
        cmdifPrintf("msg_type    \t: %d\n", xelinfo_tbl[i].init_data.msg_type);
        cmdifPrintf("data_addr   \t: %d\n", xelinfo_tbl[i].init_data.data_addr);
        cmdifPrintf("data_length \t: %d\n", xelinfo_tbl[i].init_data.data_lenght);
        cmdifPrintf("\n");
      }
    }
    else if (strcmp("info", argv[1]) == 0 && xelsIsOpen() == true)
    {
      for (int i=0; i<xelinfo_count; i++)
      {
        cmdifPrintf("xelinfo     \t: %d\n", i);
        cmdifPrintf("xel_id      \t: %d\n", xelinfo_tbl[i].init_data.xel_id);
        cmdifPrintf("data_type   \t: %d\n", xelinfo_tbl[i].init_data.data_type);
        cmdifPrintf("interval_hz \t: %d\n", xelinfo_tbl[i].init_data.data_get_interval_hz);
        cmdifPrintf("data_name   \t: %s\n", xelinfo_tbl[i].init_data.data_name);
        cmdifPrintf("msg_type    \t: %d\n", xelinfo_tbl[i].init_data.msg_type);
        cmdifPrintf("data_addr   \t: %d\n", xelinfo_tbl[i].init_data.data_addr);
        cmdifPrintf("data_length \t: %d\n", xelinfo_tbl[i].init_data.data_lenght);
        cmdifPrintf("\n");
      }
    }
    else if (strcmp("read", argv[1]) == 0 && xelsIsOpen() == true)
    {
      if (xelinfo_count == 0)
      {
        cmdifPrintf("no xels\n");
      }
      else
      {
        xel_data_type_t data;
        uint32_t pre_time[8];
        uint32_t hz_time[8];

        for (int i=0; i<xelinfo_count; i++)
        {
          pre_time[i] = millis();
          hz_time[i] = 1000/xelinfo_tbl[i].init_data.data_get_interval_hz;
        }

        while(cmdifRxAvailable() == 0)
        {
          for (int i=0; i<xelinfo_count; i++)
          {
            if (millis()-pre_time[i] >= hz_time[i])
            {
              pre_time[i] = millis();
              if (xelsReadData(&xelinfo_tbl[i]) == true)
              {
                data.u8Data[0] = xelinfo_tbl[i].data[0];
                data.u8Data[1] = xelinfo_tbl[i].data[1];
                data.u8Data[2] = xelinfo_tbl[i].data[2];
                data.u8Data[3] = xelinfo_tbl[i].data[3];

                cmdifPrintf("xel_id %d : %d\n", xelinfo_tbl[i].init_data.xel_id, data.UINT32);
              }
            }
          }
        }
      }
    }
    else
    {

    }
  }
  else if (argc == 3)
  {
    number = (uint32_t) strtoul((const char * ) argv[2], (char **)NULL, (int) 0);

    if (strcmp("open", argv[1]) == 0)
    {
      ret = xelsOpen(_DEF_DXL1, number);
      if (ret == true)
      {
        cmdifPrintf("open is OK Ver2.0, _DEF_DXL1, %d bps", number);
      }
      else
      {
        cmdifPrintf("open is FAIL, _DEF_DXL1, %d bps", number);
      }
    }
    else if (strcmp("ping", argv[1]) == 0 && xelsIsOpen() == true)
    {
      if (xelinfo_count == 0)
      {
        cmdifPrintf("no xels\n");
      }
      else if (number >= xelinfo_count)
      {
        cmdifPrintf("over xel number : < %d\n", xelinfo_count);
      }
      else if (xelsPing(&xelinfo_tbl[number]) == true)
      {
        cmdifPrintf("xel_id %d : OK\n", xelinfo_tbl[number].init_data.xel_id);
      }
    }
    else if (strcmp("read", argv[1]) == 0 && xelsIsOpen() == true)
    {
      if (xelinfo_count == 0)
      {
        cmdifPrintf("no xels\n");
      }
      else if (number >= xelinfo_count)
      {
        cmdifPrintf("over xel number : < %d\n", xelinfo_count);
      }
      else
      {
        xel_data_type_t data;
        uint32_t pre_time;
        uint32_t hz_time;


        pre_time = millis();
        hz_time = 1000/xelinfo_tbl[number].init_data.data_get_interval_hz;

        while(cmdifRxAvailable() == 0)
        {
          if (millis()-pre_time >= hz_time)
          {
            pre_time = millis();
            if (xelsReadData(&xelinfo_tbl[number]) == true)
            {
              data.u8Data[0] = xelinfo_tbl[number].data[0];
              data.u8Data[1] = xelinfo_tbl[number].data[1];
              data.u8Data[2] = xelinfo_tbl[number].data[2];
              data.u8Data[3] = xelinfo_tbl[number].data[3];

              cmdifPrintf("xel_id %d : %d\n", xelinfo_tbl[number].init_data.xel_id, data.UINT32);
            }
          }
        }
      }
    }

  }
  else
  {
    ret = false;
  }


  if (ret == false)
  {
    cmdifPrintf( "xels open baud\n");
  }

  return 0;
}
#endif