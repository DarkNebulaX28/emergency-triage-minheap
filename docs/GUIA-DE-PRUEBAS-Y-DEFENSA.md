# Guía de Pruebas, Demostración y Defensa Académica — MediPriority

Este documento detalla el protocolo paso a paso para demostrar el funcionamiento del sistema **MediPriority** ante el docente durante la revisión del proyecto.

---

## 1. Ejecución de la Suite de Pruebas Unitarias (Verificación en Consola)

Antes de abrir la interfaz gráfica, se puede demostrar en 2 segundos que toda la lógica algorítmica pasa pruebas automatizadas ejecutando:

```powershell
.\MediPriority.exe --test
```

**Qué verifica este comando frente al docente:**
1. **Ordenamiento por Prioridad y Desempate FIFO:** Inserta pacientes en orden mezclado (`P4`, `P1`, `P3`, `P2`, `P1`) y comprueba que salgan exactamente en el orden `P1 (1º) -> P1 (2º) -> P2 -> P3 -> P4`.
2. **Escalamiento Dinámico de Urgencia:** Comprueba que si un paciente en espera empeora y se actualiza su prioridad a `P1`, la estructura reubica su nodo inmediatamente en la cabecera de prioridad crítica.
3. **Sanitización y Validación Clínica:** Comprueba que nombres con números (`"Juan123"`), edades negativas o mayores a 120 años, y prioridades fuera de `[1, 5]` sean rechazados automáticamente.

---

## 2. Guion de Demostración en Vivo (Interfaz Gráfica Dear ImGui)

Al ejecutar `.\MediPriority.exe` (o `.\compilar_y_ejecutar.bat`), siga estos 5 pasos para sorprender al evaluador:

### Paso 1: Carga Instantánea del Escenario Clínico (`Cargar Demo (6)`)
1. Haga clic en el botón azul **"Cargar Demo (6)"** ubicado en el panel izquierdo (*Registro de Triage*).
2. Observe cómo los indicadores KPI superiores se actualizan inmediatamente mostrando el desglose por nivel (`P1 Crítico`, `P2 Emergencia`, `P3 Urgencia`, `P4 Menor`, `P5 No Urgente`).
3. En el panel derecho se visualizarán las **tarjetas de nodos enlazados** conectadas por flechas `sig -->` hasta `[ nullptr (FIN) ]`.

### Paso 2: Demostración de Previsualización de Inserción y Desempate FIFO
1. En el formulario de admisión, escriba un paciente nuevo (ej. Nombre: `Roberto Gomez`, Edad: `58`, Síntomas: `Dolor precordial agudo`).
2. Seleccione el botón rápido **`P1`** o **`P2`**.
3. Señale el recuadro **"Previsualización de inserción"**: el sistema calcula en vivo en qué posición exacta de la lista enlazada se insertará el nodo respetando a los pacientes de igual prioridad que llegaron antes.
4. Presione **"Registrar en Cola"**.

### Paso 3: Atención Médica en Tiempo Real (`Atender Siguiente`)
1. Presione el botón verde **"Atender Siguiente (Frente)"**.
2. Observe cómo el nodo del frente (`CABECERA`) se desencola en tiempo $O(1)$ y pasa al panel superior **"BOX MÉDICO EN ATENCIÓN"**, mostrando su Ticket `#ID`, nombre, edad, hora de ingreso, cuadro clínico y nivel de prioridad.

### Paso 4: Re-Triage por Empeoramiento Clínico (`^ Subir Urgencia`)
1. Ubique cualquier tarjeta de paciente con prioridad `P3`, `P4` o `P5` en la cola visual.
2. Presione el botón **`^ Subir Urgencia`** dentro de su tarjeta.
3. El paciente subirá un nivel de gravedad (ej. de `P3` a `P2`) y su nodo se reordenará automáticamente en la lista enlazada.

### Paso 5: Auditoría de Memoria Física (`Inspector de Punteros y Memoria RAM`)
1. Active la casilla **"Mostrar Inspector de Punteros y Memoria RAM (Nodo*)"** arriba de la cola visual.
2. Se desplegará una tabla técnica con las columnas:
   * `# Pos`
   * `Ticket`
   * `Dirección Nodo* (RAM)` (ej. `0x02A4F810`)
   * `Puntero siguiente*` (que coincide exactamente con la dirección `Nodo*` de la fila siguiente, demostrando ante el ingeniero que es una estructura enlazada real en memoria dinámica).

---

## 3. Preguntas Frecuentes de Defensa Académica

* **¿Por qué el desempate FIFO funciona sin necesidad de comparar timestamps al insertar?**
  Porque el ciclo de inserción en `Cola<T>::encolar` utiliza la condición `actual->siguiente->prioridad <= prioridad`. Al usar `<=` en lugar de `<`, el nuevo nodo recorre y rebasa a todos los nodos existentes que tienen su misma prioridad, colocándose justo detrás del último de su categoría.
* **¿Cómo se evita las fugas de memoria (*Memory Leaks*)?**
  Cada `new Nodo<T>` reservado en `encolar()` tiene su correspondiente `delete temp` en `desencolar()`, `desencolarConPrioridad()`, `eliminarEnPosicion()`, `vaciar()` y en el destructor `~Cola()`.
