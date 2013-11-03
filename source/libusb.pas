unit libusb;

{$IFDEF WINDOWS}
  {$I libusb_win.inc}
{$ELSE}
  {$I libusb_unix.inc}
{$ENDIF}
