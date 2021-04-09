## Notas revisión

1) Aplicadas observaciones de redacción.
2) Regenerado diagrama de CANVAS.
3) Ampliación de diagramas
4) Respecto a línea 141, cita a referencias de trabajos:

> Las nuevas técnicas en las áreas de Visión por Computadora e Inteligencia Artificial hallan un posible campo de aplicación en la 
> optimización de estas tareas de análisis, como lo demuestran algunos trabajos con resultados alentadores

Dejé una sola referencia:

\footnote{{\em The use of computer vision technologies in aquaculture - A review}

pero en realidad quisiera citar más trabajos. Entiendo que la consulta de como tratar y citar bibliografía en la planificación puede ser de interés para más de uno de nosotros, por ahí lo podemos ver directamente en clase.

Los trabajos a citar son:

\begin{enumerate}
\item "The use of computer vision technologies in aquaculture - A review". Boaz Zion. Institute of Agricultural Engineering, Agricultural Research Organization, The Volcani Center, Bet Dagan 50250, Israel.
\item "Deep neural networks for analysis of fisheries surveillance video and automated monitoring of fish discards" - Geoff French, Michal Mackiewicz, Mark Fisher, Helen Holah, Rachel Kilburn, Neil Campbell, and Coby Needle. 
\item Contribución a la sección para el tema: "Applications of machine learning and artificial intelligence in marine science". ICES Journal of Marine Science (2020), 77(4), 1340–1353. doi:10.1093/icesjms/fsz149.
\item "Tracking Fish Abundance by Underwater Image Recognition" - Simone Marini, Emanuela Fanelli, Valerio Sbragaglia, Ernesto Azzurro, Joaquin Del Rio Fernandez 6 y Jacopo Aguzzi. Nature Scientific Reports (2018).
\item "Convolutional Neural Networks for Counting Fish in Fisheries Surveillance Video". Mr. G. French, Dr. M. H. Fisher, Dr. M. Mackiewicz, Dr. C. L. Needle. BMVC 2015. http://www.bmva.org/bmvc/2015/mvab/papers/paper007/index.html. Accedido 01/02/2021
\item "Progress in the Automated Identification, Measurement, and Counting of Fish in Underwater Image Sequences" - Mark R. Shortis, Mehdi Ravanbakhsh, Faisal Shafait, Ajmal Mian. Marine Technology Society Journal, January/February 2016 Volume 50 Number 1.
\item "Monitoring and managing fisheries discards: New technologies and approaches" - Petri Suuronen and Eric Gilman, Marine Policy, https://doi.org/10.1016/j.marpol.2019.103554.
\end{enumerate}

5) Historias de usuario (entregado fuera de término)

Proyecto:
Sistema de recomendación para películas.

Roles:
- Experto/analísta: conoce el dominio del problema y como mínimo cuenta con un manejo básico de herramientas de análisis de datos. Su función es confeccionar reportes a partir de los datos recopilados que alimentan decisiones estratégicas, como por ejemplo establecer cuotas de captura o reasignar zonas permitidas de explotación del recurso. Por ejemplo, puede ser un científico del INIDEP.
- Administrador del Sistema (SysAdmin): es la persona que se responsabiliza de la instalación y configuración del SW y HW (no necesariamente desarrollador). Tiene conocimientos avanzados de redes, sistemas operativos, administración de bases de datos y despliegue de aplicaciones basadas en contenedores o servicios, asignación de carga a procesadores, y otras habilidades afines. No necesariamente está familiarizado con el dominio del problema. Contando con la documentación adecuada realiza y mantiene instalaciones de SW en sistemas complejos como por ejemplo centros de operación de radares, del sector aeroespacial o nuclear. 
- Operador a bordo: es la persona que opera el sistema a bordo del buque, pero sin ser ésta su función principal. En este caso su responsabilidad consiste en verificar que el SW está operativo mediante algún indicador en pantalla o mediante un comando de consola y tomar alguna acción ante un problema: reiniciar un servicio de SW o aparato o modificar una configuración. 
- Desarrollador de SW con perfil IA: es una de las personas que realiza cambios en el SW para extender su funcionalidad. Participa en la evaluación de si el SW puede adecuarse a un nuevo escenario y propone estrategias para lograrlo. Sus tareas pueden incluir: reentrenar modelos para incoporar más objetivos a la clasificación, modificar y agregar componentes en las cadenas de preprocesamiento e identificar fallas en una implementación.

Historias de usuario


Como experto/analista, y habiendo participado de la ubicación y orientación de las cámaras a bordo, deseo poder definir regiones de la imagen para cada una de las cámaras y habilitar el registro de las piezas que ingresan a una región, las que salen de una región, y el tiempo que permanece una pieza en una región. 

Como experto/analista deseo poder consultar todos los eventos registrados durante un intervalo de tiempo y obtener los resultados en un formato estándar como XML, CSV, JSON, API REST, u otro para que puedan ser tratados con otra herramienta específica, como por ejemplo python, R o Matlab. 
Estos resultados deben discriminar, como mínimo, fecha y hora y tipo de evento, para luego poder confeccionar estadísticas de actividad por momentos del día, total de capturas por tipo de pieza, total de descartes, cumplimiento de normas (permanencia máxima de una pieza en una cinta transportadora o recipiente), y otros.

Como administrador del sistema, deseo que el sistema sea desplegado en forma de servicios o contenedores (docker) para poder monitorearlos y asignarles los recursos correspondientes (procesador, RAM, cuota de disco). Además, deben poder ser configurables por medio de un archivo o línea de comando. Deben generar un log estándar para poder integrarse a un sistema de monitoreo.

Como operador a bordo deseo contar con un instructivo breve y conciso para iniciar el sistema, saber si está operativo, y realizar las tareas de mantenimiento (cambios de disco, verificación de conexiones, u otros.

Como desarrollador de SW con perfil IA, deseo contar con un ambiente de desarrollo y la documentación del código y los modelos implementados, así como la justificación para los parámetros elegidos. 

Como desarrollador de SW con perfil IA, deseo que la solución esté implementado de manera modular, con componentes que cumplan funciones específicas y puedan recombinarse para formar otras cadenas de procesamiento para ensayar modos de operación con funcionalidades alternativas.


Como 
- Como público adulto quiero que el sistema arme una lista de películas
en función a mis preferencias.
- Como marketing quiero conocer qué porcentaje de las películas recomendadas son efectivamente consumidas.
- Como director de películas quiero saber la crítica que recibo en mis películas
- Como público (adulto,niño y joven), quiero poder calificar cada película en función de "me gusta" y "no me gusta"

0, 1, 1, 2, 3, 5, 8, 13, 21, 34, ...


WIP

- Historias de usuario


Honest planning

Requisitios de HW:
	- GPU
	- CPU

Cada componente de la cadena de procesamiento tiene requisitos de librerías


Inferencia con YOLOv4. Opciones:

- Darknet 				CPU (nativo)
						GPU (nativo)
						CPU (docker)
						GPU (docker)
- OpenCV 4 (DNN)		CPU (nativo)
						GPU (nativo)
						CPU (docker)
						GPU (docker)
- Docker c/ API REST 	CPU (nativo)
						GPU (nativo)
						CPU (docker)cd
						GPU (docker)
- Tensorflow 			CPU (nativo)
						GPU (nativo)
						CPU (docker)
						GPU (docker)
- tensorflow-serving    CPU (nativo)
						GPU (nativo)
						CPU (docker)
						GPU (docker)
- Pytorch / otras		CPU (nativo)
						GPU (nativo)
						CPU (docker)
						GPU (docker)


3 ambientes de desarrollo para la versión desktop

						| Ventajas					| Desventajas
----------------------------------------------------|-------------------------						
WORKSTATION-INVAP		| Persistencia. 			| Edición remota. Req. configuración adicional de proxy
						|							| Uso compartido. Tener mucho cuidado si se modifican 
						|							| versiones de drivers CUDA, nvidia-docker, etc.
------------------------|---------------------------|---------------------------------------------					
WORKSTATION-HOME		| Persistencia.	Edición		| Usada para otras tareas.
						| de código.				| Pocas prestaciones de HW. 
						| 							| No apta para entrenamientos largos.
						|							| Límites de disco y conexión a internet.
------------------------|---------------------------|---------------------------------------------
Cloud (COLAB)			| HW de prestaciones medias | No es persistente. Persistencia con Google drive limitada.
						| Sólo para ensayos de      |
						| componentes indidividuales|
------------------------|---------------------------|---------------------------------------------
Cloud (AWS o Google)	| Ambiente de desarrollo    | Precio en U$S
						| óptimo