
#include <cmath>
#include <stdexcept>
#include <vector>

#include "cola_fibonacci.hpp"

namespace logs {

template <typename l, typename v>
ColaFibonacci<l, v>::ColaFibonacci() : _minimo(nullptr), _tamano(0) {}

template <typename l, typename v>
typename ColaFibonacci<l, v>::Nodo* ColaFibonacci<l, v>::agregar_par(l llave, v valor) {
    Nodo* nuevo_nodo = new Nodo(llave, valor);
    _tamano++;

    if (_minimo != nullptr) {
        Nodo* siguiente = _minimo->_hermano_d;

        nuevo_nodo->_hermano_i = _minimo;
        nuevo_nodo->_hermano_d = siguiente;

        _minimo->_hermano_d = nuevo_nodo;
        siguiente->_hermano_i = nuevo_nodo;

        if (llave < _minimo->_llave) {
            _minimo = nuevo_nodo;
        }

    } else {
        _minimo = nuevo_nodo;
    }

    return nuevo_nodo;
}

template <typename l, typename v>
void ColaFibonacci<l, v>::reducir_llave(Nodo* nodo, l nueva_llave) {
    if (nodo->_llave < nueva_llave) {
        throw std::logic_error("La nueva llave es mayor que la actual");
    }

    nodo->_llave = nueva_llave;

    if (nodo->_padre != nullptr && nueva_llave < nodo->_padre->_llave) {
        mover_a_raiz(nodo);
    }

    if (nueva_llave < _minimo->_llave) {
        _minimo = nodo;
    }
}

template <typename l, typename v>
void ColaFibonacci<l,v>::mover_a_raiz(Nodo* nodo) {
    Nodo* antiguo_padre = nodo->_padre;
    Nodo* antiguo_hermano_d = nodo->_hermano_d;
    Nodo* antiguo_hermano_i = nodo->_hermano_i;

    // se quita al nodo de la lista a la que pertenece
    if (antiguo_hermano_d != nodo) {
        antiguo_hermano_d->_hermano_i = antiguo_hermano_i;
        antiguo_hermano_i->_hermano_d = antiguo_hermano_d;

        antiguo_padre->_hijo = antiguo_hermano_d;

    } else {
        antiguo_padre->_hijo = nullptr;
    }

    Nodo* nuevo_hermano_d = _minimo->_hermano_d;
    
    // se añaden los nuevos hermanos al nodo
    nodo->_hermano_d = nuevo_hermano_d;
    nodo->_hermano_i = _minimo;

    // se añade el nodo a la lista de raices
    nuevo_hermano_d->_hermano_i = nodo;
    _minimo->_hermano_d = nodo;

    nodo->_marcado = false;
    nodo->_padre = nullptr;

    // se ejecuta recursivamente si el padre del nodo estaba marcado.
    antiguo_padre->_orden--;
    if (antiguo_padre->_padre != nullptr) {
        if (antiguo_padre->_marcado) {
            mover_a_raiz(antiguo_padre);
        } else {
            antiguo_padre->_marcado = true;
        }
    }
}

template <typename l, typename v>
v ColaFibonacci<l, v>::extraer_minimo() {
    if (_minimo == nullptr) {
        throw std::logic_error("No se puede extraer minimo de cola vacía");
    }

    Nodo* nodo_minimo = _minimo;
    Nodo* m_hijo = _minimo->_hijo;
    Nodo* m_hermano_d = _minimo->_hermano_d;
    Nodo* m_hermano_i = _minimo->_hermano_i;

    v ret = nodo_minimo->_valor;
    
    // esta parte solo se encarga de eliminar el nodo y añadir a sus hijos al heap
    if (m_hijo == nullptr) {
        // si el minimo no tiene hijos simplemente se quita de la lista
        if (_minimo == m_hermano_d) {
            _minimo = nullptr;

        } else {
            m_hermano_d->_hermano_i = m_hermano_i;
            m_hermano_i->_hermano_d = m_hermano_d;
            _minimo = m_hermano_d;
            consolidar();
        }

    } else {
        // si tiene hijos se tienen que elimiar todas las referencias que estos tienen a su padre
        m_hijo->_padre = nullptr;
        m_hijo->_marcado = false;

        for (Nodo* iter = m_hijo->_hermano_d; iter != m_hijo; iter = iter->_hermano_d) {
            iter->_padre = nullptr;
            iter->_marcado = false;
        }

        // si ademas tiene hermanos se concatena la lista de hijos con la de hermanos
        if (_minimo != m_hermano_d) {
            Nodo* m_ultimo_hijo = m_hijo->_hermano_i;

            m_hermano_d->_hermano_i = m_ultimo_hijo;
            m_hermano_i->_hermano_d = m_hijo;

            m_ultimo_hijo->_hermano_d = m_hermano_d;
            m_hijo->_hermano_i = m_hermano_i;
        }

        _minimo = m_hijo;
        consolidar();
    }

    nodo_minimo->_hermano_d = nullptr;
    nodo_minimo->_hermano_i = nullptr;
    nodo_minimo->_hijo = nullptr;
    delete nodo_minimo;

    _tamano--;
    return ret;
}

unsigned int log2_ceil(unsigned int n) {
    return static_cast<unsigned int>(std::ceil(std::log2(n))) + 1 ;
}

template <typename l, typename v>
void ColaFibonacci<l, v>::consolidar() {
    std::vector<Nodo*> lista_auxiliar(log2_ceil(_tamano), nullptr);
    lista_auxiliar[_minimo->_orden] = _minimo;

    Nodo* init = _minimo;
    Nodo* iter = _minimo->_hermano_d;
    Nodo* next = iter->_hermano_d;
    while (iter != init) {
        while (lista_auxiliar[iter->_orden] != nullptr) {   
            Nodo* otro = lista_auxiliar[iter->_orden];
            lista_auxiliar[iter->_orden] = nullptr;

            if (otro->_llave < iter->_llave) {
                otro->anadir_hijo(iter);
                iter = otro;
            } else {
                iter->anadir_hijo(otro);
            }
        }

        if (iter->_llave < _minimo ->_llave) {
            _minimo = iter;
        }

        lista_auxiliar[iter->_orden] = iter;

        iter = next;
        next = iter->_hermano_d;
    }

    _minimo->_hermano_i = _minimo;
    _minimo->_hermano_d = _minimo;
    lista_auxiliar[_minimo->_orden] = nullptr;

    for (unsigned int i = 0; i < lista_auxiliar.size(); i++) {
        Nodo* otro = lista_auxiliar[i];

        if (otro != nullptr) {
            Nodo* m_hermano_i = _minimo->_hermano_i;

            otro->_hermano_d = _minimo;
            otro->_hermano_i = m_hermano_i;
            _minimo->_hermano_i = otro;
            m_hermano_i->_hermano_d = otro;
        }
    }
}

template <typename l, typename v>
l ColaFibonacci<l, v>::llave_menor() const {
    return _minimo->_llave;
}

template <typename l, typename v>
v ColaFibonacci<l, v>::valor_menor() const {
    return _minimo->_valor;
}

template <typename l, typename v>
ColaFibonacci<l, v>::Nodo::Nodo(l llave, v valor)
    : _llave(llave), _valor(valor), _marcado(false), _orden(0),
    _padre(nullptr), _hijo(nullptr), _hermano_i(this), _hermano_d(this) {}


template <typename l, typename v>
l ColaFibonacci<l, v>::Nodo::llave() {
    return _llave;
}

template <typename l, typename v>
v ColaFibonacci<l, v>::Nodo::valor() {
    return _valor;
}

template <typename l, typename v>
void ColaFibonacci<l, v>::Nodo::anadir_hijo(Nodo* nuevo_hijo) {
    if (_hijo == nullptr) {
        nuevo_hijo->_hermano_d = nuevo_hijo;
        nuevo_hijo->_hermano_i = nuevo_hijo;
        _hijo = nuevo_hijo;

    } else {
        Nodo* h_hermano_i = _hijo->_hermano_i;

        nuevo_hijo->_hermano_d = _hijo;
        nuevo_hijo->_hermano_i = h_hermano_i;
        h_hermano_i->_hermano_d = nuevo_hijo;
        _hijo->_hermano_i = nuevo_hijo;
    }

    nuevo_hijo->_padre = this;
    _orden++;
}

template class ColaFibonacci<double, int>;

} // namespace logs
