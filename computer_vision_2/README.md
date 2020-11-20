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

- VGG-16
  - ILSVRC 2014
- Keras, método funcional de armar redes (ver diapo)
  - Definir input: 
  - Definir capas ocultas
  - Definir outputs
  - Instanciar el modelo
- Primer laboratorio: https://colab.research.google.com/drive/1s7dZNm-b5egx65NuDoM57VtmrrEwFP7W
  - Escribir VGG usando Keras functional.
- Problemas al entrenar redes muy profundas
  - Gradientes que explotan o tienden a cero
    - Para este problema es común usar Batch Normalization
  - Degradación de la performance de entrenamiento
    - Vamos a ver Redes residuales para esto.
- Batch Normalization
- Redes residuales
- Bloque residual
- Transfer learning

### Tarea

#### Ejercicio 1. 

Ver [Notebook VGG16 con Keras Functional](clase_3/CV2_Clase3_01_VGG16_con_Keras.ipynb)

#### Ejercicio 2.

Ver [Notebook Red Normal vs ResNet](clase_3/CV2_Clase_03_02_red_normal_vs_resnet.ipynb)

#### Ejercicio 3.

Ver [Notebook Transfer Learning con Keras](clase_3/CV2_Clase03_03_Transfer_Learning_Keras.ipynb)

## Clase 4. Viernes 13/11/2020

### Key points

- ResNet. 
  - Versiones de ResNet50
  - Explicación: uso de convolución en Skip Connection para reducir la dimensión.
  - Ejercicio de Programación: link?
- Red Inception
  - Concepto: múltiples operaciones a la vez en un mismo bloque.
  - Reducción de necesidad de cómputo utilizando convolución 1x1
  - Ejercicio de programación: https://colab.research.google.com/drive/1oA6MFtQXWo_xAWbqbm2wWVDKnwrqTPuy
- Tareas de visión por computadora:
  - Clasificación
  - Localización
  - Clasificación + Localización
  - Detección (múltiples objetos de múltiples categorías)
- Medidas de evaluación:
  - mAP: mean Average Precision para detección de objetos (hay otra definición para document retrieval)
  - El algoritmo debe encontrar los objetos (ubicarlos en bounding boxes) y clasificarlos.
  - Tenemos que evaluar cuán correctas son las bounding boxes y cuán correcta es la clasificaciónDescripción original, página 11 de: http://homepages.inf.ed.ac.uk/ckiw/postscript/ijcv_voc09.pdf
  - Curvas de Precision/Recall
- ¿Cómo implementar sliding windows de manera convolucional?
- Intersection over Union (o Jaccard Index)
- YOLO
  - Grilla:
    - Cada celda de la grilla se representa por un vectorUn objeto es asignado a una celda si su centro está dentro de ella
    - Volumen de salida: #celdas_w x #celdas_h x (1 + 4 + #clases)
    - Comparado a sliding windows:
      - permite determinar una bounding box mucho más precisa
      - Es convolucional
  - Anchor boxes
- Trabajo Final:
  - Competencia de Metadata:https://metadata.fundacionsadosky.org.ar/competition/22/
  - Para el trabajo final hay que hacer un modelo para este problema, en grupos de 3 o 4 personas

### Tarea

#### Ejercicio 1. 

- Ver [Notebook resnet50_en_keras](clase_4/CV2_Clase04_resnet50_en_keras.ipynb)

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



