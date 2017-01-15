// Copyright 2016 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#include "Core/IPC_HLE/WII_IPC_HLE_Device_usb_ven.h"
#include "Common/Logging/Log.h"
#include "Core/HW/Memmap.h"

CWII_IPC_HLE_Device_usb_ven::CWII_IPC_HLE_Device_usb_ven(const u32 device_id,
                                                         const std::string& device_name)
    : IWII_IPC_HLE_Device(device_id, device_name)
{
}

IPCCommandResult CWII_IPC_HLE_Device_usb_ven::IOCtlV(const IOSIOCtlVRequest& request)
{
  request.Dump(GetDeviceName());
  request.SetReturnValue(IPC_SUCCESS);
  return GetNoReply();
}

IPCCommandResult CWII_IPC_HLE_Device_usb_ven::IOCtl(const IOSIOCtlRequest& request)
{
  request.Log(GetDeviceName(), LogTypes::OSHLE);
  request.SetReturnValue(IPC_SUCCESS);

  IPCCommandResult reply = GetDefaultReply();
  switch (request.request)
  {
  case USBV5_IOCTL_GETVERSION:
    Memory::Write_U32(0x50001, request.buffer_out);
    reply = GetDefaultReply();
    break;

  case USBV5_IOCTL_GETDEVICECHANGE:
  {
    // sent on change
    static bool firstcall = true;
    if (firstcall)
    {
      reply = GetDefaultReply();
      firstcall = false;
    }

    // num devices
    request.SetReturnValue(0);
    return reply;
  }
  break;

  case USBV5_IOCTL_ATTACHFINISH:
    reply = GetDefaultReply();
    break;

  case USBV5_IOCTL_SUSPEND_RESUME:
    DEBUG_LOG(OSHLE, "Device: %i Resumed: %i", Memory::Read_U32(request.buffer_in),
              Memory::Read_U32(request.buffer_in + 4));
    reply = GetDefaultReply();
    break;

  case USBV5_IOCTL_GETDEVPARAMS:
  {
    s32 device = Memory::Read_U32(request.buffer_in);
    u32 unk = Memory::Read_U32(request.buffer_in + 4);

    DEBUG_LOG(OSHLE, "USBV5_IOCTL_GETDEVPARAMS device: %i unk: %i", device, unk);

    Memory::Write_U32(0, request.buffer_out);

    reply = GetDefaultReply();
  }
  break;

  default:
    request.Log(GetDeviceName(), LogTypes::OSHLE, LogTypes::LDEBUG);
  }
  return reply;
}
