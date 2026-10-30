# ADR-001: Implementación Manual de Min-Heap Binario en Arreglo Contiguo

## Estado
Aceptado

## Fecha
2026-10-29

## Contexto
El requerimiento académico de la materia **Estructura de Datos** exige implementar una estructura de **Cola de Prioridad** en C++ desde cero, con prohibición estricta de utilizar contenedores o adaptadores prefabricados de la biblioteca estándar (como `std::priority_queue`, `std::vector`, `std::list`).

Es indispensable seleccionar la representación interna más adecuada para soportar operaciones eficientes de inserción ($O(\log n)$) y extracción de la máxima prioridad ($O(\log n)$), manteniendo simplicidad de gestión de memoria y facilidad de demostración para el evaluador.

## Decisión
Se implementará una clase plantilla `CustomMinHeap<T, Comparator>` construida sobre un **arreglo contiguo de memoria dinámica** administrado manualmente mediante punteros (`T* buffer`), capacidad inicial y estrategia de duplicación de tamaño ($2 \times$).

La navegación del árbol binario completo se realizará mediante aritmética de índices:
* Padre de $i$: `(i - 1) / 2`
* Hijo izquierdo de $i$: `2 * i + 1`
* Hijo derecho de $i$: `2 * i + 2`

## Alternativas Consideradas

### 1. `std::priority_queue` de la STL
* **Ventajas:** Ya optimizada y probada.
* **Motivo de Rechazo:** Prohibida explícitamente por los requisitos del proyecto universitario. No demuestra dominio de punteros, manejo de montículos ni algoritmos de flotación/hundimiento.

### 2. Árbol Binario mediante Nodos Enlazados con Punteros (`Node* left, Node* right, Node* parent`)
* **Ventajas:** Conceptualmente representa un árbol gráfico con nodos.
* **Desventajas:** 
  * Localizar el último nodo para insertar o extraer requiere $O(\log n)$ pasos complejos o mantener punteros auxiliares intrincados.
  * Dispersión en memoria dinámica (fragmentación de heap y *cache misses*).
* **Motivo de Rechazo:** Un montículo binario es por definición un árbol casi completo (*complete binary tree*), cuya representación óptima natural es un arreglo contiguo.

### 3. Lista Enlazada Simplemente Ordenada
* **Ventajas:** Fácil de extraer el primero ($O(1)$).
* **Desventajas:** La inserción requiere búsqueda lineal de la posición correcta ($O(n)$).
* **Motivo de Rechazo:** Degrada el rendimiento teórico requerido para una cola de prioridad eficiente.

## Consecuencias
* **Positivas:**
  * Cumplimiento estricto de los requisitos de la cátedra de Estructura de Datos.
  * Localidad de referencia en memoria caché óptima.
  * Facilidad absoluta para inspeccionar el arreglo en la interfaz gráfica (exponer `buffer[0..size-1]`).
* **Negativas / Desafíos:**
  * Requiere implementar manualmente la regla de los 5 (Constructor de copia, operador de asignación de copia, constructor de movimiento, operador de asignación de movimiento y destructor) para evitar fugas de memoria (*memory leaks*).
