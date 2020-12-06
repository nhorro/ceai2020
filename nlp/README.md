# Natural Language Processing

## Clase 1. Sábado 31/10/2020

### Key points

- Introducción al Posgrado y la Materia
  - Comentarios generales. Se agregan materias el año que viene.
  - Plan:
    - Clase 1: Optimizadores. Vectorización de documentos.
    - Clase 2: Word2Vector (Teórica)
    - Clase 3: Implementación Word2Vector en Keras (Práctica)
    - Clase 4: Redes Neuronales Recurrentes. Modelos Seq2Sec (Teórica)
    - Clase 5: Implementaciones de RNN y Seq2Seq en Keras (Práctica)
    - Clase 6: Celdas LSTM. Celdas con Attention. Transformers (Teórica).
    - Clase 7: Word embedding contextuales (ELMo y BERT) (Práctica)
    - Clase 8: Sistemas de recomendación. Sistemas de Búsqueda. Chatbots. TP FINAL (Práctica)
  - Bibliografía http://web.stanford.edu/class/cs224n/
- Optimizadores para ML
  - Problema de optimización en general vs problema de optimización en ML
  - Explicación de las 2 relajaciones.
    - Modelo 1.
    - Modelo 2.
    - Modelo 3.
- Optimizadores basados en gradientes.
  1. En ML optimizar un problema puede generar overfitting.
  2. Mini Batch
  3. Shuffle de los datos antes de cada epoch
     1. b=1 -> SGD
     2. b<n -> Mini Batch
     3. b=n -> GD
  4. Optimizadores con alpha adaptativo y momentos
     1. Repaso conceptos matemáticos útiles
        - Entender información de la derivada segunda.
- Variantes de los algoritmos
  1. G.D con decaimiento lineal en alpha
  2. First Order Momentum
  3. AdaGrad (Adaptative Gradient)
  4. RMSProp (Root Mean Square Propagation) (01:32)
  5. Adam (2014) (Adaptative Moments 
- Métodos que usan H )Hessian)
  - Newton
  - Gradientes conjugados
  - BFGS

- NLP
  - Técnicas de vectorización de texto básicas (Bag of Words):
    - A. Vectores de Frecuencai
    - One hot encoding
    - Tf-idf (Elasticsearch?)
  - ¿Cómo podemos usar estas representaciones para calcular similitudes?
    - Similitud coseno (cosine simlarity) <Doc1, Doc2>

### Tarea

- Ver [Notebook](clase_1/Clase01.ipynb)

## Clase 2. Sábado 07/11/2020

### Key points

- La clase pasada se vieron documentos, esta vez se hará foco en encontrar mejores representaciones de las palabras (representaciones densas).

- Veremos dos arquitecturas para aprender representaciones densas:

  1. Continuous Bag of Words

  Mención: GloVe y Fasttex

- La clase que viene veremos como implementarlo en Keras.

- Tareas de Language Processing.:

  - Spell Checking, Keyword search, synonyns
  - Extract Information (invoice, legal)
  - Translation, Semantic Analysis, NER,Q&A

- Nota. Ya existen datasets para las tareas anteriores.

- ¿Cómo representar palabras? -> Word Embeddings

- Redes neuronales simples: GloVE, FastText

- Recurrentes

- Seq2Seq

- Avanzadas ElMO, BERT

- Representaciones de palabras

  - One-hot Vector
    - No hay concepto de similaridad
  - SVD (Singular Value Decomposition)
    - X = USV.T
    - Idea: utilizar filas de U como los embeddings de las palabras

- Deep Learning Word2Vec

  - Arquitecturas:

    - CBOW = Continuous Bag of Words
    - Skip-gram
    - Concepto (ejemplo con frase): 
      - doc "The dog wants the ball"
        - En CBOW quiero hallar 'wants' habiendo pasado 'the','dog', ?, 'the', 'ball'
        - En Skipgram es exactamente al revés, paso 'wants' y quiero encontrar ?,?,'wants',?,?

  - Cross Entropy(Loss)

  - SGD|Mini Batch + Negative Sampling

  - ML Supervisado

  - En todos los casos el objetivo es aprender los embeddings

    

## Clase 3. Sábado 14/11/2020

### Key points

- Repaso lo visto hasta ahora en clase.
- Estamos yendo de embeddings por palabras a embeddings contextuales.
- Laboratorio.
  - Introducción gensim.https://radimrehurek.com/gensim/
  - Mención de otras libs de interés:
    - https://spacy.io/
    - https://www.nltk.org/
  - Notebook Word2Vec 
    - Colab(drive): https://colab.research.google.com/drive/1FRRMcYEEkscnjMG36212CxdI_o5ynVt9
    - Mención:
      - [umap](https://umap-learn.readthedocs.io/en/latest/)
      - [tsne](https://distill.pub/2016/misread-tsne/)
      - gráficos (plotly/dash): https://plotly.com/dash/
      - wikipedia 103 millones de articulos, tiene 267k palabras (nosotros usamos 2k )
        https://blog.einstein.ai/the-wikitext-long-term-dependency-language-modeling-dataset/ 
- Comentarios Slack:
  - animacion 3d de las proyecciones: https://projector.tensorflow.org/ (editado) 
  - Hola! Queria invitarlos a que prueben la técnica UMAP y su visualización con los datos del black friday a ver que les aparece, y luego lo comparen con PCA, me parece super importante que conozcan ambas técnicas y sus diferencias.
    Casi no tendrian esfuerzo, porque la evaluación final (que todavia les debemos el feedback), necesitaron todas las variables numéricas y sin nulos (requisito de UMAP y PCA)
    Proyecten en 2 dim y compartan aca el resultado, me consultan cualquier cosa!
    Esto es no supervisado, Como regla practica elijo proyecciones que tengan “islas”:
    Se “suele” buscar eso :apuntando_hacia_arriba:  a esto :apuntando_hacia_abajo:
    Web oficial con ejemplos: https://umap-learn.readthedocs.io/en/latest/basic_usage.html
- Mail adicional: Clase 3 - Detalle de CBOW y SKIP-GRAM

### Tarea

- Ver [Notebook Word2Vec](clase_3/NLP_Clase3_01_Word2Vec.ipynb)



## Clase 4. Sábado 28/11/2020

### Key points

#### Parte 1. Embeddings

- GloVE y FastText. 
  - Ejemplo con librerías en python. [Notebook](clase_4/NLP_Clase_4_Sabado_28_11_2020_Word2Vec_y_RNN.ipynb)
    - Distancia entre palabras, heatmap usando:
      - https://docs.scipy.org/doc/scipy/reference/generated/scipy.spatial.distance.cdist.html
- CBOW y SkipGram. 
  - Repaso: objetivo de cada uno (predecir palabra central vs. predecir contexto), función de costo de cada uno. Interpretación de capa final Softmax.
- Negative Sampling
  - Ejemplo tomando el caso de SkipGram.
  - Porqué: para reducir la cantidad de parámetros que tengo que calcular en el gradiente de la función de costo.
  - Simplificaciones:
    1. Paso de GD a SGD.
    2. ¿Podemos pensar el problema como Bernoulli?
       - Creemos una variable D y digamos que:
         - D=1 cuando una palabra está en el contexto.
         - D=0 cuando una palabra NO está en el contexto.
       - P(D=1) = p, P(D=0) = 1 - p
       - Por cada fila de nuestro dataset observamos H palabras negativas que no pertenecen a ese contexto (H es un hiperparámetro, ej: 50)
  - Evaluación de Embeddings. 2 maneras:
    - **Extrínseca**: Los evalúo de manera indirecta a través de resolver un ejemplo de NLP más grande. Ej: entrenar un clasificador de sentimiento y evaluar positivo/negativo y calcular para ese clasificador el AUC para distintos embeddings.
    - **Intrínseca**: Analogías. a:b, c:?

#### Parte 2. RNNs

- Motivación: dada una secuencia de símbolos X0..Xn, queremos saber la probabilidad de p(Xi) habiendo ocurrido anteriormente antes p(Xo0),p(Xo1),...,p(XoN). 
- Enfoque clásico.
- Enfoque Deep Learning.
  - Arquitecturas tradicionales
  - (sigue en tablet)



## Clase 5. Sábado 05/12/2020

### Key points

- Repaso de la última clase. Hasta ahora hemos visto Embeddings de palabras y frases y una intro a RNN.
- Desarrollo progresivo de RNNs
  - RNN, Neurona básica.
  - Bidirectional NN (BNN)
  - Vanishing/Exploding Gradients y su solución.
  - LSTM
  - Seq2Seq Encoder/decoder. Aplic. Traducciones, chatbot.

