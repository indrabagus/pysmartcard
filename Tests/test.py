import scard
ctx=scard.context()
con=ctx.connector(0)
if(con.connect() != 0):
    print ("failed to connect")
    quit()

ls=con.transceive([0x51,0x00,0x00,0x00,0x04])
print("resp = " ,"".join("%2.2X" % c for c in ls))

con.disconnect()
