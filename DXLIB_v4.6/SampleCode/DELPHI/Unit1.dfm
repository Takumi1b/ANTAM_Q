object Form1: TForm1
  Left = 0
  Top = 0
  Caption = 'Form1'
  ClientHeight = 187
  ClientWidth = 359
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 15
    Top = 107
    Width = 94
    Height = 13
    Caption = 'write errstat:$0000'
  end
  object Label2: TLabel
    Left = 17
    Top = 154
    Width = 92
    Height = 13
    Caption = 'read errstat:$0000'
  end
  object Label3: TLabel
    Left = 15
    Top = 166
    Width = 85
    Height = 13
    Caption = 'present position:-'
  end
  object Label5: TLabel
    Left = 8
    Top = 5
    Width = 64
    Height = 13
    Caption = 'Devcice Type'
  end
  object Label6: TLabel
    Left = 106
    Top = 5
    Width = 46
    Height = 13
    Caption = 'COM Port'
  end
  object Label7: TLabel
    Left = 196
    Top = 5
    Width = 44
    Height = 13
    Caption = 'Baudrate'
  end
  object Label4: TLabel
    Left = 8
    Top = 53
    Width = 46
    Height = 13
    Caption = 'Target ID'
  end
  object Button1: TButton
    Left = 276
    Top = 8
    Width = 75
    Height = 58
    Caption = 'open'
    TabOrder = 0
    OnClick = Button1Click
  end
  object ScrollBar1: TScrollBar
    Left = 15
    Top = 126
    Width = 330
    Height = 17
    Max = 1023
    PageSize = 0
    TabOrder = 1
    OnChange = ScrollBar1Change
  end
  object ComboBox1: TComboBox
    Left = 8
    Top = 24
    Width = 92
    Height = 21
    Style = csDropDownList
    TabOrder = 2
    OnChange = ComboBox1Change
    Items.Strings = (
      'DX'
      'AX'
      'RX'
      'EX'
      'MX'
      'X')
  end
  object ComboBox2: TComboBox
    Left = 106
    Top = 24
    Width = 84
    Height = 21
    Style = csDropDownList
    TabOrder = 3
  end
  object ComboBox3: TComboBox
    Left = 196
    Top = 24
    Width = 74
    Height = 21
    Style = csDropDownList
    ItemIndex = 1
    TabOrder = 4
    Text = '57600'
    OnChange = ComboBox3Change
    Items.Strings = (
      '9600'
      '57600'
      '115200'
      '1000000'
      '2000000'
      '3000000')
  end
  object ComboBox4: TComboBox
    Left = 8
    Top = 72
    Width = 57
    Height = 21
    Style = csDropDownList
    TabOrder = 5
  end
  object Timer1: TTimer
    Enabled = False
    Interval = 100
    OnTimer = Timer1Timer
    Left = 108
    Top = 76
  end
end
