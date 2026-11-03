# Especificación Técnica: Sistema de Triage y Priorización de Pacientes (MediPriority v1.0.0)

* **Fecha:** 2026-11-03
* **Estado:** Producción / Completado
* **Estándar:** C++17 (Sin contenedores STL de cola de prioridad) + Dear ImGui (OpenGL3)

---

## 1. Contexto y Objetivo Académico

En una sala de emergencias hospitalarias, los pacientes arriban de manera continua con cuadros clínicos de diversa gravedad. El objetivo del sistema **MediPriority** es gestionar el flujo de admisión, clasificación (Triage) y despacho médico asegurando que:

1. **Prioridad Clínica Estricta:** Todo paciente con menor número de prioridad ($P_i < P_j$, donde $1$ es Crítico y $5$ es No Urgente) sea atendido antes.
2. **Estabilidad Cronológica (FIFO en Empates):** Si dos pacientes poseen el mismo nivel de urgencia ($P_i == P_j$), se atienda primero a quien ingresó antes al hospital.
3. **Implementación Manual de Estructura de Datos:** La cola de prioridad debe implementarse desde cero utilizando nodos dinámicos enlazados (`Nodo<T>` y punteros `siguiente`), sin apoyarse en `std::priority_queue`.

---

## 2. Modelo de Dominio (`src/Paciente.h` y `src/validacion.h`)

### 2.1. Entidad `Paciente`
Cada paciente registrado encapsula tanto sus datos clínicos como metadatos de trazabilidad:

| Atributo | Tipo | Descripción |
| :--- | :--- | :--- |
| `id` | `int` | Ticket único autoincremental generado al registrar al paciente (`#1, #2, ...`). |
| `nombre` | `std::string` | Nombre completo del paciente (validado únicamente con caracteres alfabéticos y espacios). |
| `edad` | `int` | Edad biológica en años cumplidos (`0` a `120`). |
| `sintomas` | `std::string` | Motivo de consulta o cuadro clínico de ingreso. |
| `horaIngreso` | `std::string` | Marca de tiempo exacta de admisión en formato `HH:MM:SS`. |

### 2.2. Reglas de Validación (`validarPaciente`)
Antes de insertar cualquier nodo en la estructura `Cola<Paciente>`, [`src/validacion.h`](../src/validacion.h) verifica:
* `nombre`: No vacío, sin espacios iniciales/finales inválidos y compuesto exclusivamente por letras o caracteres acentuados (`nombreContieneSoloLetras`).
* `edad`: Valor numérico entero dentro del rango clínico `[0, 120]`.
* `sintomas`: Cadena no vacía con descripción clínica válida.
* `prioridad`: Entero estricto dentro del intervalo `[1, 5]`.

---

## 3. Especificación de la Estructura `Cola<T>` (`src/colaPrioridad.h`)

### 3.1. Nodo Enlazado
```cpp
template <typename T>
struct Nodo {
    T dato;
    int prioridad;
    Nodo<T>* siguiente;
};
```

### 3.2. Operaciones Fundamentales y Avanzadas
* `encolar(const T& valor, int prioridad)`: Inserta un nodo dinámicamente en su posición correspondiente según `prioridad` (`1..5`), respetando orden FIFO mediante la condición `actual->siguiente->prioridad <= prioridad`.
* `desencolar()` y `desencolarConPrioridad(int& prioridadSalida)`: Extrae el nodo ubicado en `frente` en tiempo $O(1)$, libera su memoria con `delete` y retorna el paciente para su atención en el Box Médico.
* `obtenerPosicionInsercion(int prioridad)`: Calcula en tiempo real en qué posición (`1..N`) quedará ubicado un paciente antes de presionar el botón de registro.
* `actualizarPrioridad(int posicion, int nuevaPrioridad)`: Permite re-clasificar a un paciente que empeora en la sala de espera, extrayendo su nodo y reinsertándolo en su nueva ubicación de urgencia.
* `eliminarEnPosicion(int posicion, T& valorEliminado)`: Permite retirar a un paciente específico de la fila (por alta voluntaria o traslado).
* `obtenerDireccionNodo(int posicion)` y `obtenerDireccionSiguiente(int posicion)`: Retornan los punteros reales `const void*` en memoria RAM para su auditoría académica en la interfaz gráfica.
