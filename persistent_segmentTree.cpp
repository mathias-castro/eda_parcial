#include <iostream>
#include <vector>
#include <stdexcept>
using namespace std;


struct Node {
    long long val;      // suma del rango [l, r]
    int l, r;           // rango que cubre este nodo
    Node* left;
    Node* right;
    Node(int l, int r, long long val, Node* left = nullptr, Node* right = nullptr)
        : val(val), l(l), r(r), left(left), right(right) {}
};

class PersistentSegmentTree {
private:
    int n;
    vector<Node*> roots;    // roots[v] = raiz de la version v

    // Construye la version 0. O(n)
    Node* build(const vector<long long>& a, int l, int r) {
        if (l == r) return new Node(l, r, a[l]);
        int mid = (l + r) / 2;
        Node* L = build(a, l, mid);
        Node* R = build(a, mid + 1, r);
        return new Node(l, r, L->val + R->val, L, R);
    }

    // Path copying (Algoritmo 3 de la diapositiva):
    // devuelve un nodo NUEVO; nunca modifica 'prev'. O(lg n)
    Node* update(Node* prev, int pos, long long newVal) {
        if (prev->l == prev->r) return new Node(prev->l, prev->r, newVal);

        int mid = (prev->l + prev->r) / 2;
        Node* L = prev->left;     // por defecto se COMPARTEN los hijos
        Node* R = prev->right;
        if (pos <= mid) L = update(prev->left, pos, newVal);    // solo se copia el lado que cambia
        else            R = update(prev->right, pos, newVal);
        return new Node(prev->l, prev->r, L->val + R->val, L, R);
    }

    // Suma en [ql, qr] dentro del subarbol 'node'. O(lg n)
    long long query(Node* node, int ql, int qr) const {
        if (qr < node->l || ql > node->r) return 0;          // sin interseccion
        if (ql <= node->l && node->r <= qr) return node->val; // contenido completo
        return query(node->left, ql, qr) + query(node->right, ql, qr);
    }

    void checkVersion(int v) const {
        if (v < 0 || v >= (int)roots.size()) throw out_of_range("version inexistente");
    }

public:
    explicit PersistentSegmentTree(const vector<long long>& a) : n((int)a.size()) {
        roots.push_back(n > 0 ? build(a, 0, n - 1) : nullptr);   // version 0
    }

    // Crea una version nueva = version v con a[pos] = newVal. Devuelve su numero.
    int update(int v, int pos, long long newVal) {
        checkVersion(v);
        if (pos < 0 || pos >= n) throw out_of_range("posicion fuera de rango");
        roots.push_back(update(roots[v], pos, newVal));
        return (int)roots.size() - 1;
    }

    // Suma de a[l..r] en la version v.
    long long query(int v, int l, int r) const {
        checkVersion(v);
        if (n == 0) return 0;
        if (l < 0) l = 0;
        if (r > n - 1) r = n - 1;
        if (l > r) return 0;
        return query(roots[v], l, r);
    }

    int versions() const { return (int)roots.size(); }
};

int main() {
    vector<long long> a = {1, 2, 3, 4, 5};
    PersistentSegmentTree st(a);

    cout << st.query(0, 0, 4) << '\n';     // 15

    int v1 = st.update(0, 2, 10);          // v1: a[2] = 10
    cout << st.query(0, 0, 4) << '\n';     // 15  (version vieja intacta)
    cout << st.query(v1, 0, 4) << '\n';    // 22

    int v2 = st.update(0, 0, 100);         // rama desde v0
    cout << st.query(v2, 0, 4) << '\n';    // 114

    try {
        st.update(0, 7, 50);               // posicion invalida
    } catch (const out_of_range& e) {
        cout << "Error: " << e.what() << '\n';
    }
    return 0;
}