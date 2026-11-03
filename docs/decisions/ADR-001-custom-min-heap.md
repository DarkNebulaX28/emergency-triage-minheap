# ADR-001: Implementación de Cola de Prioridad Dinámica Enlazada con Estabilidad FIFO

* **Fecha:** 2026-11-03
* **Estado:** Aceptado e Implementado

## Contexto

El sistema de Triage Hospitalario (**MediPriority**) requiere administrar pacientes según 5 niveles de urgencia clínica (`1 = Resucitación` a `5 = No Urgente`) sin utilizar contenedores de prioridad prefabricados de la STL (`std::priority_queue`). Además, la defensa académica exige poder visualizar explícitamente el enlace entre nodos y garantizar que dos pacientes con el mismo nivel de prioridad sean atendidos en estricto orden de llegada (**FIFO**).

## Decisión

Implementar una plantilla genérica **`Cola<T>`** basada en una **lista simplemente enlazada ordenada por prioridad** mediante nodos dinámicos (`Nodo<T>` con puntero `Nodo<T>* siguiente` y puntero de cabecera `frente`), complementada con métodos de inspección de direcciones físicas de memoria (`obtenerDireccionNodo` y `obtenerDireccionSiguiente`).

## Justificación Técnica

1. **Estabilidad FIFO Natural y Determinista:** En una lista enlazada ordenada con inserción mediante `actual->siguiente->prioridad <= prioridad`, la estabilidad cronológica ante prioridades iguales se preserva de forma nativa sin requerir contadores secundarios de desempate ni reordenamientos no estables.
2. **Despacho Médico en Tiempo Constante $O(1)$:** El paciente más crítico siempre se encuentra en el puntero `frente`. Extraerlo para atención médica (`desencolar` / `desencolarConPrioridad`) toma tiempo $O(1)$ exacto.
3. **Transparencia Pedagógica e Inspección de Punteros:** Cada paciente reside en una dirección independiente del *heap* (`new Nodo<T>`), lo que permite mostrar en la interfaz gráfica de Dear ImGui la cadena exacta de punteros `Nodo* -> siguiente* -> nullptr`.

## Consecuencias

* **Positivas:** Extracción $O(1)$, estabilidad FIFO garantizada, soporte directo para re-priorización (`actualizarPrioridad`) y eliminación posicional (`eliminarEnPosicion`), y visualización gráfica intuitiva de nodos enlazados.
* **Mitigación de Memoria:** El destructor `~Cola()` y el método `vaciar()` liberan todos los nodos dinámicos garantizando cero fugas de memoria (*zero memory leaks*).
