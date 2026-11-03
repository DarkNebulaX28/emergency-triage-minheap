#ifndef COLA_PRIORIDAD_H
#define COLA_PRIORIDAD_H

template <typename T>

class Cola{

private:
    struct Nodo{
        T dato;
        Nodo* sig;
        int prioridad;

        Nodo(const T& valor, int prio): dato(valor), sig(nullptr), prioridad(prio){}
    };

    Nodo* frente;       
    Nodo* fin;          
    int cantidad;

public:
    Cola();
    ~Cola();
    Cola(const Cola&) = delete;   //Evitar crear copias superficiales
    Cola&operator = (const Cola&) = delete;

    bool verificar_vacio() const;
    int mostrar_tamanio() const;
    void encolar(const T& valor, int prioridad);
    bool desencolar(T& atendido);
    bool consultarFrente(T& dato) const;
    void mostrarEstado() const;
    void limpiar();

    bool mayorPrioridad(const int prioA, const int prioB);
};



#endif //COLA_PRIORIDAD_H