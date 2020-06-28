import numpy as np
import pickle
import os
import csv

class Singleton(type):
    _instances = {}
    def __call__(cls, *args, **kwargs):
        if cls not in cls._instances:
            cls._instances[cls] = super(Singleton, cls).__call__(*args, **kwargs)
        return cls._instances[cls]

class MovieRatingsDatasetImpl:    
    ds_struct = [
        ('userId', np.int64),
        ('movieId', np.int64),
        ('rating', np.float32),
        ('timestamp', np.uint64)
    ]

    def __init__(self, filename, force_csv_reload=False):
        self.filename = filename
        pickle_filename = self.filename+".pkl"
        if os.path.isfile(pickle_filename) and force_csv_reload==False:
            self.__load_from_pkl(pickle_filename)
        else:
             self.__load_from_csv()
        self.cursor = 0                
        

    def __load_from_csv(self):
        print(f"Loading from CSV {self.filename}")
        
        with open(self.filename, newline='') as csvfile:
            results = csv.reader(csvfile)
            total_rows = 0
            for row in results:
                total_rows+=1
            print(f"Counted {total_rows} rows.")
            self.records = self.create_empty_records(total_rows)
            csvfile.seek(0)
            next(results)  # Skip header row.
            for idx, row in enumerate(results):
                self.records[idx]['userId'] = row[0]
                self.records[idx]['movieId'] = row[1]
                self.records[idx]['rating'] = row[2]
                self.records[idx]['timestamp'] = row[3]  
                #print( self.records[idx] )
            self.__dump()
    
    def __load_from_pkl(self, filename):
        print(f"Loading from PKL {filename}")
        with open(filename, 'rb') as file:
            self.records = pickle.load(file)
    
    def __dump(self):    
        pickle_filename = self.filename+".pkl"
        with open(pickle_filename, 'wb') as file:
            pickle.dump(self.records, file, protocol=pickle.HIGHEST_PROTOCOL)
        print(f"Dumped to {pickle_filename}")
    
    def create_empty_records(self, n):
        return np.empty(n, dtype=np.dtype(MovieRatingsDatasetImpl.ds_struct))

    def size(self):
        return self.records.shape[0] # 26024290

    def get_record(self, idx):
        return self.records[idx]


def MovieRatingsDatasetGenerator(datasource):
    cursor = 0
    while cursor < datasource.size():
        yield datasource.get_record(cursor)
        cursor += 1 

class MovieRatingsDataset(MovieRatingsDatasetImpl, metaclass=Singleton):
    pass