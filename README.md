<div align="center">

# MediPriority™ Engine
### Real-Time Clinical Triage & Deterministic Patient Prioritization System

[![C++17 Standard](https://img.shields.io/badge/Language-C%2B%2B17-00599C?style=flat-square&logo=c%2B%2B)](https://en.cppreference.com/w/cpp/17)
[![Build Status](https://img.shields.io/badge/Build-Passing-brightgreen?style=flat-square)](https://cmake.org/)
[![Architecture](https://img.shields.io/badge/Architecture-Screaming%20Domain-blueviolet?style=flat-square)](./docs/SPEC-triage-system.md)
[![Data Structure](https://img.shields.io/badge/Core-Zero--Dependency%20Min--Heap-orange?style=flat-square)](./src/Core/PriorityQueue.hpp)
[![License](https://img.shields.io/badge/Academic-Open%20Source-lightgrey?style=flat-square)](./LICENSE)
[![Release](https://img.shields.io/badge/Release-v1.0.0-informational?style=flat-square)](https://github.com/)

**Sistema de Alto Rendimiento para Gestión de Salas de Urgencias Hospitalarias**  
*Implementación de Cola de Prioridad basada en Montículo Binario Manual sobre Memoria Contigua Dinámica*

---

</div>

## Resumen Ejecutivo

**MediPriority Engine** es una solución de ingeniería de software diseñada para modelar, simular y optimizar el flujo de atención en salas de emergencias críticas. El sistema resuelve el problema clásico de inversión de prioridad y congestión hospitalaria mediante una **Cola de Prioridad basada en un Min-Heap Binario determinista desarrollado desde cero en C++17**.

El proyecto prescinde intencionalmente de abstracciones de la biblioteca estándar de C++ (`std::priority_queue`, `std::vector`, `<queue>`, `<algorithm>`) para implementar la gestión de memoria cruda, la indexación de árbol y los algoritmos de reestructuración en tiempo logarítmico $\mathcal{O}(\log n)$, acoplado a un panel de control clínico en tiempo real desarrollado con **Dear ImGui** y **OpenGL 3.3**.

---

## 1. Fundamentos Algorítmicos y Estructura de Datos

### 1.1. Invariante del Montículo Mínimo (Min-Heap Invariant)
La cola de prioridad se rige por la propiedad formal de montículo mínimo, donde la raíz del árbol contiene estrictamente al elemento con el mayor índice de severidad clínica (menor valor numérico de triage):

$$\forall i \in [1, n-1], \quad \operatorname{Key}(\operatorname{Parent}(i)) \le \operatorname{Key}(i)$$

En el modelo de urgencias médicas:
* $\operatorname{Key} = \langle \text{Nivel Manchester}, \text{Secuencia Temporal FIFO} \rangle$
* **Nivel 1 (Resucitación)** precede estrictamente a **Nivel 2 (Emergencia)**.

```
                         [Índice 0]
                  Nivel 1 - Resucitación (Raíz)
                        /              \
           [Índice 1]                      [Índice 2]
     Nivel 2 - Emergencia            Nivel 1 - Resucitación
        (Llegada: t=1)                   (Llegada: t=5)
          /        \                       /        \
     [Índice 3]  [Índice 4]          [Índice 5]   [Índice 6]
      Nivel 3     Nivel 4              Nivel 2     Nivel 5
```

### 1.2. Mapeo Aritmético en Memoria Contigua
Para maximizar la tasa de aciertos en la memoria caché del procesador (*cache locality*) y eliminar la fragmentación por punteros dispersos, el árbol se mapea sobre un búfer contiguo de memoria dinámica (`T* buffer`):

| Relación de Nodo | Expresión Algorítmica | Complejidad Temporal |
| :--- | :--- | :---: |
| **Nodo Padre** | $\operatorname{parent}(i) = \lfloor \frac{i - 1}{2} \rfloor$ | $\mathcal{O}(1)$ |
| **Hijo Izquierdo** | $\operatorname{leftChild}(i) = 2i + 1$ | $\mathcal{O}(1)$ |
| **Hijo Derecho** | $\operatorname{rightChild}(i) = 2i + 2$ | $\mathcal{O}(1)$ |

### 1.3. Matriz de Complejidad Asintótica
Todas las operaciones han sido analizadas formalmente según el estándar internacional de evaluación algorítmica:

| Operación | Método Interno | Caso Promedio | Peor Caso | Complejidad Espacial | Invariante Garantizado |
| :--- | :--- | :---: | :---: | :---: | :--- |
| **Admisión (Push)** | `heapifyUp` | $\mathcal{O}(1)$ | $\mathcal{O}(\log n)$ | $\mathcal{O}(1)$ amortizado | Flotación ascendente con duplicación $2\times$ |
| **Atención (Pop)** | `heapifyDown` | $\mathcal{O}(\log n)$ | $\mathcal{O}(\log n)$ | $\mathcal{O}(1)$ | Extracción de raíz y hundimiento binario |
| **Inspección (Top)** | Acceso a `buffer[0]` | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | Acceso directo en tiempo constante |
| **Consulta de Estado** | `empty()` / `size()` | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | Lectura atómica de metadatos |
| **Vaciado (Clear)** | `clear()` | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | Reseteo lógico del contador de tamaño |

### 1.4. Estabilidad Determinista y Resolución de Empates (FIFO Tie-Breaking)
Los montículos binarios clásicos son inestables por naturaleza. En un contexto médico, atender a un paciente que llegó después antes que a uno con la misma urgencia viola las directrices de bioética clínica. 

Para resolver esto sin degradar el rendimiento, el comparador formal `TriageComparator` implementa un ordenamiento lexicográfico compuesto:

$$P_A \prec P_B \iff (\operatorname{Nivel}_A < \operatorname{Nivel}_B) \lor (\operatorname{Nivel}_A = \operatorname{Nivel}_B \land \operatorname{Secuencia}_A < \operatorname{Secuencia}_B)$$

Donde $\operatorname{Secuencia}$ es un entero monótono incremental de 64 bits (`uint64_t`) asignado en el instante exacto del triage.

---

## 2. Protocolo Clínico de Triage (Sistema Manchester)

El motor implementa el estándar internacional del **Manchester Triage System (MTS)**, categorizando los cuadros patológicos en 5 estratos operativos:

| Estrato | Categoría Clínica | Color Normalizado | Umbral Máximo de Espera | Criterio de Activación |
| :---: | :--- | :---: | :---: | :--- |
| **Nivel 1** | **Resucitación** | `#E63946` (Rojo) | 0 minutos (Inmediato) | Paro cardiorrespiratorio, shock severo, trauma mayor. |
| **Nivel 2** | **Emergencia** | `#F4A261` (Naranja) | $\le 10$ minutos | Dolor torácico agudo, compromiso hemodinámico. |
| **Nivel 3** | **Urgencia** | `#E9C46A` (Amarillo) | $\le 60$ minutos | Dolor abdominal moderado, fracturas cerradas, fiebre alta. |
| **Nivel 4** | **Menor** | `#2A9D8F` (Verde) | $\le 120$ minutos | Traumatismos leves, cuadros víricos estables. |
| **Nivel 5** | **No Urgente** | `#457B9D` (Azul) | $\le 240$ minutos | Renovación de recetas, trámites clínicos, problemas crónicos. |

---

## 3. Arquitectura del Sistema (Screaming Architecture)

El código fuente está diseñado bajo el principio de separación estricta de responsabilidades. La lógica matemática central es agnóstica de la plataforma y de los mecanismos de renderizado visual:

```
MediPriority/
│
├── CMakeLists.txt                 # Pipeline universal de configuración y compilación
├── README.md                      # Especificación técnica principal
│
├── docs/                          # Documentación formal de ingeniería
│   ├── SPEC-triage-system.md      # Documento de Requerimientos de Software (PRD)
│   ├── GUIA-DE-PRUEBAS-Y-DEFENSA.md # Batería de pruebas y sustentación académica
│   ├── PLAN-DE-TRABAJO-EQUIPO.md  # Asignación de roles y cronograma de commits
│   └── decisions/                 # Architecture Decision Records (ADRs)
│       ├── ADR-001-custom-min-heap.md
│       ├── ADR-002-dear-imgui-glfw.md
│       └── ADR-003-manchester-triage-tiebreaking.md
│
├── vendor/                        # Dependencias externas autocontenidas
│   └── imgui/                     # Framework Dear ImGui (Immediate Mode GUI)
│
├── src/
│   ├── Core/                      # [CAPA 1: ESTRUCTURAS DE DATOS CRÍTICAS]
│   │   ├── DynamicArray.hpp       # Búfer contiguo con gestión manual de memoria
│   │   └── PriorityQueue.hpp      # Min-Heap con métricas de benchmarking internas
│   │
│   ├── Triage/                    # [CAPA 2: DOMINIO CLÍNICO Y HOSPITALARIO]
│   │   ├── Patient.hpp            # Entidad Paciente y descriptores de estado
│   │   ├── TriageLevel.hpp        # Enumeración y metadatos clínicos Manchester
│   │   ├── TriageComparator.hpp   # Regla de ordenamiento y desempate FIFO
│   │   └── EmergencyRoom.hpp      # Motor de orquestación de camas y consultorios
│   │
│   ├── Presentation/              # [CAPA 3: INTERFAZ Y RENDERIZADO VISUAL]
│   │   ├── UITheme.hpp            # Design tokens clínicos y estilos tipográficos
│   │   ├── AdmissionView.hpp      # Módulo de ingreso y parametrización médica
│   │   ├── WaitingRoomView.hpp    # Monitor en tiempo real de pacientes priorizados
│   │   ├── ConsultationView.hpp   # Box médico de atención activa y despacho
│   │   └── HeapInspectorView.hpp  # Visualizador de bajo nivel del arreglo en memoria
│   │
│   └── main.cpp                   # Punto de entrada y ciclo de vida de la aplicación
```

---

## 4. Interfaz Gráfica (Dear ImGui & OpenGL 3.3)

La capa de presentación provee un **Centro de Mando de Emergencias** estructurado en cuatro cuadrantes de supervisión:

1. **Terminal de Triage & Admisión:** Formulario reactivo para el registro rápido de pacientes con cálculo automático de secuencia de llegada y selectores de severidad clínica.
2. **Tablero de Espera Priorizado (Monitor en Vivo):** Tabla con clasificación cromática que refleja el orden real de atención según la raíz del montículo.
3. **Módulo de Consultorio Activo:** Interfaz de despacho médico (*"Llamar Siguiente"*), registrando tiempos de latencia y estado del paciente.
4. **Inspector del Montículo (Vista Ingenieril):** Panel de depuración que expone en tiempo real el arreglo interno `buffer[0..n-1]`, permitiendo auditar visualmente las relaciones de nodos padre-hijos y los intercambios durante las operaciones de inserción y extracción.

---

## 5. Compilación y Despliegue

El proyecto utiliza un sistema de construcción autocontenido con **CMake**, garantizando portabilidad absoluta sin dependencias preinstaladas en el sistema operativo del evaluador.

### Requisitos del Sistema
* **Compilador C++:** Soporte para estándar **C++17** (GCC $\ge 7.0$, Clang $\ge 5.0$, MSVC $\ge 2017$).
* **Sistema de Construcción:** **CMake** versión 3.16 o superior.
* **Aceleración Gráfica:** Controlador compatible con **OpenGL 3.3 Core Profile**.

### Pipeline de Compilación Estándar

```bash
# 1. Clonar el repositorio
git clone <URL_DEL_REPOSITORIO>
cd "Sistema de Triage y Priorización de Pacientes en Emergencias"

# 2. Generar el entorno de compilación optimizado (Release)
cmake -B build -DCMAKE_BUILD_TYPE=Release

# 3. Compilar el binario del sistema
cmake --build build --config Release

# 4. Ejecutar el motor
./build/MediPriority
```

### Compilación Directa con MinGW / GCC (Modo Fallback)
En entornos donde CMake no esté en el `PATH` del sistema:

```bash
g++ -std=c++17 -O3 src/main.cpp -I src/ -o MediPriority.exe
./MediPriority.exe
```

---

## 6. Aseguramiento de Calidad y Verificación

El proyecto sigue lineamientos estrictos de calidad de software:

* **Gestión de Memoria:** Cumplimiento de la *Rule of 5* en clases administradoras de memoria dinámica para asegurar cero fugas (*zero memory leaks*).
* **Aislamiento de Dominio:** La lógica del montículo y de los pacientes no contiene dependencias de la consola ni del motor gráfico (`imgui.h`).
* **Sustentación Académica:** Consulte la [Guía de Pruebas y Sustentación](file:///C:/Users/GAMING/Desktop/Sistema%20de%20Triage%20y%20Priorizaci%C3%B3n%20de%20Pacientes%20en%20Emergencias/docs/GUIA-DE-PRUEBAS-Y-DEFENSA.md) para ejecutar los 5 casos de prueba de borde y responder las preguntas teóricas del tribunal evaluador.

---

## 7. Equipo de Ingeniería y Gobernanza

* **Anthony Aimacaña** — *Lead Software Architect & UI Subsystem*  
  Responsable de la arquitectura global, infraestructura CMake, capa de presentación Dear ImGui e integración del ciclo de vida de la aplicación.
* **Mateo Soto** — *Core Data Structures & Algorithmic Optimization*  
  Responsable del diseño matemático e implementación del Min-Heap manual, asignación dinámica de memoria y métricas de complejidad.
* **Alejandro Rivadeneira** — *Clinical Domain & Workflow Engine*  
  Responsable del modelado del protocolo Manchester, reglas de precedencia, desempate determinista FIFO y motor de sala de emergencias.

---

<div align="center">
<sub>Proyecto desarrollado para la cátedra de Estructura de Datos · Carrera de Ingeniería de Software / Sistemas · 2026</sub>
</div>
