#include <iostream>
#include <vector>
using namespace std;


struct PersistentTrie {

public:
    // n = cantidad de commits. Cada commit copia 31 nodos (raiz + 30 bits).
    PersistentTrie(int n) {
        nodos.reserve((size_t)(n + 1) * (BITS + 1) + 5);
        nodos.push_back(Nodo());      // nodo 0 = vacio
        version.assign(n + 1, 0);     // version[0] = vacia (el "padre" de la raiz)
        padre.assign(n + 1, 0);
    }

    // Registra el commit 'u' con su padre y su firma.
    // El padre ya tiene que estar registrado (se cumple porque p_i < i).
    void agregarCommit(int u, int p, int firma) {
        padre[u] = p;
        version[u] = insertar(version[p], firma);
    }

    // max(a_c XOR x) sobre el historial de u hasta w.
    int maximoXor(int u, int w, int x) {
        return consultar(version[u], version[padre[w]], x);
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
    vector<int> version;   // version[u] = raiz del trie con el camino raiz -> u
    vector<int> padre;     // padre[u] (0 para la raiz)

    int copiar(int x) {
        Nodo copia = nodos[x];
        nodos.push_back(copia);
        return (int)nodos.size() - 1;
    }

    // Devuelve la raiz de una version nueva: la de 'raizVieja' + el numero.
    // Dos dedos: 'viejo' recorre la version del padre y 'actual' la copia.
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

    // Bajo por las dos versiones al mismo tiempo. La resta de cantidades me
    // dice cuantos commits del historial siguen "vivos" por cada camino.
    int consultar(int raizU, int raizPadreW, int x) {
        int nodoU = raizU;
        int nodoP = raizPadreW;
        int respuesta = 0;

        for (int pos = BITS - 1; pos >= 0; pos--) {
            int bitDeX   = (x >> pos) & 1;
            int bitIdeal = 1 - bitDeX;     // bits distintos -> este bit del XOR es 1

            // Ojo: no basta con que el hijo exista en la version u; podria ser
            // de un commit que esta por encima de w. Por eso resto.
            int cuantosIdeal = nodos[nodos[nodoU].hijo[bitIdeal]].cantidad
                             - nodos[nodos[nodoP].hijo[bitIdeal]].cantidad;

            if (cuantosIdeal > 0) {
                respuesta |= (1 << pos);
                nodoU = nodos[nodoU].hijo[bitIdeal];
                nodoP = nodos[nodoP].hijo[bitIdeal];
            } else {
                // Todos los del historial tienen el mismo bit que x aqui.
                nodoU = nodos[nodoU].hijo[bitDeX];
                nodoP = nodos[nodoP].hijo[bitDeX];
            }
        }
        return respuesta;
    }
};


int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;
    PersistentTrie trie(n);

    // Como p_i < i, cuando llego al commit i su padre ya esta registrado.
    for (int i = 1; i <= n; i++) {
        int p, a;
        cin >> p >> a;
        trie.agregarCommit(i, p, a);
    }

    int q;
    cin >> q;
    while (q--) {
        int u, w, x;
        cin >> u >> w >> x;
        cout << trie.maximoXor(u, w, x) << '\n';
    }
    return 0;
}