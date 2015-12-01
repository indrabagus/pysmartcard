from _mp300 import *
#import _mp300
import logging
import enum 


#__version__         = _mp300.__version__
#__author__          = _mp300.__author__
#__doc__             = _mp300.__doc__


class ClockMode:
    EXTERNAL    = 1
    INTERNAL    = 2


class SendAPDU:
    NO_LC       = 0x80000000  # No Lc field in CAPDU
    LC_EXTENDED = 0x40000000  # Extended Lc field in CAPDU
    NO_LC_SEND  = 0x20000000  # The Lc field is not transmit
    NO_LE       = 0x80000000  # No Le field in CAPDU
    LE_EXTENDED = 0x40000000  # Extended Le field in CAPDU
    NO_LE_SEND  = 0x20000000  # The Le field is not transmit


#Internal Parameter
class InternalParam:
    VCC             = 0
    VOH	            = 1
    VOL	            = 2
    VIH	            = 3
    VIL	            = 4
    CLOCKMODE       = 5
    PROTOCOL        = 6
    PROT_ISO	    = 1
    PROT_USB	    = 2
    PROT_MMC	    = 4
    PROT_SWP	    = 8
    CLOCK_FREQUENCY = 7
    CLOCK_DC		= 8
    PULLUP			= 9
    VOH_PU			= 10
    VPP				= 11
    RISING_SLOPE	= 12
    FALLING_SLOPE	= 13
    C6MODE			= 14
    SPYMODE			= 15
    MEASUREMODE		= 16
    CLOCKSOURCE		= 17


# Resources ID define
class ResourceId:
    SIMULATOR		            = 0x100	#For MP300SC1 only
    PORT_SERIAL1		        = 1
    PORT_SERIAL2	            = 2
    PORT_USB_EP1	            = 101
    PORT_USB_EP2	            = 102
    C1				            = 201
    C2				            = 207
    CSWP			            = 224
    MC1				            = 228
    TC1				            = 202
    TC2				            = 205
    CL1				            = 203
    CL2				            = 206
    TCL1			            = 120
    MHC6			            = 204
    SWPSPY			            = 232
    SC1				            = (SIMULATOR | SWPSPY)
    MT1				            = 210
    SCL1			            = 200             
    TPC1			            = 208


class SemaphoreMode:
    WAIT    = 1
    NO_WAIT = 0


# Blocking mode
class BlockingMode:
    BLOCCKING       = SemaphoreMode.WAIT
    NOT_BLOCKING    = SemaphoreMode.NO_WAIT
    CONCURRENT      = 2
    OVERRIDE        = 3


class Coupler:
    CPL0    = 0
    CPL1    = 1
    CPL2	= 2
    CPL3	= 3
    CPL4	= 4
    CPL5	= 5
    CPL6	= 6
    CPL7	= 7
    CPL8	= 8
    CPL9	= 9
    CPL10	= 10
    CPL11	= 11

class ClockDivider:
    DIV_128     = 128
    DIV_1024    = 1024
    DIV_8192    = 8192
    DIV_131072  = 131072


class LogEvent:
    EN_C1		= (1 << 0)	# Change of state on contact C1 (VCC)
    EN_C2		= (1 << 1)	# Change of state on contact C2 (Reset)
    EN_C3		= (1 << 2)	# Synchronous : Change of state on contact C3 (Strobe) or Asynchronous : Frequency change on C3 (Clock)
    EN_C4		= (1 << 3)	# Change of state on contact C4
    EN_C6		= (1 << 4)	# Change of state on contact C6
    EN_C7		= (1 << 5)	# Change of state on contact C7
    EN_C8		= (1 << 6)	# Change of state on contact C8
    EN_CHAR		= (1 << 7)	# Character received or transmitted
    EN_ETU		= (1 << 8)	# ETU change
    EN_AUXIN	= (1 << 9)	# Change of any auxiliary input (TRIG_IN, MDR_IN, TTL_IN1, TTL_IN2, TTL_IN3)
    EN_USER		= (1 << 10)	# Change of user events

class MP300Connector:
    def __init__(self,cplnum=Coupler.CPL1):
        self._cplnum = cplnum
        self.logger = logging.getLogger('mp300conn')
        self.logger.debug("__init__(), cplnum:%d",cplnum)
        self._address = "10779:192.168.107.80"
        self._atr = ""
        setdlltimeoutval(10)
    
            
    def __enter__(self):
        self.logger.debug("__ENTER__")
        return self


    def __exit__(self, exc_type, exc_value, traceback):
        self.logger.debug("__EXIT__")
        self.closedown()
    
    def closedown(self):
        self.logger.debug("closedown(), Switch off the power")
        mps_offcmm(self._cplnum)
        self.logger.debug("closedown(), Closing log")
        mps_closelog(self._cplnum)
        self.logger.debug("closedown(), End of download")
        mps_enddownload(self._cplnum)
        self.logger.debug("closedown(), Closing resource")
        mpos_closeresource(ResourceId.TC2,self._cplnum)
        #closecommunication
        closecomm()
                
    def connect(self):
        """
            connect()->None
            
            Create Connection to MP300 device
            
            Raises
            ------
            RuntimeError
                Something happen during connecting process, The args of RuntimeError class consist of
                args[0] = error message, args[1] = error code
        """
        try:
            self.logger.debug("connect(), opening communication at '%s'" % self._address)
            opencomm(self._address)
            # Open TC2 Resource
            self.logger.debug("connect(), open resource")
            mpos_openresource(ResourceId.TC2,self._cplnum, BlockingMode.NOT_BLOCKING)
            self.logger.debug("connect(), log clock selection")
            mps_logclockselect(self._cplnum, ClockMode.INTERNAL)
            self.logger.debug("connect(), set clock divider")
            mps_setclkdiv(self._cplnum, ClockDivider.DIV_8192);
            self.logger.debug("connect(), startdownloadto")
            startdownloadto(self._cplnum, "spy.mplog");
            self.logger.debug("connect(), opening log")
            mps_openlog(self._cplnum, LogEvent.EN_C1 | 
                                      LogEvent.EN_C2 | 
                                      LogEvent.EN_C3 | 
                                      LogEvent.EN_C7 | 
                                      LogEvent.EN_CHAR | 
                                      LogEvent.EN_ETU, 
                                      0)
            self.logger.debug("connect(),vcc selection")
            mps_vccselectmv(self._cplnum, 5000);
            self._atr= mps_oncmm(self._cplnum, 2000000);
            self.logger.debug("connect(), get atr and send pps, atr: %s",self._atr)
            mps_sendppscmm(self._cplnum, 0, 9, 6);
            self.logger.info(about())
        except ResourceMgmt as res:
            self.logger.error("Resource allocation is failed, errno=",res.args[0])
            #Try to overide resource and closing it down
            if(res.args[1] == 3902):
                mpos_openresource(ResourceId.TC2,self._cplnum,BlockingMode.OVERRIDE)
            mpos_closeresource(ResourceId.TC2,self._cplnum)
            #closecommunication
            closecomm()
            raise res

        except SpyMgmt as spy:
            self.logger.error("Spying failed, errno: ",spy.args[1])
            mps_closelog(self._cplnum)
            mps_enddownload(self._cplnum)
            mpos_closeresource(TC2,self._cplnum)
            #closecommunication
            closecomm()
            raise spy
        
        except RuntimeError as re:
            self.logger.error("Caught runtime error: %s, %d " % re.args)
            self.closedown()
            raise re

        except Exception as ex:
            self.logger.error("Caught unknown exception: %s, %d" % ex.args)
            self.closedown()       
            raise ex

    def transceive(self,apdu):
        self.logger.debug("transceive(), transceiving")
        dwcmd=int.from_bytes(apdu[0:4],byteorder='big',signed='False')
        lc=int(apdu[4])
        le = SendAPDU.NO_LE
        if(len(apdu)==5):
            le = lc
            lc = SendAPDU.NO_LC
            sendpdu=None
        else:
            sendpdu = bytes(apdu[5:5+lc])
        try:
            retval = mps_sendapdu(self._cplnum,dwcmd,lc,sendpdu,le)
        except RuntimeError as re:
            self.logger.error("Runtime Error, cause: %s, errno: %d" % re.args)
            retval = None
        return retval

    def disconnect(self):
        self.closedown()

    def reset(self):
        self.closedown()
        
        
        