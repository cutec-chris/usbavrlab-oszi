unit uUsb;

interface

{$H+}

{$IFDEF WINDOWS}
  {$DEFINE WINNATIVE}
{$ENDIF}

uses
  LCLIntf, LCLProc, SysUtils, Classes, Forms, Controls
  {$IFDEF WINNATIVE}
  ,Windows,Messages,JwaWinUser,SetupApi,Dialogs
  {$ELSE}
  ,libusb
  {$ENDIF}
  ;

type
  TUSBController = class;
  TUSBDevice = class;

  TUSBHubEntryTyp = (heUnconnected,heHub,heDevice);

  TUSBDeviceStatus = (dsNotConnected=0,dsConnected=1,dsEnumerationFailed=2,dsDeviceFailure=3,dsOvercurrent=4,dsNotEnougthPower=5);

  { TUSBGenericDevice }

  TUSBGenericDevice = class(TList)
  private
    FController: TUSBController;
    FDeviceDescription: string;
    FDeviceID: dword;
    FParent: TUSBGenericDevice;
    FPath: string;
    FSerial: string;
    FStatus: TUSBDeviceStatus;
    FTag: Integer;
    FVendor: string;
    FVendorID: dword;
  public
    property DeviceID : dword read FDeviceID;
    property VendorID : dword read FVendorID;
    property DeviceDescription : string read FDeviceDescription;
    property Vendor : string read FVendor;
    property SerialNumber : string read FSerial;
    property Path : string read FPath;
    property Parent : TUSBGenericDevice read FParent;
    property Controller : TUSBController read FController;
    property Tag : Integer read FTag write FTag;
    property Status : TUSBDeviceStatus read FStatus;
    constructor Create(aPath : string;aParent : TUSBGenericDevice;aController : TUSBController;aStatus : TUSBDeviceStatus);
  end;

  { TUSBHostController }

  TUSBHostController = class(TList)
  private
    FPath: string;
    FTag: Integer;
    function GetHubEntry(idx : Integer): TUSBGenericDevice;
  public
    property Devices[idx : Integer] : TUSBgenericDevice read GetHubEntry;
    property Path : string read FPath;
    property Tag : Integer read FTag write FTag;
    constructor Create(aPath : string);
  end;

  { TUSBHub }

  TUSBHub = class(TUSBGenericDevice)
  private
    FBusPowered: Boolean;
    function GetHubEntry(idx : Integer): TUSBGenericDevice;
  public
    property Devices[idx : Integer] : TUSBGenericDevice read GetHubEntry;
    property BusPowered : Boolean read FBusPowered;
    destructor Destroy;override;
  end;

  { TUSBDevice }

  TUSBDevice = class(TUSBGenericDevice)
  private
    FAlternativeDescription: string;
    FFileHandle: THandle;
    FHasReadWriteAccess: Boolean;
    FUSBSerialPort: string;
  protected
    FLibUSBDevHandle : Pointer;
    FDriver: string;
    function OpenDevice : Boolean;virtual;
    procedure CloseDevice;virtual;
  public
    property Driver: string read FDriver;
    property FileHandle : THandle read FFileHandle;
    property USBSerialPort : string read FUSBSerialPort;
    property HasReadWriteAccess : Boolean read FHasReadWriteAccess;
    property AlternativeDescription : string read FAlternativeDescription;
    property Tag;
    constructor Create(aDeviceHandle : string;aParent : TUSBGenericDevice;aController : TUSBController;aStatus : TUSBDeviceStatus);virtual;
    destructor Destroy;override;
  end;

  {$IFNDEF WINNATIVE}

  { TUSBControllerAsync }

  TUSBControllerAsync = class(TThread)
  private
    FParent : TUSBController;
    procedure DoCheck;
  public
    constructor Create(aParent : TUSBController);
    procedure Execute;override;
  end;

  {$ENDIF}

  TUSBDeviceClass = class of tUSBdevice;
  TGetDeviceClassEvent = procedure(VendorID,DeviceID : word;var aClass : TUSBDeviceClass) of object;

  { TUSBController }

  TUSBController = class(TComponent)
  private
    FOnGetDeviceClass: TGetDeviceClassEvent;
    FOnUSBArrival: TNotifyEvent;
    FOnUSBRemove: TNotifyEvent;
    FDeviceList : TList;
    FBusList : TList;
  {$IFDEF WINNATIVE}
    FWindowHandle: HWND;
    procedure WndProc(var Msg: TMessage);
    function USBRegister: Boolean;
  {$ELSE}
    Async : TUSBControllerAsync;
  {$ENDIF}
    function GetHostController(idx : Integer): TUsbHostController;
    function GetCount: Integer;
    function GetUSBDeviceClass(VendorID,DeviceID : word) : TUSBDeviceClass;
  protected
  {$IFDEF WINNATIVE}
    procedure WMDeviceChange(var Msg: TMessage); dynamic;
  {$ENDIF}
    procedure RefreshList;
  public
    constructor Create(AOwner: TComponent); override;
    destructor Destroy; override;
    procedure Enumerate;
    property HostController[idx : Integer] : TUsbHostController read GetHostController;
  published
    property Count : Integer read GetCount;
    property OnUSBArrival: TNotifyEvent read FOnUSBArrival write FOnUSBArrival;
    property OnUSBRemove: TNotifyEvent read FOnUSBRemove write FOnUSBRemove;
    property OnGetDeviceClass: TGetDeviceClassEvent read FOnGetDeviceClass write FOnGetDeviceClass;
  end;

  {$IFDEF WINNATIVE}
  {$I uusbwintypes.inc}
  {$ELSE}
  {$I uusblibusbtypes.inc}
  {$ENDIF}

var
  USBDeviceStatusStings : array[0..5] of string = ('No device connected', 'Device connected', 'Device FAILED enumeration', 'Device general FAILURE', 'Device caused overcurrent', 'Not enough power for device');


implementation

  {$IFDEF WINNATIVE}
  {$I uusbwinimplementation.inc}
  {$ELSE}
  {$I uusblibusbimplementation.inc}
  {$ENDIF}

procedure TUSBController.Enumerate;
begin
  RefreshList;
end;

{ TUSBHostController }

function TUSBHostController.GetHubEntry(idx : Integer): TUSBGenericDevice;
begin
  Result := nil;
  if idx < Count then
    Result := TUSBgenericDevice(Items[idx]);
end;

function TUSBController.GetCount: Integer;
begin
  Result := FBusList.Count;
end;

function TUSBController.GetHostController(idx : Integer): TUsbHostController;
begin
  Result := nil;
  if idx < FBusList.Count then
    Result := TUSBHostController(FBusList[idx]);
end;

constructor TUSBHostController.Create(aPath: string);
begin
  inherited Create;
  FPath := aPath;
end;

{ TUSBGenericDevice }

constructor TUSBGenericDevice.Create(aPath : string;aParent : TUSBGenericDevice;aController : TUSBController;aStatus : TUSBDeviceStatus);
begin
  inherited Create;
  FPath := aPath;
  FParent := aParent;
  FController := aController;
  FStatus := aStatus;
end;

function TUSBController.GetUSBDeviceClass(VendorID, DeviceID: word
  ): TUSBDeviceClass;
begin
  Result := TUSBDevice;
  if Assigned(FOnGetDeviceClass) then
    FOnGetDeviceClass(VendorID,DeviceID,Result);
end;

{ TUSBHub }

function TUSBHub.GetHubEntry(idx : Integer): TUSBGenericDevice;
begin
  Result := nil;
  if idx < Count then
    Result := TUSBgenericDevice(Items[idx]);
end;

destructor TUSBHub.Destroy;
var
  i: Integer;
begin
  For i := 0 to Count-1 do
    if Assigned(Items[i]) then
      TUSBGenericDevice(Items[i]).Free;
  if Assigned(Controller.OnUSBRemove) then
    Controller.OnUSBRemove(Self);
  inherited Destroy;
end;

initialization
{$IFDEF WINNATIVE}
 SA.nLength := sizeof(SA);  {for Win2000}
 SA.lpSecurityDescriptor := NIL;
 SA.bInheritHandle := false;
{$ENDIF}

end.

