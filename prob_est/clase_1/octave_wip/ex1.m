% ==============================================================================
% Probabilidad y Estadística para I.A. Cursos FIUBA 2020 
% Clase 1. Ej. 1
% ==============================================================================
pkg load statistics

% (a) ¿Cuál es la probabilidad de obtener exactamente 3 cecas en 10 tiradas
% de una moneda balanceada?
% ------------------------------------------------------------------------------

n_trials = 10 ; 
k = 3;          
p = 0.5;
q = 1.0 - p;

prob =  ( factorial(n_trials) / 
  (factorial(k)*factorial(n_trials-k))*(p**k)*(q**(n_trials-k)) );
disp(prob);

% (b) ¿Cuál es la probabilidad de obtener al menos 3 cecas en 10 tiradas de
% una moneda cargada donde la probabilidad de ceca es 0.4?
% ------------------------------------------------------------------------------

n_trials = 10 ; 
k = 3;          
p = 0.4;
q = 1.0 - p;


% (c) Una moneda cargada con probabilidad de ceca 0.4 es arrojada al aire.
% El resultado es cara. ¿Cuál es la probabilidad de obtener al menos 3
% cecas en las próximas 10 tiradas?
% ------------------------------------------------------------------------------

% (d) Simular los items anteriores en Octave usando: i) una distribución
% uniforme con la función rand para simular el proceso Bernoulli; ii) las
% funciones binopdf,binocdf para calcular la probabilidad binomial
% (Octave statistics package).
% ------------------------------------------------------------------------------
