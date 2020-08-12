import numpy as np
import matplotlib.pyplot as plt

def get_distances(X, C):
    """Obtiene para cada vector de X la distancia a cada vector de C.
    X -- Array. Cada fila es un vector
    C -- Array. Cada fila es un vector
    """
   
    """
    Ignorando el cuadrado y raíz por el momento, quiero calcular distances = X - C, tal que:

        ~~~
        D = [
            X[0] - C[0],
            X[0] - C[1],
            X[1] - C[0],
            X[1] - C[1],
            X[2] - C[0],
            X[2] - C[1]
        ]
        ~~~

        siendo:

        ~~~
            X[0] - C[0] = [1,2,3] - [1,0,0] = [0,-2,3],
            X[0] - C[1] = [1,2,3] - [1,0,0] = [0,2,3],
            ...
        ~~~
        
    Según https://scipy.github.io/old-wiki/pages/EricsBroadcastingDoc:
    
    > In order to broadcast, the size of the trailing axes for both arrays 
    > in an operation must either be the same size or one of them must be one.

    Si intento restar directamente (3,3) - (2,3) Falla la regla de broadcast porque 3!=2
    y ninguno de los dos es 1.
    Pero si en cambio pruebo con (3,3)-(2,1,3) la regla de broadcasting permite calcular
    (1,3)-(1,3) sobre el eje 0 (filas):
        Operation Axis (0) Size       Trailing Axis Size
        3                            (3) automatically reshaped to (1,3)
        2                            (1,3)
        
    Nota: interpreto que es el (1,3) de dimensión 2 el que lleva a expandir el (3) a (1,3).
    """
    expanded_C = C[:, None]
    return np.sqrt(np.sum((expanded_C - X) ** 2, axis=-1))

# Ejercicio 2
def get_nearest(distances):
    return np.argmin(distances,axis=0)    



def build_cluster(n_samples, inv_overlap):
    """
    Genera muestras pertenecientes a dos clusters.
    n_samples -- Cantidad de muestras.
    inv_overlap -- Distancia de separación.    
    """
    centroids = np.array([
        [1,0,0,0],
        [0,1,0,0],
    ], dtype=np.float32)
    centroids = centroids * inv_overlap
    data = np.repeat(centroids, n_samples / 2, axis=0)
    normal_noise = np.random.normal(loc=0, scale=1, size=(n_samples, 4))
    data = data + normal_noise
    cluster_ids = np.array([[0],[1],])
    cluster_ids = np.repeat(cluster_ids, n_samples / 2, axis=0)
    return data, cluster_ids


# Ejercicio 3

MAX_ITERATIONS = 10

def k_means(X, n_clusters,max_iterations=MAX_ITERATIONS):
    centroids = np.eye(n_clusters, X.shape[1])
    #print(centroids)
    for i in range(max_iterations):
        #print("Iteration # {}".format(i))
        centroids, cluster_ids = k_means_loop(X, centroids)
        #print(centroids)
    return centroids, cluster_ids

def k_means_loop(X, centroids):
    # find labels for rows in X based in centroids values
    expanded_centroids = centroids[:, None]
    distances = np.sqrt(np.sum((expanded_centroids - X) ** 2, axis=2))
    arg_min = np.argmin(distances, axis=0)
    # recompute centroids
    for i in range(centroids.shape[0]):
        centroids[i] = np.mean(X[arg_min == i, :], axis=0)
    return centroids, arg_min

# Ejercicio 5
def exponential_random_variable(lambda_param, size):
    u = np.random.uniform(low=0.0, high=1.0, size=size)
    return (-1 / lambda_param) * np.log(1 - u)    

# Ejercicio 6 
def cubic_random_variable(size):
    u = np.random.uniform(low=0.0, high=1.0, size=size)
    return u**(1/3)    

# Ejercicio 7
def zscore_norm(dataset):
    """ Normaliza un dataset c/ z-score
    -- dataset: mxn, m=muestras,n=features
    """
    feat_mean = np.mean(data,axis=0)   
    feat_std = np.std(data,axis=0)
    return (data-feat_mean)/feat_std    

# Ejercicio 8
def my_pca(x):
    x2 = (x - x.mean(axis=0))
    cov_1 = np.cov(x2.T)
    w, v = np.linalg.eig(cov_1)
    idx = w.argsort()[::-1]
    w = w[idx]
    v = v[:,idx]
    return np.matmul(x2, v[:, :2])        



# Función helper para poblar dataset con NaNs
def corrupt_dataset(dataset, percent,feat_range):   
    """Corrompe un dataset con NaNs
        -- percent: porcentaje de 0=0%, 1=100% a corromper
        -- feat_range: tupla con intervalo de columnas a corromper
    """
    dataset_sz = dataset.shape[0]
    dataset_feat = dataset.shape[1]
    alt_row_indexes = np.random.randint(low=0, high=dataset_sz, size=int(dataset_sz*percent), dtype='l')  
    alt_col_indexes = np.random.randint(
        low=feat_range[0], 
        high=feat_range[1], 
        size=int(dataset_sz*percent), dtype='l')  
    dataset[alt_row_indexes,alt_col_indexes] = None
    return dataset        


def drop_rows_with_nan(corrupted_dataset):
    """Elimina filas que tengan NaNs
    """
    return corrupted_dataset[np.all(np.isfinite(corrupted_dataset), axis=1)]

def drop_cols_with_nan(corrupted_dataset):
    """Elimina columnas que tengan NaNs
    """
    return corrupted_dataset[:,np.all(np.isfinite(corrupted_dataset), axis=0)]    


# Ejercicio 10
def replace_nans_with_mean(dataset):
    filtered_rows = drop_rows_with_nan(dataset)
    col_avg = filtered_rows.mean(axis=0)
    nan_idx = np.where(np.isnan(dataset))
    dataset[nan_idx] = np.take(col_avg,nan_idx[1])
    return dataset    

# Ejercicio 11
def split_dataset(dataset, training=0.7, validation=0.2,testing=0.1):
    """ Dado un dataset, lo particiona en training, validation y testing y devuelve estos tres arrays.
    """
    dataset_sz = dataset.shape[0]
    i0 = int(dataset_sz*training)
    i1 = int(dataset_sz*(training+validation))
    indices = np.random.permutation(dataset_sz)
    training_idx, validation_idx, testing_idx = indices[0:i0], indices[i0:i1],indices[i1:]    
    return dataset[training_idx], dataset[validation_idx], dataset[testing_idx]    


def plot_clusters(compressed_dataset,cluster_ids):
    plt.figure(figsize=(12,8))
    plt.scatter(compressed_dataset[cluster_ids==0,0],compressed_dataset[cluster_ids==0,1])
    plt.scatter(compressed_dataset[cluster_ids==1,0],compressed_dataset[cluster_ids==1,1])
    plt.legend(["cluster 0", "cluster 1"])
    plt.title("Clusters de dataset comprimido con PCA")
    plt.xlabel("X")
    plt.ylabel("Y")
    plt.grid(which="Both")
    plt.show()    