require "fiddle/import"

module DXLIB
  extend Fiddle::Importer
  dlload './dxlib.so.4.5'
  extern 'void * DX_OpenPort( const char *, long )'
  extern 'char DX_Active( void * )'
  extern 'char DX_Ping( void *, unsigned char, unsigned short * )'
  extern "char DX_ReadWordData( long, unsigned char, unsigned short, unsigned short *, int, unsigned short * )"
  extern "char DX_WriteWordData( long, unsigned char, unsigned short, unsigned short, int, unsigned short * )"
  extern "char DX_ReadByteData( long, unsigned char, unsigned short, unsigned char *, int, unsigned short * )"
  extern "char DX_WriteByteData( long, unsigned char, unsigned short, unsigned char, int, unsigned short * )"
  extern 'char DX_ClosePort( void * )'
end
comport = "/dev/ttyUSB0"
br = 57143
uid = 1
TORQUE_ENABLE_ADDR = 24
GOAL_POS_ADDR = 30
TORQUE_LIMIT_ADDR = 34
PRESENT_POS_ADDR = 36
MAX_POSITION = 620
MIN_POSITION = 400
SLEEP_TIME = 0.05
LOOP_COUNT = 20
PARTIAL_COUNT = 20
OK = 1
NG = 0
terr = ' ' * 2
rbuff = ' ' * 2
pos = 0
devid = DXLIB.DX_OpenPort( comport, br )
if ( Fiddle::Pointer[devid].to_i != 0 ) then
  if ( DXLIB.DX_Ping( devid, uid, terr ) == OK ) then
    if ( DXLIB.DX_WriteWordData( devid, uid, TORQUE_LIMIT_ADDR, 300, terr ) != OK ) then
      printf( "torque limit write error(%x)\n", terr.unpack('S')[0] )
    end
    DXLIB.DX_WriteByteData( devid, uid, TORQUE_ENABLE_ADDR, 1, terr )
    i = 0
    while( i < LOOP_COUNT )
      if ( pos == MAX_POSITION ) then
        pos = MIN_POSITION
      else
        pos = MAX_POSITION
      end
      if ( DXLIB.DX_WriteWordData( devid, uid, GOAL_POS_ADDR, pos, terr ) == OK ) then
        j = 0
        while( j < PARTIAL_COUNT )
          if ( DXLIB.DX_ReadWordData( devid, uid, PRESENT_POS_ADDR, rbuff, terr ) == OK ) then
            printf("POS:%d   \r", rbuff.unpack('S')[0])
          else
            printf("read error(%x)\n", terr.unpack('S')[0])
          end
          sleep SLEEP_TIME
          j += 1
        end
      else
        printf( "write error(%x)\n", terr.unpack('S')[0] )
      end
      i += 1
    end
    DXLIB.DX_WriteByteData( devid, uid, TORQUE_ENABLE_ADDR, 0, terr )
    if ( DXLIB.DX_ClosePort( devid ) != OK ) then
      print "CLOSE ERROR\n"
    end
  else
    p "Motor dead"
  end
else
  print "Port Open Failed"
end
print("\r\nfin\r\n")
