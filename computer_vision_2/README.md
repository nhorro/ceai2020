# Apuntes Visión por Computador II

## Clase 1. Viernes 23/10/2020

### Key points

- Clasificación, localización, detección.
- Redes Convolucionales
  - Cantidad de parámetros e invarianza.
  - Operación de convolución
  - Ejemplo: detección de bordes verticales.
  - Capas fundamentales:
    - CONV: Convolucional
    - POOL: Pooling
    - FC: Fully Connected
  - Hiperpárametros:
    - padding(p)
    - stride(s)
    - tamaño de los filtros (w,h)
    - cantidad de filtros
    - zero-padding. ¿Qué pasa cuando no hay padding? (diapo 25)
    - stride (zancada) 
  - Volumen de entrada y salida
  - Capa Convolucional
  - Capa Pooling (max o average)

### Tarea

#### Ejercicio 1. 

Ver [Notebook de Capas Convolucionales](clase_1/CV2_01_Capas_convolucionales.ipynb)

#### Ejercicio 2.

Ver [Notebook Red CNN simple y visualización de capas](clase_1/CV2_02_red_cnn_simple.ipynb)

#### Ejercicio 3.

Ver [Notebook Aplicación Keras y cálculo manual de HPs](clase_1/CV2_03_Modelo_convolucional_aplicacion_keras.ipynb)

## Clase 2. Viernes 30/10/2020

### Key points

- Plan:
  - Experimento: Pooling vs No usar Pooling
  - Dataset de Kaggle reducido. Primer intento de tener una red que clasifique perros y gatos.
  - Kahoot.
    - Para el 1er Kahoot tener fórmulas a mano.
  - Arquitecturas clásicas: LeNet, AlexNet
- Repaso clase pasada:
  - 3 capas importantes: conv, pool, fc. Repaso fórmulas para calcular HPs y output shape de cada tipo de capa.
- Ejercicio: https://colab.research.google.com/drive/135L0rpSA6RUXBKLsDUzzJ6hUgPvkap_-
- Kahoot preguntas de redes
- Break
- Data Augmentation con PCA
- LeNet
  - Ejercicio: implementar LeNet-5
- AlexNet
  - Características principales:
    - Rectified Linear Units (RELU)
    - Uso de múltiples GPU 
    - Dropout
    - Local Response Normalization, no tan usado hoy
    - Capas pool con ventanas superpuestas
    - Data augmentation

### Tarea

#### Ejercicio 1.

- En pool vs no pool agregar uno o dos bloques más con y sin pool para ver si hay diferencia.

[Notebook](clase_2/CV2_Clase_2_01_Con_pool_vs_sin_pool.ipynb)

#### Ejercicio 2

- Entrenar la red para perros y gatos y ver qué accuracy pueden obtener.

[Notebook](clase_2/CV2_Clase_2_02_Dataset_y_data_augmentation.ipynb)

#### Ejercicio 3

- Implementar LeNet-5 y utilizarla en MNIST.

[Notebook](clase_2/CV2_Clase_2_03_Lenet5Mnist.ipynb)

#### Bonus

- Leer el capítulo de redes convolucionales del libro de Goodfellow et al.
  https://www.deeplearningbook.org/contents/convnets.html
- Otro machete:
  https://github.com/afshinea/stanford-cs-230-deep-learning/blob/master/en/cheatsheet-convolut
  ional-neural-networks.pdf

### Próxima clase

- Redes residuales. ¿Cómo podemos hacer redes más profundas?
- Transfer Learning.

## Clase 3. Viernes 06/11/2020

### Key points

- TBD

### Tarea

- TBD

## Clase 4. Viernes 13/11/2020

### Key points

- TBD

### Tarea

- TBD

## Clase 5. Viernes 20/11/2020 

### Key points

- TBD

### Tarea

- TBD

## Clase 6. Viernes 27/11/2020

### Key points

- TBD

### Tarea

- TBD

## Clase 7. Viernes 04/12/2020 

### Key points

- TBD

### Tarea

- TBD

## Clase 8. Viernes 11/12/2020 (examen/proyecto final)



