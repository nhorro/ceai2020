FROM jupyter/scipy-notebook

USER root
RUN apt-get update && apt-get install graphviz -y
USER $NB_UID

RUN pip install elasticsearch elasticsearch_dsl pydotplus xgboost

