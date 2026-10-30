# 🎓 Guía de Pruebas y Defensa Académica

Este documento está diseñado para que el equipo prepare la sustentación del proyecto ante el docente y valide el correcto funcionamiento del **Min-Heap** y el **Sistema de Triage**.

---

## 🧪 1. Batería de Casos de Prueba Críticos

Para demostrar que la estructura de datos no presenta fallas, se deben ejecutar los siguientes escenarios en la interfaz gráfica o mediante pruebas unitarias:

### Caso de Prueba 1: Flotación a la Raíz (`heapifyUp` $O(\log n)$)
* **Objetivo:** Probar que un paciente crítico desplaza a pacientes de menor gravedad hasta ubicarse en la raíz (`index 0`).
* **Pasos:**
  1. Registrar 3 pacientes Nivel 4 (Verde).
  2. Registrar 1 paciente Nivel 1 (Rojo - Paro Cardiorrespiratorio).
* **Resultado Esperado:**
  * En la sala de espera y en el Inspector de Memoria (`buffer[0]`), el paciente Nivel 1 debe quedar automáticamente en la primera posición.
  * Al presionar *"Llamar Siguiente"*, sale el paciente Nivel 1 inmediatamente.

---

### Caso de Prueba 2: Estabilidad y Desempate FIFO (Mismo Nivel de Triage)
* **Objetivo:** Demostrar que a igual prioridad clínica se respeta estrictamente el orden de llegada.
* **Pasos:**
  1. Registrar al paciente "Ana Gómez" como Nivel 2 (Naranja) a las 10:00:01 (Turno #1).
  2. Registrar al paciente "Luis Pérez" como Nivel 2 (Naranja) a las 10:00:15 (Turno #2).
* **Resultado Esperado:**
  * Ambos tienen gravedad 2, pero "Ana Gómez" debe estar antes en la cola de atención por tener `arrivalSequence = 1`.
  * Al llamar al siguiente paciente, se atiende a "Ana Gómez" y luego a "Luis Pérez".

---

### Caso de Prueba 3: Hundimiento al Extraer (`heapifyDown` $O(\log n)$)
* **Objetivo:** Probar la reorganización correcta del árbol tras extraer la raíz.
* **Pasos:**
  1. Con 5 pacientes en el montículo, presionar *"Llamar Siguiente Paciente"*.
* **Resultado Esperado:**
  * El último elemento del arreglo se mueve a la posición `0`.
  * El algoritmo compara con sus dos hijos (`2i+1`, `2i+2`), selecciona al de mayor severidad y desciende intercambiándose hasta restaurar la propiedad de montículo.
  * El tamaño del Heap se reduce exactamente en 1.

---

### Caso de Prueba 4: Realocación Dinámica de Memoria ($2 \times$)
* **Objetivo:** Probar que la estructura no se desborda cuando se supera la capacidad inicial.
* **Pasos:**
  1. Iniciar con capacidad inicial de 4 elementos.
  2. Insertar 10 pacientes consecutivos.
* **Resultado Esperado:**
  * El arreglo dinámico duplica su capacidad automáticamente (`4 -> 8 -> 16`) sin perder datos ni corromper las relaciones padre-hijo.
  * Cero fugas de memoria (*no memory leaks*).

---

### Caso de Prueba 5: Resiliencia ante Cola Vacía
* **Objetivo:** Evitar accesos indebidos de memoria cuando no hay pacientes.
* **Pasos:**
  1. Con 0 pacientes en espera, presionar *"Llamar Siguiente Paciente"*.
* **Resultado Esperado:**
  * El sistema no produce *Segmentation Fault* ni cuelgues.
  * La interfaz muestra un mensaje informativo claro: *"No hay pacientes en espera"*.

---

## 💬 2. Preguntas Típicas del Docente y Cómo Responderlas

### P1: "¿Por qué usaron un arreglo y no un árbol con punteros `left`, `right` y `parent`?"
> **Respuesta:**  
> *"Un montículo binario es por definición un árbol binario casi completo. Si usáramos nodos enlazados dispersos, encontrar el último nodo para insertar o extraer requeriría $O(\log n)$ pasos complejos y fragmentaría la memoria. En un arreglo contiguo, calcular el padre `(i-1)/2` o los hijos `2i+1` y `2i+2` toma tiempo $O(1)$ directo por aritmética de índices y aprovecha al máximo la memoria caché del procesador."*

---

### P2: "¿Por qué un Min-Heap y no un Max-Heap?"
> **Respuesta:**  
> *"En los sistemas internacionales de triage médico (como el Protocolo de Manchester), los códigos de gravedad son numéricos: Nivel 1 es Resucitación (máxima urgencia) y Nivel 5 es No Urgente (mínima urgencia). Con un Min-Heap, el número 1 queda naturalmente en la raíz sin necesidad de invertir artificialmente los valores."*

---

### P3: "¿Cómo solucionaron el problema de que el Heap no es una estructura estable?"
> **Respuesta:**  
> *"Un montículo binario clásico no garantiza orden FIFO en caso de empates. Para solucionarlo, creamos la estructura `TriageComparator` con ordenamiento lexicográfico: primero compara el nivel de urgencia; si los niveles son iguales, desempata comparando el campo `arrivalSequence`, que es un número correlativo asignado estrictamente por orden de llegada."*

---

### P4: "¿Dónde está demostrado que no usaron `std::priority_queue`?"
> **Respuesta:**  
> *"En la carpeta `src/Core/`, en los archivos `PriorityQueue.hpp` y `DynamicArray.hpp`. Toda la gestión de memoria con punteros (`new[]` / `delete[]`), la duplicación dinámica de capacidad y los métodos de flotación (`heapifyUp`) y hundimiento (`heapifyDown`) fueron programados a mano desde cero."*
