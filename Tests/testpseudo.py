import scard
import time

ctx=scard.context()
con=ctx.connector(0)
#con.connect()
con.direct_connect()
#res = con.control(scard.IOCTL_CCID_ESCAPE_SCARD_CTL_CODE,[0xFF,0x00,0x48,0x00,0x00])
#print(" ".join("%2.2X" % c for c in res))

res = con.control(scard.IOCTL_CCID_ESCAPE_SCARD_CTL_CODE,[0xFF,0x00,0x51,0xFF,0x00])
print(" ".join("%2.2X" % c for c in res))
time.sleep(0.5)
#res = con.control(scard.IOCTL_CCID_ESCAPE_SCARD_CTL_CODE,[0xFF,0x00,0x00,0x00,0x02,0xF4,0xE0])
#print(" ".join("%2.2X" % c for c in res))
con.disconnect()