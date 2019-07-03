# # # OLD STUFF # #  # # # # # # #
'''
from scipy.optimize import fsolve


a = 3
b = 2


def f(i):
    def f_int(x):
        return a*x*100e6-b-i
    return (f_int)


res = fsolve(f(1), 0)

print(res)
'''
# # # # # # # # # # # # # # # # #

from interconnect import Interconnect, Driver, DataStream

driv = Driver.predefined()
ic_2D = Interconnect(8, 0.3e-6, 0.3e-6)
