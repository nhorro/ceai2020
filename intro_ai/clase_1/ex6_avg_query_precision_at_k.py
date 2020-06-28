import numpy as np

from clase_1.ex5_avg_query_precision import BaseQueryMetric

class QueryMeanPrecisionAtK(BaseQueryMetric):
    def __init__(self,k):
        self.k = k

    def __call__(self, predicted_rank, truth_relevance, query_ids):
        # get count of queries with at least one true relevant document
        true_relevance_mask = (truth_relevance == 1)
        filtered_query_id = query_ids[true_relevance_mask]
        filtered_true_relevance_count = np.bincount(filtered_query_id) # en las queries con relevance, cuento
        # complete the count of queries with zeros in queries without true relevant documents
        unique_query_ids = np.unique(query_ids)
        non_zero_count_idxs = np.where(filtered_true_relevance_count > 0)
        true_relevance_count = np.zeros(unique_query_ids.max() + 1)
        true_relevance_count[non_zero_count_idxs] = filtered_true_relevance_count[non_zero_count_idxs]
        # get the count only for existing queries
        true_relevance_count_by_query = true_relevance_count[unique_query_ids]
        # compute the metric
        print(true_relevance_count_by_query)
        precision_by_query_at_k = true_relevance_count_by_query / self.k
        return np.mean(precision_by_query_at_k)