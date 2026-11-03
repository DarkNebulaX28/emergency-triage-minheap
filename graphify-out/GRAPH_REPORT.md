# Graph Report - Sistema de Triage y Priorización de Pacientes en Emergencias  (2026-11-03)

## Corpus Check
- Corpus is ~10,306 words - fits in a single context window. You may not need a graph.

## Summary
- 120 nodes · 192 edges · 8 communities (7 shown, 1 thin omitted)
- Extraction: 92% EXTRACTED · 8% INFERRED · 0% AMBIGUOUS · INFERRED: 15 edges (avg confidence: 0.85)
- Token cost: 1,200 input · 450 output

## Community Hubs (Navigation)
- Cola de Prioridad (Nodos Dinamicos)
- Interfaz Grafica (Dear ImGui)
- Cola de Prioridad (Nodos Dinamicos)
- Dominio de Paciente y Triaje
- Validacion Clinica de Datos
- Interfaz Grafica (Dear ImGui)
- Interfaz Grafica (Dear ImGui)
- Cola de Prioridad (Nodos Dinamicos)

## God Nodes (most connected - your core abstractions)
1. `Cola` - 26 edges
2. `Paciente` - 11 edges
3. `dibujarInterfaz()` - 11 edges
4. `validarPaciente()` - 9 edges
5. `Nodo` - 6 edges
6. `obtenerNombrePrioridad()` - 6 edges
7. `dibujarVistaPrevia()` - 6 edges
8. `WndProcWin32()` - 6 edges
9. `SPEC-triage-system.md (PRD & Arquitectura)` - 6 edges
10. `obtenerColor()` - 5 edges

## Surprising Connections (you probably didn't know these)
- `ADR-002 (Dear ImGui + OpenGL3 / GLFW)` --specifies--> `dibujarInterfaz()`  [EXTRACTED]
  docs/decisions/ADR-002-dear-imgui-glfw.md → src/interfaz.cpp
- `ADR-001 (Cola de Prioridad Nodos Dinamicos)` --specifies--> `Cola`  [EXTRACTED]
  docs/decisions/ADR-001-custom-min-heap.md → src/colaPrioridad.h
- `GUIA-DE-PRUEBAS-Y-DEFENSA.md (Sustentacion)` --specifies--> `ejecutarPruebasUnitarias()`  [EXTRACTED]
  docs/GUIA-DE-PRUEBAS-Y-DEFENSA.md → src/main.cpp
- `README.md (Manual Principal & Guia Docente)` --documents--> `SPEC-triage-system.md (PRD & Arquitectura)`  [EXTRACTED]
  README.md → docs/SPEC-triage-system.md
- `agregarPaciente()` --calls--> `validarPaciente()`  [INFERRED]
  src/interfaz.cpp → src/validacion.h

## Import Cycles
- None detected.

## Communities (8 total, 1 thin omitted)

### Community 0 - "Cola de Prioridad (Nodos Dinamicos)"
Cohesion: 0.08
Nodes (13): ADR-001 (Cola de Prioridad Nodos Dinamicos), ADR-002 (Dear ImGui + OpenGL3 / GLFW), ADR-003 (Protocolo Manchester & Estabilidad FIFO), PLAN-DE-TRABAJO-EQUIPO.md (Roles: Anthony, Mateo, Alejandro), README.md (Manual Principal & Guia Docente), SPEC-triage-system.md (PRD & Arquitectura), Cola, cantidad (+5 more)

### Community 1 - "Interfaz Grafica (Dear ImGui)"
Cohesion: 0.15
Nodes (13): agregarPaciente(), atenderPaciente(), cargarFuentesSistema(), cargarPacientesDemo(), configurarEstilo(), dibujarInterfaz(), dibujarTarjeta(), dibujarTitulo() (+5 more)

### Community 2 - "Cola de Prioridad (Nodos Dinamicos)"
Cohesion: 0.13
Nodes (8): Cola<T>::consultarFrente(), Cola<T>::consultarFrenteConPrioridad(), Cola<T>::desencolar(), Cola<T>::desencolarConPrioridad(), Cola<T>::eliminarEnPosicion(), Cola<T>::encolar(), Cola<T>::mostrarEstado(), Cola<T>::obtenerElemento()

### Community 3 - "Dominio de Paciente y Triaje"
Cohesion: 0.18
Nodes (6): Paciente, edad, horaIngreso, id, nombre, sintomas

### Community 4 - "Validacion Clinica de Datos"
Cohesion: 0.47
Nodes (7): nombreContieneSoloLetras(), textoVacio(), validarEdad(), validarNombre(), validarPaciente(), validarPrioridad(), validarSintomas()

### Community 5 - "Interfaz Grafica (Dear ImGui)"
Cohesion: 0.28
Nodes (4): GUIA-DE-PRUEBAS-Y-DEFENSA.md (Sustentacion), ejecutarPruebasUnitarias(), main(), mostrarAyuda()

### Community 7 - "Cola de Prioridad (Nodos Dinamicos)"
Cohesion: 0.40
Nodes (4): Nodo, dato, prioridad, sig

## Knowledge Gaps
- **13 isolated node(s):** `id`, `nombre`, `edad`, `sintomas`, `horaIngreso` (+8 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 53 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **1 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Cola` connect `Cola de Prioridad (Nodos Dinamicos)` to `Cola de Prioridad (Nodos Dinamicos)`, `Interfaz Grafica (Dear ImGui)`, `Cola de Prioridad (Nodos Dinamicos)`?**
  _High betweenness centrality (0.382) - this node is a cross-community bridge._
- **Are the 2 inferred relationships involving `validarPaciente()` (e.g. with `agregarPaciente()` and `ejecutarPruebasUnitarias()`) actually correct?**
  _`validarPaciente()` has 2 INFERRED edges - model-reasoned connections that need verification._
- **What connects `id`, `nombre`, `edad` to the rest of the system?**
  _13 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `Cola de Prioridad (Nodos Dinamicos)` be split into smaller, more focused modules?**
  _Cohesion score 0.07671957671957672 - nodes in this community are weakly interconnected._
- **Why does `Paciente` connect `Dominio de Paciente y Triaje` to `Interfaz Grafica (Dear ImGui)`?**
  _High betweenness centrality (0.102) - this node is a cross-community bridge._
- **Should `Interfaz Grafica (Dear ImGui)` be split into smaller, more focused modules?**
  _Cohesion score 0.14855072463768115 - nodes in this community are weakly interconnected._
- **Why does `WndProcWin32()` connect `Interfaz Grafica (Dear ImGui)` to `Interfaz Grafica (Dear ImGui)`?**
  _High betweenness centrality (0.083) - this node is a cross-community bridge._