require "fiddle/import"

module DXLIB
  extend Fiddle::Importer
  dlload './dxlib.so.4.5'
  extern 'void * DX_OpenPort( const char *, long )'
  extern 'char DX_Active( void * )'
  extern 'char DX_Ping( void *, unsigned char, unsigned short * )'
  extern 'char DX_ClosePort( void * )'
end

comport = "/dev/ttyUSB0"
br = 57143
terr = ' ' * 2
mark = "\n"
devid = DXLIB.DX_OpenPort( comport, br )
if ( Fiddle::Pointer[devid].to_i != 0 ) then
  ret = DXLIB.DX_Active( devid )
  if ( ret == 1 ) then
    sleep 1
    i = 0
    while ( i <= 252 )
      ret = DXLIB.DX_Ping( devid, i, terr )
      if ( ret == 1 ) then
        printf( "\n%d alive", i )
        mark = "\n"
      else
        printf( "%s%d dead(%x)", mark, i, terr.unpack('S')[0] )
        mark = "\r"
      end
      i += 1
    end
    ret = DXLIB.DX_ClosePort( devid )
    if ( ret == 0 ) then
      print "CLOSE ERROR\n"
    end
  end
end
print("\r\nfin\r\n")
