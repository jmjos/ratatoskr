import unittest
import numpy as np
from interconnect.driver import Driver


class test_Driver_object(unittest.TestCase):
    def test_instant(self):
        print('\n'+"Test Driver module ")
        R_on = np.array([1.95, 1.57, 1.55])*1e3  # on resistance (WC)
        t_0 = np.array([26.4, 27.7, 28])*1e-12  # delay offset (WC)
        E_toggle = np.array([2.85, 2.95, 3])*1e-15  # mean energy for lh and hl (TC)
        boundaries = np.array([0, 5.02, 129.5])*1e-15
        C_in = 1.2e-15  # gate capacitance (TC)
        A = 1.5876e-12  # Area in [um]*[um]
        V_dd = 0.9
        id = 'test_Driver'
        test_dr = Driver(R_on, t_0, C_in, boundaries, E_toggle, V_dd, A, id)
        self.assertEqual(C_in, test_dr.C_in)
        self.assertEqual(id, test_dr.id)
        self.assertEqual(id, test_dr.id)
        self.assertEqual(V_dd, test_dr.V_dd)
        self.assertEqual(A, test_dr.area)
        self.assertEqual(R_on[0], test_dr.R_on(0))
        self.assertEqual(t_0[0], test_dr.t_0(0))
        self.assertEqual(R_on[1], test_dr.R_on(boundaries[1]))
        self.assertEqual(t_0[1], test_dr.t_0(boundaries[1]))
        self.assertEqual(R_on[2], test_dr.R_on(boundaries[2]))
        self.assertEqual(t_0[2], test_dr.t_0(boundaries[2]))
        self.assertEqual(R_on[2], test_dr.R_on(10))
        self.assertEqual(t_0[2], test_dr.t_0(10))
        self.assertEqual(0.5*(R_on[1]+R_on[0]),
                         test_dr.R_on(0.5*boundaries[1]))
        self.assertEqual(0.5*(t_0[1]+t_0[0]), test_dr.t_0(0.5*boundaries[1]))
        self.assertEqual(E_toggle[0]*0.5e9, test_dr.mean_power(C_load=0))
        self.assertEqual(E_toggle[1]*0.5e9,
                         test_dr.mean_power(C_load=boundaries[1]))
        self.assertEqual(E_toggle[2]*0.5e9,
                         test_dr.mean_power(C_load=boundaries[2]))
        self.assertEqual(E_toggle[0]*1e9, test_dr.mean_power(C_load=0, f=2e9))
        self.assertEqual(E_toggle[0]*2e9,
                         test_dr.mean_power(C_load=0, f=2e9, toggle_activity=1))
        self.assertTrue(np.array_equal(np.array([E_toggle[0], E_toggle[0]])*0.5e9,
                                       test_dr.mean_power(C_load=0,
                                                          toggle_activity=np.array([0.5, 0.5]))))
        test_dr2 = Driver.predefined()
        self.assertEqual(test_dr2.C_in, test_dr.C_in)
        self.assertFalse(test_dr2.id == test_dr.id)
        self.assertEqual(test_dr2.V_dd, test_dr.V_dd)
        self.assertEqual(test_dr2.area, test_dr.area)
        self.assertEqual(test_dr2.R_on(0), test_dr.R_on(0))
        self.assertEqual(test_dr2.t_0(0), test_dr.t_0(0))
        self.assertEqual(test_dr2.R_on(boundaries[1]), test_dr.R_on(boundaries[1]))
        self.assertEqual(test_dr2.R_on(0.5*boundaries[1]),
                         test_dr.R_on(0.5*boundaries[1]))
        self.assertEqual(test_dr2.t_0(0.5*boundaries[1]), test_dr.t_0(0.5*boundaries[1]))
        self.assertEqual(test_dr2.mean_power(C_load=0), test_dr.mean_power(C_load=0))


if __name__ == '__main__':
    unittest.main()
