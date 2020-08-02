import numpy as np
import matplotlib.pyplot as plt

# ------------------------------------------------------------------------------------------
# Dataset
# ------------------------------------------------------------------------------------------

class Data(object):    
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

# ------------------------------------------------------------------------------------------
# Models
# ------------------------------------------------------------------------------------------
    
class BaseModel(object):

    def __init__(self):
        self.model = None

    def fit(self, X, Y):
        return NotImplemented

    def predict(self, X):
        return NotImplemented

    def get_model(self):
        return self.model


class ConstantModel(BaseModel):

    def fit(self, X, Y):
        W = Y.mean()
        self.model = W

    def predict(self, X):
        return np.ones(len(X)) * self.model


class LinearRegression(BaseModel):

    def fit(self, X, y):
        if len(X.shape) == 1:
            W = X.T.dot(y) / X.T.dot(X)
        else:
            W = np.linalg.inv(X.T.dot(X)).dot(X.T).dot(y)
        self.model = W

    def predict(self, X):
        return self.model * X


class LinearRegressionWithB(BaseModel):

    def fit(self, X, y):
        X_expanded = np.vstack((X, np.ones(len(X)))).T
        W = np.linalg.inv(X_expanded.T.dot(X_expanded)).dot(X_expanded.T).dot(y)
        self.model = W

    def predict(self, X):
        X_expanded = np.vstack((X, np.ones(len(X)))).T
        return X_expanded.dot(self.model)       

# ------------------------------------------------------------------------------------------
# Metrics
# ------------------------------------------------------------------------------------------
    
class Metric(object):    
    def __call__(self,target,prediction):
        return NotImplemented
    
class MSE(Metric):
    def __call__(self,target,prediction):
        assert(target.shape==prediction.shape)
        n = target.shape[0]
        return np.sum(np.square(prediction-target))/n        


def k_folds(X_train, y_train, k=5):
    l_regression = LinearRegression()
    error = MSE()

    chunk_size = int(len(X_train) / k)
    mse_list = []
    for i in range(0, len(X_train), chunk_size):
        end = i + chunk_size if i + chunk_size <= len(X_train) else len(X_train)
        new_X_valid = X_train[i: end]
        new_y_valid = y_train[i: end]
        new_X_train = np.concatenate([X_train[: i], X_train[end:]])
        new_y_train = np.concatenate([y_train[: i], y_train[end:]])

        l_regression.fit(new_X_train, new_y_train)
        prediction = l_regression.predict(new_X_valid)
        mse_list.append(error(new_y_valid, prediction))

    mean_MSE = np.mean(mse_list)

    return mean_MSE


# ------------------------------------------------------------------------------------------
# Optimizers
# ------------------------------------------------------------------------------------------

def gradient_descent(X_train, y_train, lr=0.01, amt_epochs=100):
    """
    shapes:
        X_t = nxm
        y_t = nx1
        W = mx1
    """
    n = X_train.shape[0]
    m = X_train.shape[1]

    # initialize random weights
    W = np.random.randn(m).reshape(m, 1)

    for i in range(amt_epochs):
        prediction = np.matmul(X_train, W)  # nx1
        error = y_train - prediction  # nx1

        grad_sum = np.sum(error * X_train, axis=0)
        grad_mul = -2/n * grad_sum  # 1xm
        gradient = np.transpose(grad_mul).reshape(-1, 1)  # mx1

        W = W - (lr * gradient)

    return W


def stochastic_gradient_descent(X_train, y_train, lr=0.01, amt_epochs=100):
    """
    shapes:
        X_t = nxm
        y_t = nx1
        W = mx1
    """
    n = X_train.shape[0]
    m = X_train.shape[1]

    # initialize random weights
    W = np.random.randn(m).reshape(m, 1)

    for i in range(amt_epochs):
        idx = np.random.permutation(X_train.shape[0])
        X_train = X_train[idx]
        y_train = y_train[idx]

        for j in range(n):
            prediction = np.matmul(X_train[j].reshape(1, -1), W)  # 1x1
            error = y_train[j] - prediction  # 1x1

            grad_sum = error * X_train[j]
            grad_mul = -2/n * grad_sum  # 2x1
            gradient = np.transpose(grad_mul).reshape(-1, 1)  # 2x1

            W = W - (lr * gradient)

    return W


def mini_batch_gradient_descent(X_train, y_train, lr=0.01, amt_epochs=100):
    """
    shapes:
        X_t = nxm
        y_t = nx1
        W = mx1
    """
    b = 16
    n = X_train.shape[0]
    m = X_train.shape[1]

    # initialize random weights
    W = np.random.randn(m).reshape(m, 1)

    for i in range(amt_epochs):
        idx = np.random.permutation(X_train.shape[0])
        X_train = X_train[idx]
        y_train = y_train[idx]

        batch_size = int(len(X_train) / b)
        for i in range(0, len(X_train), batch_size):
            end = i + batch_size if i + batch_size <= len(X_train) else len(X_train)
            batch_X = X_train[i: end]
            batch_y = y_train[i: end]

            prediction = np.matmul(batch_X, W)  # nx1
            error = batch_y - prediction  # nx1

            grad_sum = np.sum(error * batch_X, axis=0)
            grad_mul = -2/n * grad_sum  # 1xm
            gradient = np.transpose(grad_mul).reshape(-1, 1)  # mx1

            W = W - (lr * gradient)

    return W        