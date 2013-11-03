//------------------------------------------------------
// LibUSB V0.12.1
// Port for Lazarus / Delphi
// Tested with Lazarus V0.9.26, Delphi V7 on Windows
// ToDo: Test with Lazarus Linux
// Date 15.11.2008/Markus Mueller
//------------------------------------------------------


Interface

Const
{$IFDEF WIN32}
  External_library='libusb0.dll'; {Setup as you need}
{$ELSE}
  External_library='/usr/lib/libusb.so'; {Setup as you need}
{$ENDIF}

Const
// PATH_MAX from limits.h can't be used on Windows if the dll and
// import libraries are build/used by different compilers
   LIBUSB_PATH_MAX = 512;
// for DeviceClass
   USB_CLASS_PER_INTERFACE = 0;   
   USB_CLASS_AUDIO         = 1;
   USB_CLASS_COMM          = 2;
   USB_CLASS_HID           = 3;
   USB_CLASS_PTP           = 6;
   USB_CLASS_PRINTER       = 7;
   USB_CLASS_MASS_STORAGE  = 8;
   USB_CLASS_HUB           = 9;
   USB_CLASS_DATA          = 10;
   USB_CLASS_VENDOR_SPEC   = $ff;
// Descriptor types
   USB_DT_DEVICE    = $01;
   USB_DT_CONFIG    = $02;
   USB_DT_STRING    = $03;
   USB_DT_INTERFACE = $04;   
   USB_DT_ENDPOINT  = $05;
   USB_DT_HID       = $21;
   USB_DT_REPORT    = $22;
   USB_DT_PHYSICAL  = $23;
   USB_DT_HUB       = $29;
// Descriptor sizes per descriptor type
   USB_DT_DEVICE_SIZE         = 18;
   USB_DT_CONFIG_SIZE         = 9;
   USB_DT_INTERFACE_SIZE      = 9;
   USB_DT_ENDPOINT_SIZE       = 7;
// Audio extension
   USB_DT_ENDPOINT_AUDIO_SIZE = 9;   
   USB_DT_HUB_NONVAR_SIZE     = 7;
// ensure byte-packed structures

Type
  TArray0to0OfWord = Array[0..0] Of Word;

// All standard descriptors have these 2 fields in common
Type
   TUSBDescriptorHeader = Packed Record
        bLength         : Byte;
        bDescriptorType : Byte;
     End;

// String descriptor
   TUSBStringDescriptor = Packed Record
        bLength         : Byte;
        bDescriptorType : Byte;
        wData           : TArray0to0OfWord;
     End;

// HID descriptor
   TUSBHidDescriptor = Packed Record
        bLength         : Byte;
        bDescriptorType : Byte;
        bcdHID          : Word;
        bCountryCode    : Byte;
        bNumDescriptors : Byte;
     End;

// Endpoint descriptor
Const
   USB_MAXENDPOINTS = 32;   
// Extra descriptors
Type
   PUSBEndpointDescriptor = ^TUSBEndpointDescriptor;
   TUSBEndpointDescriptor = Packed Record
        bLength          : Byte;
        bDescriptorType  : Byte;
        bEndpointAddress : Byte;
        bmAttributes     : Byte;
        wMaxPacketSize   : Word;
        bInterval        : Byte;
        bRefresh         : Byte;
        bSynchAddress    : Byte;
        extra            : PByte;
        extralen         : LongInt;
     End;

// in bEndpointAddress
Const
   USB_ENDPOINT_ADDRESS_MASK     = $0f;
   USB_ENDPOINT_DIR_MASK         = $80;
// in bmAttributes
   USB_ENDPOINT_TYPE_MASK        = $03;
   USB_ENDPOINT_TYPE_CONTROL     = 0;
   USB_ENDPOINT_TYPE_ISOCHRONOUS = 1;   
   USB_ENDPOINT_TYPE_BULK        = 2;
   USB_ENDPOINT_TYPE_INTERRUPT   = 3;
// Interface descriptor
   USB_MAXINTERFACES             = 32;
// Extra descriptors
Type
   PUSBInterfaceDescriptor = ^TUSBInterfaceDescriptor;
   TUSBInterfaceDescriptor = Packed Record
        bLength            : Byte;
        bDescriptorType    : Byte;
        bInterfaceNumber   : Byte;
        bAlternateSetting  : Byte;
        bNumEndpoints      : Byte;
        bInterfaceClass    : Byte;
        bInterfaceSubClass : Byte;
        bInterfaceProtocol : Byte;
        iInterface         : Byte;
        endpoint           : PUSBEndpointDescriptor;
        extra              : PByte;
        extralen           : LongInt;
     End;

// Hard limit
Const
   USB_MAXALTSETTING = 128;   
Type
   PUSBInterface = ^TUSBInterface;
   TUSBInterface = Packed Record
        altsetting     : PUSBInterfaceDescriptor;
        num_altsetting : LongInt;
     End;

// Configuration descriptor information..
Const
   USB_MAXCONFIG = 8;   
// Extra descriptors
Type
   PUSBConfigDescriptor = ^TUSBConfigDescriptor;
   TUSBConfigDescriptor = Packed Record
        bLength             : Byte;
        bDescriptorType     : Byte;
        wTotalLength        : Word;
        bNumInterfaces      : Byte;
        bConfigurationValue : Byte;
        iConfiguration      : Byte;
        bmAttributes        : Byte;
        MaxPower            : Byte;
        intf                : PUSBInterface;
        extra               : PByte;
        extralen            : LongInt;
     End;

// Device descriptor
   PUSBDeviceDescriptor = ^TUSBDeviceDescriptor;
   TUSBDeviceDescriptor = Packed Record
        bLength            : Byte;
        bDescriptorType    : Byte;
        bcdUSB             : Word;
        bDeviceClass       : Byte;
        bDeviceSubClass    : Byte;
        bDeviceProtocol    : Byte;
        bMaxPacketSize0    : Byte;
        idVendor           : Word;
        idProduct          : Word;
        bcdDevice          : Word;
        iManufacturer      : Byte;
        iProduct           : Byte;
        iSerialNumber      : Byte;
        bNumConfigurations : Byte;
     End;

   TUSBCtrlSetup = Packed Record
        bRequestType : Byte;
        bRequest     : Byte;
        wValue       : Word;
        wIndex       : Word;
        wLength      : Word;
     End;

// Standard requests
Const
   USB_REQ_GET_STATUS        = $00;
   USB_REQ_CLEAR_FEATURE     = $01;
// 0x02 is reserved
   USB_REQ_SET_FEATURE       = $03;
// 0x04 is reserved
   USB_REQ_SET_ADDRESS       = $05;
   USB_REQ_GET_DESCRIPTOR    = $06;
   USB_REQ_SET_DESCRIPTOR    = $07;
   USB_REQ_GET_CONFIGURATION = $08;   
   USB_REQ_SET_CONFIGURATION = $09;   
   USB_REQ_GET_INTERFACE     = $0A;
   USB_REQ_SET_INTERFACE     = $0B;
   USB_REQ_SYNCH_FRAME       = $0C;
   USB_TYPE_STANDARD         = $00 Shl 5;
   USB_TYPE_CLASS            = $01 Shl 5;
   USB_TYPE_VENDOR           = $02 Shl 5;
   USB_TYPE_RESERVED         = $03 Shl 5;
   USB_RECIP_DEVICE          = $00;
   USB_RECIP_INTERFACE       = $01;
   USB_RECIP_ENDPOINT        = $02;
   USB_RECIP_OTHER           = $03;
// Various libusb API related stuff
   USB_ENDPOINT_IN  = $80;
   USB_ENDPOINT_OUT = $00;   
// Error codes
   USB_ERROR_BEGIN  = 500000;
  	USB_ENOSTAT = 4001;
  	USB_ENOPOOL = 4002;
  	USB_ENOFILE = 4003;
  	USB_EINVAL  = 22;


// Data types
// struct usb_device;
// struct usb_bus;
// Darwin support
Type
   PUSBBus = ^TUSBBus;
   PUSBDevice = ^TLibUSBDevice;
   TLibUSBDevice = Packed Record
        next         : PUSBDevice;
        prev         : PUSBDevice;
        filename     : Array[0..(LIBUSB_PATH_MAX)-1] Of Char;
        bus          : PUSBBus;
        descriptor   : TUSBDeviceDescriptor;
        config       : PUSBConfigDescriptor;
        dev          : Pointer;
        devnum       : Byte;
        num_children : Byte;
        children     : ^PUSBDevice;
     End;
   TUSBBus = Packed Record
        next     : PUSBBus;
        prev     : PUSBBus;
        dirname  : Array[0..(LIBUSB_PATH_MAX)-1] Of Char;
        devices  : PUSBDevice;
        location : Longword;
        root_dev : PUSBDevice;
     End;

// Version information, Windows specific
   PUSBVersion = ^TUSBVersion;
   TUSBVersion = Packed Record
        dll : Packed Record
             major : LongInt;
             minor : LongInt;
             micro : LongInt;
             nano  : LongInt;
          End;
        driver : Packed Record
             major : LongInt;
             minor : LongInt;
             micro : LongInt;
             nano  : LongInt;
          End;
     End;

   PUSBDevHandle = ^TUSBDevHandle;
   TUSBDevHandle = Packed Record
  		fd				: LongInt;
      bus			: PUSBBus;
      device		: PUSBDevice;
      config		: LongInt;
      intf			: LongInt;
      altsetting	: LongInt;
      impl_info	: Pointer; // Added by RMT so implementations can store other per-open-device data
   End;


// Variables

Var   LibUSBLoaded: Boolean;

// Function prototypes

Function usb_open(var dev:PUSBDevice):PUSBDevHandle;
Function usb_close(var dev:PUSBDevHandle):LongInt;
Function usb_get_string(var dev:PUSBDevHandle; index:LongInt; langid:LongInt; buf:pchar; buflen:Longword):LongInt;
Function usb_get_string_simple(var dev:PUSBDevHandle; index:LongInt; buf:pchar; buflen:Longword):LongInt;
Function usb_get_descriptor_by_endpoint(var dev:PUSBDevHandle; ep:LongInt; _type:Byte; index:Byte; var buf:pointer; size:LongInt):LongInt;
Function usb_get_descriptor(var dev:PUSBDevHandle; _type:Byte; index:Byte; var buf:pointer; size:LongInt):LongInt;
Function usb_bulk_write(var dev:PUSBDevHandle; ep:LongInt; bytes:pchar; size:LongInt; timeout:LongInt):LongInt;
Function usb_bulk_read(var dev:PUSBDevHandle; ep:LongInt; bytes:pchar; size:LongInt; timeout:LongInt):LongInt;
Function usb_interrupt_write(var dev:PUSBDevHandle; ep:LongInt; bytes:pchar; size:LongInt; timeout:LongInt):LongInt;
Function usb_interrupt_read(var dev:PUSBDevHandle; ep:LongInt; bytes:pchar; size:LongInt; timeout:LongInt):LongInt;
Function usb_control_msg(var dev:PUSBDevHandle; requesttype:LongInt; request:LongInt; value:LongInt; index:LongInt; bytes:pchar; size:LongInt; timeout:LongInt):LongInt;
Function usb_set_configuration(var dev:PUSBDevHandle; configuration:LongInt):LongInt;
Function usb_claim_interface(var dev:PUSBDevHandle; intf:LongInt):LongInt;
Function usb_release_interface(var dev:PUSBDevHandle; intf:LongInt):LongInt;
Function usb_set_altinterface(var dev:PUSBDevHandle; alternate:LongInt):LongInt;
Function usb_resetep(var dev:PUSBDevHandle; ep:Longword):LongInt;
Function usb_clear_halt(var dev:PUSBDevHandle; ep:Longword):LongInt;
Function usb_reset(var dev:PUSBDevHandle):LongInt;
Function usb_strerror():String;
Procedure usb_init();
Procedure usb_set_debug(level:LongInt);
Function usb_find_busses():LongInt;
Function usb_find_devices():LongInt;
Function usb_get_busses():PUSBBus;
Function usb_get_version():PUSBVersion;


// Not Implement, only for Windows
{const
   LIBUSB_HAS_INSTALL_SERVICE_NP = 0;
  int usb_install_service_np(void);
  void CALLBACK usb_install_service_np_rundll(HWND wnd, HINSTANCE instance,
                                              LPSTR cmd_line, int cmd_show);
  
  #define LIBUSB_HAS_UNINSTALL_SERVICE_NP 1
  int usb_uninstall_service_np(void);
  void CALLBACK usb_uninstall_service_np_rundll(HWND wnd, HINSTANCE instance,
                                                LPSTR cmd_line, int cmd_show);

  #define LIBUSB_HAS_INSTALL_DRIVER_NP 1
  int usb_install_driver_np(const char *inf_file);
  void CALLBACK usb_install_driver_np_rundll(HWND wnd, HINSTANCE instance,
                                             LPSTR cmd_line, int cmd_show);

  #define LIBUSB_HAS_TOUCH_INF_FILE_NP 1
  int usb_touch_inf_file_np(const char *inf_file);
  void CALLBACK usb_touch_inf_file_np_rundll(HWND wnd, HINSTANCE instance,
                                             LPSTR cmd_line, int cmd_show);

  #define LIBUSB_HAS_INSTALL_NEEDS_RESTART_NP 1
  int usb_install_needs_restart_np(void);

  int usb_isochronous_setup_async(PUSBDevHandle *dev, void **context,
                                  unsigned char ep, int pktsize);
  int usb_bulk_setup_async(PUSBDevHandle *dev, void **context,
                           unsigned char ep);
  int usb_interrupt_setup_async(PUSBDevHandle *dev, void **context,
                                unsigned char ep);

  int usb_submit_async(void *context, char *bytes, int size);
  int usb_reap_async(void *context, int timeout);
  int usb_reap_async_nocancel(void *context, int timeout);
  int usb_cancel_async(void *context);
  int usb_free_async(void **context);
   }


Implementation

{$IFDEF FPC}
Uses dynlibs;
{$ELSE}
Uses Windows;
{$ENDIF}

Type
{$IFDEF WIN32}
   T_usb_open = Function(var dev:TLibUSBDevice):PUSBDevHandle;Cdecl;
	T_usb_close = Function(var dev:TUSBDevHandle):LongInt;Cdecl;
	T_usb_get_string = Function(var dev:TUSBDevHandle; index:LongInt; langid:LongInt; buf:pchar; buflen:Longword):LongInt;Cdecl;
	T_usb_get_string_simple = Function(var dev:TUSBDevHandle; index:LongInt; buf:pchar; buflen:Longword):LongInt;Cdecl;
	T_usb_get_descriptor_by_endpoint = Function(var udev:TUSBDevHandle; ep:LongInt; _type:Byte; index:Byte; var buf:pointer; size:LongInt):LongInt;Cdecl;
	T_usb_get_descriptor = Function(var udev:TUSBDevHandle; _type:Byte; index:Byte; var buf:pointer; size:LongInt):LongInt;Cdecl;
	T_usb_bulk_write = Function(var dev:TUSBDevHandle; ep:LongInt; bytes:pchar; size:LongInt; timeout:LongInt):LongInt;Cdecl;
	T_usb_bulk_read = Function(var dev:TUSBDevHandle; ep:LongInt; bytes:pchar; size:LongInt; timeout:LongInt):LongInt;Cdecl;
	T_usb_interrupt_write = Function(var dev:TUSBDevHandle; ep:LongInt; bytes:pchar; size:LongInt; timeout:LongInt):LongInt;Cdecl;
	T_usb_interrupt_read = Function(var dev:TUSBDevHandle; ep:LongInt; bytes:pchar; size:LongInt; timeout:LongInt):LongInt;Cdecl;
	T_usb_control_msg = Function(var dev:TUSBDevHandle; requesttype:LongInt; request:LongInt; value:LongInt; index:LongInt; bytes:pchar; size:LongInt; timeout:LongInt):LongInt;Cdecl;
	T_usb_set_configuration = Function(var dev:TUSBDevHandle; configuration:LongInt):LongInt;Cdecl;
	T_usb_claim_interface = Function(var dev:TUSBDevHandle; intf:LongInt):LongInt;Cdecl;
	T_usb_release_interface = Function(var dev:TUSBDevHandle; intf:LongInt):LongInt;Cdecl;
	T_usb_set_altinterface = Function(var dev:TUSBDevHandle; alternate:LongInt):LongInt;Cdecl;
	T_usb_resetep = Function(var dev:TUSBDevHandle; ep:Longword):LongInt;Cdecl;
	T_usb_clear_halt = Function(var dev:TUSBDevHandle; ep:Longword):LongInt;Cdecl;
	T_usb_reset = Function(var dev:TUSBDevHandle):LongInt;Cdecl;
   T_usb_device = Function(dev:TUSBDevHandle):PUSBDevice; Cdecl;
{$ELSE}
   T_usb_open = Function(var dev:PUSBDevice):PUSBDevHandle;Cdecl;
	T_usb_close = Function(var dev:PUSBDevHandle):LongInt;Cdecl;
	T_usb_get_string = Function(var dev:PUSBDevHandle; index:LongInt; langid:LongInt; buf:pchar; buflen:Longword):LongInt;Cdecl;
	T_usb_get_string_simple = Function(var dev:PUSBDevHandle; index:LongInt; buf:pchar; buflen:Longword):LongInt;Cdecl;
	T_usb_get_descriptor_by_endpoint = Function(var udev:PUSBDevHandle; ep:LongInt; _type:Byte; index:Byte; var buf:pointer; size:LongInt):LongInt;Cdecl;
	T_usb_get_descriptor = Function(var udev:PUSBDevHandle; _type:Byte; index:Byte; var buf:pointer; size:LongInt):LongInt;Cdecl;
	T_usb_bulk_write = Function(var dev:PUSBDevHandle; ep:LongInt; bytes:pchar; size:LongInt; timeout:LongInt):LongInt;Cdecl;
	T_usb_bulk_read = Function(var dev:PUSBDevHandle; ep:LongInt; bytes:pchar; size:LongInt; timeout:LongInt):LongInt;Cdecl;
	T_usb_interrupt_write = Function(var dev:PUSBDevHandle; ep:LongInt; bytes:pchar; size:LongInt; timeout:LongInt):LongInt;Cdecl;
	T_usb_interrupt_read = Function(var dev:PUSBDevHandle; ep:LongInt; bytes:pchar; size:LongInt; timeout:LongInt):LongInt;Cdecl;
	T_usb_control_msg = Function(var dev:PUSBDevHandle; requesttype:LongInt; request:LongInt; value:LongInt; index:LongInt; bytes:pchar; size:LongInt; timeout:LongInt):LongInt;Cdecl;
	T_usb_set_configuration = Function(var dev:PUSBDevHandle; configuration:LongInt):LongInt;Cdecl;
	T_usb_claim_interface = Function(var dev:PUSBDevHandle; intf:LongInt):LongInt;Cdecl;
	T_usb_release_interface = Function(var dev:PUSBDevHandle; intf:LongInt):LongInt;Cdecl;
	T_usb_set_altinterface = Function(var dev:PUSBDevHandle; alternate:LongInt):LongInt;Cdecl;
	T_usb_resetep = Function(var dev:PUSBDevHandle; ep:Longword):LongInt;Cdecl;
	T_usb_clear_halt = Function(var dev:PUSBDevHandle; ep:Longword):LongInt;Cdecl;
	T_usb_reset = Function(var dev:PUSBDevHandle):LongInt;Cdecl;
   T_usb_device = Function(dev:PUSBDevHandle):PUSBDevice; Cdecl;
{$ENDIF}
	T_usb_strerror = Function():pchar;Cdecl;
	T_usb_init = Procedure();Cdecl;
	T_usb_set_debug = Procedure(level:LongInt);Cdecl;
	T_usb_find_busses = Function():LongInt;Cdecl;
	T_usb_find_devices = Function():LongInt;Cdecl;
	T_usb_get_busses = Function():PUSBBus;Cdecl;
	T_usb_get_version = Function():PUSBVersion;Cdecl;

Var
   _usb_open: T_usb_open;
	_usb_close: T_usb_close;
	_usb_get_string: T_usb_get_string;
	_usb_get_string_simple: T_usb_get_string_simple;
	_usb_get_descriptor_by_endpoint: T_usb_get_descriptor_by_endpoint;
	_usb_get_descriptor: T_usb_get_descriptor;
	_usb_bulk_write: T_usb_bulk_write;
	_usb_bulk_read: T_usb_bulk_read;
	_usb_interrupt_write: T_usb_interrupt_write;
	_usb_interrupt_read: T_usb_interrupt_read;
	_usb_control_msg: T_usb_control_msg;
	_usb_set_configuration: T_usb_set_configuration;
	_usb_claim_interface: T_usb_claim_interface;
	_usb_release_interface: T_usb_release_interface;
	_usb_set_altinterface: T_usb_set_altinterface;
	_usb_resetep: T_usb_resetep;
	_usb_clear_halt: T_usb_clear_halt;
	_usb_reset: T_usb_reset;
   _usb_device: T_usb_device;
	_usb_strerror: T_usb_strerror;
	_usb_init: T_usb_init;
	_usb_set_debug: T_usb_set_debug;
	_usb_find_busses: T_usb_find_busses;
	_usb_find_devices: T_usb_find_devices;
	_usb_get_busses: T_usb_get_busses;
	_usb_get_version: T_usb_get_version;


Function usb_open(var dev:PUSBDevice):PUSBDevHandle;
Begin
   If LibUSBLoaded And Assigned(_usb_Open) Then
   Begin
{$IFDEF WIN32}
	   Result := _usb_open(dev^);
{$ELSE}
	   Result := _usb_open(dev);
{$ENDIF}
   End Else Result := Nil;
End;

Function usb_close(var dev:PUSBDevHandle):LongInt;
Begin
   If LibUSBLoaded And Assigned(_usb_close) Then
   Begin
{$IFDEF WIN32}
	   Result := _usb_close(dev^);
{$ELSE}
	   Result := _usb_close(dev);
{$ENDIF}
   End Else Result := -USB_ENOFILE;
End;

Function usb_get_string(var dev:PUSBDevHandle; index:LongInt; langid:LongInt; buf:pchar; buflen:Longword):LongInt;
Begin
   If LibUSBLoaded And Assigned(_usb_get_string) Then
   Begin
{$IFDEF WIN32}
	   Result := _usb_get_string(dev^, index, langid, buf, buflen);
{$ELSE}
	   Result := _usb_get_string(dev, index, langid, buf, buflen);
{$ENDIF}
   End Else Result := -USB_ENOFILE;
End;

Function usb_get_string_simple(var dev:PUSBDevHandle; index:LongInt; buf:pchar; buflen:Longword):LongInt;
Begin
   If LibUSBLoaded And Assigned(_usb_get_string_simple) Then
   Begin
{$IFDEF WIN32}
	   Result := _usb_get_string_simple(dev^, index, buf, buflen);
{$ELSE}
	   Result := _usb_get_string_simple(dev, index, buf, buflen);
{$ENDIF}
   End Else Result := -USB_ENOFILE;
End;

Function usb_get_descriptor_by_endpoint(var dev:PUSBDevHandle; ep:LongInt; _type:Byte; index:Byte; var buf:pointer; size:LongInt):LongInt;
Begin
   If LibUSBLoaded And Assigned(_usb_get_descriptor_by_endpoint) Then
   Begin
{$IFDEF WIN32}
	   Result := _usb_get_descriptor_by_endpoint(dev^, ep, _type, index, buf, size);
{$ELSE}
	   Result := _usb_get_descriptor_by_endpoint(dev, ep, _type, index, buf, size);
{$ENDIF}
   End Else Result := -USB_ENOFILE;
End;

Function usb_get_descriptor(var dev:PUSBDevHandle; _type:Byte; index:Byte; var buf:pointer; size:LongInt):LongInt;
Begin
   If LibUSBLoaded And Assigned(_usb_get_descriptor) Then
   Begin
{$IFDEF WIN32}
	   Result := _usb_get_descriptor(dev^, _type, index, buf, size);
{$ELSE}
	   Result := _usb_get_descriptor(dev, _type, index, buf, size);
{$ENDIF}
   End Else Result := -USB_ENOFILE;
End;

Function usb_bulk_write(var dev:PUSBDevHandle; ep:LongInt; bytes:pchar; size:LongInt; timeout:LongInt):LongInt;
Begin
   If LibUSBLoaded And Assigned(_usb_bulk_write) Then
   Begin
{$IFDEF WIN32}
	   Result := _usb_bulk_write(dev^, ep, bytes, size, timeout);
{$ELSE}
	   Result := _usb_bulk_write(dev, ep, bytes, size, timeout);
{$ENDIF}
   End Else Result := -USB_ENOFILE;
End;

Function usb_bulk_read(var dev:PUSBDevHandle; ep:LongInt; bytes:pchar; size:LongInt; timeout:LongInt):LongInt;
Begin
   If LibUSBLoaded And Assigned(_usb_bulk_read) Then
   Begin
{$IFDEF WIN32}
	   Result := _usb_bulk_read(dev^, ep, bytes, size, timeout);
{$ELSE}
	   Result := _usb_bulk_read(dev, ep, bytes, size, timeout);
{$ENDIF}
   End Else Result := -USB_ENOFILE;
End;

Function usb_interrupt_write(var dev:PUSBDevHandle; ep:LongInt; bytes:pchar; size:LongInt; timeout:LongInt):LongInt;
Begin
   If LibUSBLoaded And Assigned(_usb_interrupt_write) Then
   Begin
{$IFDEF WIN32}
	   Result := _usb_interrupt_write(dev^, ep, bytes, size, timeout);;
{$ELSE}
	   Result := _usb_interrupt_write(dev, ep, bytes, size, timeout);
{$ENDIF}
   End Else Result := -USB_ENOFILE;
End;

Function usb_interrupt_read(var dev:PUSBDevHandle; ep:LongInt; bytes:pchar; size:LongInt; timeout:LongInt):LongInt;
Begin
   If LibUSBLoaded And Assigned(_usb_interrupt_read) Then
   Begin
{$IFDEF WIN32}
	   Result := _usb_interrupt_read(dev^, ep, bytes, size, timeout);
{$ELSE}
	   Result := _usb_interrupt_read(dev, ep, bytes, size, timeout);
{$ENDIF}
   End Else Result := -USB_ENOFILE;
End;

Function usb_control_msg(var dev:PUSBDevHandle; requesttype:LongInt; request:LongInt; value:LongInt; index:LongInt;
           bytes:pchar; size:LongInt; timeout:LongInt):LongInt;
Begin
   If LibUSBLoaded And Assigned(_usb_control_msg) Then
   Begin
{$IFDEF WIN32}
	   Result := _usb_control_msg(dev^, requesttype, request, value, index, bytes, size, timeout);
{$ELSE}
	   Result := _usb_control_msg(dev, requesttype, request, value, index, bytes, size, timeout);
{$ENDIF}
   End Else Result := -USB_ENOFILE;
End;

Function usb_set_configuration(var dev:PUSBDevHandle; configuration:LongInt):LongInt;
Begin
   If LibUSBLoaded And Assigned(_usb_set_configuration) Then
   Begin
{$IFDEF WIN32}
	   Result := _usb_set_configuration(dev^, configuration);
{$ELSE}
	   Result := _usb_set_configuration(dev, configuration);
{$ENDIF}
   End Else Result := -USB_ENOFILE;
End;

Function usb_claim_interface(var dev:PUSBDevHandle; intf:LongInt):LongInt;
Begin
   If LibUSBLoaded And Assigned(_usb_claim_interface) Then
   Begin
{$IFDEF WIN32}
	   Result := _usb_claim_interface(dev^, intf);
{$ELSE}
	   Result := _usb_claim_interface(dev, intf);
{$ENDIF}
   End Else Result := -USB_ENOFILE;
End;

Function usb_release_interface(var dev:PUSBDevHandle; intf:LongInt):LongInt;
Begin
   If LibUSBLoaded And Assigned(_usb_release_interface) Then
   Begin
{$IFDEF WIN32}
	   Result := _usb_release_interface(dev^, intf);
{$ELSE}
	   Result := _usb_release_interface(dev, intf);
{$ENDIF}
   End Else Result := -USB_ENOFILE;
End;

Function usb_set_altinterface(var dev:PUSBDevHandle; alternate:LongInt):LongInt;
Begin
   If LibUSBLoaded And Assigned(_usb_set_altinterface) Then
   Begin
{$IFDEF WIN32}
	   Result := _usb_set_altinterface(dev^, alternate);
{$ELSE}
	   Result := _usb_set_altinterface(dev, alternate);
{$ENDIF}
   End Else Result := -USB_ENOFILE;
End;

Function usb_resetep(var dev:PUSBDevHandle; ep:Longword):LongInt;
Begin
   If LibUSBLoaded And Assigned(_usb_resetep) Then
   Begin
{$IFDEF WIN32}
	   Result := _usb_resetep(dev^, ep);
{$ELSE}
	   Result := _usb_resetep(dev, ep);
{$ENDIF}
   End Else Result := -USB_ENOFILE;
End;

Function usb_clear_halt(var dev:PUSBDevHandle; ep:Longword):LongInt;
Begin
   If LibUSBLoaded And Assigned(_usb_clear_halt) Then
   Begin
{$IFDEF WIN32}
	   Result := _usb_clear_halt(dev^, ep);
{$ELSE}
	   Result := _usb_clear_halt(dev, ep);
{$ENDIF}
   End Else Result := -USB_ENOFILE;
End;

Function usb_reset(var dev:PUSBDevHandle):LongInt;
Begin
   If LibUSBLoaded And Assigned(_usb_reset) Then
   Begin
{$IFDEF WIN32}
	   Result := _usb_reset(dev^);
{$ELSE}
	   Result := _usb_reset(dev);
{$ENDIF}
   End Else Result := -USB_ENOFILE;
End;

Function usb_strerror():String;
Begin
   If LibUSBLoaded And Assigned(_usb_strerror) Then
	   Result := _usb_strerror()
   Else Result := 'Error LibUSB not loaded!';
End;

Procedure usb_init();
Begin
   If LibUSBLoaded And Assigned(_usb_init) Then
	   _usb_init();
End;

Procedure usb_set_debug(level:LongInt);
Begin
   If LibUSBLoaded And Assigned(_usb_set_debug) Then
	   _usb_set_debug(level);
End;

Function usb_find_busses():LongInt;
Begin
   If LibUSBLoaded And Assigned(_usb_find_busses) Then
	   Result := _usb_find_busses()
   Else Result := -USB_ENOFILE;
End;

Function usb_find_devices():LongInt;
Begin
   If LibUSBLoaded And Assigned(_usb_find_devices) Then
	   Result := _usb_find_devices()
   Else Result := -USB_ENOFILE;
End;

Function usb_device(var dev:PUSBDevHandle):PUSBDevice;
Begin
   If LibUSBLoaded And Assigned(_usb_device) Then
   Begin
{$IFDEF WIN32}
	   Result := _usb_device(dev^);
{$ELSE}
	   Result := _usb_device(dev);
{$ENDIF}
   End Else Result := Nil;
End;

Function usb_get_busses():PUSBBus;
Begin
   If LibUSBLoaded And Assigned(_usb_get_busses) Then
	   Result := _usb_get_busses()
	Else Result := Nil;
End;

Function usb_get_version():PUSBVersion;
Const V: TUSBVersion = (dll:(major:(-1);minor:(-1);micro:(-1);nano:(-1));driver:(major:(-1);minor:(-1);micro:(-1);nano:(-1)));
Begin
{$IFDEF WIN32}
   If LibUSBLoaded And Assigned(_usb_get_version) Then
	   Result := _usb_get_version()
	Else
{$ENDIF}
 		Result := @V;
End;


Var DLLHandle: THandle;

Procedure UnloadDLL;
Begin
  	If LibUSBLoaded Then
  	Begin
    	If DLLHandle >= 32 Then
      	FreeLibrary(DLLHandle);
    	LibUSBLoaded := False;
  	End;
End;

Procedure LoadDLL;
Begin
   If LibUSBLoaded Then
   	Exit;
   DLLHandle := LoadLibrary(External_library); // .DLL or .so File
   If DLLHandle >= 32 Then
   Begin
      LibUSBLoaded := True;
		_usb_open := T_usb_open(GetProcAddress(DLLHandle, 'usb_open'));
   	_usb_close := T_usb_close(GetProcAddress(DLLHandle, 'usb_close'));
   	_usb_get_string := T_usb_get_string(GetProcAddress(DLLHandle, 'usb_get_string'));
   	_usb_get_string_simple := T_usb_get_string_simple(GetProcAddress(DLLHandle, 'usb_get_string_simple'));
   	_usb_get_descriptor_by_endpoint := T_usb_get_descriptor_by_endpoint(GetProcAddress(DLLHandle, 'usb_get_descriptor_by_endpoint'));
   	_usb_get_descriptor := T_usb_get_descriptor(GetProcAddress(DLLHandle, 'usb_get_descriptor'));
   	_usb_bulk_write := T_usb_bulk_write(GetProcAddress(DLLHandle, 'usb_bulk_write'));
   	_usb_bulk_read := T_usb_bulk_read(GetProcAddress(DLLHandle, 'usb_bulk_read'));
   	_usb_interrupt_write := T_usb_interrupt_write(GetProcAddress(DLLHandle, 'usb_interrupt_write'));
   	_usb_interrupt_read := T_usb_interrupt_read(GetProcAddress(DLLHandle, 'usb_interrupt_read'));
   	_usb_control_msg := T_usb_control_msg(GetProcAddress(DLLHandle, 'usb_control_msg'));
   	_usb_set_configuration := T_usb_set_configuration(GetProcAddress(DLLHandle, 'usb_set_configuration'));
   	_usb_claim_interface := T_usb_claim_interface(GetProcAddress(DLLHandle, 'usb_claim_interface'));
   	_usb_release_interface := T_usb_release_interface(GetProcAddress(DLLHandle, 'usb_release_interface'));
   	_usb_set_altinterface := T_usb_set_altinterface(GetProcAddress(DLLHandle, 'usb_set_altinterface'));
   	_usb_resetep := T_usb_resetep(GetProcAddress(DLLHandle, 'usb_resetep'));
   	_usb_clear_halt := T_usb_clear_halt(GetProcAddress(DLLHandle, 'usb_clear_halt'));
   	_usb_reset := T_usb_reset(GetProcAddress(DLLHandle, 'usb_reset'));
   	_usb_strerror := T_usb_strerror(GetProcAddress(DLLHandle, 'usb_strerror'));
   	_usb_init := T_usb_init(GetProcAddress(DLLHandle, 'usb_init'));
   	_usb_set_debug := T_usb_set_debug(GetProcAddress(DLLHandle, 'usb_set_debug'));
   	_usb_find_busses := T_usb_find_busses(GetProcAddress(DLLHandle, 'usb_find_busses'));
   	_usb_find_devices := T_usb_find_devices(GetProcAddress(DLLHandle, 'usb_find_devices'));
      _usb_device := T_usb_device(GetProcAddress(DLLHandle, 'usb_device'));
   	_usb_get_busses := T_usb_get_busses(GetProcAddress(DLLHandle, 'usb_get_busses'));
{$IFDEF WIN32}
   	_usb_get_version := T_usb_get_version(GetProcAddress(DLLHandle, 'usb_get_version'));
{$ELSE}
      _usb_get_version := Nil;
{$ENDIF}
      usb_init();
  		usb_set_debug(0);
  	End Else Begin
    	LibUSBLoaded := False;
  	End;
End;


Initialization
  	LoadDLL;

Finalization
  	UnloadDLL;

End.
