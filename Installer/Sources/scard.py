from _scard import *
import time


def waittodisconnect(con):
    """Wait and blocked function until the smartcard released from reader

    Args:
        con (connector): Connector object from smart card

    Return:
        None

    Example:
        ctx = scard.contex()
        xcon = ctx.connector(0)
        scard.waittoconnect(xcon)
        scard.connect()
        scard.waittodisconnect()
        scard.disconnect()
    """
    if(not isinstance(con, connector)):
        raise TypeError("con should be smart-card connector")
        
    while(1):
        state = con.readerstate()
        if(state.eventstate & SCARD_STATE_INUSE):
            #DON'T PUT sleep in here just continue the loop!!
            continue
        if(state.eventstate & SCARD_STATE_EMPTY):
            break
        time.sleep(0.5)


def waittoconnect(con):
    """ Wait and blocked function until the reader has proper smart-card connection 
    
    Args:
        con (connector): Connector object from smart card

    Return:
        None

    Example:
        ctx = scard.contex()
        xcon = ctx.connector(0)
        scard.waittoconnect(xcon)
        scard.connect()
    """
    if(not isinstance(con, connector)):
        raise TypeError("con should be connector")
    
    while(1):
        state = con.readerstate()
        if(state.eventstate & SCARD_STATE_INUSE):
            #DON'T PUT sleep in here just continue the loop!!
            continue
        if(state.eventstate & SCARD_STATE_PRESENT):
            break
        time.sleep(0.5)


def getrespdata(resp):
    """Get data section in Smart Card APU response

    The APDU response's format should look like 
    [DATA][SW1 SW2]

    Args:
        resp (bytes): bytes data that will extracted for getting the data element

    Returns
        bytes: Data element from Smart Card response
    """
    if(isinstance(resp,bytes) == False):
        raise TypeError("Input should be list of bytes")
    
    if(len(resp) < 2):
        raise RuntimeError("Length input too little")
        
    return resp[0:len(resp)-2]


def getrespstatus(resp):
    """Get 2 Bytes status from APDU reponse as integer

    The smart card APDU response shouldbe 2 Bytes

    Args:
        resp (bytes): Bytes stream from Smart Card response

    Returns
        int: 2 Bytes Integers status
    """
    if(isinstance(resp,bytes) == False):
        raise TypeError("Input should be list of bytes")
    
    if(len(resp) < 2):
        raise RuntimeError("Length input too little")
        
    return int("".join("%2.2x" % c for c in resp[len(resp)-2:]),16)


def bytes2string(buff):
    """
    bytes2string(...) -> str

    Merubah dari bytes menjadi string yang bisa dibaca
    """
    lsinp = memoryview(buff).tolist()
    szinp = ""
    for i in lsinp:
        szinp = szinp + ("%2.2x" % i)
    return szinp


def buffer2str(buffer,**kwargs):
    """Converting bytes/list into formated string
    
    Args:
        buffer (bytes/list): Buffer contained APDU
        **kwargs: Arbitrary keyword argument
            lenblock = int
                The length of data string in a line
        
            enableaddr = bool
                Enable the address information on string returned value
            
            startaddr = int
                Starting address of the payload
    
    Return
        str: String of formated payload
    """    
    lenblock=16
    addr = 0x0000
    enaddr = True
    
    if "lenblock" in kwargs:
        if isinstance(kwargs["lenblock"],int) == False:
            raise TypeError("lenblock should be boolean type ( True/False )")
        lenblock=kwargs["lenblock"]
        
    if "startaddr" in kwargs:
        if isinstance(kwargs["startaddr"],int) == False:
            raise TypeError("startaddr should be integer type")    
        addr=kwargs["startaddr"]
    
    if "enableaddr" in kwargs:
        if isinstance(kwargs["enableaddr"],bool) == False:
            raise TypeError("enableaddr should be boolean type ( True/False )")    
        enaddr = kwargs["enableaddr"]
    
    szret = ""
    
    for i in range(len(buffer)//lenblock):
        if(enaddr):
            szret = szret + "| %4.4X   " % addr
        else:
            szret = szret + "| "
        szret = szret + "".join("%2.2X " % c for c in pdu[i*lenblock : (i*lenblock) + lenblock])
        # ingat! ada karakter spasi di operasi join diatas
        szret = szret + "|\n"
        addr = addr + lenblock
     
    sisa=len(buffer) % lenblock
    if sisa == 0:
        return szret
    
    
    if(enaddr):
        szret = szret + "| %4.4X   " % addr
    else:
        szret = szret + "| "        
        
    szret = szret + "".join("%2.2X " % c for c in buffer[len(buffer)-sisa : len(buffer)])
    szret = szret + "".join(" . " for c in range(lenblock-sisa)) + "|\n"
    return szret

