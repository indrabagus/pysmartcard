import scard
import time
print(scard.about())
ctx = scard.context()
con = ctx.connector(1)
print("tag the card")
while(1):
    state = con.readerstate()
    print("%8.8X = %8.8X" % (state.currentstate,state.eventstate))
    if(state.eventstate & scard.SCARD_STATE_INUSE):
        #DON'T PUT sleep in here just continue the loop!!
        continue
        
    if(state.eventstate & scard.SCARD_STATE_PRESENT):
        break
        
    time.sleep(0.5)

print("reader state connected")
con.connect()
res=con.transceive(bytes([0x90,0x60,0,0,0]))
print(res)
time.sleep(2)

print("lift the tag..")
while(1):
    state = con.readerstate()
    if(state.eventstate & scard.SCARD_STATE_INUSE):
        #DON'T PUT sleep in here just continue the loop!!
        continue
        
    if(state.eventstate & scard.SCARD_STATE_EMPTY):
        break
        
    time.sleep(0.5)
    
print("disconnect..")
con.disconnect()
