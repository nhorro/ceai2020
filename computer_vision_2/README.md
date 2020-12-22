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

## Clase 4. Martes 17/11/2020

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
  - Permite reutilizar los cálculos del algoritmo de sliding windows
  - Hace los cálculos en paralelo
- Intersection over Union (o Jaccard Index)
- Cómo evaluar la salida de los modelos
  - Los modelos retornan para cada imagen una lista de : una bounding box con una clase predicha y un nivel de confianza
  - Las predicciones son asociadas a un objeto del ground truth si IoU > 0.5 (para Pascal VOC es 0.5,
    para COCO se usan una serie de niveles de IoU de corte)
  - Múltiples detecciones de un objeto son ranqueadas según el nivel de confianza asignado
  - Si hay varias predicciones para un objeto sólo una se considera correcta, las demás incorrectas (el
    algoritmo debería descartar múltiples detecciones)
  - Para cada clase del dataset se calcula la curva de precision/recall, que especifica el nivel de
    precisión (definida como proporción de los ejemplos por encima de un rango que pertenecen a la
    clase correcta) para un dado recall (proporción de los ejemplos positivos que aparecen por encima
    de un cierto rango)
  - Average precision resume la forma de la curva de precision/recall, y es definido como la precision
    media en un conjunto de 11 niveles de recall equiespaciados: [0,0.1,...,1] (en Pascal VOC)
  - Es una medida que se evalúa por cada clase a clasificar, o sea, debe medirse a nivel del dataset
  - Es bastante buena para comparar métodos diferentes
- Curva de precision/recall. Interpolación Pascal VOC. Promedio de 11 intervalos AP
- Algoritmos
  - Métodos de 2 etapas: R-CNN, Fast R-CNN, Faster R-CNN
  - Métodos de una etapa: YOLO, RetinaNet, SSD
- R-CNN
  - En la primera etapa, búsqueda selectiva (selective search): 
    - Se genera una sub-segmentación inicial, se generan muchas regiones candidatas
    - Se usa un algoritmo greedy para combinar recursivamente regiones similares en regiones mayores
    - Se usan las regiones generadas para producir las propuestas de regiones candidatas (region
      proposals), alrededor de 2000 por imagen
  - Segunda etapa:
    - Cada una de las regiones es pasada por una red convolucional que extrae features
    - En la capa final se utiliza una Support Vector Machine para clasificar cada región propuesta y
      asignarle un nivel de confianza a cada región
    - Se utiliza non-max suppression para cada clase para determinar las regiones que sobreviven
      paper original: Rich feature hierarchies for accurate object detection and semantic segmentation
- Non-max supression:
  - Primero descartamos las regiones que tengan confianza < umbral1 (e.g. 0.4)
  - Consideremos una clase y tomemos todas las regiones clasificadas como de esa clase para una
    imagen
  - Se ordenan las regiones por nivel de confianza,
    - se toma la que tiene mayor nivel de confianza, se eligen todas las que tienen IoU > umbral2 (e.g. 0.5) y son descartadas se toma la proxima region por nivel de confianza hasta terminar la lista
- YOLO
  - Grilla:
    - Cada celda de la grilla se representa por un vectorUn objeto es asignado a una celda si su centro está dentro de ella
    - Volumen de salida: #celdas_w x #celdas_h x (1 + 4 + #clases)
    - Comparado a sliding windows:
      - permite determinar una bounding box mucho más precisa
      - Es convolucional
  - Anchor boxes
    - p_c,b_x,b_y,b_h,b_w,c1,c2,c3
- Trabajo Final:
  - Competencia de Metadata:https://metadata.fundacionsadosky.org.ar/competition/22/
  - Para el trabajo final hay que hacer un modelo para este problema, en grupos de 3 o 4 personas

### Tarea

#### Ejercicio 1. 

- Ver [Notebook resnet50_en_keras](clase_4/CV2_Clase04_resnet50_en_keras.ipynb)

## Clase 5. Viernes 20/11/2020 

### Key points

#### Continuación YOLO

- Clase pasada vimos YOLO, pero no profundizamos en detalles del algoritmo.
- Plan:
  - Uso de YOLO
  - Intro a Reinforcement Learning
- Arquitectura YOLO v1
- Versiones de YOLO, comparativa
- YOLO, implementaciones:
  - notebook: https://colab.research.google.com/drive/1MeZzcfUMGfaAmslwpAxIuN5lSq1ZR0Vg?usp=sharing
  - Otros repos de python y tf2:
    - yolo v4: https://github.com/emadboctorx/yolo-tf2
    - yolo v3: https://github.com/zzh8829/yolov3-tf2
  - Ejemplo de construcción de yolo v1 simple:
    ○ colab: https://colab.research.google.com/drive/1PCqlip7wt3fZ

- Sección: comentarios sobre Trabajo Final

### Introducción a Reinforcement Learning

- Plan:
  - Qué es Aprendizaje Reforzado (Reinforcement Learning)
  - Relación con otros campos
  - Diferencias con otros paradigmas de ML
  - Características propias de Aprendizaje Reforzado
  - Definición del problema
  - Ejemplos
  - Review de matemática
  - Ejercicios de programación
  - Software
  - Intro a Open AI Gym
  - Bonus: Lecturas recomendadas
- Qué es Aprendizaje Reforzado (Reinforcement Learning)
  - El problema central: aprendizaje por prueba y error a través de la
    interacción con el ambiente
  - El objetivo es aprender a ‘qué hacer’ -, o sea cómo mapear acciones a
    situaciones - para maximizar una señal de recompensa
  - El nombre se refiere a un problema, una clase de métodos de solución,
    el campo de estudio del problema
  - Al agente no se le dice lo que debe hacer, debe aprenderlo
    interactuando con el ambiente y recibiendo recompensas
  - Se deben tomar decisiones secuenciales de manera óptima
  - El agente obtiene información o ‘sensa’ u obtiene información a través
    de sus ‘sentidos’ o aparatos de medición
  - A través de esa interacción con el ambiente el agente obtiene
    información y aprende sobre el comportamiento del mundo
  - Con esa información decide qué acciones realizar en cada momento
  - El comportamiento es guiado por un objetivo que se modela en forma
    de una recompensa numérica
  - De nuevo, el objetivo la toma de decisiones secuenciales de manera
    óptima, esto es: maximizar la recompensa acumulada
- Diferencias con otros paradigmas de ML, RL vs Aprendizaje supervisado
  - No viene dado un dataset con inputs y targets
  - No hay un ‘supervisor’, o sea input y targets, hay una señal de
    recompensa
  - El feedback se recibe con retraso, no es instantáneo
  - Las decisiones son secuenciales, los datos no son i.i.d.
  - Las acciones del agente modifican los datos que va recibiendo
- Aprendizaje Reforzado
  - Considera el problema integral de un agente con un objetivo
    interactuando con el ambiente y aprendiendo a tomar decisiones
  - En ese sentido tanto aprendizaje supervisado como no supervisado son
    en general parte de sistemas mayores y resuelven un subproblema
  - El campo de RL desarrolla técnicas y busca principios generales de
    aprendizaje que funcionen en este setup general de interacción con el
    entorno
  - Marco general: procesos de decisión markovianos
  - Objetivo: aprender a tomar decisiones de tal manera de maximizar las
    recompensas futuras acumuladas
  - Aspectos distintivos:
    -  Las acciones pueden tener consecuencias de largo plazo
    - Las recompensas pueden llegar con retraso y ser esparsas
    - Puede ser bueno sacrificar recompensa ahora para obtener más en
      el futuro
    - El agente afecta el ambiente a medida que toma decisiones
- Explotación vs Exploración
  ● Uno de los elementos fundamentales es el dilema de exploración vs
  explotación:
  - Explotación significa tomar la acción que creemos dado nuestro
    modelo actual nos dará más recompensa
  - Exploración es tomar acciones de tal manera de aprender más respecto
    al ambiente y las recompensas de cada acción
  - La dificultad de equilibrar entre exploración y explotación es otro
    problema distintivo de aprendizaje reforzado que no aparece en otros
    campos de machine learning
- Definición del problema en más detalle
  - Un agente interactúa con el ambiente y recibe observaciones
  - El agente elige una acción de un conjunto en cada paso
  - El mundo responde cambiando de estado
  - El agente recibe feedback en forma de una recompensa numérica y esta es la
    manera en que se va a guiar al agente a tomar comportamientos deseables
    (reward hypothesis)
  - Al tomar acciones también modifica las partes del mundo que explora (“active
    learning”)
  - El objetivo es aprender la política óptima, el mapa observación -> acción, de
    manera de maximizar la recompensa acumulada
- Definición del problema - componentes
  - El ambiente - estados, dinámica de transición
  - Observaciones/mediciones del ambiente
  - Acciones posibles en cada estado del ambiente
  - Un agente
    - Una política (de acción): mapa de estados a acciones
    - Una señal de recompensa
    - Una función de valor (cuál es el valor de estar en cada
      estado/estado-acción)
    - Un modelo del ambiente (opcionalmente)
  - Objetivo: maximizar la esperanza recompensas acumuladas
- Ejemplos varios
- Introducción a OpenAI Gym
  - Notebook: https://colab.research.google.com/drive/1hc59p6w8QUxktk0VEpeJGWB2mWStSTQX?usp=sharing
- Repaso de Matemática:
  - Cadenas de Markov
  - Proceso de Markov
  - Proceso de recomensa de Markov
    - S conjunto de estados finito
    - P matriz de probabilida de transición de estados
    - Pss' = P[St+1=s'|St=s]
    - R función de recompensa R=E[Rt+1 | St=s]
    - Y es el factor de descuento y en [0,1]
    - Key points: Función de recompensa Rs, Retorno Función de valor v(s) = E[Gt|St = s]
- Ecuación de Bellman
  - Podemos descomponer la función del valor en dos partes:
    - La recompensa inmediata
    - La función de valor en el próximo paso descontada.
- Ejemplo de estados (diapo 43) clases, facebook, dormir pub, aprobar.
- Proceso de decición de Markov. Se agrega una acción que modifica el ambiente.
  - A es un conjunto finito de acciones.
- Política:
  - Es una distribución de probabilidad sobre las acciones dado un estado
  - Define el comportamiento del agente.
  - Dado un proceso de decisión de Markov y una política fija, la secuencia:
    - S1,R2,S2.. es un proceso de recompensa de Markov 
- Evaluación de política
- Función de valor óptima
- Ecuación de optimalidad de Bellman. ¿Cómo la obtengo?
- Predicción libre de modelo. ¿Podemos aprender sólo de la experiencia?

## Clase 6. Viernes 27/11/2020

### Key points

- TBD

## Clase 7. Viernes 04/12/2020 

### Key points

- Temas del TP final. 
  - Hints: series temporales, vecinos.
- Style Transfer
  - ¿Qué aprenden las ConvNets?
    - A medida que se profundiza en capas, features más complejas. bordes, texturas/patrones, formas, etc.
    - Función costo como J=J_contenido + J_estilo
    - Activaciones similares -> contenido similar
    - Estilo = correlación de activaciones a lo largo de los canales
    - Gram matrix, norma de Frobenius entre matrices como distancia.
    - Notebook: https://colab.research.google.com/drive/1sB8g1EPm8lB72_hsyHa3MazC-HRWJ0oq?usp=sharing
- Reconocimiento de caras
  - Dos problemas relacionados:
    1. Face verification: dado una imagen de entrada + un ID/nombre, decir si la
       imagen corresponde a esa persona
    2. Face recognition: dada una base de datos con K personas y una imagen de
       entrada -> encontrar la persona a la que corresponde la imagen o decir que no es
       ninguna de esas personas
  - One-shot Learning, Zero-shot Learning
    - **One-shot learning**: en este problema tenemos un solo ejemplo de lo que
      queremos aprender, por ejemplo cuando tenemos una sola foto de una persona
      en la base de datos
    - **Zero-shot learning**: en este problema no hay ningún ejemplo del que aprender,
      esto significa que al momento de test voy a tener ejemplos que pertenecen a
      clases que no vi en el training set
  - Naive Face Verification 1
    ● Calcular la distancia euclídea entre las imágenes que nos presentan y la
    que tenemos en la base de datos
    - No funciona demasiado bien, hay diferencias de iluminación, de ángulo
      de sombras
  - Naive approach 2
    ● construir una red que tenga K+1 salidas
    - qué pasa cuando ingresa un nuevo empleado?
    - tenemos pocos ejemplos, cómo entrenamos?
  - Función de similaridad entre imágenes d(img1,img2). 
  - Red siamesa.
    - Objetivo de entrenamiento. Si xi y xj son la misma persona (xi-xj)**2 es bajo, y de lo contrario grande.
  - Triplet Loss. Uso del anchor, función de costo.
  - ¿Cómo extraer la cara de la imagen?
    - Joint Face Detection and Alignment using Multi-task Cascaded Convolutional Networks.
    - Primero se hace una 'pirámide de imagenes', re-escalando la imagen en varios tamaños.
    - Luego hay 3 etapas de redes convolucionales:
      - P-Net (proposal network): usan una red convolucional para generar propuestas de
        bounding boxes, se usa non-max suppression para fusionar ventanas
        superpuestas.
      - R-Net (refine network): usan otra red convolucional para rechazar muchas de las
        propuestas.
      - O-Net (output network): similar a la anterior, hace output de la caja y landmarks.
    - Notebook: https://colab.research.google.com/drive/1DPLe93k0XOuSjVFsFCYuiRNr1wATEHat?usp=sharing
- Generación de imágenes
  - Generación de imágenes parecidas a las de un dataset
  - Generative Adversarial Networks
    - Notebook: https://colab.research.google.com/drive/16ZXWRFa3wT8liLxwLrkwXoJLtKx50Awm?usp=sharing
- Super Resolution
- Aplicaciones

## Clase 8. Viernes 11/12/2020 (examen/proyecto final)



