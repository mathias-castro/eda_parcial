#include <iostream>
#include <vector>
#include <string>
using namespace std;


struct PersistentTrie {

public:
    // n = cantidad de palabras. Cada palabra crea |s| + 1 nodos y las letras
    // suman a lo mas 5*10^5, asi que reservo eso de una vez para que el
    // vector no se mude a mitad de camino.
    PersistentTrie(int n) {
        nodos.reserve(MAX_LETRAS + n + 5);
        nodos.push_back(Nodo());   // nodo 0 = vacio (ver comentario de Nodo)
        version.push_back(0);      // version 0 = ninguna palabra todavia
    }

    // Agrega la siguiente palabra: si ya hay versiones 0..i-1, crea la version i.
    void agregarPalabra(const string& s) {
        version.push_back(insertar(version.back(), s));
    }

    // Cuantas de s_l, ..., s_r empiezan con p.
    int contarEnRango(int l, int r, const string& p) {
        return contarPrefijo(version[r], p) - contarPrefijo(version[l - 1], p);
    }

private:
    static const int MAX_LETRAS = 500000;

    // Los hijos son posiciones dentro del vector 'nodos' (no punteros):
    // ocupan menos memoria y no se rompen si el vector cambia de lugar.
    //
    // El nodo 0 es especial: cantidad 0 y todos sus hijos son 0 (el mismo).
    // Si deletreo una letra que no existe caigo ahi, y el contador me da 0
    // sin tener que preguntar nada.
    struct Nodo {
        int hijo[26] = {};   // hijo[c] = posicion del hijo por la letra 'a'+c (0 = no hay)
        int cantidad = 0;    // cuantas palabras (con repeticion) pasan por aqui
    };

    vector<Nodo> nodos;   // todos los nodos de todas las versiones
    vector<int> version;  // version[i] = raiz del trie con s1..si

    // Crea un nodo nuevo igual al nodo x. Solo copia los numeros de los hijos,
    // asi que comparte con x todo lo que cuelga debajo. El nodo x no se toca.
    int copiar(int x) {
        Nodo copia = nodos[x];
        nodos.push_back(copia);
        return (int)nodos.size() - 1;
    }

    // Devuelve la raiz de una version nueva: la de 'raizVieja' + la palabra s.
    //
    // Dos "dedos": 'viejo' recorre la version anterior (solo lo leo) y 'actual'
    // recorre la copia que voy armando. En cada letra copio solo el hijo de esa
    // letra, le sumo 1 y lo cuelgo en la copia; las otras 25 letras siguen
    // apuntando a lo mismo de antes.
    //
    // Con for y no con recursion: una palabra puede tener 5*10^5 letras.
    int insertar(int raizVieja, const string& s) {
        int raizNueva = copiar(raizVieja);
        nodos[raizNueva].cantidad++;

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

    // Cuantas palabras de la version con raiz 'raiz' empiezan con p.
    // Si alguna letra de p no esta, caigo en el nodo 0 y el resultado es 0.
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

    int n, q;
    cin >> n >> q;

    PersistentTrie trie(n);

    for (int i = 1; i <= n; i++) {
        string s;
        cin >> s;
        trie.agregarPalabra(s);   // queda armada la version i
    }

    while (q--) {
        int l, r;
        string p;
        cin >> l >> r >> p;
        cout << trie.contarEnRango(l, r, p) << '\n';
    }
    return 0;
}