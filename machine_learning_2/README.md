# Machine Learning 2

## Resumen de clases

### Clase 1. Jueves 29/10/2020. Optimización de Hiperparámetros

### Keypoints

- Notebook: 
- ¿Cómo elegimos los mejores hiperparámetros para nuestro problema?
  - ¿Qué es mejor? ¿Con respecto a exactitud? ¿Área bajo la curva ROC?
  - Paso 0. Definir métrica que quiero optimizar.
- Técnicas de selección de HPs.
  - Manual Search: la más rudimentaria (o sea, pruebo al azar). Cansador, tedioso y poco eficiente.
  - Grid Search. Básicamente es un todos contra todos. Disponible en Sklearn y Keras.
  - ¿Cómo evaluamos un modelo? ¿Con Train/test splot? ¿Con validación cruzada?
    - Held-Out (Test)
  - Random Search: va buscando combinaciones. Relación con K-Folds
    - Explicación de uso en Sklearn
- Automated Hyperparameter Tuning
  - Optimización Bayesiana. Se puede hacer en python con la biblioteca Hyperopt
  - Algoritmos Genéticos. Se puede hacer en python con TPOT.
  - ANN con Keras (con KerasClassifier)
- Guía para ordenar ideas

Tarea:

- Mirar video Hyperparameter tuning - Tuning process

Próxima clase:

- Se trabajará en el notebook de clase

Recursos (ver PPT para links)

Comentarios finales:

- Recordar que los prácticos se pueden entregar hasta el 6.

### Clase 2. Jueves 5/11/2020. Redes Bayesianas

### Keypoints

- ¿Qué es una red bayesiana? (aproximación semiformal)
  - Son modelos para clasificación, diagnóstico, predicción o regresión basados en el concepto de inferencia bayesiana.
  - Es una forma compacta de representar conocimiento y métodos flexibles de razonamiento.
  - Son grafos acíclicos
    - Cada nodo representa una vriabale
    - Cada arco una dependencia probabilística.
  - Mención: Un caso especial Clasificador ingenuo (Naive Bayes)
  - Dependen de un experto que permita definicir las tablas condicionales.
- Definición formal
- Representación del conocimiento
- Independencia condicional
- Algoritmos de propagación
  - Métodos exactos vs métodos aproximados
  - Dependen del tipo de topología de red. Existen 2 topologías de red:
    1. Árboles
    2. Poliárboles
- El aprendizaje en las redes bayesianas. Dos fases
  - Estructural
  - Paramétrico
- Procedimiento para implementación de Redes Bayesianas

Comentarios finales:

- Sumarle al TP de ML1 agregarle 2 temas de los vistos en ML2

### Clase 3. 

### Keypoints



### Clase 4. 

### Keypoints



### Clase 5. 

### Keypoints



### Clase 6. 

### Keypoints

### Clase 7. 

### Keypoints



### Clase 8. 

### Keypoints