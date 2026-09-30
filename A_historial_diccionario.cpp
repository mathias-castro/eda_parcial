#include <iostream>
#include <vector>
#include <string>
using namespace std;


struct PersistentTrie {

public:
    // Q = cantidad de operaciones. Con eso calculo cuanta memoria reservar.
    PersistentTrie(int Q) {
        // Cada "1 s" crea |s| + 1 nodos. Las letras suman a lo mas 5*10^5
        // y hay a lo mas Q palabras, asi que nunca paso de esta cantidad.
        // Reservo todo de una para que el vector no se mude a mitad de camino.
        nodos.reserve(MAX_LETRAS + Q + 5);

        nodos.push_back(Nodo());   // nodo 0 = vacio (ver comentario de Nodo)
        version.push_back(0);      // estado 0 = diccionario vacio
    }

    // "1 s": estado nuevo = estado actual + s.
    void agregarPalabra(const string& s) {
        version.push_back(insertar(version.back(), s));
    }

    // "2 t": estado nuevo = estado t.
    // Solo reutilizo su raiz. Si despues agrego palabras, se copian caminos
    // nuevos, asi que el estado t (y todos los demas) sigue intacto.
    void volverAEstado(int t) {
        version.push_back(version[t]);
    }

    // "3 p": respondo sobre el estado actual. El diccionario no cambia,
    // pero igual registro el estado nuevo (con la misma raiz) para que
    // el numero de estado siga coincidiendo con el numero de operacion.
    int consultarPrefijo(const string& p) {
        int respuesta = contarPrefijo(version.back(), p);
        version.push_back(version.back());
        return respuesta;
    }

private:
    static const int MAX_LETRAS = 500000;

    // Un nodo del trie. Los hijos se guardan como posiciones dentro del
    // vector 'nodos' en vez de punteros: ocupan la mitad de memoria y no se
    // rompen si el vector cambia de lugar.
    //
    // El nodo 0 es especial: cantidad 0 y todos sus hijos son 0 (el mismo).
    // Si deletreo una letra que no existe caigo ahi, y el contador me da 0
    // sin tener que preguntar nada.
    struct Nodo {
        int hijo[26] = {};   // hijo[c] = posicion del hijo por la letra 'a'+c (0 = no hay)
        int cantidad = 0;    // cuantas palabras (con repeticion) pasan por aqui
    };

    vector<Nodo> nodos;   // todos los nodos de todas las versiones
    vector<int> version;  // version[i] = raiz del estado i

    // Crea un nodo nuevo igual al nodo x: mismos hijos, mismo contador.
    // Como solo copia los numeros de los hijos, la copia comparte con x
    // todo lo que cuelga debajo. El nodo x no se toca.
    int copiar(int x) {
        Nodo copia = nodos[x];
        nodos.push_back(copia);
        return (int)nodos.size() - 1;
    }

    // Devuelve la raiz de una version nueva: la de 'raizVieja' + la palabra s.
    //
    // Avanzo con dos "dedos": 'viejo' recorre la version anterior (solo lo leo)
    // y 'actual' recorre la copia que voy armando. En cada letra copio solo el
    // hijo de esa letra, le sumo 1 y lo cuelgo en la copia. Las otras 25 letras
    // siguen apuntando a lo mismo de antes.
    //
    // Lo hago con un for y no con recursion porque una palabra puede tener
    // hasta 5*10^5 letras, y tanta recursion revienta la pila.
    int insertar(int raizVieja, const string& s) {
        int raizNueva = copiar(raizVieja);
        nodos[raizNueva].cantidad++;          // la raiz cuenta todas las palabras

        int viejo = raizVieja;
        int actual = raizNueva;
        for (char ch : s) {
            int c = ch - 'a';
            int hijoViejo = nodos[viejo].hijo[c];   // puede ser 0 si la letra no estaba
            int hijoNuevo = copiar(hijoViejo);
            nodos[hijoNuevo].cantidad++;             // s pasa por aqui
            nodos[actual].hijo[c] = hijoNuevo;

            viejo = hijoViejo;
            actual = hijoNuevo;
        }
        return raizNueva;
    }

    // Cuantas palabras de la version que empieza en 'raiz' tienen prefijo p.
    // Bajo deletreando p; si alguna letra no esta, caigo en el nodo 0 y ahi
    // me quedo (sus hijos son el mismo), asi que al final devuelvo 0 solo.
    int contarPrefijo(int raiz, const string& p) {
        int nodo = raiz;
        for (char ch : p) {
            nodo = nodos[nodo].hijo[ch - 'a'];
        }
        return nodos[nodo].cantidad;
    }
};


int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int Q;
    cin >> Q;
    PersistentTrie trie(Q);

    while (Q--) {
        int tipo;
        cin >> tipo;

        if (tipo == 1) {
            string s;
            cin >> s;
            trie.agregarPalabra(s);
        } else if (tipo == 2) {
            int t;
            cin >> t;
            trie.volverAEstado(t);
        } else {
            string p;
            cin >> p;
            cout << trie.consultarPrefijo(p) << '\n';
        }
    }
    return 0;
}