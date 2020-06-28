import numpy as np

from clase_1.ex4_metrics import BaseMetric
from clase_1.ex5_avg_query_precision import BaseQueryMetric

class MultiMetric(BaseMetric):
	def __init__(self,metrics):
		self.metrics = metrics

	def __call__(self, prediction, truth):
		result = np.zeros(len(self.metrics))
		for idx, metric in enumerate(self.metrics):
			result[idx] =  metric(prediction, truth)
		return result

class MultiQueryMetric(BaseQueryMetric):
	def __init__(self,metrics):
		self.metrics = metrics

	def __call__(self, predicted_rank, truth_relevance, query_ids):
		result = np.zeros(len(self.metrics))
		for idx, metric in enumerate(self.metrics):
			result[idx] =  metric(predicted_rank, truth_relevance, query_ids)
		return result		