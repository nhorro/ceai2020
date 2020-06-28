import numpy as np

class Indexer(object):
    def __init__(self, ids):
        unique_ids = np.unique(ids)
        id2idx = np.ones(unique_ids.max() + 1, dtype=np.int64) * -1 # Inicializa todo en 0xFFFFFFFF (N/A)
        id2idx[unique_ids] = np.arange(unique_ids.size) # Carga cada id existentes con su índice
        self.id2idx = id2idx
        self.idx2id = unique_ids

    def get_idxs(self, ids):
        ids = self.id2idx[ids]
        return ids, ids != -1
    
    def get_ids(self, idxs):
        return self.idx2id[idxs]
