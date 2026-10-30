# ADR-002: Elección de Dear ImGui con GLFW/OpenGL para la Interfaz Gráfica

## Estado
Aceptado

## Fecha
2026-10-29

## Contexto
El proyecto requiere una interfaz gráfica (GUI) moderna, clara y comprensible para operar el Sistema de Triage. Un factor crítico es que **el proyecto será subido a un repositorio Git y evaluado por el docente en su propia computadora**.

Si la interfaz requiere instalar SDKs pesados o configuraciones complejas en la máquina del evaluador, se corre un alto riesgo de fallos de compilación ("en mi máquina sí compila"), afectando la calificación.

## Decisión
Utilizar **Dear ImGui (Immediate Mode GUI)** con el backend de ventana **GLFW** y renderizado **OpenGL 3**.

Se integrarán los archivos fuentes de ImGui (`imgui.cpp`, `imgui_draw.cpp`, etc.) directamente en el repositorio (`vendor/imgui/`), permitiendo que el proyecto se compile de forma autosuficiente sin requerir paquetes externos en el sistema operativo del profesor.

## Alternativas Consideradas

### 1. Qt Framework (C++ Widgets o QML)
* **Ventajas:** Aspecto nativo pulido, herramientas de diseño visual (Qt Designer).
* **Desventajas:** 
  * Requiere instalar varios gigabytes de Qt SDK.
  * Problemas habituales de enlaces de bibliotecas y DLLs faltantes (`windeployqt`).
  * Alto acoplamiento con la herramienta `qmake` o macros `MOC` de Qt.
* **Motivo de Rechazo:** Inadecuado para un repositorio de evaluación rápida docente.

### 2. Raylib
* **Ventajas:** Muy simple para videojuegos y dibujo 2D directo (ideal para árboles con círculos y líneas).
* **Desventajas:** Diseñar formularios con campos de texto, tablas complejas y listas de pacientes requiere construir o adaptar controles manualmente.
* **Motivo de Rechazo:** Menos ergonómico para un dashboard administrativo/médico que ImGui.

### 3. Interfaz de Consola (TUI / ncurses)
* **Ventajas:** Cero dependencias gráficas.
* **Desventajas:** No cumple con la expectativa de una interfaz moderna y atractiva para presentar el proyecto.

## Consecuencias
* **Positivas:**
  * **Máxima portabilidad:** Compilación limpia mediante CMake con MinGW o MSVC en Windows, así como en Linux y macOS.
  * **Rendimiento extremo:** Renderizado en tiempo real a 60+ FPS con bajo consumo de CPU/RAM.
  * **Componentes ideales de fábrica:** Tablas con ordenamiento, badges de colores, campos de texto, botones estilizados y popups de confirmación listos para usar.
* **Negativas / Desafíos:**
  * El paradigma *Immediate Mode* requiere mantener el estado de la aplicación en las entidades de dominio y no en los controles de la UI (lo cual favorece la arquitectura limpia y desacoplada).
