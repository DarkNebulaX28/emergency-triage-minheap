#ifndef OPERACIONES_COLA_H
#define OPERACIONES_COLA_H

#include <iostream>
#include "colaPrioridad.h"

using namespace std;

template <typename T>
Cola<T>::Cola(){        
    frente = nullptr;
    fin = nullptr;
    cantidad = 0;
}

template <typename T>
Cola<T>::~Cola(){
    limpiar();
}

template <typename T>
bool Cola<T>::verificar_vacio() const{
    return frente == nullptr;
}

template <typename T>
int Cola<T>::mostrar_tamanio() const{
    return cantidad;
}

template <typename T>
bool Cola<T>::mayorPrioridad(const int prioA, const int prioB){
    return prioA < prioB;
}


template <typename T>
void Cola<T>::encolar(const T& valor, int prioridad){
    Nodo* nuevo = new Nodo(valor,prioridad);

    if(verificar_vacio()){                                                      //Caso 1: Cola vacia
        frente = nuevo;
        fin = nuevo;
    }
    else if (mayorPrioridad(prioridad, frente->prioridad)){                     //Caso 2: Nuevo debe ir al frente
        nuevo->sig = frente;
        frente = nuevo;
    }
    else{                                                                      //Caso 3: Nuevo va en medio o final
        Nodo* actual = frente;                                      
        while (actual->sig != nullptr && prioridad >= actual->sig->prioridad){
            actual = actual->sig;
        }
        nuevo->sig = actual->sig;
        actual->sig = nuevo;

        if(nuevo->sig == nullptr){
            fin = nuevo;
        }
    }

    ++cantidad;       
}

template <typename T>
bool Cola<T>::desencolar(T& atendido){
    if(verificar_vacio()){
       return false;
    }
    
    Nodo* eliminado = frente;
    atendido = eliminado->dato;
    frente = frente->sig;

    //Eliminar el ultimo nodo
    if(frente == nullptr){   
        fin = nullptr;
    }

    delete eliminado;
    --cantidad;   

    return true;
}

template <typename T>
bool Cola<T>::consultarFrente(T& dato) const{          
    if(verificar_vacio()){
        return false;
    }

    dato = frente->dato;
    return true;
}

template <typename T>
void Cola<T>::mostrarEstado() const{
    Nodo* actual = frente;

    while(actual != nullptr){
        cout << actual->dato << endl;
        
        actual = actual->sig;
    }

    if(verificar_vacio()){
        cout << "\nFrente = nullptr\t \nfin = nullptr\n";
    }else{
        cout << "\nfrente -> dato= "<<frente->dato;
        cout << "\nfin -> dato= "<<fin->dato;
    }

    cout << "\nTama" << char(164) << "o= "<<cantidad;
}

template <typename T>
void Cola<T>::limpiar(){
    Nodo* actual = frente;

    while(actual != nullptr){
        Nodo* siguiente = actual->sig;  
        delete actual;                  
        actual = siguiente;             
    }

    frente = nullptr;
    fin = nullptr;
    cantidad = 0;
}

template<typename T>
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

template<typename T>
int Cola<T>::obtenerPosicionInsercion(int prioridad) const {

    Nodo* actual = frente;
    int posicion = 0;

    while (actual != nullptr &&
           actual->prioridad <= prioridad) {

        actual = actual->sig;
        posicion++;
    }

    return posicion;
}

#endif //OPERACIONES_COLA_H
