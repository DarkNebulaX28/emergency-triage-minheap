#ifndef OPERACIONES_COLA_H
#define OPERACIONES_COLA_H

#include <iostream>
#include "colaPrioridad.h"

using namespace std;

template <typename T>
Cola<T>::Cola() {
    frente = nullptr;
    fin = nullptr;
    cantidad = 0;
    totalOperaciones = 0;
}

template <typename T>
Cola<T>::~Cola() {
    limpiar();
}

template <typename T>
bool Cola<T>::verificar_vacio() const {
    return frente == nullptr;
}

template <typename T>
int Cola<T>::mostrar_tamanio() const {
    return cantidad;
}

template <typename T>
bool Cola<T>::mayorPrioridad(const int prioA, const int prioB) const {
    return prioA < prioB;
}

template <typename T>
void Cola<T>::encolar(const T& valor, int prioridad) {
    Nodo* nuevo = new Nodo(valor, prioridad);
    ++totalOperaciones;

    if (verificar_vacio()) { // Caso 1: Cola vacia
        frente = nuevo;
        fin = nuevo;
    } else if (mayorPrioridad(prioridad, frente->prioridad)) { // Caso 2: Nuevo debe ir al frente
        nuevo->sig = frente;
        frente = nuevo;
    } else { // Caso 3: Nuevo va en medio o al final (respeta FIFO ante igual prioridad con >=)
        Nodo* actual = frente;
        while (actual->sig != nullptr && prioridad >= actual->sig->prioridad) {
            actual = actual->sig;
            ++totalOperaciones;
        }
        nuevo->sig = actual->sig;
        actual->sig = nuevo;

        if (nuevo->sig == nullptr) {
            fin = nuevo;
        }
    }

    ++cantidad;
}

template <typename T>
bool Cola<T>::desencolar(T& atendido) {
    int prioridadIgnorada = 0;
    return desencolarConPrioridad(atendido, prioridadIgnorada);
}

template <typename T>
bool Cola<T>::desencolarConPrioridad(T& atendido, int& prioridadAtendida) {
    if (verificar_vacio()) {
        return false;
    }

    Nodo* eliminado = frente;
    atendido = eliminado->dato;
    prioridadAtendida = eliminado->prioridad;
    frente = frente->sig;

    if (frente == nullptr) {
        fin = nullptr;
    }

    delete eliminado;
    --cantidad;
    ++totalOperaciones;

    return true;
}

template <typename T>
bool Cola<T>::consultarFrente(T& dato) const {
    if (verificar_vacio()) {
        return false;
    }

    dato = frente->dato;
    return true;
}

template <typename T>
bool Cola<T>::consultarFrenteConPrioridad(T& dato, int& prioridadFrente) const {
    if (verificar_vacio()) {
        return false;
    }

    dato = frente->dato;
    prioridadFrente = frente->prioridad;
    return true;
}

template <typename T>
void Cola<T>::mostrarEstado() const {
    Nodo* actual = frente;

    while (actual != nullptr) {
        cout << "[Prioridad " << actual->prioridad << "] " << actual->dato << endl;
        actual = actual->sig;
    }

    if (verificar_vacio()) {
        cout << "\nFrente = nullptr\t \nFin = nullptr\n";
    } else {
        cout << "\nFrente -> dato = " << frente->dato;
        cout << "\nFin    -> dato = " << fin->dato;
    }

    cout << "\nTamanio = " << cantidad << endl;
}

template <typename T>
void Cola<T>::limpiar() {
    Nodo* actual = frente;

    while (actual != nullptr) {
        Nodo* siguiente = actual->sig;
        delete actual;
        actual = siguiente;
    }

    frente = nullptr;
    fin = nullptr;
    cantidad = 0;
}

template <typename T>
bool Cola<T>::obtenerElemento(int posicion, T& dato, int& prioridad) const {
    if (posicion < 0 || posicion >= cantidad) {
        return false;
    }

    Nodo* actual = frente;
    int indice = 0;

    while (actual != nullptr && indice < posicion) {
        actual = actual->sig;
        indice++;
    }

    if (actual == nullptr) {
        return false;
    }

    dato = actual->dato;
    prioridad = actual->prioridad;

    return true;
}

template <typename T>
int Cola<T>::obtenerPosicionInsercion(int prioridad) const {
    Nodo* actual = frente;
    int posicion = 0;

    while (actual != nullptr && actual->prioridad <= prioridad) {
        actual = actual->sig;
        posicion++;
    }

    return posicion;
}

template <typename T>
bool Cola<T>::eliminarEnPosicion(int posicion, T& eliminado) {
    if (posicion < 0 || posicion >= cantidad || verificar_vacio()) {
        return false;
    }

    if (posicion == 0) {
        return desencolar(eliminado);
    }

    Nodo* anterior = frente;
    for (int i = 0; i < posicion - 1; ++i) {
        anterior = anterior->sig;
    }

    Nodo* objetivo = anterior->sig;
    if (objetivo == nullptr) {
        return false;
    }

    eliminado = objetivo->dato;
    anterior->sig = objetivo->sig;

    if (anterior->sig == nullptr) {
        fin = anterior;
    }

    delete objetivo;
    --cantidad;
    ++totalOperaciones;
    return true;
}

template <typename T>
bool Cola<T>::actualizarPrioridad(int posicion, int nuevaPrioridad) {
    if (posicion < 0 || posicion >= cantidad || nuevaPrioridad < 1 || nuevaPrioridad > 5) {
        return false;
    }

    T pacienteExtraido;
    if (!eliminarEnPosicion(posicion, pacienteExtraido)) {
        return false;
    }

    encolar(pacienteExtraido, nuevaPrioridad);
    return true;
}

template <typename T>
int Cola<T>::contarPorPrioridad(int prioridadBuscada) const {
    int contador = 0;
    Nodo* actual = frente;
    while (actual != nullptr) {
        if (actual->prioridad == prioridadBuscada) {
            ++contador;
        }
        actual = actual->sig;
    }
    return contador;
}

template <typename T>
const void* Cola<T>::obtenerDireccionNodo(int posicion) const {
    if (posicion < 0 || posicion >= cantidad) {
        return nullptr;
    }
    Nodo* actual = frente;
    for (int i = 0; i < posicion && actual != nullptr; ++i) {
        actual = actual->sig;
    }
    return static_cast<const void*>(actual);
}

template <typename T>
const void* Cola<T>::obtenerDireccionSiguiente(int posicion) const {
    if (posicion < 0 || posicion >= cantidad) {
        return nullptr;
    }
    Nodo* actual = frente;
    for (int i = 0; i < posicion && actual != nullptr; ++i) {
        actual = actual->sig;
    }
    if (actual == nullptr) {
        return nullptr;
    }
    return static_cast<const void*>(actual->sig);
}

template <typename T>
int Cola<T>::obtenerTotalOperaciones() const {
    return totalOperaciones;
}

#endif // OPERACIONES_COLA_H
