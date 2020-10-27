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

Ver [notebook](clase_1/CV2_01_Capas_convolucionales.ipynb)

#### Ejercicio 2.

Ver [notebook](clase_1/CV2_02_red_cnn_simple.ipynb)

#### Ejercicio 3.

|                                | Dimensiones de activación | Tamaño de Activación | Cant. de parámetros |
| ------------------------------ | ------------------------- | -------------------- | ------------------- |
| Entrada                        |                           |                      |                     |
| Conv2D(f=4, s=1,c=8, ‘same’)   |                           |                      |                     |
| MaxPool(f=8, s=8)              |                           |                      |                     |
| Conv2D(f=2, s=1, c=16, ‘same’) |                           |                      |                     |
| MaxPool(f=4, s=4)              |                           |                      |                     |
| Dense(salida=6)                |                           |                      |                     |

Ver [notebook](clase_1/CV2_03_Modelo_convolucional_aplicacion_keras.ipynb)

## Clase 2. Viernes 30/10/2020

### Key points

- TBD

### Tarea

- TBD

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

## Clase 8. Viernes 11/12/2020 (Examen)



