
{ TUSBControllerAsync }

procedure TUSBControllerAsync.DoCheck;
begin
  if (usb_find_busses() > 0)
  or (usb_find_devices() > 0) then
    FParent.Enumerate;
end;

constructor TUSBControllerAsync.Create(aParent : TUSBController);
begin
  FParent := aParent;
  inherited Create(False);
end;

procedure TUSBControllerAsync.Execute;
begin
  while not Terminated do
    begin
      Synchronize(@DoCheck);
      sleep(2000);
    end;
end;

{ TUSBController }

procedure TUSBController.RefreshList;
var
  Bus : PUSBBus;
  aHostController: TUSBHostController = nil;
  i: Integer;
  aRootHub: TUSBHub;

  procedure EnumHubDevices(Hub : TUSBHub);
  var
    aDevice,
    aChildren: PUSBDevice;
    a,i : Integer;
    buff : array[0..511] of char;
    DevHandle: PUSBDevHandle;
    Found: Boolean;
    DeviceClass: TUSBDeviceClass;
  begin
    aDevice := Bus^.devices;
    a := 0;
    for i := 0 to Hub.Count-1 do
      TUSBGenericDevice(Hub.Items[i]).Tag := 0;
    while Assigned(aDevice) do
      begin
        Found := False;
        for i := 0 to Hub.Count-1 do
          if TUSBGenericDevice(Hub.Items[i]).Path = aDevice^.filename then
            begin
              Found := True;
              TUSBGenericDevice(Hub.Items[i]).Tag := 1;
            end;
        if not Found then
          begin
            DeviceClass := GetUSBDeviceClass(aDevice^.descriptor.idVendor,aDevice^.descriptor.idProduct);
            a := Hub.Add(DeviceClass.Create(aDevice^.filename,Hub,Self,dsConnected));
            TUSBDevice(Hub.Items[a]).Tag := 1;
            TUSBDevice(Hub.Items[a]).FDeviceID := aDevice^.descriptor.idProduct;
            TUSBDevice(Hub.Items[a]).FVendorID := aDevice^.descriptor.idVendor;
            TUSBDevice(Hub.Items[a]).FLibUSBDevHandle := aDevice;
            DevHandle := usb_open(aDevice);
            if Assigned(DevHandle) then
              begin
                usb_get_string_simple(DevHandle, aDevice^.descriptor.iManufacturer, buff, 512);
                TUSBDevice(Hub.Items[a]).FVendor := buff;
                usb_get_string_simple(DevHandle, aDevice^.descriptor.iProduct, buff, 512);
                TUSBDevice(Hub.Items[a]).FDeviceDescription := buff;
                usb_get_string_simple(DevHandle, aDevice^.descriptor.iSerialNumber, buff, 512);
                TUSBDevice(Hub.Items[a]).FSerial := buff;
                usb_close(DevHandle);
              end;
            if Assigned(OnUSBArrival) then
              OnUSBArrival(TUSBGenericDevice(Hub.Items[a]));
          end;
        inc(a);
        aDevice := aDevice^.next;
      end;
    i := 0;
    while i < Hub.Count do
      begin
        if TUSBGenericDevice(Hub.Items[i]).Tag = 0 then
          begin
            TUSBGenericDevice(Hub.Items[i]).Free;
            Hub.Delete(i);
          end
        else inc(i);
      end;
  end;

begin
  usb_find_busses();
  usb_find_devices();
  bus := usb_get_busses;
  while Assigned(Bus) do
    begin
      aHostController := nil;
      for i := 0 to FBusList.Count-1 do
        if TUSBHostController(FBusList[i]).Path = Bus^.dirname then
          begin
            TUSBHostController(FBusList[i]).Tag := 1;
            aHostController := TUSBHostController(FBusList[i]);
          end;
      if not Assigned(aHostController) then
        begin
          aHostController := TUSBHostController.Create(Bus^.dirname);
          aHostController.Tag := 1;
          FBusList.Add(aHostController);
        end;
      if Assigned(aHostController) then
        begin
          aRootHub := nil;
          if (aHostController.Count > 0) then
            aRootHub := TUSBHub(aHostController.Devices[0])
          else if Assigned(Bus^.devices) then
            begin
              aRootHub := TUSBHub.Create(Bus^.dirname+'/'+Bus^.devices^.filename,nil,Self,dsConnected);
              aHostController.Add(aRootHub);
            end;
          if Assigned(aRootHub) then
            EnumHubDevices(aRootHub);
        end;
      Bus := Bus^.next;
    end;
end;

constructor TUSBController.Create(AOwner: TComponent);
begin
  inherited Create(AOwner);
  FBusList := TList.Create;
  FDeviceList := TList.Create;
  usb_init();
  Async := TUSBControllerAsync.Create(Self);
end;

destructor TUSBController.Destroy;
begin
  Async.Free;
  FBusList.Free;
  FDeviceList.Free;
  inherited Destroy;
end;


{ TUSBDevice }

function TUSBDevice.OpenDevice: Boolean;
begin

end;

procedure TUSBDevice.CloseDevice;
begin

end;

constructor TUSBDevice.Create(aDeviceHandle: string;
  aParent: TUSBGenericDevice; aController: TUSBController;
  aStatus: TUSBDeviceStatus);
begin
  inherited Create(aDeviceHandle,aParent,aController,aStatus);
end;

destructor TUSBDevice.Destroy;
begin
  if Assigned(Controller.OnUSBRemove) then
    Controller.OnUSBRemove(Self);
  inherited Destroy;
end;
