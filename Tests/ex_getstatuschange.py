import _scard
import sys

ctx= _scard.context()
print("Get status change...")
lstatus = ctx.get_status_change(0xFFFFFFFF)
for i in lstatus:
    print("Current status : 0x%X" % (i.currentstate))
    print("Eventstate: 0x%X" % (i.eventstate))
    if((i.eventstate & _scard.SCARD_STATE_CHANGED) == _scard.SCARD_STATE_CHANGED):
        print("Scard state changed")
    if((i.eventstate & _scard.SCARD_STATE_PRESENT) == _scard.SCARD_STATE_PRESENT):
        print("Scard state present")
    if((i.eventstate & _scard.SCARD_STATE_EXCLUSIVE) == _scard.SCARD_STATE_EXCLUSIVE):
        print("SCARD state Exclusive")
    print()
print("exiting..")
sys.exit(0)