from unittest import TestCase

import numpy as np

from clase_1.ex8_singleton_dataset import Dataset
 
class SingletonDatasetTestCase(TestCase):
    
    def test_load(self):
        """
        Nota: no se prueba:
            - Singleton
            - Mecanismo de carga
        """
        ds = Dataset("data/ratings.csv",force_csv_reload=True)
        np.testing.assert_equal(ds.size(), 26024290)

        #1,110,1.0,1425941529
        #for record in ds:
        #    print(record)