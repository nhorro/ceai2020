from unittest import TestCase

import numpy as np
import os

from clase_1.ex8_singleton_dataset import MovieRatingsDataset, MovieRatingsDatasetGenerator
 
class SingletonDatasetTestCase(TestCase):
    """Testcase no exhaustivo. No se prueba carga de PKL, etc.
    """

    CSV_FILENAME = "data/ratings.csv"

    def test_single_instance(self):        
        ds1 = MovieRatingsDataset(SingletonDatasetTestCase.CSV_FILENAME)
        ds2 = MovieRatingsDataset(SingletonDatasetTestCase.CSV_FILENAME) 

        np.testing.assert_equal( id(ds1), id(ds2) )
    
    def test_load_csv(self):
        ds = MovieRatingsDataset(SingletonDatasetTestCase.CSV_FILENAME,force_csv_reload=True)        
        
        np.testing.assert_equal(ds.size(), 28)
        np.testing.assert_equal(ds.size(), 28)

        # Leer primer fila 
        gen = MovieRatingsDatasetGenerator(ds)        

        expected = ds.create_empty_records(1)
        expected[0]['userId'] = 1
        expected[0]['movieId'] = 110
        expected[0]['rating'] = 1.
        expected[0]['timestamp'] = 1425941529
        result = next(gen)
        np.testing.assert_equal(result, expected)