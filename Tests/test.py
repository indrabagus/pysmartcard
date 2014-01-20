import scard
ctx=scard.context()
con=ctx.connector(0)
if(con.connect() != 0):
    print ("failed to connect")
    quit()

ls=con.transceive([0x51,0x00,0x00,0x00,0x04])
print("resp->status = 0x%X" % (ls[0]))
szdata=str()
for i in ls[1]:
    szdata=szdata + " " + str("%2.2X" % i)
print("resp->data = ",szdata)

con.disconnect()
