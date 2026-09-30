#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;


struct PersistentSegmentTree {

public:
    // m = cantidad de valores distintos (hojas). Q = cantidad de operaciones.
    PersistentSegmentTree(int m, int Q) : m(m) {
        // build crea unos 2m nodos, y cada update copia un camino de
        // ~log2(m) + 1 nodos. Con 20 por update sobra (2^18 > 2*10^5).
        nodos.reserve(2 * (size_t)m + (size_t)Q * 20 + 5);
        version.push_back(construir(0, m - 1));   // version 0: todo en 0
    }

    // "1 v x" y "2 v x": x ya viene comprimido (es un indice 0..m-1).
    void agregar(int v, int indice) {
        version.push_back(cambiar(version[v], 0, m - 1, indice, +1));
    }

    void quitar(int v, int indice) {
        version.push_back(cambiar(version[v], 0, m - 1, indice, -1));
    }

    // "3 v k": version nueva = version v; devuelvo el INDICE del k-esimo menor.
    int kesimo(int v, int k) {
        version.push_back(version[v]);
        return buscarKesimo(version[v], 0, m - 1, k);
    }

private:
    // El rango [l, r] de cada nodo no se guarda: se va pasando al bajar.
    struct Nodo {
        int izq = -1, der = -1;   // -1 en las hojas
        int cantidad = 0;         // cuantos elementos tienen valor en este rango
    };

    int m;
    vector<Nodo> nodos;
    vector<int> version;   // version[i] = raiz de la version i

    int nuevoNodo(int izq, int der, int cantidad) {
        Nodo nodo;
        nodo.izq = izq;
        nodo.der = der;
        nodo.cantidad = cantidad;
        nodos.push_back(nodo);
        return (int)nodos.size() - 1;
    }

    // Arbol completo con todas las cantidades en 0 (multiconjunto vacio).
    int construir(int l, int r) {
        if (l == r) return nuevoNodo(-1, -1, 0);
        int mid = (l + r) / 2;
        int izq = construir(l, mid);
        int der = construir(mid + 1, r);
        return nuevoNodo(izq, der, 0);
    }

    // Devuelve un nodo NUEVO equivalente a 'viejo' pero con 'delta' sumado en
    // la hoja 'pos'. Solo se crea un nodo por nivel; del lado que no cambia
    // reutilizo el hijo del nodo viejo.
    int cambiar(int viejo, int l, int r, int pos, int delta) {
        if (l == r) {
            return nuevoNodo(-1, -1, nodos[viejo].cantidad + delta);
        }
        int mid = (l + r) / 2;
        int izq = nodos[viejo].izq;
        int der = nodos[viejo].der;
        if (pos <= mid) izq = cambiar(izq, l, mid, pos, delta);
        else            der = cambiar(der, mid + 1, r, pos, delta);
        return nuevoNodo(izq, der, nodos[izq].cantidad + nodos[der].cantidad);
    }

    // En cada nodo miro cuantos hay en la mitad izquierda (los valores mas
    // chicos). Si alcanzan para k, el k-esimo esta ahi. Si no, me los salto
    // (k -= esos) y busco en la mitad derecha.
    int buscarKesimo(int nodo, int l, int r, int k) {
        if (l == r) return l;
        int mid = (l + r) / 2;
        int enIzquierda = nodos[nodos[nodo].izq].cantidad;
        if (k <= enIzquierda) {
            return buscarKesimo(nodos[nodo].izq, l, mid, k);
        } else {
            return buscarKesimo(nodos[nodo].der, mid + 1, r, k - enIzquierda);
        }
    }
};


int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int Q;
    cin >> Q;

    // Leo todo primero: necesito conocer todos los x antes de armar el arbol.
    vector<int> tipo(Q), v(Q), y(Q);
    vector<int> valores;
    for (int i = 0; i < Q; i++) {
        cin >> tipo[i] >> v[i] >> y[i];
        if (tipo[i] != 3) valores.push_back(y[i]);
    }

    // Compresion: ordeno, quito repetidos, y el indice de x es su posicion
    // en esta lista. valores[indice] me devuelve el x original.
    sort(valores.begin(), valores.end());
    valores.erase(unique(valores.begin(), valores.end()), valores.end());
    if (valores.empty()) valores.push_back(0);   // por si no hubiera inserciones

    auto indiceDe = [&](int x) {
        return (int)(lower_bound(valores.begin(), valores.end(), x) - valores.begin());
    };

    PersistentSegmentTree st((int)valores.size(), Q);

    for (int i = 0; i < Q; i++) {
        if (tipo[i] == 1)      st.agregar(v[i], indiceDe(y[i]));
        else if (tipo[i] == 2) st.quitar(v[i], indiceDe(y[i]));
        else                   cout << valores[st.kesimo(v[i], y[i])] << '\n';
    }
    return 0;
}




// IMPLEMENTACION 2

#include <iostream>
#include <vector>
#include <algorithm>
#include <stdexcept>
using namespace std;




struct Node {
    int val;            // cuantos elementos tienen valor en el rango [l, r]
    int l, r;           // rango (de indices comprimidos) que cubre este nodo
    Node* left;
    Node* right;
    Node(int l, int r, int val, Node* left = nullptr, Node* right = nullptr)
        : val(val), l(l), r(r), left(left), right(right) {}
};

class PersistentSegmentTree {
private:
    int n;                  // cantidad de hojas (valores distintos)
    vector<Node*> roots;    // roots[v] = raiz de la version v

    // Construye la version 0: todas las cantidades en 0. O(n)
    Node* build(int l, int r) {
        if (l == r) return new Node(l, r, 0);
        int mid = (l + r) / 2;
        Node* L = build(l, mid);
        Node* R = build(mid + 1, r);
        return new Node(l, r, 0, L, R);
    }

    // Path copying: devuelve un nodo NUEVO con 'delta' sumado en la hoja pos.
    // Nunca modifica 'prev'. Del lado que no cambia, reutilizo su hijo. O(lg n)
    Node* update(Node* prev, int pos, int delta) {
        if (prev->l == prev->r) return new Node(prev->l, prev->r, prev->val + delta);

        int mid = (prev->l + prev->r) / 2;
        Node* L = prev->left;     // por defecto se COMPARTEN los hijos
        Node* R = prev->right;
        if (pos <= mid) L = update(prev->left, pos, delta);    // solo se copia el lado que cambia
        else            R = update(prev->right, pos, delta);
        return new Node(prev->l, prev->r, L->val + R->val, L, R);
    }

    // Indice del k-esimo menor dentro del subarbol 'node'. O(lg n)
    // Si en la mitad izquierda (valores mas chicos) hay al menos k, esta ahi.
    // Si no, me salto esos (k -= left->val) y busco en la mitad derecha.
    int kth(Node* node, int k) const {
        if (node->l == node->r) return node->l;
        if (k <= node->left->val) return kth(node->left, k);
        return kth(node->right, k - node->left->val);
    }

    void checkVersion(int v) const {
        if (v < 0 || v >= (int)roots.size()) throw out_of_range("version inexistente");
    }

public:
    explicit PersistentSegmentTree(int n) : n(n) {
        roots.push_back(build(0, n - 1));   // version 0: multiconjunto vacio
    }

    // Version nueva = version v con 'delta' (+1 o -1) en la hoja pos.
    int add(int v, int pos, int delta) {
        checkVersion(v);
        if (pos < 0 || pos >= n) throw out_of_range("posicion fuera de rango");
        roots.push_back(update(roots[v], pos, delta));
        return (int)roots.size() - 1;
    }

    // Version nueva = misma raiz que la version v (no se copia nada).
    int copyVersion(int v) {
        checkVersion(v);
        roots.push_back(roots[v]);
        return (int)roots.size() - 1;
    }

    // Indice (comprimido) del k-esimo menor de la version v.
    int kth(int v, int k) const {
        checkVersion(v);
        if (k < 1 || k > roots[v]->val) throw out_of_range("k fuera de rango");
        return kth(roots[v], k);
    }

    int versions() const { return (int)roots.size(); }
};


int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int Q;
    cin >> Q;

    // Leo todo primero: necesito conocer todos los x antes de armar el arbol.
    vector<int> type(Q), v(Q), y(Q);
    vector<int> values;
    for (int i = 0; i < Q; i++) {
        cin >> type[i] >> v[i] >> y[i];
        if (type[i] != 3) values.push_back(y[i]);
    }

    // Compresion: ordeno y quito repetidos. El indice de x es su posicion en
    // 'values', y values[indice] me devuelve el x original.
    sort(values.begin(), values.end());
    values.erase(unique(values.begin(), values.end()), values.end());
    if (values.empty()) values.push_back(0);   // por si no hubiera inserciones

    auto indexOf = [&](int x) {
        return (int)(lower_bound(values.begin(), values.end(), x) - values.begin());
    };

    PersistentSegmentTree st((int)values.size());

    // La operacion i (1-indexada) crea la version i, asi que "v" siempre
    // se refiere a una version que ya existe.
    for (int i = 0; i < Q; i++) {
        if (type[i] == 1) {
            st.add(v[i], indexOf(y[i]), +1);
        } else if (type[i] == 2) {
            st.add(v[i], indexOf(y[i]), -1);
        } else {
            st.copyVersion(v[i]);
            cout << values[st.kth(v[i], y[i])] << '\n';
        }
    }
    return 0;
}