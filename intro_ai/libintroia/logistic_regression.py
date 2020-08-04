import sys
import numpy as np
from libintroia import Model

def sigmoid(z):
    return 1 / (1 + np.exp(-z))

class LogisticRegression(Model):
    def __init__(self, lr=0.0001, num_iter=1000):
        self.lr = lr
        self.num_iter = num_iter
        self.w = None
        self.b = None
    
    def fit(self, X, y):     
        m = X.shape[0]
        n_feat = X.shape[1]
        
        self.theta = np.zeros((n_feat,1))
        self.b = np.random.rand()
        
        #X_aug = np.hstack((X,np.ones((X.shape[0],1))))
        #self.theta = np.matrix([np.zeros(n_feat),np.ones(n_feat)]).T
        
        for i in range(self.num_iter):
            A = np.dot(X,self.theta) + self.b 
            h = sigmoid(A)            
            dtheta = np.dot(X.T, (h - y)) / m    
            db = (1 / m) * np.sum(h - y)
            self.theta = self.theta - self.lr * dtheta
            self.b = self.b - self.lr * db    
        return self.theta
    
    def predict_prob(self, X):
        return sigmoid(np.dot(X,self.theta))
    
    def predict(self, X, threshold=0.49):
        return (self.predict_prob(X) < threshold)