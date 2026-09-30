#include <iostream>
#include <vector>
using namespace std;


struct PersistentTrie {

public:
    // n = tamano del arreglo. Cada numero copia 31 nodos (raiz + 30 bits).
    PersistentTrie(int n) {
        nodos.reserve((size_t)(n + 1) * (BITS + 1) + 5);
        nodos.push_back(Nodo());   // nodo 0 = vacio
        version.push_back(0);      // version 0 = ningun numero
    }

    // Agrega el siguiente numero del arreglo: crea la version i.
    void agregar(int numero) {
        version.push_back(insertar(version.back(), numero));
    }

    // k-esimo menor de a_l XOR x, ..., a_r XOR x.
    int kesimoXor(int l, int r, int x, int k) {
        int nodoR = version[r];
        int nodoL = version[l - 1];
        int respuesta = 0;

        for (int pos = BITS - 1; pos >= 0; pos--) {
            int b = (x >> pos) & 1;   // el hijo[b] es el lado de los resultados chicos

            // Cuantos numeros del rango caen del lado chico en este bit.
            int enChicos = nodos[nodos[nodoR].hijo[b]].cantidad
                         - nodos[nodos[nodoL].hijo[b]].cantidad;

            if (k <= enChicos) {
                // El k-esimo esta entre los chicos: este bit del resultado es 0.
                nodoR = nodos[nodoR].hijo[b];
                nodoL = nodos[nodoL].hijo[b];
            } else {
                // Me salto todos los chicos y sigo en el otro lado,
                // donde este bit del resultado es 1.
                k -= enChicos;
                respuesta |= (1 << pos);
                nodoR = nodos[nodoR].hijo[1 - b];
                nodoL = nodos[nodoL].hijo[1 - b];
            }
        }
        return respuesta;
    }

private:
    static const int BITS = 30;   // a_i, x <= 10^9 < 2^30

    // Hijos como posiciones dentro de 'nodos'. El nodo 0 es vacio: cantidad 0
    // y sus dos hijos son el mismo, asi nunca hay que preguntar si existe.
    struct Nodo {
        int hijo[2] = {0, 0};
        int cantidad = 0;
    };

    vector<Nodo> nodos;
    vector<int> version;   // version[i] = raiz del trie con a_1..a_i

    int copiar(int x) {
        Nodo copia = nodos[x];
        nodos.push_back(copia);
        return (int)nodos.size() - 1;
    }

    // Devuelve la raiz de una version nueva: la de 'raizVieja' + el numero.
    // Dos dedos: 'viejo' recorre la version anterior y 'actual' la copia.
    // Solo copio el hijo del bit que toca; el otro queda compartido.
    int insertar(int raizVieja, int numero) {
        int raizNueva = copiar(raizVieja);
        nodos[raizNueva].cantidad++;

        int viejo = raizVieja;
        int actual = raizNueva;
        for (int pos = BITS - 1; pos >= 0; pos--) {
            int b = (numero >> pos) & 1;
            int hijoViejo = nodos[viejo].hijo[b];
            int hijoNuevo = copiar(hijoViejo);
            nodos[hijoNuevo].cantidad++;
            nodos[actual].hijo[b] = hijoNuevo;

            viejo = hijoViejo;
            actual = hijoNuevo;
        }
        return raizNueva;
    }
};


int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;
    PersistentTrie trie(n);

    for (int i = 1; i <= n; i++) {
        int a;
        cin >> a;
        trie.agregar(a);   // queda armada la version i
    }

    int q;
    cin >> q;
    while (q--) {
        int l, r, x, k;
        cin >> l >> r >> x >> k;
        cout << trie.kesimoXor(l, r, x, k) << '\n';
    }
    return 0;
}