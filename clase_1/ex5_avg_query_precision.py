import numpy as np

class BaseQueryMetric:
    def __call___(self, predicted_rank, truth_relevance, query_ids):
        raise NotImplementedError


class QueryMeanPrecision(BaseQueryMetric):
    def __call__(self, predicted_rank, truth_relevance, query_ids):
        """
        Definition:
        Mean Precision is the mean of the Precision of each query
        Precision for a query is the number of true positives documents divided by the number of fetched documents
        Precision with value 1 means perfect precision for the query
        """
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
        # get the count of fetched documents
        fetched_documents_count = np.bincount(query_ids)[unique_query_ids]
        # compute the metric
        precision_by_query = true_relevance_count_by_query / fetched_documents_count
        return np.mean(precision_by_query)