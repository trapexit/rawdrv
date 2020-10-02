/*
  ISC License

  Copyright (c) 2021, Antonio SJ Musumeci <trapexit@spawn.link>

  Permission to use, copy, modify, and/or distribute this software for any
  purpose with or without fee is hereby granted, provided that the above
  copyright notice and this permission notice appear in all copies.

  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include "rawio.h"

#include "types.h"
#include "io.h"
#include "filefunctions.h"
#include "device.h"

static Item g_RAWDRIVER = 0;

Err
rawio_init(void)
{
  if(g_RAWDRIVER == 0)
    g_RAWDRIVER = LoadProgram("system/Drivers/rawdrv");
  if(g_RAWDRIVER < 0)
    return g_RAWDRIVER;

  return 0;
}

Err
rawio_destroy(void)
{
  return DeleteItem(g_RAWDRIVER);
}

Item
rawio_open_device(void)
{
  return OpenNamedDevice("raw",0);
}

Err
rawio_close_device(Item device_)
{
  return CloseNamedDevice(device_);
}

Item
rawio_create_ioreq(Item device_)
{
  return CreateIOReq(NULL,0,device_,0);
}

Err
rawio_r_uint32(Item    device_,
               void   *src_,
               uint32 *dest_,
               int32   len_)
{
  Err rv;
  Item ioreq;
  IOInfo ioi = {0};

  ioreq = rawio_create_ioreq(device_);
  if(ioreq < 0)
    return ioreq;

  ioi.ioi_Command         = CMD_READ;
  ioi.ioi_Send.iob_Buffer = src_;
  ioi.ioi_Recv.iob_Buffer = dest_;
  ioi.ioi_Recv.iob_Len    = len_;

  rv = DoIO(ioreq,&ioi);

  DeleteIOReq(ioreq);

  return rv;
}

Err
rawio_w_uint32(Item    device_,
               void   *src_,
               uint32 *dest_,
               int32   len_)
{
  Err rv;
  Item ioreq;
  IOInfo ioi = {0};

  ioreq = rawio_create_ioreq(device_);
  if(ioreq < 0)
    return ioreq;

  ioi.ioi_Command         = CMD_WRITE;
  ioi.ioi_Send.iob_Buffer = src_;
  ioi.ioi_Send.iob_Len    = len_;
  ioi.ioi_Recv.iob_Buffer = dest_;

  rv = DoIO(ioreq,&ioi);

  DeleteIOReq(ioreq);

  return rv;
}
