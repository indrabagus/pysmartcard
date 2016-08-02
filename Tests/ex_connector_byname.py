import _scard
ctx=_scard.context()
con=ctx.connector_byname("Identive CLOUD 4700 F Contact Reader 2")
con.connect()
atr=con.atr()
print("-".join("%2.2X" % c for c in atr))
con.disconnect()