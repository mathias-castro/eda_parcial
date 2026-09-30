# eda_parcial

/*
================================================================================
  CS3014 - Estructuras de Datos Avanzadas
  Problemset de practica: Montículos + Range Trees / Fractional Cascading
  (sin respuestas; nivel intermedio = nivel de la parte escrita del examen)
================================================================================

  Sugerencia de orden: empezar por A4, A6, B4 y B5, que son las mas parecidas
  a lo que suele venir en la parte escrita.


################################################################################
  PARTE A: MONTICULOS
################################################################################

--------------------------------------------------------------------------------
  FACILES
--------------------------------------------------------------------------------

  A1. (Binario)
      En un MIN-heap binario con n valores distintos guardado en un arreglo
      indexado desde 1, demuestra que el MAXIMO siempre esta en una hoja,
      es decir, en alguna posicion entre floor(n/2) + 1 y n.
      Usa solo la propiedad de heap y la definicion de hoja en el arreglo.

  A2. (Binomial)
      Un monticulo binomial tiene n = 45 elementos.
        - ¿Que arboles binomiales lo forman y cuantas raices revisa Find-Min?
        - Si se inserta un elemento mas, ¿cuantos Binomial-Link ocurren y que
          arboles quedan?
        - Responde lo mismo si n = 47.

  A3. (Fibonacci)
      Explica por que Union de dos monticulos de Fibonacci cuesta O(1) REAL,
      y por que su cambio de potencial (con Phi = t + 2m) es exactamente 0.

--------------------------------------------------------------------------------
  INTERMEDIAS (nivel examen)
--------------------------------------------------------------------------------

  A4. (Binario)
      Demuestra que Build-Max-Heap cuesta O(n):
        - justifica cuantos nodos hay a cada altura,
        - cuanto cuesta Max-Heapify en cada uno,
        - y resuelve la suma.
      Luego da un arreglo concreto de tamaño 7 para el cual construir el heap
      INSERTANDO UNO POR UNO hace la maxima cantidad posible de intercambios,
      y explica por que en general ese metodo cuesta Theta(n lg n) en el peor
      caso.

  A5. (Binomial)
      Demuestra que despues de quitar la raiz de un arbol B_k en Extract-Min,
      sus hijos forman un monticulo binomial valido. Menciona explicitamente
      que propiedad de B_k usas y por que hace falta invertir la lista de
      hijos antes de la Union. Concluye el costo de Extract-Min.

  A6. (Fibonacci)
      En el analisis de Decrease-Key con c cortes se usa Phi = t + 2m.
        - Calcula el costo amortizado con ese potencial.
        - Repite el calculo con Phi = t + m y muestra que ya no sale O(1).
        - Explica con palabras para que sirve cada una de las "dos monedas"
          que guarda un nodo marcado.

  A7. (Fibonacci)
      Se insertan las llaves 1, 2, ..., 8 en un monticulo de Fibonacci vacio
      y luego se hace un Extract-Min. Dibuja la lista de raices despues de
      consolidar (con la forma de cada arbol y el grado de cada raiz).
      ¿Que relacion tiene el resultado con la representacion binaria de 7?

--------------------------------------------------------------------------------
  DIFICILES
--------------------------------------------------------------------------------

  A8. (Fibonacci)
      Demuestra que si en un monticulo de Fibonacci NUNCA se usa Decrease-Key
      ni Delete (solo Insert, Union y Extract-Min), entonces todo arbol cuya
      raiz tiene grado k tiene exactamente 2^k nodos.

  A9. (Fibonacci)
      Supon que se cambia la regla de las marcas: un nodo puede perder HASTA
      DOS hijos antes de ser cortado.
        - ¿Que cota inferior tiene ahora el grado del i-esimo hijo?
        - Escribe la recurrencia del minimo numero de nodos s_k de un arbol
          con raiz de grado k.
        - ¿Sigue siendo D(n) = O(lg n)? Justifica si s_k sigue creciendo
          exponencialmente.

  A10. (Binario d-ario)
      En un heap d-ario (cada nodo tiene d hijos), calcula la altura y el
      costo de Insert, Decrease-Key y Extract-Min en funcion de n y d.
      Si Dijkstra hace V extracciones y E decrease-keys, ¿que valor de d
      minimiza el costo total?


################################################################################
  PARTE B: RANGE TREES Y FRACTIONAL CASCADING
################################################################################

--------------------------------------------------------------------------------
  FACILES
--------------------------------------------------------------------------------

  B1. (Cascading en listas)
      Con  L1 = {3, 8, 14, 20},  L2 = {1, 9, 15},  L3 = {2, 6, 10, 18, 22}:
        - Construye L'3, L'2 y L'1 promoviendo los elementos en posiciones
          pares, y marca a donde apunta cada puente.
        - Busca x = 12 en las tres listas: una busqueda binaria en L'1 y luego
          bajando por puentes. En cada lista indica cuantas comparaciones de
          ajuste hiciste y cual es el sucesor de 12 en la lista ORIGINAL.

  B2. (Layered)
      Un nodo del Layered Range Tree tiene hijos con listas (ordenadas por y)
      [2, 7, 9] y [4, 5, 11].
        - Escribe la lista del padre y, para cada posicion, los punteros al
          hijo izquierdo y al derecho (primer elemento >= ese valor).
        - Si en el padre y1 = 6 cae en la posicion del 7, ¿en que posicion
          cae en cada hijo? Muestra que lo obtienes sin ninguna busqueda.

  B3. (Layered)
      Explica por que en un Layered Range Tree no hace falta "promover la
      mitad" como en el fractional cascading general, y por que eso no
      aumenta el espacio.

--------------------------------------------------------------------------------
  INTERMEDIAS (nivel examen)
--------------------------------------------------------------------------------

  B4. (Layered 2D)
      Describe paso a paso una consulta [x1, x2] x [y1, y2] para REPORTAR
      puntos en un Layered Range Tree:
        - donde se hace la unica busqueda binaria,
        - por que nodos se baja,
        - en que nodos se usan los punteros y en cuales se reporta.
      Demuestra que el costo es O(lg n + k).
      ¿Que cambia si solo se quiere CONTAR?

  B5. (Cascading en listas)
      Generaliza el analisis de fractional cascading promoviendo una fraccion
      alfa (0 < alfa < 1) de cada lista:
        - Demuestra una cota para |L'1| en funcion de n y alfa.
        - ¿Cuantos elementos quedan, a lo mas, entre dos puentes consecutivos?
        - Explica que pasa en los casos extremos alfa -> 0 y alfa -> 1, y por
          que 1/2 es una buena eleccion.

  B6. (Cascading, limites)
      El fractional cascading acelera "buscar EL MISMO VALOR en k listas".
      Da un ejemplo de un problema donde en cada lista buscas un valor
      DISTINTO (que depende de lo que encontraste en la lista anterior de una
      forma arbitraria) y explica por que los puentes ya no garantizan O(1)
      por lista.

  B7. (3D)
      Explica por que colgar, en cada nodo del arbol en x, un arreglo
      cascadeado por y Y OTRO INDEPENDIENTE por z no permite responder
          [x1, x2] x (-inf, b2] x (-inf, b3]
      en O(lg n + k). Construye un ejemplo concreto con 4 puntos donde las
      posiciones de b2 y b3 en cada arreglo no te digan que puntos cumplen
      AMBAS condiciones.

--------------------------------------------------------------------------------
  DIFICILES
--------------------------------------------------------------------------------

  B8. (d dimensiones)
      Para un range tree en d dimensiones, deduce (justificando cada factor
      lg n):
        - espacio y consulta SIN cascading;
        - con LAYERED (cascading solo en el ultimo nivel);
        - con CASCADING COMPLETO (la construccion D1-D4 generalizada).

  B9. (Cascading generalizado)
      En un grafo de cascading donde cada vertice tiene a lo mas d vecinos y
      recibe una fraccion alfa de la lista aumentada de cada uno:
        - Demuestra que si d*alfa < 1, toda lista aumentada tiene tamaño a lo
          mas n / (1 - d*alfa).
        - Considera una ESTRELLA (un centro con k hojas). Muestra que, si k no
          esta acotado, no existe un alfa constante que de a la vez espacio
          lineal y ajuste O(1) al cruzar desde el centro.

  B10. (Dominancia)
      En la construccion 3D se usa para D1 una subdivision de Chazelle con
      O(n) espacio, en vez de un Layered Range Tree 2D que usa O(n lg n).
      Calcula el espacio total de la estructura 3D completa en ambos casos,
      y explica por que el profesor insiste en que D1 ocupe O(n).

================================================================================
*/

int main() {
    return 0;
}
