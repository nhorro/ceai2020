from unittest import TestCase

import numpy as np

from clase_1.ex6_avg_query_precision_at_k import QueryMeanPrecisionAtK


class MetricsTestCase(TestCase):

    def test_query_mean_precision_at_k(self):        
    	# TODO
        q_id = np.array( [1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 4] )
        predicted_rank = np.array([0, 1, 2, 3, 0, 1, 2, 0, 1, 2, 3, 4, 0, 1, 2, 3])
        truth_relevance = np.array([1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1 ])
        k = 3
        metric = QueryMeanPrecisionAtK(k)
        result = metric(predicted_rank, truth_relevance, q_id)
        expected = ((2/k) + (3/k) + (0/k) + (1/k)) / 4
        np.testing.assert_equal(result, expected)