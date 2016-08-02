import scard
import time

def GetUID():
    ctx=scard.context()
    con=ctx.connector(0)
    print("Get connector 1 handle") 
    con.direct_connect()
    res = con.control(scard.IOCTL_CCID_ESCAPE_SCARD_CTL_CODE,bytearray([0xFF, 0xCA, 0x00, 0x00, 0x00]))
    status = int("".join("%2.2x" % c for c in res[len(res)-2:]),16)
    if status != 0x9000:
        print("Failed to get the TAG's UID")
        exit()
    
    print("UID : ",end="")
    print(" ".join("%2.2X" % c for c in res[:7]))

    res = con.control(scard.IOCTL_CCID_ESCAPE_SCARD_CTL_CODE,bytearray([0xFF, 0xCA, 0x01, 0x00, 0x00]))
    status = int("".join("%2.2x" % c for c in res[len(res)-2:]),16)
    if status != 0x9000:
        print("Failed to get the TAG's ATS, ERR Status=0x%X" % status)
        exit()
    print("ATS : ",end="")
    print(" ".join("%2.2X" % c for c in res[:7]))

    con.disconnect()
    
    
if __name__ == '__main__':
    for c in range(1000):
        GetUID()
