# Aprendizaje Profundo (Cursos IA FIUBA 2020)

Ejercicios en Python del Curso Aprendizaje Profundo.

### Clase 01 (03/09/2020). Red neuronal sin vectorizar (numpy)

- [Notebook](clase_1/clase01.ipynb)

### Clase 02 (10/09/2020). Red neuronal vectorizada Mini Batch (numpy)

- [Notebook versión no OO](clase_2/clase02.ipynb)
- [Notebook versión OO (keritas)](clase_2/clase02-keritas.ipynb)

### Clase 03 (17/09/2020). Red neuronal en Keras

Laboratorio:

- [Notebook](clase_3/clase03 - Modelo de Clase 2 en Keras.ipynb)
- [01 Introducción a la clasificación con Keras (Collab/drive)](clase_3/01_Introducción_a_clasificación_con_Keras.ipynb)
  - Key points: clasificador básico, efecto de lr en Adam

### Clase 04 (24/09/2020). DL con Keras

- Teoría:
     - Curva ROC
- Laboratorio:
  - [Detección de Anomalías con Keras](https://colab.research.google.com/drive/1KqFJQ1sYAdUwdO6X2QRaU9pwqkqz8p0D?usp=sharing)
- Links:
   - [ROC](https://en.wikipedia.org/wiki/Receiver_operating_characteristic)
       - [Discrimination Threshold](https://www.scikit-yb.org/en/latest/api/classifier/threshold.html#discrimination-threshold)
       - https://machinelearningmastery.com/plot-a-decision-surface-for-machine-learning/
       - [¿Qué es un Embedding?](https://www.youtube.com/watch?v=RkYuH_K7Fx4)
       - [Cross Entropy](https://www.youtube.com/watch?v=tRsSi_sqXjI)

### Clase 05 (01/10/2020). DL / embeddings

- Teoría (0-1:30):
     - KL Divergence 
          - [Ejemplo partiendo de comparación distribuciones](clase_5/Deep_Learning_Clase_5.ipynb)
          - [Optimizar DKl equivale a optimizar MV Cross Entropy](clase_5/pizarron/Deep Learning - Clase 5.pdf)
  - Softmax
  - M.V. aplicada al modelo polinomial:
    - One Hot Encoding
    - Negative Sampling
- Laboratorio (>1:45hs):
    - [Embeddings con Keras](https://colab.research.google.com/drive/1RFnCHmbbZne40qBHVZp9t9-oo9qCE25s?usp=sharing)
      - Enfoque 1) Hacer One Hot Encoding
      - Enfoque 2)
      - Curvas de entrenamiento con métricas Loss=MSE y Metric=MAE (02:25)
      - LabelEncoder para el Código Postal (02:28)
      - Capa Embedding y Flatten (02:32)
      - Chequeo de sanidad/validación
      - Ejercicio -> encontrar el error
        - Muchos parámetors para aprender con pocos datos (20.000 params con 4000 filas)?
- Links:
    - [Keras - Embedding Layers](https://www.kaggle.com/colinmorris/embedding-layers)

### Clase 06  (08/10/2020). Regularización y optimizadores/ autoencoders

- Teoría (0-135hs)
     - Regularización. 
          - Curvas AUC, los 3 casos en función de número de muestras
               - Caso 1Alto bias, alta varianza (underfitting)
               - Caso 2 Quiero bajar bias (overfitting)
               - Caso 3 Quiero bajar varianza (fit correcto)
          - Bias vs. Varianza.
          - L1, L2, Dropout, Bagging (bootstrapping + aggregation)
          - Interpretación matemática de la regularización.
- Laboratorio (>1:40hs):
  - [Sequential vs Functional]()
    - Limitaciones del modelo secuencial
    - Modelo funcional:
      - "Grafos diferenciables"
      - Definimos 1 o más inputs
      - Definimos 1 o más outputs
      - hidden1 = Dense(8,activation="relu")(input) # los () son flechas input->hidden1
      - ¿Para qué sirve? Por ejemplo para concatenar distintos tipos de datos (ejemplo de predicción de precios de casas con datos tabulares + imágenes)
  - [Autoencoders](clase_6/Autoencoders_con_Keras.ipynb) (01:52)
    - Espacio latente
    - Ejemplo en keras usando Functional
    - Memorización vs Overfitting
    - ¿Cómo se entrana, cuál es la función costo? (02:12)
    - Variational Autoencoder usa KL Divergence
    - Ejercicios (02:29)
  - 2:54 planning próximas clases
- Links:

     - [BiasVariance](http://scott.fortmann-roe.com/docs/BiasVariance.html)
          -[Variational Autoencoder](https://www.siarez.com/projects/variational-autoencoder)
     - [Asymetric Variational Autoencoders](https://arxiv.org/pdf/1711.08352.pdf)

### Clase 07 (16/10/2020). Ejercicio integrador End to End

- Laboratorio
  - Ejercicio integrador, key points:
    - Preprocesamiento
      - Encoding de vendor_ids (~26:09)
        - No hacer one-hot encoding de más de ~20 cols
        - 500 Ids, Candidato ideal para usar embeddings
    - Limpieza de datos
      - Tratamiento de NaNs (~38:00)
    - Ing. de Features Básica
      - Transformar tipo en one-hot encoding (dos clases) (41:00)
        - uso de pd.get_dummies()
        - Análisis de correlaciones (Pearson 1/-1= correlación lineal perfecta)
          - Antes: remover columnas que no sean de interés
          - Regla general:
            - si una variable se acerca a 1/-1, es una muy buena feature, pero si no lo está no por eso tirarla, combinada con otra(s) sí puede tener efecto y no estar reflejado en la correlación.
            - Si la varianza de una columna es cero, entonces sí se puede tirar.
        - Boxplot. Mención: test de varianza (01:03).
          - ¿Qué tan buena es la variable alcohol para determinar quality?
          - Grafico la distrución cuando quality=0 y cuando quality=1.
          - Miro el overlap de regiones.
        - Transformar vendor_id en vendor_idx (01:08)
    - Split en:
      - x_train,x_validation,x_test
      - y_train,y_validation,y_test
      - vendor_idx_train, vendor_idx_validation,vendor_idx_test
    - Normalizar entre 0 y 1 (MinMaxScaler)
    - Modelo Base (random) (01:42)
    - Entrenamiento de Modelos
      - Keras sin embeddings
        - Normalizamos para que entrene más rápido y evitar vanishing/exploding gradients
      - Keras con embeddings
    - Parte de puesta en producción (02:25)

Indexado

```python
_, vendor_idx = np.unique(vendor_id,return_inverse=True)
vendor_id2idx = {id:idx for id,idx in zip(vendor_id,vendor_idx)}
# Embeddings (ej: embeddings de 4)
id(bodega1) = 719843234 -> 0 -> [12,34,56,23]
id(bodega2) = 433243234 -> 1 -> [67,43,13,11] 
```

### Clase 08 (23/10/2020). Examen

- 

## Otros links y recursos de interés

- Misceláenos
  - [Clarifiying exceptions and visualizing tensor operations](https://explained.ai/tensor-sensor/index.html)
- Autoencoders
  - https://www.siarez.com/projects/variational-autoencoder
- Análisis exploratorio
  - para analisis exploratorio, tienen que conocer https://github.com/pandas-profiling/pandas-profiling
  - medir correlacion no lineal: https://minepy.readthedocs.io/en/latest/
- Debugging en Collab:
  - https://colab.research.google.com/github/jakevdp/PythonDataScienceHandbook/blob/master/notebooks/01.06-Errors-and-Debugging.ipynb#scrollTo=GWQXnPjUfxD_


## Notas varias, snippets útiles

Montar directorio en Google Drive.
~~~python
from google.colab import drive
import os
drive.mount('/content/drive')
os.chdir("/content/drive/My Drive/Colab Notebooks/")
!ls
~~~

