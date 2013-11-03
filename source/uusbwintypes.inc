  PDevBroadcastHdr  = ^DEV_BROADCAST_HDR;
  DEV_BROADCAST_HDR = packed record
    dbch_size: DWORD;
    dbch_devicetype: DWORD;
    dbch_reserved: DWORD;
  end;

  PDevBroadcastDeviceInterface  = ^DEV_BROADCAST_DEVICEINTERFACE;
  DEV_BROADCAST_DEVICEINTERFACE = record
    dbcc_size: DWORD;
    dbcc_devicetype: DWORD;
    dbcc_reserved: DWORD;
    dbcc_classguid: TGUID;
    dbcc_name: short;
  end;

const
  GUID_DEVINTERFACE_USB_DEVICE: TGUID = '{A5DCBF10-6530-11D2-901F-00C04FB951ED}';
  DBT_DEVICEARRIVAL          = $8000;          // system detected a new device
  DBT_DEVICEREMOVECOMPLETE   = $8004;          // device is gone
  DBT_DEVTYP_DEVICEINTERFACE = $00000005;      // device interface class

var
 SA:TSecurityAttributes;{for Win2000}

const
 TUnicodeNameMaxLong = 256;//MAXIMUM_USB_STRING_LENGTH +1 (zero)

type
 PUnicodeName = ^TUnicodeName;
 TUnicodeName = array[0..TUnicodeNameMaxLong - 1] of WideChar;{WChar}

 USB_HUB_NODE = DWord;

  PUSB_HUB_NAME = ^USB_HUB_NAME;
  USB_HUB_NAME = packed record
	ActualLength :DWord;
	HubName :TUnicodeName;
  end;

 TDSetupPacket = packed record
  bmRequest,
  bRequest :Byte;
  wValueLo,
  wValueHi :Byte;
  wIndex,
  wLength :Word;
 end;

 TDESCRIPTOR_REQUEST = packed record
    ConnectionIndex :DWord;
    SetupPacket :TDSetupPacket;
    Data :array[0..2048 - 1] of Byte;
 end;

 TDEVICE_DESCRIPTOR = packed record  {USB_DEV....?}
    bLength,
    bDescriptorType :Byte;
    bcdUSB :array[0..1] of Byte;
    bDeviceClass,
    bDeviceSubClass,
    bDeviceProtocol,
    bMaxPacketSize0 :Byte;
    idVendor,
    idProduct,
    bcdDevice :array[0..1] of Byte;
    iManufacturer,
    iProduct,
    iSerialNumber,
    bNumConfigurations :Byte;
 end;

 THUB_DESCRIPTOR = packed record
    bDescriptorLength,
    bDescriptorType,
    bNumberOfPorts :Byte;
    wHubCharacteristics :array[0..1] of Byte;
    bPowerOnToPowerGood,
    bHubControlCurrent :Byte;
    bRemoveAndPowerMask :array[0..63] of Byte;
 end;

 TNODE_INFORMATION = packed record
    NodeType :USB_HUB_NODE;
    HubDescriptor :THUB_DESCRIPTOR;
    HubIsBusPowered :Boolean;
 end;

  PUSB_ENDPOINT_DESCRIPTOR = ^USB_ENDPOINT_DESCRIPTOR;
  USB_ENDPOINT_DESCRIPTOR = packed record
         bLength,
         bDescriptorType,
         bEndpointAddress,
         bmAttributes :Byte;
         wMaxPacketSize :Word;
         bInterval :Byte;
  end;

  PUSB_PIPE_INFO = ^USB_PIPE_INFO;
  USB_PIPE_INFO = packed record
	EndpointDescriptor :USB_ENDPOINT_DESCRIPTOR;
	ScheduleOffset :DWord;
  end;

 TNODE_CONNECTION_INFORMATION = packed record
    ConnectionIndex :DWord;
    DeviceDescriptor :TDEVICE_DESCRIPTOR;
    CurrentConfigurationValue :Byte;
    LowSpeed,
    DeviceIsHub :Boolean;
    DeviceAddress :array[0..1] of Byte;
    NumberOfOpenPipes,
    ConnectionStatus :array[0..3] of Byte;
    PipeList :array[0..31] of USB_PIPE_INFO;
 end;

  PUSB_NODE_CONNECTION_NAME = ^USB_NODE_CONNECTION_NAME;
  USB_NODE_CONNECTION_NAME = packed record
	ConnectionIndex :DWord;
	ActualLength :DWord;
	NodeName :TUnicodeName;
  end;

  PUSB_STRING_DESCRIPTOR = ^USB_STRING_DESCRIPTOR;
  USB_STRING_DESCRIPTOR = packed record
         bLength,
         bDescriptorType :Byte;
         bString :TUniCodeName;
  end;

const
 USB_SUBMIT_URB                    = 0;
 USB_RESET_PORT                    = 1;
 USB_GET_ROOTHUB_PDO               = 3;
 USB_GET_PORT_STATUS               = 4;
 USB_ENABLE_PORT                   = 5;
 USB_GET_HUB_COUNT                 = 6;
 USB_CYCLE_PORT                    = 7;
 USB_GET_HUB_NAME                  = 8;
 USB_IDLE_NOTIFICATION             = 9;
 USB_GET_BUS_INFO                  = 264;
 USB_GET_CONTROLLER_NAME           = 265;
 USB_GET_BUSGUID_INFO              = 266;
 USB_GET_PARENT_HUB_INFO           = 267;
 USB_GET_DEVICE_HANDLE             = 268;

 HCD_GET_STATS_1                   = 255;
 HCD_DIAGNOSTIC_MODE_ON            = 256;
 HCD_DIAGNOSTIC_MODE_OFF           = 257;
 HCD_GET_ROOT_HUB_NAME             = 258;
 HCD_GET_DRIVERKEY_NAME            = 265;
 HCD_GET_STATS_2                   = 266;
 HCD_DISABLE_PORT                  = 268;
 HCD_ENABLE_PORT                   = 269;
 HCD_USER_REQUEST                  = 270;

 USB_GET_NODE_INFORMATION                = 258;
 USB_GET_NODE_CONNECTION_INFORMATION     = 259;
 USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION = 260;
 USB_GET_NODE_CONNECTION_NAME            = 261;
 USB_DIAG_IGNORE_HUBS_ON                 = 262;
 USB_DIAG_IGNORE_HUBS_OFF                = 263;
 USB_GET_NODE_CONNECTION_DRIVERKEY_NAME  = 264;
 USB_GET_HUB_CAPABILITIES                = 271;
 USB_GET_NODE_CONNECTION_ATTRIBUTES      = 272;

 METHOD_BUFFERED      = 0;
 METHOD_IN_DIRECT     = 1; {+}
 METHOD_OUT_DIRECT    = 2; {+}
 METHOD_NEITHER       = 3;

 FILE_ANY_ACCESS      =            0;
 FILE_SPECIAL_ACCESS  =   (FILE_ANY_ACCESS); {+}
 FILE_READ_ACCESS     =      ( $0001 );    // file & pipe {+}
 FILE_WRITE_ACCESS    =      ( $0002 );    // file & pipe {+}

 FILE_DEVICE_UNKNOWN  = $00000022;
 FILE_DEVICE_USB      = FILE_DEVICE_UNKNOWN;

 USB_CTL_BASE         = (FILE_DEVICE_USB shl 16) or (FILE_ANY_ACCESS shl 14);
 USB_CTL_CONST        = USB_CTL_BASE or METHOD_BUFFERED;
 USB_KERNEL_CTL_CONST = USB_CTL_BASE or METHOD_NEITHER;

 MAXIMUM_USB_STRING_LENGTH                  = 255;

 USB_DEVICE_CLASS_RESERVED                  = $00;
 USB_DEVICE_CLASS_AUDIO                     = $01;
 USB_DEVICE_CLASS_COMMUNICATIONS            = $02;
 USB_DEVICE_CLASS_HUMAN_INTERFACE           = $03;
 USB_DEVICE_CLASS_MONITOR                   = $04;
 USB_DEVICE_CLASS_PHYSICAL_INTERFACE        = $05;
 USB_DEVICE_CLASS_POWER                     = $06;
 USB_DEVICE_CLASS_PRINTER                   = $07;
 USB_DEVICE_CLASS_STORAGE                   = $08;
 USB_DEVICE_CLASS_HUB                       = $09;
 USB_DEVICE_CLASS_VENDOR_SPECIFIC           = $FF;

 USB_RESERVED_DESCRIPTOR_TYPE               = $06;
 USB_CONFIG_POWER_DESCRIPTOR_TYPE           = $07;
 USB_INTERFACE_POWER_DESCRIPTOR_TYPE        = $08;

 USB_REQUEST_GET_STATUS                     = $00;
 USB_REQUEST_CLEAR_FEATURE                  = $01;
 USB_REQUEST_SET_FEATURE                    = $03;
 USB_REQUEST_SET_ADDRESS                    = $05;
 USB_REQUEST_GET_DESCRIPTOR                 = $06;
 USB_REQUEST_SET_DESCRIPTOR                 = $07;
 USB_REQUEST_GET_CONFIGURATION              = $08;
 USB_REQUEST_SET_CONFIGURATION              = $09;
 USB_REQUEST_GET_INTERFACE                  = $0A;
 USB_REQUEST_SET_INTERFACE                  = $0B;
 USB_REQUEST_SYNC_FRAME                     = $0C;

 USB_GETSTATUS_SELF_POWERED                 = $01;
 USB_GETSTATUS_REMOTE_WAKEUP_ENABLED        = $02;

 BMREQUEST_HOST_TO_DEVICE                   = 0;
 BMREQUEST_DEVICE_TO_HOST                   = 1;

 BMREQUEST_STANDARD                         = 0;
 BMREQUEST_CLASS                            = 1;
 BMREQUEST_VENDOR                           = 2;

 BMREQUEST_TO_DEVICE                        = 0;
 BMREQUEST_TO_INTERFACE                     = 1;
 BMREQUEST_TO_ENDPOINT                      = 2;
 BMREQUEST_TO_OTHER                         = 3;

// USB_COMMON_DESCRIPTOR.bDescriptorType constants
 USB_DEVICE_DESCRIPTOR_TYPE                 = $01;
 USB_CONFIGURATION_DESCRIPTOR_TYPE          = $02;
 USB_STRING_DESCRIPTOR_TYPE                 = $03;
 USB_INTERFACE_DESCRIPTOR_TYPE              = $04;
 USB_ENDPOINT_DESCRIPTOR_TYPE               = $05;

 IOCTL_INTERNAL_USB_CYCLE_PORT          = USB_KERNEL_CTL_CONST or (USB_CYCLE_PORT shl 2);
 IOCTL_INTERNAL_USB_ENABLE_PORT         = USB_KERNEL_CTL_CONST or (USB_ENABLE_PORT shl 2);
 IOCTL_INTERNAL_USB_GET_BUS_INFO        = USB_CTL_CONST or (USB_GET_BUS_INFO shl 2);
 IOCTL_INTERNAL_USB_GET_BUSGUID_INFO    = USB_CTL_CONST or (USB_GET_BUSGUID_INFO shl 2);
 IOCTL_INTERNAL_USB_GET_CONTROLLER_NAME = USB_CTL_CONST or (USB_GET_CONTROLLER_NAME shl 2);
 IOCTL_INTERNAL_USB_GET_DEVICE_HANDLE   = USB_KERNEL_CTL_CONST or (USB_GET_DEVICE_HANDLE shl 2);
 IOCTL_INTERNAL_USB_GET_HUB_COUNT       = USB_KERNEL_CTL_CONST or (USB_GET_HUB_COUNT shl 2);
 IOCTL_INTERNAL_USB_GET_HUB_NAME        = USB_CTL_CONST or (USB_GET_HUB_NAME shl 2);
 IOCTL_INTERNAL_USB_GET_PARENT_HUB_INFO = USB_CTL_CONST or (USB_GET_PARENT_HUB_INFO shl 2);
 IOCTL_INTERNAL_USB_GET_PORT_STATUS     = USB_KERNEL_CTL_CONST or (USB_GET_PORT_STATUS shl 2);
 IOCTL_INTERNAL_USB_RESET_PORT          = USB_KERNEL_CTL_CONST or (USB_RESET_PORT shl 2);
 IOCTL_INTERNAL_USB_GET_ROOTHUB_PDO     = USB_KERNEL_CTL_CONST or (USB_GET_ROOTHUB_PDO shl 2);
 IOCTL_INTERNAL_USB_SUBMIT_IDLE_NOTIFICATION = USB_KERNEL_CTL_CONST or (USB_IDLE_NOTIFICATION shl 2);
 IOCTL_INTERNAL_USB_SUBMIT_URB          = USB_KERNEL_CTL_CONST or (USB_SUBMIT_URB shl 2);

 IOCTL_USB_DIAG_IGNORE_HUBS_ON          = USB_CTL_CONST or (USB_DIAG_IGNORE_HUBS_ON shl 2);
 IOCTL_USB_DIAG_IGNORE_HUBS_OFF         = USB_CTL_CONST or (USB_DIAG_IGNORE_HUBS_OFF shl 2);
 IOCTL_USB_DIAGNOSTIC_MODE_OFF          = USB_CTL_CONST or (HCD_DIAGNOSTIC_MODE_OFF shl 2);
 IOCTL_USB_DIAGNOSTIC_MODE_ON           = USB_CTL_CONST or (HCD_DIAGNOSTIC_MODE_ON shl 2);
 IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION = USB_CTL_CONST or (USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION shl 2);
 IOCTL_USB_GET_HUB_CAPABILITIES         = USB_CTL_CONST or (USB_GET_HUB_CAPABILITIES shl 2);
 IOCTL_USB_GET_ROOT_HUB_NAME            = USB_CTL_CONST or (HCD_GET_ROOT_HUB_NAME shl 2);
 IOCTL_GET_HCD_DRIVERKEY_NAME           = USB_CTL_CONST or (HCD_GET_DRIVERKEY_NAME shl 2);
 IOCTL_USB_GET_NODE_INFORMATION         = USB_CTL_CONST or (USB_GET_NODE_INFORMATION shl 2);
 IOCTL_USB_GET_NODE_CONNECTION_INFORMATION = USB_CTL_CONST or (USB_GET_NODE_CONNECTION_INFORMATION shl 2);
 IOCTL_USB_GET_NODE_CONNECTION_ATTRIBUTES  = USB_CTL_CONST or (USB_GET_NODE_CONNECTION_ATTRIBUTES shl 2);
 IOCTL_USB_GET_NODE_CONNECTION_NAME     = USB_CTL_CONST or (USB_GET_NODE_CONNECTION_NAME shl 2);
 IOCTL_USB_GET_NODE_CONNECTION_DRIVERKEY_NAME = USB_CTL_CONST or (USB_GET_NODE_CONNECTION_DRIVERKEY_NAME shl 2);
 IOCTL_USB_HCD_DISABLE_PORT             = USB_CTL_CONST or (HCD_DISABLE_PORT shl 2);
 IOCTL_USB_HCD_ENABLE_PORT              = USB_CTL_CONST or (HCD_ENABLE_PORT shl 2);
 IOCTL_USB_HCD_GET_STATS_1              = USB_CTL_CONST or (HCD_GET_STATS_1 shl 2);
 IOCTL_USB_HCD_GET_STATS_2              = USB_CTL_CONST or (HCD_GET_STATS_2 shl 2);

