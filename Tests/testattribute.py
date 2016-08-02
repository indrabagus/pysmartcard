import scard
ctx=scard.context()
con=ctx.connector(0)
con.connect()
atr = con.atr()
print("ATR: ","".join("%2.2X" % c for c in atr))
maxrate = con.max_datarate()
print("Max Data Rate: %d KBps "% ((maxrate/8)/1024))
maxclk = con.max_clock()
print("Max Clock Rate: %d kHz" % maxclk)
con.disconnect()