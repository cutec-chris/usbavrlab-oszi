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
unit uMain;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, FileUtil, LResources, Forms, Controls, Graphics, Dialogs,
  ExtCtrls, StdCtrls, ComCtrls, ColorBox, Buttons, Menus, LCLproc,uChannel
  ,ActnList, Spin, EditBtn, XMLPropStorage, Grids, Math, Utils, uUsb, ulibUSBDevice;

type
  PPoint = ^TPoint;

  { TfMain }

  TfMain = class(TForm)
    acRefresh: TAction;
    acInfo: TAction;
    ActionList1: TActionList;
    bStart: TSpeedButton;
    bStop: TSpeedButton;
    cbTriggerChannel: TComboBox;
    lkSPS: TLabel;
    miInfo: TMenuItem;
    seSR: TFloatSpinEdit;
    Label3: TLabel;
    seBufferSize: TFloatSpinEdit;
    GroupBox1: TGroupBox;
    ImageList1: TImageList;
    Label1: TLabel;
    Label2: TLabel;
    lSampleRate: TLabel;
    lbProgrammer: TListBox;
    lTime: TLabel;
    lTriggerLevel: TLabel;
    lTriggerType: TLabel;
    MainMenu1: TMainMenu;
    miLanguage: TMenuItem;
    miOptions: TMenuItem;
    Panel1: TPanel;
    Panel2: TPanel;
    pChannels: TPanel;
    Panel4: TPanel;
    pSelect: TPanel;
    pbScreen: TPaintBox;
    Properties: TXMLPropStorage;
    sbTime: TScrollBar;
    sbTriggerContinuus: TSpeedButton;
    sbTriggerFalling: TSpeedButton;
    sbTriggerRaising: TSpeedButton;
    SpeedButton1: TSpeedButton;
    StatusBar: TStatusBar;
    tbTriggerLevel: TTrackBar;
    udTime: TUpDown;
    procedure acInfoExecute(Sender: TObject);
    procedure bStartClick(Sender: TObject);
    procedure bStopClick(Sender: TObject);
    procedure ControllerGetDeviceClass(VendorID, DeviceID: word;
      var aClass: TUSBDeviceClass);
    procedure ControllerUSBArrival(Sender: TObject);
    procedure ControllerUSBRemove(Sender: TObject);
    procedure pbScreenMouseWheel(Sender: TObject; Shift: TShiftState;
      WheelDelta: Integer; MousePos: TPoint; var Handled: Boolean);
    procedure seBufferSizeChange(Sender: TObject);
    procedure seSRChange(Sender: TObject);
    procedure FormCloseQuery(Sender: TObject; var CanClose: boolean);
    procedure FormCreate(Sender: TObject);
    procedure lbProgrammerSelectionChange(Sender: TObject; User: boolean);
    procedure NewMItemClick(Sender: TObject);
    procedure Panel1Resize(Sender: TObject);
    procedure pbScreenPaint(Sender: TObject);
    procedure sbTriggerContinuusClick(Sender: TObject);
    procedure sbTriggerFallingClick(Sender: TObject);
    procedure sbTriggerRaisingClick(Sender: TObject);
    procedure tbTriggerLevelChange(Sender: TObject);
    procedure tbTriggerLevelMouseDown(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure tbTriggerLevelMouseUp(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure udTimeClick(Sender: TObject; Button: TUDBtnType);
  private
    { private declarations }
    FScaleTextColor,
    FScaleColorLight,
    FScaleColorDark,
    FScreenColor,
    FBeamC1Color,
    FBeamC2Color: TColor;
    TriggerLevelSet : Boolean;
    FScreen : Graphics.TBitmap;
    FTmpScreen : Graphics.TBitmap;
    Language : string;
    Controller : TUSBController;

    function  InitTarget : Boolean;
    procedure InitDisplay(Screen : Graphics.TBitmap);
    procedure DrawScale(aScreen : Graphics.TBitmap);
    procedure SetLanguage(Lang : string);
  public
    { public declarations }
    HCenter,
    VCenter :integer;
    MsperDiv : Float;
    DividerX : Float;
    DividerY : Float;
    procedure ChannelUpdated(Channel : TfChannel);
  end;

const
  DataPointsperMs             = 26;

  FUNC_TYPE                   = $FE;
  FUNC_START_BOOTLOADER       = 30;

  CONTROLLER_ATMEGA8          = 1;
  CONTROLLER_ATMEGA88         = 2;
  CONTROLLER_ATMEGA168        =	3;

var
  fMain: TfMain;

resourcestring
  strNoOszilloscopeConnected   = 'kein USB AVR Lab Oszilloskop verbunden';
  strErrorConnectingToDevice   = 'Fehler beim verbinden zum Gerät';
  strChannel                   = 'Kanal ';
  strInfo                      = 'www:  http://www.ullihome.de'+lineending
                                +'mail: christian@ullihome.de'+lineending
                                +lineending
                                +'Lizenz:'+lineending
                                +'Die Software und ihre Dokumentation wird wie sie ist zur'+lineending
                                +'Verfuegung gestellt. Da Fehlfunktionen auch bei ausfuehrlich'+lineending
                                +'getesteter Software durch die Vielzahl an verschiedenen'+lineending
                                +'Rechnerkonfigurationen niemals ausgeschlossen werden koennen,'+lineending
                                +'uebernimmt der Autor keinerlei Haftung fuer jedwede Folgeschaeden,'+lineending
                                +'die sich durch direkten oder indirekten Einsatz der Software'+lineending
                                +'oder der Dokumentation ergeben. Uneingeschraenkt ausgeschlossen'+lineending
                                +'ist vor allem die Haftung fuer Schaeden aus entgangenem Gewinn,'+lineending
                                +'Betriebsunterbrechung, Verlust von Informationen und Daten und'+lineending
                                +'Schaeden an anderer Software, auch wenn diese dem Autor bekannt'+lineending
                                +'sein sollten. Ausschliesslich der Benutzer haftet fuer Folgen der'+lineending
                                +'Benutzung dieser Software.'+lineending
                                +lineending
                                +'erstellt mit Freepascal + Lazarus'+lineending
                                +'http://www.freepascal.org, http://lazarus.freepascal.org'+lineending
                                +'Iconset von:'+lineending
                                +'http://www.famfamfam.com/lab/icons/silk/'+lineending
                                ;

implementation

uses uInfo;

{ TfMain }

procedure TfMain.FormCreate(Sender: TObject);
var
  i: Integer;
  sl: TStringList;
  NewMItem: TMenuItem;
begin
  TriggerLevelSet := False;
  Language := 'Deutsch';
  ForceDirectories(GetConfigDir('usbavrlab'));
  Properties.FileName := GetConfigDir('usbavrlab')+'usbavrlab.xml';
  Properties.Restore;
  Top := StrToIntDef(Properties.StoredValue['TOP'],Top);
  Left := StrToIntDef(Properties.StoredValue['LEFT'],Left);
  Height := StrToIntDef(Properties.StoredValue['HEIGHT'],Height);
  Width := StrToIntDef(Properties.StoredValue['WIDTH'],Width);
  sl := TStringList.Create;
  if FileExistsUTF8(AppendPathDelim(AppendPathDelim(ProgramDirectory) + 'languages')+'languages.txt') then
    sl.LoadFromFile(UTF8ToSys(AppendPathDelim(AppendPathDelim(ProgramDirectory) + 'languages')+'languages.txt'));
  for i := 0 to sl.Count-1 do
    begin
      NewMItem := TMenuItem.Create(nil);
      NewMItem.Caption := sl[i];
      NewMItem.AutoCheck := True;
      NewMItem.OnClick :=@NewMItemClick;
      NewMItem.GroupIndex := 11;
      miLanguage.Add(NewMItem);
      if UTF8UpperCase(NewMItem.Caption) = UTF8UpperCase(Properties.StoredValue['LANGUAGE']) then
        begin
          NewMItem.Checked := True;
          Language := Properties.StoredValue['LANGUAGE'];
        end;
    end;
  sl.Free;
  SetLanguage(Language);
  fInfo := TfInfo.Create(Self);
  with fInfo do
    begin
      Version := {$I version.inc};
      Version := Version+{$I revision.inc} / 100;
      ProgramName := 'USB AVR Lab Oszilloscope';
      Copyright := '2007-2009 C.Ulrich';
      InfoText := strInfo;
    end;
  fInfo.SetLanguage;
  FScaleColorLight := $00E8E8AA;
  FScaleColorDark := $00C4C435;
  FScreenColor := $00A9A92E;
  FScaleTextColor := $00F1F1CD;
  FScreen := Graphics.TBitmap.Create;
  FScreen.Height:= pbScreen.Height;
  FScreen.Width := pbScreen.Width;
  FTmpScreen := Graphics.TBitmap.Create;
  FTmpScreen.Height:= pbScreen.Height;
  FTmpScreen.Width := pbScreen.Width;
  MsperDiv := 10;
  Controller := TUSBController.Create(Self);
  Controller.OnUSBArrival:=@ControllerUSBArrival;
  Controller.OnUSBRemove:=@ControllerUSBRemove;
  Controller.OnGetDeviceClass:=@ControllerGetDeviceClass;
  Controller.Enumerate;
end;

procedure TfMain.lbProgrammerSelectionChange(Sender: TObject; User: boolean);
begin
  //Implement me
end;

procedure TfMain.NewMItemClick(Sender: TObject);
var
  i: Integer;
begin
  for i := 0 to miLanguage.Count-1 do
    if miLanguage[i].Caption = Language then
      miLanguage[i].Checked := false;
  TmenuItem(Sender).Checked := True;
  Language := TmenuItem(Sender).Caption;
  SetLanguage(Language);
  Properties.StoredValue['LANGUAGE'] := Language;
end;

procedure TfMain.acInfoExecute(Sender: TObject);
begin
  fInfo.Showmodal;
end;

procedure TfMain.bStartClick(Sender: TObject);
var
  i: Integer;
begin
  for i := 0 to lbProgrammer.Items.Count-1 do
    TfChannel(lbprogrammer.Items.Objects[i]).Start;
end;

procedure TfMain.bStopClick(Sender: TObject);
var
  i: Integer;
begin
  for i := 0 to lbProgrammer.Items.Count-1 do
    TfChannel(lbprogrammer.Items.Objects[i]).Stop;
end;

procedure TfMain.ControllerGetDeviceClass(VendorID, DeviceID: word;
  var aClass: TUSBDeviceClass);
begin
  if  ((VendorID = $16C0)
  and
      ((DeviceID = $05dc)
      ))
  then
    aClass := TLibUSBDevice;
end;

procedure TfMain.ControllerUSBArrival(Sender: TObject);
var
  Typ : byte;
  aChannel: TfChannel;
  i: LongInt;
begin
  if  ((TUSBDevice(Sender).VendorID = $16C0)
  and
      ((TUSBDevice(Sender).DeviceID = $05dc)
      ))
    then
    begin
      if TUSBDevice(Sender) is TLibUSbDevice then
        with TLibUSBDevice(Sender) do
          if OpenDevice then
            begin
              if SendControlMsg(USB_TYPE_VENDOR or USB_RECIP_DEVICE or USB_ENDPOINT_IN,FUNC_TYPE, 0, 0,@typ, 1, 5000) = 1 then
                begin
                  Tag := Typ;
                  if Typ = 9 then
                    begin
                      aChannel := TfChannel.Create(Self);
                      randomize;
                      aChannel.Name:='channel_'+IntToStr(random(999));
                      aChannel.DisplayScreen := FScreen;
                      aChannel.Device := TLibUSbDevice(Sender);
                      aChannel.Stop;
                      lbProgrammer.Items.AddObject('USB AVR-Lab Oszilloscope',aChannel);
                      aChannel.BorderStyle:=bsNone;
                      aChannel.Parent := pChannels;
                      aChannel.Left:=lbProgrammer.Items.Count*aChannel.Width;
                      aChannel.ChannelName:=strChannel+IntToStr(lbProgrammer.Items.Count);
                      aChannel.Align:=alLeft;
                      aChannel.UpdateChannelBuffer(seBufferSize.Value);
                      aChannel.SetkSps(round(seSR.Value));
                      aChannel.Init;
                      aChannel.Show;
                      if bStart.Down then aChannel.Start;
                      pChannels.Width:=lbProgrammer.Items.Count*aChannel.Width;
                      pbScreen.Invalidate;
                      i := cbTriggerChannel.ItemIndex;
                      cbTriggerChannel.Items.Clear;
                      for i := 0 to lbProgrammer.Items.Count-1 do
                        cbTriggerChannel.Items.Add(TfChannel(lbprogrammer.Items.Objects[i]).ChannelName);
                      if cbTriggerChannel.Items.Count <= i+1 then
                        cbTriggerChannel.ItemIndex := i;
                      cbTriggerChannel.Enabled := (cbTriggerChannel.Items.Count > 1);
                      InitDisplay(FScreen);
                      DrawScale(FScreen);
                    end;
                end;
              CloseDevice;
            end;
    end;
end;

procedure TfMain.ControllerUSBRemove(Sender: TObject);
var
  i: Integer;
  a: LongInt;
begin
  for i := 0 to lbProgrammer.Items.Count-1 do
    if TfChannel(lbProgrammer.Items.Objects[i]).Device = Sender then
      begin
        pChannels.RemoveControl(TfChannel(lbprogrammer.Items.Objects[i]));
        TfChannel(lbprogrammer.Items.Objects[i]).Device:=nil;
        TfChannel(lbprogrammer.Items.Objects[i]).Free;
        lbProgrammer.Items.Delete(i);
        if lbprogrammer.Items.Count > 0 then
          pChannels.Width:=lbProgrammer.Items.Count*TfChannel(lbprogrammer.Items.Objects[0]).Width
        else
          pChannels.Width := 0;
        pbScreen.Invalidate;
        a := cbTriggerChannel.ItemIndex;
        cbTriggerChannel.Items.Clear;
        for a := 0 to lbProgrammer.Items.Count-1 do
          cbTriggerChannel.Items.Add(TfChannel(lbprogrammer.Items.Objects[i]).ChannelName);
        if cbTriggerChannel.Items.Count <= a+1 then
          cbTriggerChannel.ItemIndex := a;
        cbTriggerChannel.Enabled := (cbTriggerChannel.Items.Count > 1);
        exit;
      end;
end;

procedure TfMain.pbScreenMouseWheel(Sender: TObject; Shift: TShiftState;
  WheelDelta: Integer; MousePos: TPoint; var Handled: Boolean);
var
  Delta: Extended;
begin
  Delta := WheelDelta/10;
  if udTime.Position / 100 <= 5 then Delta := Delta/10;
  if udTime.Position-Delta < 0 then exit;
  udTime.Position:=round(udTime.Position-Delta);
  udTimeClick(nil,btNext);
end;

procedure TfMain.seBufferSizeChange(Sender: TObject);
var
  i: Integer;
begin
  for i := 0 to lbProgrammer.Items.Count-1 do
    TfChannel(lbprogrammer.Items.Objects[i]).UpdateChannelBuffer(seBufferSize.Value);
end;

procedure TfMain.seSRChange(Sender: TObject);
var
  i: Integer;
begin
  for i := 0 to lbProgrammer.Items.Count-1 do
    TfChannel(lbprogrammer.Items.Objects[i]).SetkSps(round(seSR.Value));
end;

procedure TfMain.FormCloseQuery(Sender: TObject; var CanClose: boolean);
var
  i: Integer;
begin
  Properties.StoredValue['TOP'] := IntToStr(Top);
  Properties.StoredValue['LEFT'] := IntToStr(Left);
  Properties.StoredValue['HEIGHT'] := IntToStr(Height);
  Properties.StoredValue['WIDTH'] := IntToStr(Width);
  for i := 0 to lbProgrammer.Items.Count-1 do
    TfChannel(lbprogrammer.Items.Objects[i]).Stop;
end;

procedure TfMain.Panel1Resize(Sender: TObject);
var
  i: Integer;
begin
  FScreen.Height:= pbScreen.Height;
  FScreen.Width := pbScreen.Width;
  FtmpScreen.Height:= pbScreen.Height;
  FtmpScreen.Width := pbScreen.Width;
  for i := 0 to lbProgrammer.Items.Count-1 do
    TfChannel(lbprogrammer.Items.Objects[i]).UpdateChannelData;
  InitDisplay(FScreen);
  DrawScale(FScreen);
end;

procedure TfMain.pbScreenPaint(Sender: TObject);
var
  i: Integer;
  y: Integer;
  aTime: String;
begin
  with FtmpScreen.Canvas do
    begin
      Pen.Color := FScaleTextColor;
      Draw(0,0,FScreen);
      Font.Color := FScaleTextColor;
      Brush.Style:=bsClear;
      aTime := FloatToStr(Msperdiv)+' ms/div';
      TextOut(pbScreen.Width-pbScreen.Canvas.TextExtent(aTime).cx,0,aTime);
      y := 0;
      Pen.Style:=psSolid;
      for i := 0 to lbProgrammer.Items.Count-1 do
        y := y+TfChannel(lbprogrammer.Items.Objects[i]).DrawChannelData(FtmpScreen.Canvas,y);
      for i := 0 to lbProgrammer.Items.Count-1 do
        begin
          Pen.Color := TfChannel(lbprogrammer.Items.Objects[i]).cbColor.ButtonColor;
          PolyLine(PPoint(TfChannel(lbprogrammer.Items.Objects[i]).ChannelScreen),TfChannel(lbprogrammer.Items.Objects[i]).ScreenPoints);
    //      pbScreen.Canvas.PolyBezier(PPoint(TfChannel(lbprogrammer.Items.Objects[i]).ChannelScreen),TfChannel(lbprogrammer.Items.Objects[i]).ScreenPoints,True,False);
        end;
    end;
  pbScreen.Canvas.Draw(0,0,FtmpScreen);
end;

procedure TfMain.sbTriggerContinuusClick(Sender: TObject);
begin
  tbtriggerLevel.Enabled:=False;
end;

procedure TfMain.sbTriggerFallingClick(Sender: TObject);
begin
  tbtriggerLevel.Enabled:=True;
end;

procedure TfMain.sbTriggerRaisingClick(Sender: TObject);
begin
  tbtriggerLevel.Enabled:=True;
end;

procedure TfMain.tbTriggerLevelChange(Sender: TObject);
var
  PointsperDiv: Extended;
  VperPoint: Extended;
  y: Integer;
  ScalePos: Extended;
  RealOffs: Integer;
begin
  if not Assigned(cbTriggerChannel) then exit;
  if cbTriggerChannel.ItemIndex = -1 then exit;
  pbScreen.Invalidate;
  with pbScreen.Canvas do
    begin
      Pen.Color := FScaleTextColor;
      PointsperDiv := FScreen.Height / DividerY;
      VperPoint := TfChannel(lbprogrammer.Items.Objects[cbTriggerChannel.ItemIndex]).VperDiv / PointsPerDiv;
      ScalePos := ((tbTriggerLevel.Position-500) * ((FScreen.Height / 2)-64 {Offset of Scrollbar})) / 500;
      RealOffs := TfChannel(lbprogrammer.Items.Objects[cbTriggerChannel.ItemIndex]).Offset-(FScreen.Height div 2);
      TfChannel(lbprogrammer.Items.Objects[cbTriggerChannel.ItemIndex]).SetTriggerLevel(((-ScalePos)+RealOffs)*VPerPoint);
      y := (FScreen.Height div 2) + round(ScalePos);
      MoveTo(0,y);
      LineTo(FScreen.Width,y);
      TextOut(0,y-TextExtent('0').cy-1,FormatFloat('0.00',((-ScalePos)+RealOffs)*VPerPoint)+' V');
    end;
end;

procedure TfMain.tbTriggerLevelMouseDown(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
begin
  TriggerLevelSet := True;
end;

procedure TfMain.tbTriggerLevelMouseUp(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
begin
  TriggerLevelSet := False;
end;

procedure TfMain.udTimeClick(Sender: TObject; Button: TUDBtnType);
var
  i: Integer;
begin
  MsperDiv := udTime.Position/100;
  for i := 0 to lbProgrammer.Items.Count-1 do
    TfChannel(lbprogrammer.Items.Objects[i]).UpdateChannelData;
end;

function TfMain.InitTarget: Boolean;
begin

end;

procedure TfMain.InitDisplay(Screen : Graphics.TBitmap);
begin
  Screen.Canvas.Brush.Color := FScreenColor;
  Screen.Canvas.Brush.Style := bsSolid;
  Screen.Canvas.FillRect(0,0,Screen.Width,Screen.Height);
end;

procedure TfMain.DrawScale(aScreen: Graphics.TBitmap);
var
  loop:integer;
  w,h:integer;
  a,b:Word;
  FScreenCenter: Int64;
  DivSize: Integer;
begin
  h := aScreen.Height div 8;
  w := aScreen.Width div 100;
  DivSize := aScreen.Height div 8;
  if DivSize = 0 then exit;
  DividerX := aScreen.Width / DivSize;
  DividerY := aScreen.Height / DivSize;
  FScreenCenter := round(aScreen.Height div 2);
  with aScreen do
  begin
    HCenter := FScreenCenter;
    VCenter := round(aScreen.Width/2);

    Canvas.Pen.Width := 1;
    Canvas.Pen.Color := FScaleColorDark;

    Canvas.Pen.Style:=psDot;
    for loop:= 1 to 4 do
    begin
      Canvas.MoveTo(0,HCenter+(loop*DivSize));
      Canvas.LineTo(aScreen.Width,HCenter+(loop*DivSize));
    end;
    for loop:= 1 to 4 do
    begin
      Canvas.MoveTo(0,HCenter-(loop*DivSize));
      Canvas.LineTo(aScreen.Width,HCenter-(loop*DivSize));
    end;
    for loop:= 1 to round((Width / DivSize) / 2) do
    begin
      Canvas.MoveTo(VCenter-round(Loop*DivSize),0);
      Canvas.LineTo(VCenter-round(Loop*DivSize),aScreen.Height);
    end;
    for loop:= 1 to round((Width / DivSize) / 2) do
    begin
      Canvas.MoveTo(VCenter+round(Loop*DivSize),0);
      Canvas.LineTo(VCenter+round(Loop*DivSize),aScreen.Height);
    end;
    Canvas.Pen.Style:=psSolid;
    Canvas.Pen.Color := FScaleColorLight;

    for loop:= 1 to 20 do
    begin
      DivMod(loop,5,a,b);

      if b = 0 then
      begin
        Canvas.MoveTo(VCenter-4, HCenter+round(Loop*(DivSize / 5)));
        Canvas.LineTo(VCenter+5, HCenter+round(Loop*(DivSize / 5)));
      end
      else
      begin
        Canvas.MoveTo(VCenter-2, HCenter+round(Loop*(DivSize / 5)));
        Canvas.LineTo(VCenter+3, HCenter+round(Loop*(DivSize / 5)));
      end;
    end;

    for loop:= 1 to 20 do
    begin
      DivMod(loop,5,a,b);

      if b = 0 then
      begin
        Canvas.MoveTo(VCenter-4, HCenter-round(Loop*(DivSize / 5)));
        Canvas.LineTo(VCenter+5, HCenter-round(Loop*(DivSize / 5)));
      end
      else
      begin
        Canvas.MoveTo(VCenter-2, HCenter-round(Loop*(DivSize / 5)));
        Canvas.LineTo(VCenter+3, HCenter-round(Loop*(DivSize / 5)));
      end;
    end;

    for loop:= 1 to round(((Width / DivSize) / 2)*5) do
    begin
      DivMod(loop,5,a,b);
      if b = 0 then
      begin
        Canvas.MoveTo(VCenter-round(Loop*(DivSize / 5)), HCenter-4);
        Canvas.LineTo(VCenter-round(Loop*(DivSize / 5)), HCenter+5);
      end
      else
      begin
        Canvas.MoveTo(VCenter-round(Loop*(DivSize / 5)),HCenter-2);
        Canvas.LineTo(VCenter-round(Loop*(DivSize / 5)),HCenter+3);
      end
    end;
    for loop:= 1 to round(((Width / DivSize) / 2)*5) do
    begin
      DivMod(loop,5,a,b);
      if b = 0 then
      begin
        Canvas.MoveTo(VCenter+round(Loop*(DivSize / 5)), HCenter-4);
        Canvas.LineTo(VCenter+round(Loop*(DivSize / 5)), HCenter+5);
      end
      else
      begin
        Canvas.MoveTo(VCenter+round(Loop*(DivSize / 5)),HCenter-2);
        Canvas.LineTo(VCenter+round(Loop*(DivSize / 5)),HCenter+3);
      end
    end;

    Canvas.MoveTo(0,HCenter);
    Canvas.LineTo(aScreen.Width,HCenter);
    Canvas.MoveTo(VCenter,0);
    Canvas.LineTo(VCenter,aScreen.Height);
    //----------------------------------------------------------
    if FScreenColor = clBlack then
      Canvas.Font.Color := clgray
    else
      Canvas.Font.Color := clSilver;

    Canvas.Font.Style:=[];
    Canvas.Font.Name   := 'Small Fonts';
    Canvas.Font.Size   := 6;
    Canvas.Brush.Style := bsClear;
    Canvas.Font.Color := FScaleColorDark;
    Canvas.TextOut( 10,aScreen.Height-15,'USB AVR Lab Oszilloscope (c) 2008');

     if lbProgrammer.Items.Count = 0 then
      with Canvas do
        begin
          Font.Name:='default';
          Font.Color:=clWhite;
          {$ifndef LCLGTK2}
          Font.Height:=25;
          {$else}
          Font.Height:=17;
          {$endif}
          Font.Style:=[fsBold];
          TextOut((FScreen.Width - TextExtent(strNoOszilloscopeConnected).cx) div 2,(FScreen.Height - TextExtent(strNoOszilloscopeConnected).cy) div 2,strNoOszilloscopeConnected);
        end;
 end; //with Image2
end;

procedure TfMain.SetLanguage(Lang: string);
begin
  Utils.LoadLanguage(Lang);
end;

procedure TfMain.ChannelUpdated(Channel: TfChannel);
begin
  pbScreen.Invalidate;
  if TriggerLevelSet then
    tbTriggerLevelChange(nil);
end;

initialization
  {$I umain.lrs}

end.
