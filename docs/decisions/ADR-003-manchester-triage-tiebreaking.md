# ADR-003: Protocolo de Triage Manchester y Desempate Estable FIFO

## Estado
Aceptado

## Fecha
2026-10-29

## Contexto
En un servicio de emergencias médicas real, la prioridad de atención no solo depende de la severidad del cuadro clínico, sino también del orden de llegada cuando dos o más pacientes comparten la misma categoría de gravedad.

Un montículo binario clásico (*Binary Heap*) **no es una estructura de datos estable**. Cuando dos elementos tienen la misma clave de prioridad, las operaciones de flotación (`heapifyUp`) o hundimiento (`heapifyDown`) pueden alterar arbitrariamente el orden relativo en el que fueron insertados. En un hospital, esto implicaría que un paciente que llegó más tarde podría ser atendido antes que otro con el mismo nivel de urgencia, lo cual es inaceptable tanto ética como clínicamente.

## Decisión
1. Adoptar el **Protocolo de Triage de Manchester (MTS)** con 5 niveles numéricos discretos:
   * **Nivel 1:** Resucitación (Máxima urgencia clínica)
   * **Nivel 2:** Emergencia
   * **Nivel 3:** Urgencia
   * **Nivel 4:** Menor
   * **Nivel 5:** No Urgente (Mínima urgencia clínica)

2. Incorporar un **contador secuencial correlativo global (`sequenceCounter`)** asignado a cada paciente en el momento exacto de su admisión (`arrivalSequence`), junto con el registro de la hora del sistema.

3. Implementar un comparador lexicográfico bidimensional (`TriageComparator`) con la siguiente regla estricta:
   $$P_A < P_B \iff (Nivel_A < Nivel_B) \lor (Nivel_A == Nivel_B \land Secuencia_A < Secuencia_B)$$

## Alternativas Consideradas

### 1. Desempate únicamente por `std::chrono::system_clock`
* **Desventajas:** En entornos de simulación de alta velocidad o pruebas automatizadas, dos pacientes podrían ser insertados en el mismo milisegundo o microsegundo, generando una condición de empate no resuelta o indeterminada.
* **Motivo de Rechazo:** Un contador secuencial entero incremental (`uint64_t`) garantiza unicidad absoluta y orden total determinista.

### 2. Dejar el Heap inestable (ignorar el orden de llegada en empates)
* **Motivo de Rechazo:** Invalida el realismo del sistema y representaría una falla de diseño conceptual frente al docente evaluador.

## Consecuencias
* **Positivas:**
  * Estabilidad FIFO 100% garantizada para pacientes con el mismo nivel de triage.
  * Comportamiento predecible y reproducible en las pruebas unitarias.
  * Cumplimiento con las directrices éticas de gestión hospitalaria.
* **Negativas / Desafíos:**
  * La entidad `Patient` debe almacenar y acarrear el campo de secuencia correlativa.
