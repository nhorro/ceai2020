# -*- coding: utf-8 -*-
import numpy as np

class ExtendedKalmanFilter:
    """ Filtro de Kalman extendido
    """
    def __init__(self,X,P,H,R,compute_state_transition_jacobian_fn,dt=1):
        """ Constructor por defecto
        """
        self.X = X
        self.P = P
        self.H = H
        self.R = R
        self.dt = dt
        self.compute_state_transition_jacobian = compute_state_transition_jacobian_fn
        
        self.n_states = self.X.shape[0]
        self.n_measurements = self.R.shape[0]
        
        self.I = np.eye(self.n_states)
                
        assert(self.X.shape==(self.n_states,1))
        assert(self.P.shape==(self.n_states,self.n_states))
        assert(self.H.shape==(self.n_measurements,self.n_states))
        assert(self.R.shape==(self.n_measurements,self.n_measurements))
        assert(self.I.shape==(self.n_states,self.n_states))
    
    def update_and_predict(self,z, OTHER = None):
        """ Actualizar medición y predecir nuevo estado
        """
        Z = np.array([[z[0],z[1]]])
        assert(Z.shape==(1,self.n_measurements))
        
        y = Z.T - (self.H @ self.X)
        assert(y.shape==(self.n_measurements,1))
        
        S = self.H @ self.P @ self.H.T + self.R
        assert(S.shape==(self.n_measurements,self.n_measurements))        
        K = self.P @ self.H.T @ np.linalg.inv(S)
        
        self.X = self.X + (K@y)
        assert(self.X.shape==(self.n_states,1))
        
        self.P = (self.I - (K@self.H)) @ self.P
        assert(self.P.shape==(self.n_states,self.n_states))        
        
        x = self.X[0][0]
        y = self.X[1][0]
        dist = self.X[2][0]
        theta = self.X[3][0]
        dtheta = self.X[4][0]
        
        self.X = np.array([
            [ x + dist*np.cos(theta+dtheta)],
            [ y + dist*np.sin(theta+dtheta)],
            [dist],
            [theta+dtheta],
            [dtheta]      
        ])
        assert(self.X.shape==(self.n_states,1))
    
        A = self.compute_state_transition_jacobian(self.X,self.dt)
        assert(A.shape==(self.n_states,self.n_states))
        
        self.P = A @ self.P @ A.T
        assert(self.X.shape==(self.n_states,1))
        
        xy_estimate = [self.X[0][0],self.X[1][0]]
        return xy_estimate, OTHER

def compute_state_transition_jacobian(X,dt):
    x = X[0][0]
    y = X[1][0]
    dist = X[2][0]
    theta = X[3][0]
    dtheta = X[4][0]
    A = np.array([
        [1.0,  0.0 ,  np.cos(theta+dtheta), -dist*np.sin(theta+dtheta), -dist*np.sin(theta+dtheta) ],
        [0.0,  1.0 ,  np.sin(theta+dtheta),  dist*np.cos(theta+dtheta),  dist*np.cos(theta+dtheta) ],
        [0. ,  0.  ,  1.0                 ,  0.                       , 0.                         ],
        [0. ,  0.  ,  0.                  ,  1.                       , dt                         ],
        [0. ,  0.  ,  0.                  ,  0.                       , 1.0                        ]
    ])
    return A
