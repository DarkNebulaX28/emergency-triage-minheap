# Especificación Técnica del Sistema (PRD)

## 1. Información del Proyecto
* **Nombre del Sistema:** MediPriority - Sistema de Triage y Priorización de Pacientes en Emergencias.
* **Materia:** Estructura de Datos.
* **Institución / Contexto:** Proyecto Universitario de Ingeniería.
* **Paradigma / Lenguaje:** C++17 (Modern C++, sin librerías de contenedores de prioridad).
* **Mecanismo de Entrega Visual:** Dear ImGui con backend GLFW y OpenGL 3.

---

## 2. Objetivo y Alcance
Desarrollar un sistema de gestión y priorización hospitalaria en tiempo real para salas de emergencias. El sistema debe gestionar el flujo de ingreso, clasificación, espera y atención de pacientes utilizando una **Cola de Prioridad basada en un Min-Heap Binario implementado estrictamente a mano** (sin uso de `std::priority_queue`, `<queue>`, `<vector>` o equivalentes para la estructura central).

### Objetivos Específicos:
1. **Modelar el Protocolo de Triage de Manchester (MTS):** Clasificación de pacientes en 5 niveles de gravedad médica.
2. **Garantizar Estabilidad FIFO en Empates:** A igual severidad clínica, el paciente que llegó primero es atendido primero.
3. **Desacoplamiento Arquitectónico (Screaming Architecture):** Separar completamente la estructura matemática del Heap de las reglas médicas y de la interfaz gráfica.
4. **Visibilidad Académica (Vista Ingenieril):** La interfaz debe mostrar tanto la cola clínica (sala de espera) como el estado interno del arreglo del montículo binario en memoria para demostrar el funcionamiento del algoritmo al evaluador.

---

## 3. Modelo de Dominio (Triage Médico)

### 3.1. Niveles de Gravedad (Protocolo Manchester)
| Nivel | Categoría | Color Representativo | Tiempo Objetivo de Atención | Prioridad Numérica |
| :---: | :--- | :--- | :--- | :---: |
| **1** | **Resucitación** | Rojo (`#E63946`) | Inmediato (0 min) | 1 (Máxima) |
| **2** | **Emergencia** | Naranja (`#F4A261`) | < 10 minutos | 2 |
| **3** | **Urgencia** | Amarillo (`#E9C46A`) | < 60 minutos | 3 |
| **4** | **Menor** | Verde (`#2A9D8F`) | < 120 minutos | 4 |
| **5** | **No Urgente** | Azul (`#457B9D`) | < 240 minutos | 5 (Mínima) |

### 3.2. Estructura de la Entidad `Patient`
```cpp
struct Patient {
    unsigned int id;              // Identificador único correlativo
    std::string fullName;        // Nombre y apellidos
    std::string nationalId;      // Cédula / DNI / Pasaporte
    int age;                     // Edad en años
    std::string symptoms;        // Motivo de consulta / signos vitales
    TriageLevel level;           // Nivel 1 al 5
    long long arrivalTimestamp;  // Marca de tiempo o turno correlativo para desempate
    PatientStatus status;        // WAITING (En espera), IN_ATTENTION (Atendiéndose), DISCHARGED (Atendido)
};
```

### 3.3. Criterio de Ordenamiento y Desempate
Dado que un Min-Heap sitúa en la raíz el elemento de "menor valor", la función de comparación `TriageComparator` se define formalmente como:

$$P_A < P_B \iff (Nivel_A < Nivel_B) \lor (Nivel_A == Nivel_B \land Turno_A < Turno_B)$$

Esto garantiza matemáticamente:
* Un paciente Nivel 1 (Rojo) siempre tiene mayor prioridad que un Nivel 2 (Naranja).
* Si dos pacientes son Nivel 2 (Naranja), el que ingresó primero ($Turno_A < Turno_B$) se ubica antes en el montículo.

---

## 4. Estructura de Datos Central: `CustomMinHeap<T, Comparator>`

### 4.1. Representación en Memoria
El montículo se implementa sobre un **arreglo contiguo dinámico** administrado manualmente mediante punteros (`T* buffer`), capacidad dinámica y realocación manual:

* **Índice Padre:** $\lfloor \frac{i - 1}{2} \rfloor$
* **Hijo Izquierdo:** $2i + 1$
* **Hijo Derecho:** $2i + 2$

```
          [0] (Raíz / Mayor Prioridad)
         /   \
       [1]   [2]
      /  \   /  \
    [3]  [4][5] [6]
```

### 4.2. Operaciones y Complejidad Temporal
| Operación | Método | Complejidad Temporal | Complejidad Espacial | Descripción |
| :--- | :--- | :---: | :---: | :--- |
| **Inserción** | `push(element)` | $O(\log n)$ | $O(1)$ amortizado | Agrega al final y aplica `heapifyUp` (flotación). |
| **Extracción** | `pop()` | $O(\log n)$ | $O(1)$ | Retira la raíz, mueve el último elemento arriba y aplica `heapifyDown` (hundimiento). |
| **Consulta Raíz** | `top()` | $O(1)$ | $O(1)$ | Retorna referencia constante al elemento de mayor prioridad. |
| **Inspección de Arreglo** | `getBuffer()` | $O(1)$ | $O(1)$ | Expone el arreglo interno para graficar el árbol en la GUI. |
| **Limpieza** | `clear()` | $O(1)$ | $O(1)$ | Restablece el tamaño a 0. |

---

## 5. Arquitectura del Software (*Screaming Architecture*)

```text
SistemaTriage/
├── CMakeLists.txt                 # Script de compilación universal
├── README.md                      # Manual para el evaluador / profesor
├── docs/                          # Documentación formal y ADRs
│   ├── SPEC-triage-system.md      # Este documento
│   └── decisions/
│       ├── ADR-001-custom-min-heap.md
│       ├── ADR-002-dear-imgui-glfw.md
│       └── ADR-003-manchester-triage-tiebreaking.md
├── vendor/                        # Librerías sin instalación externa
│   └── imgui/                     # Código fuente de Dear ImGui
├── src/
│   ├── Core/                      # Capa 1: Estructuras de Datos Puras
│   │   ├── DynamicArray.hpp       # Arreglo dinámico gestionado a mano
│   │   └── PriorityQueue.hpp      # Min-Heap genérico a mano
│   │
│   ├── Triage/                    # Capa 2: Lógica de Dominio del Hospital
│   │   ├── Patient.hpp            # Entidad paciente
│   │   ├── TriageLevel.hpp        # Enum y metadatos de Manchester
│   │   ├── TriageComparator.hpp   # Regla de prioridad y desempate FIFO
│   │   └── HospitalManager.hpp    # Servicio de emergencias (ingreso, llamada, historial)
│   │
│   ├── Presentation/              # Capa 3: Interfaz Gráfica (Dear ImGui)
│   │   ├── UITheme.hpp            # Paleta de colores clínicos y estilos
│   │   ├── AdmissionView.hpp      # Formulario de registro de paciente
│   │   ├── WaitingRoomView.hpp    # Visualización de la cola de espera
│   │   ├── ConsultationView.hpp   # Box de atención activa
│   │   └── HeapInspectorView.hpp  # Inspección del montículo para el profesor
│   │
│   └── main.cpp                   # Punto de entrada y ciclo de vida de la aplicación
```

---

## 6. Diseño de la Interfaz de Usuario (Dashboard Clínico)

La ventana principal se organiza en 3 columnas ergonómicas de alta visibilidad:

1. **Columna Izquierda (Admisión & Triage):**
   * Inputs de texto para Nombre, Documento, Edad y Síntomas.
   * Selector rápido de Nivel de Triage con botones de colores luminosos (Rojo, Naranja, Amarillo, Verde, Azul).
   * Botón de acción: *"Ingresar Paciente al Triage"*.
2. **Columna Central (Sala de Espera y Turnos):**
   * Métrica de Pacientes en espera.
   * Tabla interactiva que muestra el orden real de atención previsto.
   * Insignias (badges) de color según el nivel de urgencia.
   * Indicador del tiempo transcurrido desde el ingreso.
3. **Columna Derecha (Consultorio Activo & Despacho):**
   * Botón destacado: *"Llamar Siguiente Paciente"*.
   * Ficha médica del paciente actualmente en atención.
   * Historial de pacientes atendidos.
4. **Panel Inferior (Inspector del Heap para el Profesor):**
   * Muestra el arreglo contiguo: `[0] ID:5 (Rojo) | [1] ID:2 (Rojo) | [2] ID:8 (Naranja)...`
   * Indicador visual de relaciones Padre-Hijos (`Padre i -> Hijos 2i+1, 2i+2`).
   * Contador de operaciones realizadas (`push`, `pop`, comparaciones).

---

## 7. Criterios de Aceptación (Definition of Done)
- [ ] La cola de prioridad no utiliza ningún contenedor de la STL (`std::vector`, `std::priority_queue`, etc.).
- [ ] La inserción y extracción operan en tiempo $O(\log n)$ verificable.
- [ ] El desempate entre pacientes de igual nivel es estrictamente FIFO.
- [ ] El proyecto compila limpiamente con CMake y un compilador C++17 (MinGW o MSVC) sin requerir instalación de SDKs pesados.
- [ ] La interfaz gráfica refleja los cambios de la cola de forma instantánea y fluida (60 FPS).
- [ ] **Protocolo de Entrega Final:** El último commit actualiza obligatoriamente el `README.md` con las instrucciones definitivas y verificadas de compilación, ejecución y primeros pasos para el docente evaluador.
