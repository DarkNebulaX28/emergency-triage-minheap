#ifndef COLA_PRIORIDAD_H
#define COLA_PRIORIDAD_H

template <typename T>
class Cola {
private:
    struct Nodo {
        T dato;
        Nodo* sig;
        int prioridad;

        Nodo(const T& valor, int prio)
            : dato(valor), sig(nullptr), prioridad(prio) {}
    };

    Nodo* frente;
    Nodo* fin;
    int cantidad;
    int totalOperaciones;

public:
    Cola();
    ~Cola();
    Cola(const Cola&) = delete;            // Evitar copias superficiales
    Cola& operator=(const Cola&) = delete; // Evitar asignacion superficial

    bool verificar_vacio() const;
    int mostrar_tamanio() const;
    void encolar(const T& valor, int prioridad);
    bool desencolar(T& atendido);
    bool desencolarConPrioridad(T& atendido, int& prioridadAtendida);
    bool consultarFrente(T& dato) const;
    bool consultarFrenteConPrioridad(T& dato, int& prioridadFrente) const;
    void mostrarEstado() const;
    void limpiar();

    bool mayorPrioridad(const int prioA, const int prioB) const;

    bool obtenerElemento(int posicion, T& dato, int& prioridad) const;
    int obtenerPosicionInsercion(int prioridad) const;

    bool eliminarEnPosicion(int posicion, T& eliminado);
    bool actualizarPrioridad(int posicion, int nuevaPrioridad);
    int contarPorPrioridad(int prioridadBuscada) const;

    const void* obtenerDireccionNodo(int posicion) const;
    const void* obtenerDireccionSiguiente(int posicion) const;
    int obtenerTotalOperaciones() const;
};

#endif // COLA_PRIORIDAD_H
