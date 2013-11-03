
function TUSBController.USBRegister: Boolean;
var
  dbi: DEV_BROADCAST_DEVICEINTERFACE;
  Size: Integer;
  r: Pointer;
begin
  Result := False;
  Size := SizeOf(DEV_BROADCAST_DEVICEINTERFACE);
  ZeroMemory(@dbi, Size);
  dbi.dbcc_size := Size;
  dbi.dbcc_devicetype := DBT_DEVTYP_DEVICEINTERFACE;
  dbi.dbcc_reserved := 0;
  dbi.dbcc_classguid  := GUID_DEVINTERFACE_USB_DEVICE;
  dbi.dbcc_name := 0;

  r := RegisterDeviceNotification(FWindowHandle, @dbi, DEVICE_NOTIFY_WINDOW_HANDLE);
  if Assigned(r) then Result := True;
end;

constructor TUSBController.Create(AOwner: TComponent);
begin
  inherited Create(AOwner);
  FWindowHandle := LCLIntf.AllocateHWnd(@WndProc);
  FDeviceList := TList.Create;
  FBusList := TList.Create;
  USBRegister;
end;

destructor TUSBController.Destroy;
var
  i: Integer;
begin
  for i := 0 to FBusList.Count-1 do
    TUSBHostController(FBusList[i]).Free;
  FDeviceList.Free;
  FBusList.Free;
  LCLIntf.DeallocateHWnd(FWindowHandle);
  inherited Destroy;
end;

procedure TUSBController.WndProc(var Msg: TMessage);
begin
  if (Msg.Msg = WM_DEVICECHANGE) then
  begin
    try
      WMDeviceChange(Msg);
    except
      Application.HandleException(Self);
    end;
  end
  else
    Msg.Result := DefWindowProc(FWindowHandle, Msg.Msg, Msg.wParam, Msg.lParam);
end;

procedure TUSBController.WMDeviceChange(var Msg: TMessage);
var
  devType: Integer;
  Datos: PDevBroadcastHdr;
begin
  if (Msg.wParam = DBT_DEVICEARRIVAL) or (Msg.wParam = DBT_DEVICEREMOVECOMPLETE) then
    begin
      Datos := PDevBroadcastHdr(Msg.lParam);
      devType := Datos^.dbch_devicetype;
      if devType = DBT_DEVTYP_DEVICEINTERFACE then
        begin
          sleep(150);
          RefreshList;
        end;
    end;
end;

function UnicodeStr(uni :TUnicodeName) :string;
var
 s :string;
 i :word;
begin
 s:='';
 for i:=0 to TUnicodeNameMaxLong - 1 do
  if uni[i]<>#0 then s:=s + uni[i] else break;
 UnicodeStr := s;
end;

function UnicodeStrLen(len :DWord; uni :TUnicodeName) :string;
var
 s :string;
 i :DWord;
begin
 s:='';
 for i:=0 to len - 1 do
  if uni[i]<>#0 then s:=s + uni[i] else break;
 UnicodeStrLen := s;
end;

function USB_GetLangID(hHub :THandle; PortIndex :DWord) :DWord;
var
 BytesReturned :DWord;
 Packet :TDESCRIPTOR_REQUEST;
begin
  fillchar(Packet, sizeof(Packet), 0);
  with Packet do begin
    ConnectionIndex := PortIndex;
    SetupPacket.bmRequest := $80;
    SetupPacket.bRequest := USB_REQUEST_GET_DESCRIPTOR;
    SetupPacket.wValueHi := USB_STRING_DESCRIPTOR_TYPE;
    SetupPacket.wLength := 4;
  end;
  if DeviceIoControl(hHub,
                     IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION,
                     @Packet, sizeof(Packet),
	             @Packet, sizeof(Packet),
                     BytesReturned, nil)
  then
     USB_GetLangID := Packet.Data[2] or (Packet.Data[3] shl 8)
  else
     USB_GetLangID := 0;
end;

function USB_GetDescrStr(hHub :THandle; PortIndex :DWord; LangID :Word; Index :Byte) :string;
var
 BytesReturned :DWord;
 Packet :TDESCRIPTOR_REQUEST;
 p :PUSB_STRING_DESCRIPTOR;
begin
  p:=@Packet.Data;
  fillchar(Packet, sizeof(Packet), 0);
  with Packet do begin
    ConnectionIndex := PortIndex;
    SetupPacket.bmRequest := $80;
    SetupPacket.bRequest := USB_REQUEST_GET_DESCRIPTOR;
    SetupPacket.wValueHi := USB_STRING_DESCRIPTOR_TYPE;
    SetupPacket.wValueLo := Index;
    SetupPacket.wIndex := LangID;
    SetupPacket.wLength := 255;
  end;
  if DeviceIoControl(hHub,
                     IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION,
                     @Packet, sizeof(Packet),
	             @Packet, sizeof(Packet),
                     BytesReturned, nil)
  then
     USB_GetDescrStr := UnicodeStrLen(p^.bLength, p^.bString)
  else
     USB_GetDescrStr := 'error';
end;

procedure TUSBController.RefreshList;
var
  HostNum: Integer;
  DevName: String;
  hHost: LongWord;
  aHostController: TUSBHostController;
  i: Integer;
  UnicodeName : USB_HUB_NAME;
  BytesReturned :DWord;
  aRootHub: TUSBHub;
  pc: PChar;

  procedure EnumHubDevices(Hub : TUSBHub);
  var
    hHub: LongWord;
    hHub2: LongWord;
    NodeInfo :TNODE_INFORMATION;
    NodeInfo2 :TNODE_INFORMATION;
    ConInfo :TNODE_CONNECTION_INFORMATION;
    ConnectedHub :USB_NODE_CONNECTION_NAME;
    BytesReturned :DWord;
    a: Integer;
    b: Integer;
    DeviceClass: TUSBDeviceClass;
  begin
    hHub := CreateFile(pchar(Hub.Path),GENERIC_WRITE,FILE_SHARE_WRITE,@SA, OPEN_EXISTING, 0, 0);
    if hHub <> INVALID_HANDLE_VALUE then
      begin
        if DeviceIoControl(hHub,IOCTL_USB_GET_NODE_INFORMATION,@NodeInfo, sizeof(NodeInfo),@NodeInfo, sizeof(NodeInfo),BytesReturned, nil) then
          begin
            Hub.FBusPowered := NodeInfo.HubIsBusPowered;
            while Hub.Count > NodeInfo.HubDescriptor.bNumberOfPorts do //Is this possible (nessessary) ??
              begin
                if Hub.Items[Hub.Count-1] <> nil then
                  begin
                    FDeviceList.Remove(Hub.Items[Hub.Count-1]);
                    TUSBgenericDevice(Hub.Items[Hub.Count-1]).Free;
                  end;
                Hub.Delete(Hub.Count-1);
              end;
            while Hub.Count < NodeInfo.HubDescriptor.bNumberOfPorts do
              Hub.Add(nil);
            for a := 0 to NodeInfo.HubDescriptor.bNumberOfPorts-1 do
              begin
                ConInfo.ConnectionIndex := a+1;
                if DeviceIoControl(hHub,IOCTL_USB_GET_NODE_CONNECTION_INFORMATION,@ConInfo, sizeof(ConInfo),@ConInfo, sizeof(ConInfo),BytesReturned, nil) then
                  begin
                    if (ConInfo.ConnectionStatus[0] = 0) then
                      begin
                        //no device connected, do disconnect
                        if (Hub.Items[a] <> nil) then
                          begin
                            FDeviceList.Remove(Hub.Items[a]);
                            TUSBGenericDevice(Hub.Items[a]).Free;
                          end;
                        Hub.Items[a] := nil;
                      end
                    else
                      begin
                        ConnectedHub.ConnectionIndex:=a+1;
                        ConnectedHub.ActualLength:=sizeof(ConnectedHub);
                        if ConInfo.DeviceIsHub then
                          begin
                            if DeviceIoControl(hHub,IOCTL_USB_GET_NODE_CONNECTION_NAME,@ConnectedHub, sizeof(ConnectedHub),@ConnectedHub, sizeof(ConnectedHub),BytesReturned, nil) then
                              devname := UnicodeStrLen(ConnectedHub.ActualLength,ConnectedHub.NodeName);
                            if (not Assigned(Hub.Items[a]))
                            or (TUSBGenericDevice(Hub.Items[a]).Path <> '\\.\'+devname) then
                              begin
                                if Assigned(Hub.Items[a]) and (TUSBGenericDevice(Hub.Items[a]).Path <> '\\.\'+devname) then
                                  TUSBGenericDevice(Hub.Items[a]).Free;
                                Hub.Items[a] := TUSBHub.Create('\\.\'+devname,Hub,Self,TUSBDeviceStatus(ConInfo.ConnectionStatus[0]));
                                hHub2 := CreateFile(pchar('\\.\'+devname),GENERIC_WRITE,FILE_SHARE_WRITE,@SA, OPEN_EXISTING, 0, 0);
                                if hHub2 <> INVALID_HANDLE_VALUE then
                                  begin
                                    if DeviceIoControl(hHub2,IOCTL_USB_GET_NODE_INFORMATION,@NodeInfo2, sizeof(NodeInfo2),@NodeInfo2, sizeof(NodeInfo2),BytesReturned, nil) then
                                      begin
                                        for b := 0 to NodeInfo2.HubDescriptor.bNumberOfPorts-1 do
                                          TUSBHub(Hub.Items[a]).Add(nil);
                                      end;
                                  end;
                                if Assigned(OnUSBArrival) then
                                  OnUSBArrival(TUSBGenericDevice(Hub.Items[a]));
                              end;
                            EnumHubDevices(TUSBHub(Hub.Items[a]));
                          end
                        else
                          begin
                            if DeviceIoControl(hHub,IOCTL_USB_GET_NODE_CONNECTION_DRIVERKEY_NAME,@ConnectedHub, sizeof(ConnectedHub),@ConnectedHub, sizeof(ConnectedHub),BytesReturned, nil) then
                              devname := UnicodeStrLen(ConnectedHub.ActualLength,ConnectedHub.NodeName);
                            if (not Assigned(Hub.Items[a]))
                            or (TUSBDevice(Hub.Items[a]).Driver <> devname) then
                              begin
                                if Assigned(Hub.Items[a]) and (TUSBDevice(Hub.Items[a]).Driver <> devname) then
                                  begin
                                    FDeviceList.Remove(Hub.Items[a]);
                                    TUSBGenericDevice(Hub.Items[a]).Free;
                                  end;
                                DeviceClass := GetUSBDeviceClass(ConInfo.DeviceDescriptor.idVendor[1] << 8+ConInfo.DeviceDescriptor.idVendor[0],ConInfo.DeviceDescriptor.idProduct[1] << 8+ConInfo.DeviceDescriptor.idProduct[0]);
                                Hub.Items[a] := DeviceClass.Create(devname,Hub,Self,TUSBDeviceStatus(ConInfo.ConnectionStatus[0]));
                                FDeviceList.Add(Hub.Items[a]);
                                if TUSBDevice(Hub.Items[a]).Status = dsConnected then
                                  begin
                                    with TUSBDevice(Hub.Items[a]) do
                                      begin
                                        FVendor := USB_GetDescrStr(hHub,a+1,USB_GetLangID(hHub,a+1),ConInfo.DeviceDescriptor.iManufacturer);
                                        FDeviceDescription := USB_GetDescrStr(hHub,a+1,USB_GetLangID(hHub,a+1),ConInfo.DeviceDescriptor.iProduct);
                                        FSerial := USB_GetDescrStr(hHub,a+1,USB_GetLangID(hHub,a+1),ConInfo.DeviceDescriptor.iSerialNumber);
                                        FDeviceID := ConInfo.DeviceDescriptor.idProduct[1] << 8+ConInfo.DeviceDescriptor.idProduct[0];
                                        FVendorID := ConInfo.DeviceDescriptor.idVendor[1] << 8+ConInfo.DeviceDescriptor.idVendor[0];
                                      end;
                                  end;
                                if Assigned(OnUSBArrival) then
                                  OnUSBArrival(TUSBGenericDevice(Hub.Items[a]));
                              end;
                          end;
                      end;
                  end;
              end;
          end;
        CloseHandle(hHub);
      end;
  end;
begin
  for i := 0 to FBusList.Count-1 do
    TUSBHostController(FBusList[i]).Tag := 0;
  for HostNum := 0 to 9 do
    begin
      DevName := '\\.\HCD' + char(HostNum+ord('0'));
      hHost := CreateFile(PChar(devname),GENERIC_WRITE,FILE_SHARE_WRITE,@SA, OPEN_EXISTING, 0, 0);
      if hHost <> INVALID_HANDLE_VALUE then
        begin
          aHostController := nil;
          for i := 0 to FBusList.Count-1 do
            if TUSBHostController(FBusList[i]).Path = DevName then
              begin
                TUSBHostController(FBusList[i]).Tag := 1;
                aHostController := TUSBHostController(FBusList[i]);
                break;
              end;
          if not Assigned(aHostController) then
            begin
              aHostController := TUSBHostController.Create(DevName);
              aHostController.Tag := 1;
              FBusList.Add(aHostController);
            end;
          if DeviceIoControl(hHost,IOCTL_USB_GET_ROOT_HUB_NAME,@UnicodeName, sizeof(UnicodeName),@UnicodeName, sizeof(UnicodeName), BytesReturned, nil) then
            begin
              devname := UnicodeStrLen(UnicodeName.ActualLength,UnicodeName.HubName);
              if (aHostController.Count > 0) and (aHostController.Devices[0].Path = '\\.\'+devname) then
                aRootHub := TUSBHub(aHostController.Devices[0])
              else
                begin
                  aRootHub := TUSBHub.Create('\\.\'+devname,nil,Self,dsConnected);
                  aHostController.Add(aRootHub);
                end;
              EnumHubDevices(aRootHub)
            end;
          CloseHandle(hHost);
        end;
    end;
end;

function TUSBDevice.OpenDevice : Boolean;
begin
  if FFileHandle = INVALID_HANDLE_VALUE then // if not already opened
    begin
      FFileHandle := CreateFile(PChar(FPath), GENERIC_READ or GENERIC_WRITE,
        FILE_SHARE_READ or FILE_SHARE_WRITE, nil, OPEN_EXISTING, 0, 0);
      FHasReadWriteAccess := FFileHandle <> INVALID_HANDLE_VALUE;
      if not HasReadWriteAccess then
        FFileHandle := CreateFile(PChar(FPath), 0, FILE_SHARE_READ or FILE_SHARE_WRITE, nil, OPEN_EXISTING, 0, 0);
    end;
  Result := FileHandle <> INVALID_HANDLE_VALUE;
end;

procedure TUSBDevice.CloseDevice;
begin
  if FFileHandle <> INVALID_HANDLE_VALUE then
    CloseHandle(FFileHandle);
  FFileHandle := INVALID_HANDLE_VALUE;
end;

function CM_Get_Child(var dnChildInst: DWord;dnDevInst: DWord;ulFlags: LongWord): DWord; stdcall; external 'CFGMGR32';

constructor TUSBDevice.Create(aDeviceHandle: string;aParent : TUSBGenericDevice;aController : TUSBController;aStatus : TUSBDeviceStatus);
var
  aBytesReturned: DWORD;
  Success: Boolean;
  ChildInst : DWORD;
const
  cHidGuid: TGUID = '{4d1e55b2-f16f-11cf-88cb-001111000030}';
  cRawUSBGuid: TGUID = '{a5dcbf10-6530-11d2-901f-00c04fb951ed}';
  cComPortGuid : TGUID = '{86E0D1E0-8089-11D0-9CE4-08003E301F73}';
//  cComPortGuid : TGUID = '{4D36E978-E325-11CE-BFC1-08002BE10318}';

  function GetRegistryPropertyString(PnPHandle: HDEVINFO; const DevData: PSP_DEVINFO_DATA; Prop: DWORD): shortstring;
  var
    RegDataType:   DWORD;
    Buffer:        array [0..256] of Char;
  begin
    aBytesReturned := 0;
    RegDataType   := 0;
    Buffer[0]     := #0;
    SetupDiGetDeviceRegistryPropertyA(PnPHandle, DevData, Prop, @RegDataType, PBYTE(@Buffer[0]), SizeOf(Buffer), @aBytesReturned);
    Result := PChar(Buffer);
  end;

  function GetDevicePathFromDriverKey(DriverKey : string;GUID : TGUID) : string;
  var
    PnPHandle: HDEVINFO;
    DevData: SP_DEVINFO_DATA;
    DeviceInterfaceData: SP_DEVICE_INTERFACE_DATA;
    FunctionClassDeviceData: ^SP_DEVICE_INTERFACE_DETAIL_DATA_A;
    Success: LongBool;
    Devn: Integer;
    tmp: String;
    Flags: LongWord;
  begin
    PnPHandle := SetupDiGetClassDevsA(@Guid, nil, 0,DIGCF_PRESENT or DIGCF_DEVICEINTERFACE);
    if PnPHandle = Pointer(INVALID_HANDLE_VALUE) then
      Exit;
    Devn := 0;
    repeat
      DeviceInterfaceData.cbSize := SizeOf(SP_DEVICE_INTERFACE_DATA);
      Success := SetupDiEnumDeviceInterfaces(PnPHandle, nil, @Guid, Devn, @DeviceInterfaceData);
      if Success then
        begin
          DevData.cbSize := SizeOf(DevData);
          aBytesReturned  := 0;
          SetupDiGetDeviceInterfaceDetailA(PnPHandle, @DeviceInterfaceData, nil, 0, @aBytesReturned, @DevData);
          if (aBytesReturned <> 0) and (GetLastError = ERROR_INSUFFICIENT_BUFFER) then
            begin
              FunctionClassDeviceData := AllocMem(aBytesReturned);
              FunctionClassDeviceData^.cbSize := 5;
              if SetupDiGetDeviceInterfaceDetailA(PnPHandle, @DeviceInterfaceData, FunctionClassDeviceData, aBytesReturned, @aBytesReturned, @DevData) then
                begin
                  if GetRegistryPropertyString(PnPHandle,@DevData,SPDRP_DRIVER) = DriverKey then
                    begin
                      if CM_Get_Child(ChildInst,DevData.DevInst,0) <> {CR_SUCCESS}0 then
                        ChildInst := 0;
                      FUSBSerialPort := GetRegistryPropertyString(PnPHandle,@DevData,SPDRP_FRIENDLYNAME);
                      if pos('COM',FUSBSerialPort) > 0 then
                        begin
                          ChildInst := 0;
                          if pos('(COM',FUSBSerialPort) > 0 then
                            begin
                              FUSBSerialPort := copy(FUSBSerialPort,pos('(COM',FUSBSerialPort)+1,length(FUSBSerialPort));
                              FUSBSerialPort := copy(FUSBSerialPort,0,pos(')',FUSBSerialPort)-1);
                            end;
                        end;
                      Result := PChar(@FunctionClassDeviceData^.DevicePath);
                    end;
                end;
              FreeMem(FunctionClassDeviceData);
            end;
          Inc(Devn);
        end;
    until not Success;
    SetupDiDestroyDeviceInfoList(PnPHandle);
  end;

  function GetComPortFromChild(GUID : TGUID) : string;
  var
    PnPHandle: HDEVINFO;
    DevData: SP_DEVINFO_DATA;
    DeviceInterfaceData: SP_DEVICE_INTERFACE_DATA;
    FunctionClassDeviceData: ^SP_DEVICE_INTERFACE_DETAIL_DATA_A;
    Success: LongBool;
    Devn: Integer;
    tmp: String;
    Flags: LongWord;
  begin
    PnPHandle := SetupDiGetClassDevsA(@Guid, nil, 0,DIGCF_PRESENT or DIGCF_DEVICEINTERFACE);
    if PnPHandle = Pointer(INVALID_HANDLE_VALUE) then
      Exit;
    Devn := 0;
    repeat
      DeviceInterfaceData.cbSize := SizeOf(SP_DEVICE_INTERFACE_DATA);
      Success := SetupDiEnumDeviceInterfaces(PnPHandle, nil, @Guid, Devn, @DeviceInterfaceData);
      if Success then
        begin
          DevData.cbSize := SizeOf(DevData);
          aBytesReturned  := 0;
          SetupDiGetDeviceInterfaceDetailA(PnPHandle, @DeviceInterfaceData, nil, 0, @aBytesReturned, @DevData);
          if (aBytesReturned <> 0) and (GetLastError = ERROR_INSUFFICIENT_BUFFER) then
            begin
              FunctionClassDeviceData := AllocMem(aBytesReturned);
              FunctionClassDeviceData^.cbSize := 5;
              if SetupDiGetDeviceInterfaceDetailA(PnPHandle, @DeviceInterfaceData, FunctionClassDeviceData, aBytesReturned, @aBytesReturned, @DevData) then
                begin
                  if ChildInst = DevData.DevInst then
                    Result := GetRegistryPropertyString(PnPHandle,@DevData,SPDRP_FRIENDLYNAME);
                end;
              FreeMem(FunctionClassDeviceData);
            end;
          Inc(Devn);
        end;
    until not Success;
    SetupDiDestroyDeviceInfoList(PnPHandle);
  end;

begin
  inherited Create(aDeviceHandle,aParent,aController,aStatus);
  FFileHandle := INVALID_HANDLE_VALUE;
  FDriver := aDeviceHandle;
  FPath := GetDevicePathFromDriverKey(aDeviceHandle,cRawUSBGuid);
  if ChildInst <> 0 then
    FUSBSerialPort := GetComPortFromChild(cComPortGuid);
  if not OpenDevice then exit;
  CloseDevice;
end;

destructor TUSBDevice.Destroy;
begin
  if Assigned(Controller.OnUSBRemove) then
    Controller.OnUSBRemove(Self);
  inherited Destroy;
end;
