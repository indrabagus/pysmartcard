import scard

ctx=scard.context()
con = ctx.connector(0)
pduin=bytearray([0x90,0x60,0,0,0])
con.connect()
print("TX: ",end="")
print(" ".join("%2.2X" % c for c in pduin))
pduout = con.transceive(pduin)
print("RX: ",end="")
print(" ".join("%2.2X" % c for c in pduout))

pduin[1]=0xAF
print("TX: ",end="")
print(" ".join("%2.2X" % c for c in pduin))
pduout = con.transceive(pduin)
print("RX: ",end="")
print(" ".join("%2.2X" % c for c in pduout))

print("tx: ",end="")
print(" ".join("%2.2X" % c for c in pduin))
pduout = con.transceive(pduin)
print("rx: ",end="")
print(" ".join("%2.2X" % c for c in pduout))

con.disconnect()
