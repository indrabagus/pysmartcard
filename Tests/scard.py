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

def send_raw_ctl_code(con,apdu):
    if(isinstance(con,scard.connector) == False):
        raise TypeError("argument should be from scard.connector")
        
    con.direct_connect()
    res = con.control(scard.IOCTL_CCID_ESCAPE_SCARD_CTL_CODE,apdu)
    
    if(len(res) < 5 or res[4] == 0x00):
        con.disconnect()    
        raise Exception("failed getting current firmware : resp-> %s" % " ".join("%2.2X" % c for c in res))
    con.disconnect()
    return res

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


class acr1281U(object):
    def __init__(self,con):
        if(isinstance(con,_scard.connector) == False):
            raise TypeError("argument should be from scard.connector")
        self._con = con

    def getfirmware(self):
        res = send_raw_ctl_code(self._con,bytes([0xE0,0x00,0x00,0x18,0x00]))
        """ 
        Response : E1 00 00 00 10 41 43 52 31 32 38 31 55 5F 56 35 30 36 2E 30 36 
        Firmware data : 41 43 52 31 32 38 31 55 5F 56 35 30 36 2E 30 36
        """
        retval = res[5:]
        return retval


    def refresh_iface_cc(self):
        pass

    def read_card_insert_counter(self):
        pass

    def buzzer_control(self):
        pass

    def green_led(self,onoff):
        pass

    def red_led(self,onoff):
        pass

    def read_exclusive_mode(self):
        pass

    def set_exclusive_mode(self):
        pass

    def read_picc_ops_param(self):
        pass

    def read_auto_pps(self):
        pass

