# MediPriority — Sistema de Triage y Priorización de Pacientes en Emergencias

[![Grupo](https://img.shields.io/badge/Grupo%202-Colas%20de%20Prioridad-0052CC.svg)]()
[![Standard](https://img.shields.io/badge/C%2B%2B-17-00599C.svg?logo=c%2B%2B&logoColor=white)]()
[![IDE](https://img.shields.io/badge/IDE-Code%3A%3ABlocks%20%7C%20MinGW-2EA44F.svg)]()
[![UI](https://img.shields.io/badge/UI-Dear%20ImGui%20%2B%20OpenGL3-1A73E8.svg)]()
[![STL](https://img.shields.io/badge/STL%20Containers-None%20(100%25%20Manual)-critical.svg)]()

> **Documento de Propuesta Técnica y Manual de Ejecución — Actividad en Equipo: Aplicación de Estructuras de Datos Dinámicas (Grupo 2: Colas de Prioridad)**

---

## Tabla de Contenidos (Según Rúbrica Oficial de Entregables)

1. [Nombre del Proyecto e Integrantes (Grupo 2)](#1-nombre-del-proyecto-e-integrantes-grupo-2)
2. [Problema Real y Objetivo del Sistema](#2-problema-real-y-objetivo-del-sistema)
3. [Justificación de la Estructura de Datos y Limitaciones](#3-justificación-de-la-estructura-de-datos-y-limitaciones)
4. [Operaciones Requeridas y Complejidad Algorítmica](#4-operaciones-requeridas-y-complejidad-algorítmica)
5. [Diseño del TDA, Encapsulamiento y Reutilización de Clases Genéricas (`templates`)](#5-diseño-del-tda-encapsulamiento-y-reutilización-de-clases-genéricas-templates)
6. [Propuesta de UI y Justificación de la Librería Seleccionada](#6-propuesta-de-ui-y-justificación-de-la-librería-seleccionada)
7. [Organización de Archivos del Ejercicio](#7-organización-de-archivos-del-ejercicio)
8. [Instrucciones de Ejecución y Compilación (Code::Blocks, Script 1 Clic y Consola)](#8-instrucciones-de-ejecución-y-compilación-codeblocks-script-1-clic-y-consola)
9. [Funcionalidad de la Demostración y Casos Límite para Defensa en Zoom](#9-funcionalidad-de-la-demostración-y-casos-límite-para-defensa-en-zoom)

---

## 1. Nombre del Proyecto e Integrantes (Grupo 2)

* **Nombre del Proyecto:** **MediPriority** — Sistema Clínico de Triage y Priorización de Pacientes en Emergencias Hospitalarias.
* **Tema Asignado:** **Grupo 2 — Colas de Prioridad Dinámicas**.
* **Integrantes del Equipo:**

| Integrante | Rol y Módulos Desarrollados |
| :--- | :--- |
| **Anthony Aimacaña** (`@xavieraimacana`) | Coordinación arquitectónica, integración general, diseño de interfaz gráfica animada auto-adaptable (Cero Scroll), inspector de punteros RAM, reutilización de templates y documentación técnica. |
| **Mateo Soto** (`@MateoS`) | Diseño e implementación manual desde cero del TDA genérico `Cola<T>` y su gestión de memoria dinámica (`colaPrioridad.h`, `operaciones_colaPriori.h`). |
| **Alejandro Rivadeneira** | Modelado del TDA `Paciente`, reglas de validación clínica (`validacion.h`) y estructura base de interfaz gráfica (`interfaz.h`, `interfaz.cpp`). |

---

## 2. Problema Real y Objetivo del Sistema

### 2.1. Problema Real (Sala de Emergencias Hospitalarias)
En el área de urgencias de un hospital, los pacientes arriban de manera continua presentando cuadros clínicos de severidad heterogénea: desde paros cardiorrespiratorios o traumas craneoencefálicos severos (**Nivel 1 - Resucitación**) hasta esguinces leves o controles ambulatorios (**Nivel 5 - No Urgente**).

Si el hospital utilizara una cola común *First-In, First-Out (FIFO)* por orden de llegada, la vida de un paciente crítico correría peligro mientras espera detrás de pacientes con dolencias menores. Por el contrario, se requiere un mecanismo que ordene dinámicamente a los pacientes por su **nivel de gravedad clínica (Escala de Triaje Manchester 1 a 5)** y que, ante dos pacientes con la **misma prioridad**, respete estrictamente quién llegó primero (**desempate cronológico FIFO**).

### 2.2. Objetivo
Diseñar e implementar desde cero en **C++17** un sistema de triaje hospitalario basado en una **Cola de Prioridad Dinámica Enlazada (`Cola<T>`)**, aplicando Tipos de Datos Abstractos (TDA) con atributos encapsulados (`private`), plantillas genéricas (`templates`), gestión manual de memoria dinámica (`new` / `delete`) sin utilizar contenedores de la STL (`<vector>`, `<algorithm>`, `<stack>`, `<queue>`), e integrando una interfaz gráfica interactiva (**Dear ImGui + OpenGL3**) que permita observar en tiempo real las operaciones, punteros (`frente`, `fin`, `sig*`) y casos límite de la estructura.

---

## 3. Justificación de la Estructura de Datos y Limitaciones

### 3.1. ¿Por qué una Cola de Prioridad Dinámica Enlazada es adecuada para este problema?
1. **Atención Médica Inmediata en $O(1)$:** Al mantener la lista enlazada ordenada por prioridad en todo momento, el paciente más crítico siempre se ubica en el puntero `frente`. Cuando un médico queda libre en el consultorio (`desencolar`), la extracción toma **tiempo constante $O(1)$**.
2. **Estabilidad FIFO Natural y Exacta:** Al insertar un nuevo nodo (`encolar`) avanzando mientras `prioridad >= actual->sig->prioridad`, el nuevo paciente se posiciona automáticamente detrás de todos los pacientes previamente registrados con igual o mayor urgencia, preservando el orden cronológico de llegada sin estructuras auxiliares.
3. **Crecimiento Dinámico sin Límite Fijo:** A diferencia de un arreglo estático, la memoria se reserva nodo a nodo en el *heap* (`new Nodo(valor, prioridad)`), adaptándose a picos de demanda en emergencias sin desperdiciar memoria ni requerir redimensionamientos costosos.
4. **Visibilidad Explícita de Enlaces:** Permite inspeccionar en vivo las referencias `frente`, `fin` y los punteros `sig` (`Nodo* -> sig* -> nullptr`).

### 3.2. Limitaciones de la Estructura
1. **Costo de Inserción Lineal $O(N)$:** Para mantener el orden de prioridad y la estabilidad FIFO, la operación `encolar` debe recorrer en el peor caso hasta $N$ nodos para ubicar el punto exacto de inserción. (En salas de emergencia reales donde $N$ oscila entre $10$ y $200$ pacientes activos, este recorrido toma menos de $1\ \mu\text{s}$, siendo imperceptible).
2. **Acceso Posicional Secuencial $O(N)$:** A diferencia de estructuras indexadas en memoria contigua, consultar o eliminar una posición arbitraria `k` (`obtenerElemento`, `eliminarEnPosicion`) requiere recorrer los punteros `sig` desde `frente` en $O(k)$.

---

## 4. Operaciones Requeridas y Complejidad Algorítmica

| Operación del TDA `Cola<T>` | Firma Pública en [`src/colaPrioridad.h`](src/colaPrioridad.h) | Descripción Funcional | Complejidad |
| :--- | :--- | :--- | :---: |
| **Inserción Priorizada** | `void encolar(const T& valor, int prioridad)` | Reserva un `Nodo` en memoria dinámica y lo enlaza según su prioridad (`1..5`) respetando FIFO. | $O(N)$ |
| **Extracción / Atención** | `bool desencolar(T& atendido)` / `desencolarConPrioridad(...)` | Extrae el nodo apuntado por `frente`, actualiza `frente` (y `fin` si queda vacía) y ejecuta `delete`. | $O(1)$ |
| **Consulta de Cabecera** | `bool consultarFrente(T& dato) const` | Obtiene el elemento de mayor urgencia en `frente` sin modificar la estructura. | $O(1)$ |
| **Consulta / Recorrido** | `bool obtenerElemento(int posicion, T& dato, int& prio) const` | Recorre los enlaces `sig` desde `frente` hasta la posición solicitada por el usuario o la UI. | $O(N)$ |
| **Eliminación por Posición** | `bool eliminarEnPosicion(int posicion, T& eliminado)` | Desenlaza y libera (`delete`) el nodo en la posición indicada por el usuario, reajustando `frente`/`fin`. | $O(N)$ |
| **Re-categorización Clínica** | `bool actualizarPrioridad(int posicion, int nuevaPrioridad)` | Extrae un paciente que empeora clínicamente y lo reinserta en su nuevo nivel de urgencia. | $O(N)$ |
| **Verificación de Vacío** | `bool verificar_vacio() const` | Comprueba si `frente == nullptr`. | $O(1)$ |
| **Liberación Total** | `void limpiar()` / `~Cola()` | Recorre toda la estructura liberando cada nodo con `delete` para evitar fugas de memoria (*memory leaks*). | $O(N)$ |

---

## 5. Diseño del TDA, Encapsulamiento y Reutilización de Clases Genéricas (`templates`)

### 5.1. Encapsulamiento Estricto (`private` / `public`)
Tanto el contenedor genérico **`class Cola<T>`** ([`src/colaPrioridad.h`](src/colaPrioridad.h)) como la entidad clínica **`class Paciente`** ([`src/Paciente.h`](src/Paciente.h)) encapsulan todos sus atributos bajo acceso **`private:`**:

* En **`class Cola<T>`**:
  * **Atributos `private`:** `struct Nodo { T dato; Nodo* sig; int prioridad; }`, puntero `Nodo* frente`, puntero `Nodo* fin`, `int cantidad` e `int totalOperaciones`.
  * **Operaciones `public`:** Constructor, destructor (con constructores de copia eliminados para evitar *shallow copy* y dobles liberaciones), inserción, extracción, consulta, recorrido e inspección de punteros.
* En **`class Paciente`**:
  * **Atributos `private`:** `int id`, `string nombre`, `int edad`, `string sintomas`, `string horaIngreso`.
  * **Operaciones `public`:** Constructores sobrecargados, *getters* constantes (`getId()`, `getNombre()`, `getEdad()`, `getSintomas()`, `getHoraIngreso()`), *setters* y sobrecarga de `operator<<`.

### 5.2. Demostración de Reutilización de la Plantilla (`Cola<T>`)
Para demostrar que el TDA `Cola<T>` es verdaderamente genérico y reutilizable con distintos tipos de datos sin modificar una sola línea de su implementación:
1. **Instancia 1 — `Cola<Paciente> colaPacientes`:** Administra objetos complejos de la clase `Paciente` en la sala de emergencias.
2. **Instancia 2 — `Cola<std::string> colaAlertasClinicas`:** Administra cadenas de texto (`std::string`) como una bitácora priorizada de alertas médicas en tiempo real, visible directamente en la interfaz gráfica seleccionando la vista **`Template Cola<string>`** o en las pruebas unitarias (`--test`).

---

## 6. Propuesta de UI y Justificación de la Librería Seleccionada

* **Librería UI Seleccionada:** **Dear ImGui v1.91 + OpenGL3** (con backend híbrido automático **Win32 WGL** para compiladores de 32 bits y **GLFW 3.5** para compiladores de 64 bits).
* **Justificación Técnica:**
  1. **Integración Nativa 100% C++:** Dear ImGui se compila directamente junto con el código fuente del proyecto sin requerir instaladores externos, entornos pesados (como Qt de varios gigabytes) ni DLLs adicionales en el sistema del evaluador.
  2. **Renderizado en Tiempo Real para Estructuras de Datos:** Al ser una interfaz de modo inmediato (*Immediate Mode GUI*) acelerada por GPU, redibuja a 60 FPS el estado exacto de `Cola<Paciente>` en cada cuadro, permitiendo animar la inserción de nodos (`easeOutCubic`), el pulso de pacientes críticos (`Prioridad 1`) y el flujo de partículas sobre los punteros `sig*` entre nodos.
  3. **Diseño Auto-Adaptable con Cero Scroll:** La ventana calcula dinámicamente las proporciones de cada panel y escala las tarjetas de nodos enlazados según el área cliente disponible (`GetClientRect` / `GetContentRegionAvail`), logrando visualizar toda la cola sin barras de desplazamiento.

---

## 7. Organización de Archivos del Ejercicio

El proyecto cumple rigurosamente la estructura modular solicitada en la guía de la actividad:

| Archivo | Rol Según Especificación de la Actividad |
| :--- | :--- |
| [`src/colaPrioridad.h`](src/colaPrioridad.h) y [`src/Paciente.h`](src/Paciente.h) | **Archivos `.h` con las clases, atributos `private` y funciones únicamente declaradas.** |
| [`src/operaciones_colaPriori.h`](src/operaciones_colaPriori.h) | **Archivo `.h` con el cuerpo y desarrollo algorítmico de cada una de las funciones declaradas.** |
| [`src/validacion.h`](src/validacion.h) | **Archivo `.h` con las validaciones del caso clínico, validación de posiciones y el menú de opciones disponibles.** |
| [`src/main.cpp`](src/main.cpp) | **Archivo `.cpp` con un único método principal `main` que invoca a las funciones de los archivos cabecera.** |
| [`src/interfaz.h`](src/interfaz.h) y [`src/interfaz.cpp`](src/interfaz.cpp) | Módulo de representación visual interactiva desacoplado de la lógica interna del TDA. |
| [`ColasPrioridad.cbp`](ColasPrioridad.cbp) | Archivo de proyecto oficial para el IDE **Code::Blocks**. |
| [`compilar_y_ejecutar.bat`](compilar_y_ejecutar.bat) | Script automatizado de compilación y ejecución en 1 clic para Windows. |

---

## 8. Instrucciones de Ejecución y Compilación (Code::Blocks, Script 1 Clic y Consola)

El repositorio incluye dentro de `external/` y `fonts/` todo el código de **Dear ImGui** y las fuentes tipográficas necesarias. No requiere instalar paquetes externos.

### Opción A — Ejecución Rápida en 1 Clic desde cualquier PC con Windows (Recomendado)

```powershell
git clone https://github.com/xavieraimacana/emergency-triage-minheap.git
cd emergency-triage-minheap
.\compilar_y_ejecutar.bat
```
*(El script detecta automáticamente el compilador `g++` en `PATH`, `C:\MinGW\bin`, `C:\msys64\mingw64\bin` o `C:\Program Files\CodeBlocks\MinGW\bin`, compila `MediPriority.exe`, ejecuta las pruebas unitarias y abre la interfaz gráfica).*

### Opción B — Desde el IDE Code::Blocks (Requisito Oficial)

1. Abrir **Code::Blocks**.
2. Seleccionar **File -> Open...** y abrir el archivo [`ColasPrioridad.cbp`](ColasPrioridad.cbp) ubicado en la raíz del repositorio.
3. Presionar **F9 (Build and Run)**.

### Opción C — Compilación Manual por Consola (`g++`)

* **En MinGW de 32 bits:**
  ```powershell
  g++ -std=c++17 -O2 -Isrc -Iexternal/imgui -Iexternal/imgui/backends src/main.cpp src/interfaz.cpp external/imgui/imgui.cpp external/imgui/imgui_draw.cpp external/imgui/imgui_tables.cpp external/imgui/imgui_widgets.cpp external/imgui/backends/imgui_impl_win32.cpp external/imgui/backends/imgui_impl_opengl3.cpp -o MediPriority.exe -lopengl32 -lgdi32 -luser32 -limm32
  .\MediPriority.exe
  ```
* **En MinGW-w64 de 64 bits:**
  ```powershell
  g++ -std=c++17 -O2 -Isrc -Iexternal/imgui -Iexternal/imgui/backends -Iexternal/glfw-3.5.1.bin.WIN64/include src/main.cpp src/interfaz.cpp external/imgui/imgui.cpp external/imgui/imgui_draw.cpp external/imgui/imgui_tables.cpp external/imgui/imgui_widgets.cpp external/imgui/backends/imgui_impl_glfw.cpp external/imgui/backends/imgui_impl_opengl3.cpp external/glfw-3.5.1.bin.WIN64/lib-mingw-w64/libglfw3.a -o MediPriority.exe -lopengl32 -lgdi32 -luser32 -lshell32 -limm32
  .\MediPriority.exe
  ```

---

## 9. Funcionalidad de la Demostración y Casos Límite para Defensa en Zoom

Durante la defensa del proyecto, cualquier integrante puede demostrar todos los puntos de la sección 5 del PDF directamente desde la interfaz gráfica:

1. **Casos Límite en 1 Clic (Barra Superior):**
   * Botón **`Caso: Vacía`**: Vacía la cola (`frente = nullptr, fin = nullptr, cantidad = 0`). Si se presiona **`ATENDER PACIENTE`** o **`Eliminar Pos`**, el sistema intercepta la operación y muestra el mensaje de validación de estructura vacía sin colapsar.
   * Botón **`Caso: 1 Nodo`**: Inserta un único paciente demostrando visualmente el caso límite donde **`[FRENTE = FIN #1]`** apuntan exactamente a la misma dirección de memoria y `sig -> NULL`.
   * Botón **`Cargar Demo (6)`**: Inserta 6 pacientes con prioridades mezcladas (`1, 2, 3, 4`) y empates (`P1` y `P2`) demostrando el ordenamiento automático y el desempate **FIFO**.
2. **Inserción, Eliminación y Consulta por Posición Ingresada por el Usuario:**
   * En la barra de la cola inferior se incluye el campo **`Pos: [ 1 ]`** junto con los botones **`Consultar Pos`** y **`Eliminar Pos`**, además de los botones **`^ Urgencia`** y **`Retirar`** en cada tarjeta.
3. **Visualización de Punteros (`frente`, `fin`, `sig*`) y Reutilización de Template:**
   * Con los 3 botones de radio (`Nodos (Frente->Fin)`, `Punteros RAM (Nodo*)` y `Template Cola<string>`), se alterna en vivo entre el diagrama de tarjetas enlazadas, la tabla de direcciones hexadecimales reales en memoria RAM y la segunda instancia de la plantilla `Cola<std::string>`.
