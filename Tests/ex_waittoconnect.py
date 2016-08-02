import _scard
import sys

ctx= _scard.context()
print("wait to connect")
lstatus = ctx.get_status_change(0xFFFFFFFF)
lstatus.event