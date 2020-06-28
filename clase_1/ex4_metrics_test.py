from unittest import TestCase

import numpy as np

from clase_1.ex4_metrics import Precision, Accuracy, Recall


class MetricsTestCase(TestCase):

    def test_precision(self):

        truth = np.array([1,1,0,1,1,1,0,0,0,1])
        prediction = np.array([1,1,1,1,0,0,1,1,0,0])
        metric = Precision()
        result = metric(prediction, truth)
        expected = 0.5
        np.testing.assert_equal(result, expected)

    def test_accuracy(self):
        truth = np.array([1,1,0,1,1,1,0,0,0,1])
        prediction = np.array([1,1,1,1,0,0,1,1,0,0])
        metric = Accuracy()
        result = metric(prediction, truth)
        expected = 0.4
        np.testing.assert_equal(result, expected)

    def test_recall(self):
        truth = np.array([1,1,0,1,1,1,0,0,0,1])
        prediction = np.array([1,1,1,1,0,0,1,1,0,0])
        metric = Recall()
        result = metric(prediction, truth)
        expected = 0.5
        np.testing.assert_equal(result, expected)                