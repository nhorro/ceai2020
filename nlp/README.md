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

## Clase 2. Sábado 07/10/2020

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

    

  

  

  

  

  

  