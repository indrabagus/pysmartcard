import _scard
ctx=_scard.context()
con=ctx.connector(0)
con.connect()
atr = con.atr()
print("ATR: ","".join("%2.2X" % c for c in atr))