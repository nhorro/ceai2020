% ==============================================================================
% Probabilidad y Estadística para I.A. Cursos FIUBA 2020 
% Clase 1. Ej. 1
%
% (a) ¿Cuál es la probabilidad de obtener exactamente 3 cecas en 10 tiradas
% de una moneda balanceada?
% (b) ¿Cuál es la probabilidad de obtener al menos 3 cecas en 10 tiradas de
% una moneda cargada donde la probabilidad de ceca es 0.4?
% (c) Una moneda cargada con probabilidad de ceca 0.4 es arrojada al aire.
% El resultado es cara. ¿Cuál es la probabilidad de obtener al menos 3
% cecas en las próximas 10 tiradas?
% (d) Simular los items anteriores en Octave usando: i) una distribución
% uniforme con la función rand para simular el proceso Bernoulli; ii) las
% funciones binopdf,binocdf para calcular la probabilidad binomial
% (Octave statistics package).
% ==============================================================================
pkg load statistics

N_TRIALS = 10

results = rand(N_TRIALS,1) > 0.5

%disp(results)cls