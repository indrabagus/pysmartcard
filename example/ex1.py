import scard

def list2strbyte(ls):
    pass

def resp2status(buffer):
    pass        

context = scard.create_context()        
listcard = context.listreader()
for i in listcard:
    print(i)

# Anggap saja list card isinya tidak 0    

scard = context.connect(listcard[0])
lstx=[0x51,0x00,0x00,0x00,0x04]

resp = scard.transceive(list3strbyte(lstx))
status = resp2status(resp)
print('Status = %d' % status)
scard.disconnect()
context.destroy()