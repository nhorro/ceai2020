from unittest import TestCase

import numpy as np

from clase_1.ex7_allmetrics import MultiMetric, MultiQueryMetric
from clase_1.ex4_metrics import Precision, Accuracy, Recall
from clase_1.ex5_avg_query_precision import QueryMeanPrecision
from clase_1.ex6_avg_query_precision_at_k import QueryMeanPrecisionAtK


class MetricsTestCase(TestCase):

    def test_all_metrics(self):
        truth = np.array([1,1,0,1,1,1,0,0,0,1])
        prediction = np.array([1,1,1,1,0,0,1,1,0,0])
        metric = MultiMetric(metrics= [Precision(), Accuracy(), Recall()])
        result = metric(prediction, truth)
        expected = [ 0.5, 0.4, 0.5]
        np.testing.assert_equal(result, expected)

    def test_all_query_metrics(self):
        q_id = np.array( [1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 4] )
        predicted_rank = np.array([0, 1, 2, 3, 0, 1, 2, 0, 1, 2, 3, 4, 0, 1, 2, 3])
        truth_relevance = np.array([1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1 ])
        k = 3
        metric = MultiQueryMetric(metrics= [QueryMeanPrecision(), QueryMeanPrecisionAtK(k)])
        result = metric(predicted_rank, truth_relevance, q_id)
        expected = [
                        ((2/4) + (3/3) + (0/5) + (2/4)) / 4,
                        ((2/k) + (3/k) + (0/k) + (1/k)) / 4
                    ]
        np.testing.assert_equal(result, expected)