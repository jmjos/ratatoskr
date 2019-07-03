import unittest
import numpy as np
from interconnect.interconnect import Interconnect
from interconnect.driver import Driver


class test_interconnect_object(unittest.TestCase):
    def test_instant(self):
        print('\nTest Interconnect')
        driv = Driver.predefined()  # default 45nm driver
        ic_2D = Interconnect(9, 0.3e-6, 0.4e-6, 100e-6, metal_layer=6,
                             segments=1)
        self.assertEqual(ic_2D.wire_spacing, 0.3e-6)
        self.assertEqual(ic_2D.segments, 1)
        self.assertEqual(ic_2D.wire_width, 0.4e-6)
        self.assertEqual(ic_2D.wire_length, 100e-6)
        self.assertEqual(ic_2D.metal_layer, 6)
        self.assertEqual(driv.id, ic_2D.Driver.id)  # check on equality
        self.assertFalse(ic_2D.is_3D)
        self.assertEqual(ic_2D.power(f=1), ic_2D.E())
        self.assertEqual(ic_2D.area_3D, 0)
        t = ic_2D.max_metal_wire_length(f=1e9, factor=0.7)
        ic2_2D = Interconnect(8, 0.3e-6, 0.4e-6, t['Max Wire Length'],
                              segments=t['Segments'], metal_layer=6)
        np.testing.assert_almost_equal(ic2_2D.prop_delay(), 0.7e-9, decimal=11,
                                       err_msg="\nDesired delay != True delay")
        ic2_2D_worse = Interconnect(9, 0.3e-6, 0.4e-6, t['Max Wire Length'],
                                    segments=t['Segments']+1, metal_layer=6)
        self.assertTrue(ic2_2D_worse.prop_delay() > ic2_2D.prop_delay())
        with self.assertRaises(AttributeError):
            self.C_3D
            self._C_3D_g
            self.R_3D
            self.TSV_array_n
            self.TSV_array_m
            self.TSV_pitch
            self.TSV_radius
            self.KOZ
            self.ground_ring
        # # TEST 3D
        ic_3D = Interconnect(9, 0.3e-6, 0.4e-6, 100e-6, metal_layer=6,
                             segments=1, TSVs=True, Driver=driv)
        self.assertEqual(ic_2D.wire_spacing, ic_3D.wire_spacing)
        self.assertEqual(ic_2D.segments, ic_3D.segments)
        self.assertEqual(ic_2D.wire_width, ic_3D.wire_width)
        self.assertEqual(ic_2D.wire_length, ic_3D.wire_length)
        self.assertEqual(ic_2D.metal_layer, ic_3D.metal_layer)
        self.assertEqual(ic_2D.Driver.id, ic_3D.Driver.id)
        self.assertTrue(ic_3D.is_3D)
        self.assertEqual(ic_3D.power(f=1), ic_3D.E())
        self.assertTrue(ic_3D.area_3D > 0)
        self.assertTrue(ic_3D.power() > ic_2D.power())
        t2 = ic_3D.max_metal_wire_length(f=1e9, factor=0.7)
        self.assertTrue(t2['Max Wire Length'] < t['Max Wire Length'])
        self.assertTrue(t2['Segments'] <= t['Segments'])
        # print('\n Max Length with TSVs: %f [um]' % t2['Max Wire Length'])
        # print('\n Max Length withous TSVs: %f [um]' % t['Max Wire Length'])
        ic2_3D = Interconnect(9, 0.3e-6, 0.4e-6, t2['Max Wire Length'],
                              segments=t2['Segments'], metal_layer=6,
                              TSVs=True, Driver=driv)
        np.testing.assert_almost_equal(ic2_3D.prop_delay(), 0.7e-9, decimal=11,
                                       err_msg="\nDesired 3D delay != True delay")


if __name__ == '__main__':
    unittest.main()
