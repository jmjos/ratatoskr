import unittest
import numpy as np
import random
from interconnect import data


class test_data_module(unittest.TestCase):

    def test_functions(self):
        print('\n'+"Test Data module ")
        self.assertEqual(data.de2bis([10], 4), ['1010'])
        self.assertEqual(data.de2bis([-6], 4), ['1010'])

    def test_DataStream_object(self):
        a = data.DataStream([0, 0], 3)
        b = data.DataStream([7, 7], 3)
        c = data.DataStream([0, 7], 3)
        d = data.DataStream([1, 3], 3)
        self.assertTrue(
            np.array_equal(a.binary_samples_mat, [[0, 0, 0], [0, 0, 0]]))
        self.assertTrue(np.array_equal(
            b.binary_samples_mat, [[1, 1, 1], [1, 1, 1]]))
        self.assertTrue(np.array_equal(c.bit_prob_mat, 0.5*np.ones((3, 3))))
        self.assertTrue(np.array_equal(
            d.bit_prob_mat, [[0, 0, 0], [0, .5, .5], [0, .5, 1]]))
        with self.assertRaises(ValueError):
            data.DataStream(4, 2)

    def test_DataStreamProb_object(self):
        a = data.DataStream.from_stoch(
            1000, 8, uniform=1, ro=0.4)
        b = data.DataStream.from_stoch(
            1000, 8, uniform=0, ro=0.3, mu=0, log2_std=6)
        c = data.DataStream.from_stoch(1000, 8)  # random data
        a_prob = data.DataStreamProb(a)
        b_prob = data.DataStreamProb(b)
        c_prob = data.DataStreamProb(c)
        self.assertEqual(a_prob.B, a.B)
        self.assertEqual(b_prob.B, 8)
        self.assertTrue(
            np.array_equal(a_prob.toggle_prob_vec, a.toggle_prob_vec))
        self.assertFalse(
            np.array_equal(c_prob.toggle_prob_vec, b.toggle_prob_vec))
        self.assertTrue(np.array_equal(
            c_prob.corr_switching_mat, c.corr_switching_mat))
        self.assertFalse(np.array_equal(
            a_prob.corr_switching_mat, b_prob.corr_switching_mat))
        self.assertTrue(np.array_equal(
            b_prob.bit_prob_mat, b.bit_prob_mat))
        self.assertFalse(np.array_equal(
            a_prob.bit_prob_mat, c.bit_prob_mat))
        # testing of the multiplexing?
        # testing of the class method "from_prob"
        Ts, Tc, pr = [1, 2, 3]
        Ts_v, Tc_m, pr_m = np.ones(4), 2*np.ones((4, 4)), 3*np.ones((4, 4))
        a_prob = data.DataStreamProb.from_probs(Ts, Tc, pr, 4)
        b_prob = data.DataStreamProb.from_probs(Ts_v, Tc_m, pr_m, 4)
        with self.assertRaises(IndexError):
            data.DataStreamProb.from_probs(Tc_m, Tc_m, pr_m, 4)
            data.DataStreamProb.from_probs(Ts, Tc_m, pr_m, 4)

    def test_DataStream_interleaving(self):
        a = data.DataStream.from_stoch(
            10000, 8, uniform=0, ro=0.4, mu=0, log2_std=6)
        b = data.DataStream.from_stoch(
            10000, 8, uniform=0, ro=-0.3, mu=0, log2_std=6)
        c = data.DataStream.from_stoch(
            10000, 10, uniform=0, ro=-0.3, mu=0, log2_std=6)
        d1 = a.samples
        d2 = b.samples
        tr_pr_d1 = [.5, .3, .2, 0]  # trans probabilities from d1 to (d1, d2, h1, h2)
        tr_pr_d2 = [.5, .4, 0, .1]  # trans probabilities from d2 to (d1, d2, h1, h2)
        tr_pr_h1 = [.6, .3, .1, 0]  # trans probabilities from h1 to (d1, d2, h1, h2)
        tr_pr_h2 = [.1, .7, 0, .2]  # trans probabilities from h1 to (d1, d2, h1, h2)
        state = 0  # start in state d1
        tr_pr = tr_pr_d1
        values = np.array([d1[0]])
        d1 = np.delete(d1, 0)
        mux = np.zeros((4, 4))
        for i in range(10000-1):
            x = random.random()
            if x <= tr_pr[0]:
                values = np.append(values, d1[0])
                d1 = np.delete(d1, 0)
                next_state = 0
                next_tr_pr = tr_pr_d1
            elif x <= (tr_pr[0] + tr_pr[1]):
                values = np.append(values, d2[0])
                d2 = np.delete(d2, 0)
                next_state = 1
                next_tr_pr = tr_pr_d2
            elif x <= (tr_pr[0] + tr_pr[1]+tr_pr[2]):
                values = np.append(values, values[len(values)-1])
                next_state = 2
                next_tr_pr = tr_pr_h1
            else:
                values = np.append(values, values[len(values)-1])
                next_state = 3
                next_tr_pr = tr_pr_h2
            mux[state, next_state] += 1
            tr_pr = next_tr_pr
            state = next_state
        mux = mux / (10000-1)
        # self.assertEqual(sum(sum(mux)), 1)
        ab_muxed = data.DataStream(values, B=8, is_signed=a.is_signed)
        ab_muxed_theo = data.DataStreamProb([a, b], mux)
        with self.assertRaises(ValueError):
            data.DataStreamProb([a, c], mux)
        with self.assertRaises(IndexError):
            data.DataStreamProb([a, b], [0, 0])
        # the following lines will sometimes cause faulty test results due to rounding!!    
        np.testing.assert_almost_equal(ab_muxed.toggle_prob_vec, ab_muxed_theo.toggle_prob_vec, decimal=1,
                                       err_msg="\nMux Toggle Misprediction")
        np.testing.assert_almost_equal(ab_muxed.corr_switching_mat, ab_muxed_theo.corr_switching_mat, decimal=1,
                                       err_msg="\nMux Toggle Misprediction")
        np.testing.assert_almost_equal(ab_muxed.bit_prob_vec, ab_muxed_theo.bit_prob_vec, decimal=1,
                                       err_msg="\nMux Toggle Misprediction")


if __name__ == '__main__':
    unittest.main()
