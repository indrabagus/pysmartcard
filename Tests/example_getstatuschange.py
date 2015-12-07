import _scard
import sys

ctx= _scard.context()
print("Get status change...")
lstatus = ctx.get_status_change(2000)
for i in lstatus:
    print("Current status : %X" % (i.currentstate))
    print("Eventstate: %X" % (i.eventstate))
print("exiting..")
sys.exit(0)