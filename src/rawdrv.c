#include "kernel_calls.h"

#include "portfolio.h"
#include "strings.h"

enum driver_tags_e
  {
   CREATEDRIVER_TAG_ABORTIO = TAG_ITEM_LAST+1, //a
   CREATEDRIVER_TAG_MAXCMDS,	//b
   CREATEDRIVER_TAG_CMDTABLE,	//c
   CREATEDRIVER_TAG_MSGPORT,	//d
   CREATEDRIVER_TAG_INIT,	//e
   CREATEDRIVER_TAG_DISPATCH	//f
  };

enum device_tags_e
  {
   CREATEDEVICE_TAG_DRVR = TAG_ITEM_LAST+1, //a
   CREATEDEVICE_TAG_CRIO,          /* createIO routine */ //b
   CREATEDEVICE_TAG_DLIO,          /* deleteIO routine */ //c
   CREATEDEVICE_TAG_OPEN,          /* open routine */ //d
   CREATEDEVICE_TAG_CLOSE,         /* close routine */ //e
   CREATEDEVICE_TAG_IOREQSZ,       /* optional request size */ //f
   CREATEDEVICE_TAG_INIT           /* InitRoutine to call */ //10
  };


void drv_abortio(struct IOReq *ior);
//int32 drv_dispatch(struct IOReq * ior);
Item drv_init(struct Driver *drv);
//int32 drv_cmdtable(struct IOReq * ior);

int32 dev_init(struct Device *dev);
int32 dev_open(struct Device *dev);
void  dev_close(struct Device *dev);
int32 drv_cmdwrite(struct IOReq *ior);
int32 drv_cmdread(struct IOReq *ior);
int32 drv_cmdstatus(struct IOReq *ior);

static volatile Item g_DeviceItem = 0;

static
void*
g_CMDTABLE[3] =
  {
   (void*)drv_cmdwrite,
   (void*)drv_cmdread,
   (void*)drv_cmdstatus
  };

static
TagArg
g_DRV_TAGS[] =
  {
   {TAG_ITEM_PRI,(void*)1},	//0
   {TAG_ITEM_NAME,(void*)"rawdriver"}, //1
   {CREATEDRIVER_TAG_ABORTIO,(void*)drv_abortio}, //2
   {CREATEDRIVER_TAG_MAXCMDS,(void*)3}, //3
   {CREATEDRIVER_TAG_CMDTABLE,(void*)g_CMDTABLE}, //4
   {CREATEDRIVER_TAG_INIT,(void*)drv_init}, //5
   {TAG_END,(void*)0}
  };

static
TagArg
g_DEV_TAGS[] =
  {
   {TAG_ITEM_PRI,(void*)0x96},	//0
   {CREATEDEVICE_TAG_DRVR,(void*)1}, //1
   {TAG_ITEM_NAME,(void*)"raw"}, //2
   {CREATEDEVICE_TAG_INIT,(void*)dev_init}, //3
   {CREATEDEVICE_TAG_OPEN,(void*)dev_open}, //4
   {CREATEDEVICE_TAG_CLOSE,(void*)dev_close}, //5
   {TAG_END,(void*)0}
  };

static
Item
CreateDriver(TagArg *tags_)
{
  return CreateItem(MKNODEID(KERNELNODE,DRIVERNODE),tags_);
}

static
Item
CreateDevice(TagArg *tags_)
{
  return CreateItem(MKNODEID(KERNELNODE,DEVICENODE),tags_);
}

int
main()
{
  Item drv;
  Item dev;
  int32 signal;
  int32 rxsignal;

  Debug();
  kprintf("rawdrv: start %s %s\n",__DATE__,__TIME__);

  drv = CreateDriver(g_DRV_TAGS);
  if(drv <= 0)
    {
      kprintf("rawdrv: create driver failed - %x - ",drv);
      PrintfSysErr(drv);
      return 0;
    }
  else
    {
      kprintf("rawdrv: driver item: %x\n",drv);
    }

  g_DEV_TAGS[1].ta_Arg = (void*)drv;

  dev = CreateDevice(g_DEV_TAGS);
  if(dev <= 0)
    {
      kprintf("rawdrv: create device failed - %x - \n",dev);
      PrintfSysErr(dev);
      return 0;
    }
  else
    {
      kprintf("rawdrv: device item = %x\n",dev);
    }

  g_DeviceItem = dev;

  signal = AllocSignal(0);
  if(signal <= 0)
    {
      kprintf("rawdrv: unable to alloc signal - ");
      PrintfSysErr(signal);
      return 0;
    }

  kprintf("rawdrv: entering wait signal loop\n");
  for(;;)
    {
      rxsignal = WaitSignal(signal);
      if(rxsignal & SIGF_ABORT)
        {
          kprintf("rawdrv: SIGF_ABORT received\n");
          return 0;
        }
      else
        {
          kprintf("rawdrv: received signal %x\n",rxsignal);
        }
    }

  kprintf("rawdrv: out of loop\n");

  return 0;
}

void
drv_abortio(struct IOReq *ior_)
{
  (void)ior_;

  svc_kprintf("rawdrv: drv_abortio\n");
}

Item
drv_init(struct Driver *drv_)
{
  svc_kprintf("rawdrv: drv_init\n");

  return drv_->drv.n_Item;
}

static
int32
drv_cmdwrite_uint32(struct IOReq *ior_)
{
  int32   i;
  int32   len  =          ior_->io_Info.ioi_Send.iob_Len;
  uint32 *src  = (uint32*)ior_->io_Info.ioi_Send.iob_Buffer;
  uint32 *dest = (uint32*)ior_->io_Info.ioi_Recv.iob_Buffer;

  dest += ior_->io_Info.ioi_Offset;
  for(i = 0; i < len; i++)
    dest[i] = src[i];

  return 1;

}

int32
drv_cmdwrite(struct IOReq *ior_)
{
  return drv_cmdwrite_uint32(ior_);
}

static
int32
drv_cmdread_uint32(struct IOReq *ior_)
{
  int32   i;
  int32   len  =          ior_->io_Info.ioi_Recv.iob_Len;
  uint32 *src  = (uint32*)ior_->io_Info.ioi_Send.iob_Buffer;
  uint32 *dest = (uint32*)ior_->io_Info.ioi_Recv.iob_Buffer;

  src += ior_->io_Info.ioi_Offset;
  for(i = 0; i < len; i++)
    dest[i] = src[i];

  return 1;
}

int32
drv_cmdread(struct IOReq *ior_)
{
  return drv_cmdread_uint32(ior_);
}

int32
drv_cmdstatus(struct IOReq *ior_)
{
  (void)ior_;

  return 0;
}

int32
dev_init(struct Device *dev_)
{
  svc_kprintf("rawdrv: dev_init\n");

  dev_->dev_OpenCnt = 0;

  return dev_->dev.n_Item;
}

int32
dev_open(struct Device *dev_)
{
  svc_kprintf("rawdrv: dev_open - "
              "opencnt=%d; "
              "driver=%p"
              "\n",
              dev_->dev_OpenCnt,
              dev_->dev_Driver);

  return dev_->dev.n_Item;
}

void
dev_close(struct Device *dev_)
{
  svc_kprintf("rawdrv: dev_close - "
              "opencnt=%d; "
              "driver=%p"
              "\n",
              dev_->dev_OpenCnt,
              dev_->dev_Driver);
}
