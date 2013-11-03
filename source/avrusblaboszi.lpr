program avrusblaboszi;

{$mode objfpc}{$H+}

uses
  {$IFDEF UNIX}{.$IFDEF UseCThreads}
  cthreads,
  {.$ENDIF}{$ENDIF}
  Interfaces, // this includes the LCL widgetset
  Forms, uMain, general, uChannel;

{$R avrusblaboszi.res}

begin
  Application.Initialize;
  Application.CreateForm(TfMain, fMain);
  Application.Run;
end.

