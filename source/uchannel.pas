{ <Beschreibung>

  Copyright (C) 2013 c. Ulrich info@cu-tec.de

  This source is free software; you can redistribute it and/or modify it under
  the terms of the GNU General Public License as published by the Free
  Software Foundation; either version 2 of the License, or (at your option)
  any later version.

  This code is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
  details.

  A copy of the GNU General Public License is available on the World Wide Web
  at <http://www.gnu.org/copyleft/gpl.html>. You can also obtain it by writing
  to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
  MA 02111-1307, USA.
}
unit uChannel;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, FileUtil, LResources, Forms, ExtCtrls, StdCtrls, ComCtrls,
  uLibUsbDevice, Graphics, ColorBox, Dialogs;

type
  TChannelInfo = record
    Valid : byte; //A5
    Name : string[20];
    VperDiv : real;
    msperDiv : real;
    offset : Integer;
  end;

  TfChannel = class;

  { TChannelTimer }

  TChannelTimer = class(TThread)
  private
    Parent : TFChannel;
    data : array[0..63] of byte;
    res: Integer;
    procedure UpdateMainData;
    procedure UpdateMainState;
    procedure GetData;
    function GetState : byte;
    procedure StartSampling;
  public
    constructor Create(aParent : TfChannel);
    procedure Execute;override;
  end;

  ArrayData = array [0..0] of byte;
  PArrayData = ^ArrayData;

  ScreenData = array [0..0] of TPoint;
  PScreenData = ^ScreenData;

  { TfChannel }

  TfChannel = class(TFrame)
    cbColor: TColorButton;
    eChannelName: TEdit;
    Label1: TLabel;
    lOffset: TLabel;
    RadioButton1: TRadioButton;
    RadioButton2: TRadioButton;
    tbOffset: TTrackBar;
    Timer1: TTimer;
    udVoltage: TUpDown;
    procedure eChannelNameChange(Sender: TObject);
    procedure tbOffsetChange(Sender: TObject);
    procedure Timer1Timer(Sender: TObject);
    procedure udVoltageClick(Sender: TObject; Button: TUDBtnType);
  private
    FChannelName: string;
    FScreen: TBitmap;
    KSps : Integer;                                   //actual KSps
    FBufferlength: Double;                            //Bufferlength in s
    SamplesPerScreen: Extended;                       //Samples visible on Screen
    SamplesPerPoint: Extended;                        //Samples per Pixel
    FactorY: Extended;
    DoUpdate : Boolean;
    procedure SetChannelName(const AValue: string);
    procedure SetScreen(const AValue: TBitmap);
    procedure ChannelStateChanged(NewState : byte);
    { private declarations }
  public
    { public declarations }
    Device :  TLibUSBDevice;
    Channel_data : array of byte;
    ChannelScreen : PScreenData;
    ScreenPoints: LongInt;
    ChannelTimer : TChannelTimer;
    VperDiv : real;
    Offset: Integer;
    procedure UpdateChannel;
    procedure UpdateChannelData;
    procedure Start;
    procedure Stop;
    property DisplayScreen : TBitmap read FScreen write SetScreen;
    function DrawChannelData(aCanvas : TCanvas;Pos : Integer) : Integer;
    property ChannelName : string read FChannelName write SetChannelName;
    constructor Create(AOwner: TComponent); override;
    destructor Destroy;override;
    procedure WriteByte(Addr : byte;data : Byte);
    function Readbyte(Addr : byte) : Byte;
    function SetkSps(NewKSps : Integer) : Integer;
    function SetTriggerLevel(Level : real) : Integer;
    procedure Init;
    procedure Save;
    procedure UpdateChannelBuffer(newlength : real);
  end;

const
 FUNC_READ_BUFFER = 1;
 FUNC_SET_PARAMS  = 2;
 FUNC_STOP        = 3;
 FUNC_START       = 4;
 FUNC_READ_EEP    = 6;
 FUNC_WRITE_EEP   = 5;
 FUNC_SET_TRIGGER = 7;
 FUNC_SET_TRIGGER_LEVEL = 8;
 FUNC_SET_SAMPLERATE = 9;
 FUNC_GET_STATE	= 10;
 STATE_STOPPED    = 3;
 STATE_RUNNING    = 4;
 STATE_AUTO_STOPPED = 5;

implementation

uses uMain;

{ TfChannel }

procedure TfChannel.tbOffsetChange(Sender: TObject);
begin
  UpdateChannelData;
  UpdateChannel;
  fMain.ChannelUpdated(Self);
end;

procedure TfChannel.Timer1Timer(Sender: TObject);
begin
  UpdateChannel;
end;

procedure TfChannel.udVoltageClick(Sender: TObject; Button: TUDBtnType);
begin
  VperDiv := udVoltage.Position/10;
  UpdateChannelData;
end;

procedure TfChannel.eChannelNameChange(Sender: TObject);
begin
  ChannelName := eChannelName.Text;
  fMain.pbScreen.Invalidate;
end;

procedure TfChannel.SetChannelName(const AValue: string);
begin
  if FChannelName=AValue then exit;
  FChannelName:=AValue;
  if eChannelName.Text <> aValue then
    eChannelName.Text:=AValue;
end;

procedure TfChannel.SetScreen(const AValue: TBitmap);
begin
  if FScreen = aValue then exit;
  FScreen := AValue;
end;

procedure TfChannel.ChannelStateChanged(NewState: byte);
begin
  case NewState of
  STATE_STOPPED:fMain.bStop.Down:=True;
  STATE_RUNNING,STATE_AUTO_STOPPED:fMain.bStart.Down:=True;
  end;
end;

procedure TfChannel.UpdateChannel;
var
  i: Integer;
  DataStart: Extended;
  X: Extended;
begin
  if not DoUpdate then exit;
  DataStart := length(Channel_data)-1;
  X := FScreen.Width;
  i := ScreenPoints-1;
  while (i >= 0) and (DataStart >= 0) do
    begin
      try
      ChannelScreen^[i].x := Trunc(X);
      ChannelScreen^[i].y := Offset+round(-Channel_data[trunc(DataStart)]*FactorY);
      if SamplesPerPoint > 1 then
        begin
          X := X-1;
          DataStart := DataStart-SamplesPerPoint;
        end
      else
        begin
          X := X-(1/SamplesPerPoint);
          DataStart := DataStart-1;
        end;
      except
        Showmessage('Bkla');
      end;
      dec(i);
    end;
  if i > -1 then
    begin
      for i := i+1 downto 0 do
        ChannelScreen[i] := ChannelScreen[i+1];
    end;
  fMain.ChannelUpdated(Self);
  DoUpdate := False;
end;

procedure TfChannel.UpdateChannelData;
var
  i: Integer;
begin
  if (not Assigned(FScreen)) or (kSps = 0) or (FScreen.Width = 0) then exit;
  SamplesPerScreen := (fMain.MsperDiv / 1000);
  SamplesPerScreen := SamplesPerScreen * (kSps*1000);
  SamplesPerScreen := SamplesPerScreen * fMain.DividerX;
  SamplesPerPoint := SamplesPerScreen / FScreen.Width;
  ScreenPoints := trunc(SamplesPerScreen)+2;
  if SamplesPerPoint = 0 then exit;
  if ScreenPoints > (length(Channel_data)/SamplesPerPoint) then
    ScreenPoints := trunc(length(Channel_data)/SamplesPerPoint);
  if ScreenPoints = 0 then exit;
  if Assigned(ChannelScreen) then
    begin
      SysFreeMem(ChannelScreen);
      ChannelScreen := SysGetMem(ScreenPoints*sizeof(TPoint));
    end
  else
    ChannelScreen := SysGetMem(ScreenPoints*sizeof(TPoint));
  for i := 0 to ScreenPoints-1 do
    begin
      ChannelScreen^[i].x:=0;
      ChannelScreen^[i].y:=0;
    end;
  FactorY := ((FScreen.Height div 8)/255)*10.9;//1V / div
  FactorY := FactorY / VperDiv;
  Offset := (FScreen.Height div 2);
  Offset := Offset+round(((FScreen.Height*tbOffset.Position)/tbOffset.Max))-(FScreen.Height div 2);
end;

procedure TfChannel.Start;
var
  res: LongInt;
begin
  if not Assigned(ChannelTimer) then
    ChannelTimer := TChannelTimer.Create(Self);
  if Device.OpenDevice then
    begin
      if (fMain.cbTriggerChannel.ItemIndex > -1) and (fMain.lbprogrammer.Items.Objects[fMain.cbTriggerChannel.ItemIndex] = Self) then
        begin
          if fMain.sbTriggerContinuus.Down then
            res := Device.SendControlMsg(USB_TYPE_VENDOR or USB_RECIP_DEVICE or USB_ENDPOINT_IN,FUNC_START, 0, 0,nil, 0, 5000)
          else if fMain.sbTriggerFalling.Down then
            res := Device.SendControlMsg(USB_TYPE_VENDOR or USB_RECIP_DEVICE or USB_ENDPOINT_IN,FUNC_SET_TRIGGER, 0, 0,nil, 0, 5000)
          else if fMain.sbTriggerRaising.Down then
            res := Device.SendControlMsg(USB_TYPE_VENDOR or USB_RECIP_DEVICE or USB_ENDPOINT_IN,FUNC_SET_TRIGGER, 0, 0,nil, 0, 5000)
        end
      else
        begin
          res := Device.SendControlMsg(USB_TYPE_VENDOR or USB_RECIP_DEVICE or USB_ENDPOINT_IN,FUNC_START, 0, 0,nil, 0, 5000)
        end;
      Device.CloseDevice;
    end;
end;

procedure TfChannel.Stop;
var
  res: LongInt;
  state : byte;
begin
  if not Assigned(ChannelTimer) then exit;
  ChannelTimer.FreeOnTerminate:=True;
  ChannelTimer.Terminate;
  ChannelTimer := nil;
  sleep(50);
  if Device.OpenDevice then
    begin
      res := Device.SendControlMsg(USB_TYPE_VENDOR or USB_RECIP_DEVICE or USB_ENDPOINT_IN,FUNC_STOP, 0, 0,nil, 0, 50);
      sleep(10);
      res := Device.SendControlMsg(USB_TYPE_VENDOR or USB_RECIP_DEVICE or USB_ENDPOINT_IN,FUNC_STOP, 0, 0,nil, 0, 50);
      res := Device.SendControlMsg(USB_TYPE_VENDOR or USB_RECIP_DEVICE or USB_ENDPOINT_IN,FUNC_GET_STATE, 0, 0,@state, 1, 50);
      Device.CloseDevice;
      ChannelStateChanged(state);
    end;
end;

function TfChannel.DrawChannelData(aCanvas: TCanvas; Pos: Integer): Integer;
var
  aVoltage: String;
  aTime: String;
begin
  aVoltage := FloatToStr(VperDiv)+' V/div';
  aCanvas.Font.Size:=7;
  Result := aCanvas.TextExtent(FChannelName).cy;
  aCanvas.TextOut(5,Pos,FChannelName);
  aCanvas.TextOut(5,Pos+Result,aVoltage);
  Result := Result+aCanvas.TextExtent(aVoltage).cy;
  aCanvas.TextOut(5,Pos+Result,aTime);
  Result := Result+aCanvas.TextExtent(aTime).cy;
end;

constructor TfChannel.Create(AOwner: TComponent);
begin
  inherited Create(AOwner);
  Setlength(Channel_data,1000);
  ChannelScreen := nil;
  VperDiv := 1;
  ChannelTimer := nil;
  UpdateChannelData;
end;

destructor TfChannel.Destroy;
begin
  Save;
  if Assigned(ChannelTimer) then ChannelTimer.Terminate;
  if Assigned(ChannelScreen) then
    SysFreeMem(ChannelScreen);
  inherited Destroy;
end;

procedure TfChannel.WriteByte(Addr: byte; data: Byte);
var
  res: LongInt;
begin
  if not Assigned(Device) then exit;
  if Device.OpenDevice then
    begin
      res := Device.SendControlMsg(USB_TYPE_VENDOR or USB_RECIP_DEVICE or USB_ENDPOINT_IN,FUNC_WRITE_EEP, data << 8 + Addr, 0,nil, 0, 100);
      Device.CloseDevice;
    end;
end;

function TfChannel.Readbyte(Addr: byte): Byte;
var
  res: LongInt;
  Data : byte;
begin
  if not Assigned(Device) then exit;
  if Device.OpenDevice then
    begin
      res := Device.SendControlMsg(USB_TYPE_VENDOR or USB_RECIP_DEVICE or USB_ENDPOINT_IN,FUNC_READ_EEP, Addr, 0,@data, 1, 100);
      Device.CloseDevice;
      Result := Data;
    end;
end;

function TfChannel.SetkSps(NewKSps: Integer): Integer;
var
  res: LongInt = 0;
  Data : word = 0;
begin
  if Device.OpenDevice then
    begin
      while data = 0 do
        res := Device.SendControlMsg(USB_TYPE_VENDOR or USB_RECIP_DEVICE or USB_ENDPOINT_IN,FUNC_SET_SAMPLERATE, word(NewKSps), 0,@data, 2, 50);
      Device.CloseDevice;
      fMain.lkSPS.Caption:=IntToStr(Data);
      Result := Data;
      kSps := data;
      UpdateChannelData;
      UpdateChannelBuffer(FBufferlength);
    end;
end;

function TfChannel.SetTriggerLevel(Level: real): Integer;
begin

end;

procedure TfChannel.Init;
type
  PByte = ^byte;
var
  Info : TChannelInfo;
  i: Integer;
begin
  for i := 0 to sizeof(TChannelInfo)-1 do
    PByte(@Info+i)^ := ReadByte(i);
  if Info.Valid <> $A5 then exit;
  tbOffset.Position:=Info.offset;
  VperDiv := Info.VperDiv;
  udVoltage.Position:=trunc(VperDiv*10);
  ChannelName := Info.Name;
end;

procedure TfChannel.Save;
type
  PByte = ^byte;
var
  Info : TChannelInfo;
  i: Integer;
begin
  Info.Valid := $A5;
  Info.offset := tbOffset.Position;
  Info.VperDiv := VperDiv;
  Info.Name := ChannelName;
  for i := 0 to sizeof(TChannelInfo)-1 do
    WriteByte(i,PByte(@Info+i)^);
end;

procedure TfChannel.UpdateChannelBuffer(newlength: real);
var
  aOldLength: Integer;
  aNewLength: Integer;
begin
  aOldLength := length(Channel_data);
  aNewLength := round(KSps*1000*newlength);
  FBufferlength := newlength;
  Setlength(Channel_data,aNewLength);
  UpdateChannelData;
end;

{ TChannelTimer }

procedure TChannelTimer.UpdateMainData;
begin
  move(Parent.channel_data[res],Parent.channel_data[0],length(Parent.channel_data)-res);
  move(data,Parent.channel_data[length(Parent.channel_data)-res],res);
  Parent.DoUpdate:=True;
end;

procedure TChannelTimer.UpdateMainState;
begin
  Parent.ChannelStateChanged(GetState);
end;

procedure TChannelTimer.getData;
begin
  res := Parent.Device.SendControlMsg(USB_TYPE_VENDOR or USB_RECIP_DEVICE or USB_ENDPOINT_IN,FUNC_READ_BUFFER, 0, 0,@data[0], 64, 500);
end;

function TChannelTimer.GetState: byte;
var
  state : byte;
begin
  try
    res := Parent.Device.SendControlMsg(USB_TYPE_VENDOR or USB_RECIP_DEVICE or USB_ENDPOINT_IN,FUNC_GET_STATE, 0, 0,@state, 1, 50);
  except
  end;
  result := state;
end;

procedure TChannelTimer.StartSampling;
begin
  res := Parent.Device.SendControlMsg(USB_TYPE_VENDOR or USB_RECIP_DEVICE or USB_ENDPOINT_IN,FUNC_START, 0, 0,nil, 0, 500);
end;

constructor TChannelTimer.Create(aParent: TfChannel);
begin
  Parent := aParent;
  inherited Create(False);
end;

procedure TChannelTimer.Execute;
var
  tmpState: Byte;
  aState: Byte;
begin
  while not Terminated do
    begin
      if Parent.Device.OpenDevice then
        begin
          tmpState := GetState;
          if tmpState <> aState then
            begin
              if tmpState = STATE_AUTO_STOPPED then
                begin
                  Self.Synchronize(@GetData);
                  while res > 0 do
                    begin
                      Self.Synchronize(@UpdateMainData);
                      GetData;
                    end;
                  Self.Synchronize(@StartSampling);
                  tmpState := STATE_RUNNING;
                end;
              aState := tmpState;
              Self.Synchronize(@UpdateMainState);
            end;
          Parent.Device.CloseDevice;
        end;
    end;
end;

initialization
  {$I uchannel.lrs}

end.
