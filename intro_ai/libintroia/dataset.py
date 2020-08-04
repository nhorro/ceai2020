import numpy as np
import matplotlib.pyplot as plt

# ------------------------------------------------------------------------------------------
# Dataset
# ------------------------------------------------------------------------------------------

class Data(object):
    
    def split(self, percentage=0.8):
        return NotImplemented
    

class Data1FeatXY(Data):    
    """
        Structured 1-Feature Numpy Array Dataset X,Y
    """
    def __init__(self, path, skip_header=False):
        self.dataset = self._build_dataset(path,skip_header)        
    
    def split(self, percentage=0.8):
        """ Particiona un dataset y devuelve las dos partes
        """
        dataset_sz = self.dataset.shape[0]
        i0 = int(dataset_sz*percentage)
        indices = np.random.permutation(dataset_sz)
        training_idx, validation_idx = indices[0:i0], indices[i0:]
        return self.dataset[training_idx], self.dataset[validation_idx]
    
    def _build_dataset(self,path,skip_header):
        """Construye un dataset de tipo X,Y con X de un único feature.
        """
        structure_dt = [('x', np.float32),('y', np.float32)]
        structure = np.dtype(structure_dt)
        with open(path, encoding="utf8") as dataset_csv:
            if skip_header:
                next(dataset_csv) # skip header
            gen = ((line.split(',')[1],line.split(',')[2]) for line in dataset_csv)
            samples = np.fromiter(gen, structure)
        return samples



class DataXY(Data):    
    """
        N-Feature Numpy Array Dataset X,Y.
    """
    def __init__(self, path, skip_header=False,delimiter=','):
        self.dataset = self._build_dataset(path,skip_header,delimiter)        
    
    def split(self, percentage=0.8):
        """ Particiona un dataset y devuelve las dos partes
        """
        dataset_sz = self.dataset.shape[0]
        i0 = int(dataset_sz*percentage)
        indices = np.random.permutation(dataset_sz)
        training_idx, validation_idx = indices[0:i0], indices[i0:]
        return self.dataset[training_idx], self.dataset[validation_idx]
    
    def _build_dataset(self,path,skip_header,delimiter):
        return np.genfromtxt(path, skip_header=skip_header, delimiter=delimiter)        