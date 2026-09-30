#include <iostream>
#include <vector>
using namespace std;



struct Nodo {
    Nodo* hijo[2];   // hijo[0] si el bit es 0, hijo[1] si es 1
    int cantidad;    // cuantos numeros de esta version pasan por aqui
};


struct PersistentTrie {
    static const int BITS = 15;   // los x_i y a son < 2^15, con 15 bits alcanza

    // En vez de usar nullptr para "no hay hijo", uso este nodo.
    // Tiene cantidad 0 y sus hijos son el mismo, asi que puedo bajar por el
    // todo lo que quiera sin que nada se rompa, y la resta de cantidades da 0.
    Nodo VACIO;

    vector<Nodo*> version;        // version[i] es la raiz del trie con x1..xi
    vector<Nodo*> nodosCreados;   // me los guardo para hacer delete al final

    PersistentTrie() {
        VACIO.hijo[0] = &VACIO;
        VACIO.hijo[1] = &VACIO;
        VACIO.cantidad = 0;
        version.push_back(&VACIO);   // version 0: todavia no hay ningun numero
    }

    ~PersistentTrie() {
        for (Nodo* nodo : nodosCreados) delete nodo;
    }

    // Si alguien copiara el trie, los dos intentarian borrar los mismos nodos.
    PersistentTrie(const PersistentTrie&) = delete;
    PersistentTrie& operator=(const PersistentTrie&) = delete;


    // Agrega un numero encima de la ultima version.
    // Llamarlo con x1, x2, ..., xn en orden deja armadas las versiones 1..n.
    int agregar(int numero) {
        Nodo* raizNueva = insertar(version.back(), numero, BITS - 1);
        version.push_back(raizNueva);
        return (int)version.size() - 1;
    }

    // Lo que pide el problema. Uso la version r y la version l-1:
    // restandolas me quedo exactamente con x_l..x_r.
    int maximoXor(int a, int l, int r) {
        return consultar(version[r], version[l - 1], a);
    }


    // Saca el bit 'pos' de un numero. Por ejemplo 6 = 110: bit 2 -> 1, bit 0 -> 0.
    int bitEn(int numero, int pos) {
        return (numero >> pos) & 1;
    }

    // Hace un nodo nuevo igual al original. Ojo: los hijos NO se copian,
    // solo se copian los punteros. Asi la copia comparte todo lo que cuelga
    // debajo con la version vieja, y la version vieja queda intacta.
    Nodo* copiar(Nodo* original) {
        Nodo* nuevo = new Nodo();
        nuevo->hijo[0]  = original->hijo[0];
        nuevo->hijo[1]  = original->hijo[1];
        nuevo->cantidad = original->cantidad;
        nodosCreados.push_back(nuevo);
        return nuevo;
    }

    // Devuelve el nodo equivalente a 'viejo' pero con 'numero' agregado.
    //
    // Nunca modifico 'viejo': hago una copia, le sumo 1 (porque el numero
    // nuevo pasa por aqui) y bajo solo por el lado del bit que toca.
    // El otro lado se queda apuntando a lo mismo que antes.
    // Resultado: por cada numero se crean 16 nodos (la raiz + 15 bits)
    // y todo lo demas se reutiliza de la version anterior.
    Nodo* insertar(Nodo* viejo, int numero, int pos) {
        Nodo* nuevo = copiar(viejo);
        nuevo->cantidad++;

        if (pos < 0) return nuevo;   // ya pasamos por los 15 bits, llegamos a la "hoja"

        int b = bitEn(numero, pos);
        nuevo->hijo[b] = insertar(viejo->hijo[b], numero, pos - 1);
        return nuevo;
    }

    // Arma el XOR maximo de a contra los numeros de [l, r].
    //
    // Voy bajando por las dos versiones al mismo tiempo, siempre por el mismo
    // camino. En cada nodo, nodoR->cantidad - nodoL->cantidad me dice cuantos
    // numeros del rango siguen "vivos" por ese camino.
    //
    // Empiezo por el bit mas alto porque es el que mas pesa: un 1 en el bit 14
    // vale mas que todos los bits de abajo juntos. Asi que en cada bit intento
    // ir por el contrario al de a (bits distintos -> el XOR da 1). Si por ese
    // lado no queda ningun numero del rango, no hay opcion y voy por el otro.
    int consultar(Nodo* raizR, Nodo* raizL, int a) {
        Nodo* nodoR = raizR;
        Nodo* nodoL = raizL;
        int respuesta = 0;

        for (int pos = BITS - 1; pos >= 0; pos--) {
            int bitDeA   = bitEn(a, pos);
            int bitIdeal = 1 - bitDeA;

            // Ojo que no basta con que el hijo exista en la version r:
            // puede que ese numero este antes de l. Por eso resto.
            int cuantosIdeal = nodoR->hijo[bitIdeal]->cantidad
                             - nodoL->hijo[bitIdeal]->cantidad;

            if (cuantosIdeal > 0) {
                respuesta += (1 << pos);   // este bit del XOR queda en 1
                nodoR = nodoR->hijo[bitIdeal];
                nodoL = nodoL->hijo[bitIdeal];
            } else {
                // Todos los del rango tienen el mismo bit que a aqui,
                // asi que este bit del XOR queda en 0.
                nodoR = nodoR->hijo[bitDeA];
                nodoL = nodoL->hijo[bitDeA];
            }
        }
        return respuesta;
    }
};


int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;

    while (t--) {
        int n, q;
        cin >> n >> q;

        // Lo declaro dentro del while para que cada caso empiece con un trie
        // limpio; al terminar la vuelta el destructor libera la memoria.
        PersistentTrie trie;

        for (int i = 1; i <= n; i++) {
            int x;
            cin >> x;
            trie.agregar(x);   // queda armada la version i
        }

        for (int k = 0; k < q; k++) {
            int a, l, r;
            cin >> a >> l >> r;
            cout << trie.maximoXor(a, l, r) << '\n';
        }
    }
    return 0;
}