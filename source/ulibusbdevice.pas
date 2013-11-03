unit uLibUSBDevice;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, libusb, uUsb;

type

  { TLibUSBDevice }

  TLibUSBDevice = class(TUSBDevice)
  private
    FOpen : Boolean;
    DevHandle: PUSBDevHandle;
    procedure GetDeviceHandle;
  public
    function OpenDevice : Boolean;override;
    procedure CloseDevice;override;
    constructor Create(aDeviceHandle : string;aParent : TUSBGenericDevice;aController : TUSBController;aStatus : TUSBDeviceStatus);override;
    function SendControlMsg(Requesttype : LongInt;Request : LongInt;Value : LongInt;Index : LongInt;Bytes : pchar;Size : Longint;Timeout : LongInt) : LongInt;
    function LastError : string;
  end;

const
  USB_TYPE_STANDARD         = $00 Shl 5;
  USB_TYPE_CLASS            = $01 Shl 5;
  USB_TYPE_VENDOR           = $02 Shl 5;
  USB_TYPE_RESERVED         = $03 Shl 5;
  USB_RECIP_DEVICE          = $00;
  USB_RECIP_INTERFACE       = $01;
  USB_RECIP_ENDPOINT        = $02;
  USB_RECIP_OTHER           = $03;
  USB_ENDPOINT_IN  = $80;
  USB_ENDPOINT_OUT = $00;

implementation

{ TLibUSBDevice }

procedure TLibUSBDevice.GetDeviceHandle;
var
  busses: PUSBBus;
  usb_bus: PUSBBus;
  adev: PUSBDevice;
  aDevHandle: PUSBDevHandle;
  buff : array[0..511] of char;
begin
  if Assigned(FLibUSBDevHandle) then exit;
  usb_init();
  usb_find_busses();
  usb_find_devices();
  busses := usb_get_busses();
  usb_bus := busses;
  while Assigned(usb_bus) do
    begin
      adev := usb_bus^.devices;
      while Assigned(adev) do
        begin
          if  ((adev^.descriptor.idVendor = Self.VendorID)
          and  (adev^.descriptor.idProduct = Self.DeviceID)
              ) then
            begin
              aDevHandle := usb_open(adev);
              usb_get_string_simple(aDevHandle, aDev^.descriptor.iSerialNumber, buff, 512);
              usb_close(aDevHandle);
              if SerialNumber = buff then
                begin
                  FLibUSBDevHandle := aDev;
                  exit;
                end;
            end;
          adev := adev^.next;
        end;
      usb_bus := usb_bus^.next
    end;
end;

function TLibUSBDevice.OpenDevice: Boolean;
begin
  if FOpen then exit;
  GetDeviceHandle;
  if Assigned(FLibUSBDevHandle) then
    DevHandle := usb_open(FLibUSBDevHandle);
  FOpen := DevHandle <> nil;
  Result := FOpen;
end;

procedure TLibUSBDevice.CloseDevice;
begin
  if not FOpen then exit;
  usb_close(DevHandle);
  FOpen := false;
end;

constructor TLibUSBDevice.Create(aDeviceHandle: string;
  aParent: TUSBGenericDevice; aController: TUSBController;
  aStatus: TUSBDeviceStatus);
begin
  inherited Create(aDeviceHandle, aParent, aController, aStatus);
end;

function TLibUSBDevice.SendControlMsg(Requesttype: LongInt; Request: LongInt;
  Value: LongInt; Index: LongInt; Bytes: pchar; Size: Longint;Timeout : LongInt): LongInt;
begin
  Result := usb_control_msg(DevHandle,requesttype,request,value,index,bytes,size,timeout);
end;

function TLibUSBDevice.LastError: string;
begin
  result := usb_strerror;
end;

end.

