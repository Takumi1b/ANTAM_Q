unit Unit1;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, DXLIB, ExtCtrls;

type
  TForm1 = class(TForm)
    Button1: TButton;
    ScrollBar1: TScrollBar;
    Label1: TLabel;
    Label2: TLabel;
    Label3: TLabel;
    Timer1: TTimer;
    Label5: TLabel;
    Label6: TLabel;
    Label7: TLabel;
    ComboBox1: TComboBox;
    ComboBox2: TComboBox;
    ComboBox3: TComboBox;
    ComboBox4: TComboBox;
    Label4: TLabel;
    procedure FormCreate(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure Button1Click(Sender: TObject);
    procedure ScrollBar1Change(Sender: TObject);
    procedure Timer1Timer(Sender: TObject);
    procedure ComboBox3Change(Sender: TObject);
    procedure ComboBox1Change(Sender: TObject);
  private
    { Private 宣言 }
    devid:  TDeviceID;
    function AllActive:boolean;

    function ReadMaxMinPos(dvid:TDeviceID; id:uint8):boolean;
    function WritePos (dvid:TDeviceID; id:uint8; pos:int32; var err:TErrorCode):boolean;
    function ReadPos (dvid:TDeviceID; id:uint8; var pos:int32; var err:TErrorCode) :boolean;
  public
    { Public 宣言 }
  end;

var
  Form1: TForm1;

implementation

{$R *.dfm}

// DLLのロードとポートオープン成功の状態取得
function TForm1.AllActive:boolean;
begin
  result := False;
  if DXLIB_Active then
    if (devid <> null) then result := DX_Active (devid);
end;

// フォームを作成
procedure TForm1.FormCreate(Sender: TObject);
var
  i:integer;
  b:bool;
begin
  devid := 0;
  b:=DXLIB_Load;    // DLLのダイナミックロード

  for i := 1 to 256 do
    ComboBox2.Items.Add('COM'+IntToStr(i));
  ComboBox2.ItemIndex := 0;

  for i := 0 to 253 do
    ComboBox4.Items.Add(IntToStr(i));
  ComboBox4.ItemIndex := 1;
end;

// フォームを閉じる
procedure TForm1.FormClose(Sender: TObject; var Action: TCloseAction);
begin
  if AllActive then DX_ClosePort(devid);  // DLLがロードされておりポートが開いていたら閉じる
  Timer1.Enabled := False;
  DXLIB_Unload;  // DLLのアンロード
end;

// 指令可能な位置の範囲取得
function TForm1.ReadMaxMinPos(dvid:TDeviceID; id:uint8):boolean;
var
  err: TErrorCode;
  v16cw,v16ccw:uint16;
  v32cw,v32ccw:uint32;
  cw,ccw:int32;
begin
  result := false;
  if AllActive then
  begin
    case ComboBox1.ItemIndex of
      0,//DX
      1,//AX
      2,//RX
      3,//EX
      4://MX
      begin
        result := DX_ReadWordData(dvid, id, 6, v16cw, err) and
          DX_ReadWordData(dvid, id, 8, v16ccw, err);
        if result then
        begin
          cw := int16(v16cw);
          ccw := int32(v16ccw);
          if cw > ccw then
          begin
            ScrollBar1.Max := cw;
            ScrollBar1.Min := ccw;
          end else
          begin
            ScrollBar1.Max := ccw;
            ScrollBar1.Min := cw;
          end;
        end;
      end;
      5://X
      begin
        result := DX_ReadLongData(dvid, id, 48, v32cw, err) and
          DX_ReadLongData(dvid, id, 52, v32ccw, err);
        if result then
        begin
          cw := int32(v32cw);
          ccw := int32(v32ccw);
          if cw >= ccw then
          begin
            ScrollBar1.Max := cw;
            ScrollBar1.Min := ccw;
          end else
          begin
            ScrollBar1.Max := ccw;
            ScrollBar1.Min := cw;
          end;
        end;
      end;
    else
    end;

  end;
end;

// トルクイネーブルと目標位置指令
function TForm1.WritePos (dvid:TDeviceID; id:uint8; pos:int32; var err:TErrorCode):boolean;
begin
  result := false;

  if AllActive then
  begin
    case ComboBox1.ItemIndex of
      0,//DX
      1,//AX
      2,//RX
      3,//EX
      4://MX
      begin
        result := DX_WriteByteData(dvid, id, 24, 1, err)
          and DX_WriteWordData(dvid, id, 30, ScrollBar1.Position, err);
      end;
      5://X
      begin
        result := DX_WriteByteData(dvid, id, 64, 1, err)
          and DX_WriteLongData(dvid, id, 116, ScrollBar1.Position, err);
      end;
    else
    end;

  end;
end;

// 現在位置取得
function TForm1.ReadPos (dvid:TDeviceID; id:uint8; var pos:int32; var err:TErrorCode):boolean;
var
  v16:uint16;
  v32:uint32;
begin
  result := false;
  if AllActive then
  begin
    case ComboBox1.ItemIndex of
      0,//DX
      1,//AX
      2,//RX
      3,//EX
      4://MX
      begin
        result := DX_ReadWordData(dvid, id, 36, v16, err);
        pos := v16;
      end;
      5://X
      begin
        result := DX_ReadLongData(dvid, id, 116, v32, err);
        pos := v32;
      end;
    else
    end;
  end;
end;

procedure TForm1.Button1Click(Sender: TObject);
var
  portname: ansistring;
begin
  // ポートオープン
  if (devid = 0) and DXLIB_Active then
  begin
    portname := '\\.\'+ComboBox2.Text;
    devid := DX_OpenPort(PAnsiChar(portname), StrToInt(ComboBox3.Text));
    if AllActive then
    begin
      Button1.Caption := 'close';
      Timer1.Enabled := True;
      ComboBox1.Enabled := false;
      ComboBox2.Enabled := false;
    end else
    begin
      beep;
      Timer1.Enabled := False;
    end;
  end else
  // ポートクローズ
  begin
    if AllActive then DX_ClosePort(devid);
    Button1.Caption := 'open';
    devid := 0;
    Timer1.Enabled := False;
    ComboBox1.Enabled := true;
    ComboBox2.Enabled := true;
  end;
end;

procedure TForm1.ComboBox1Change(Sender: TObject);
begin
  Button1.Enabled := (ComboBox1.ItemIndex <> -1);
end;

procedure TForm1.ComboBox3Change(Sender: TObject);
begin
  if AllActive then
  begin
    DX_SetBaudrate(devid, StrToInt(ComboBox3.Text));
  end;
end;

// スクロールバーのスライドで位置指令
procedure TForm1.ScrollBar1Change(Sender: TObject);
var
  err: TErrorCode;
begin
  WritePos (devid, StrToInt(ComboBox4.Text), ScrollBar1.Position, err);
  Label1.Caption := 'write errstat:$' + IntToHex(err, 4);
end;

// タイマで現在位置取得
procedure TForm1.Timer1Timer(Sender: TObject);
var
  err: TErrorCode;
  pos:int32;
begin
  if AllActive then
  begin
    ReadMaxMinPos (devid, StrToInt(ComboBox4.Text));
    if ReadPos(devid, StrToInt(ComboBox4.Text), pos, err) then
      Label3.Caption := 'present position:' + IntToStr(pos)
    else
      Label3.Caption := 'present position:?';
    Label2.Caption := 'read errstat:$' + IntToHex(err, 4);
  end else
  begin
    Timer1.Enabled := False;
    Label3.Caption := 'present position:-';
  end;
end;

end.
