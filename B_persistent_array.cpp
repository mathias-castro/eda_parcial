#include <iostream>
#include <vector>
#include <deque>
#include <string>
#include <stdexcept>
using namespace std;

// B. Persistent Array
//
// Es el uso mas directo del segment tree persistente: cada revision es una
// version del arbol.
//   create i j x -> version nueva = version i con a[j] = x   (path copying)
//   get i j      -> leo la hoja j de la version i
//
// Ojo con la numeracion: el problema llama "revision 1" al arreglo inicial,
// y en mi clase ese es roots[0]. Por eso resto 1 al numero de revision.


struct Node {
    long long val;      // suma del rango [l, r] (en una hoja: el valor a[l])
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

    // En vez de 'new' saco los nodos de este deque. Es lo mismo, pero sin los
    // ~16 bytes extra que gasta 'new' por cada nodo. El deque nunca mueve lo
    // que ya guardo, asi que los punteros siguen siendo validos.
    deque<Node> pool;
    Node* newNode(int l, int r, long long val, Node* left = nullptr, Node* right = nullptr) {
        pool.emplace_back(l, r, val, left, right);
        return &pool.back();
    }

    Node* build(const vector<long long>& a, int l, int r) {
        if (l == r) return newNode(l, r, a[l]);
        int mid = (l + r) / 2;
        Node* L = build(a, l, mid);
        Node* R = build(a, mid + 1, r);
        return newNode(l, r, L->val + R->val, L, R);
    }

    // Path copying: devuelve un nodo NUEVO; nunca modifica 'prev'.
    Node* update(Node* prev, int pos, long long newVal) {
        if (prev->l == prev->r) return newNode(prev->l, prev->r, newVal);

        int mid = (prev->l + prev->r) / 2;
        Node* L = prev->left;     // por defecto se comparten los hijos
        Node* R = prev->right;
        if (pos <= mid) L = update(prev->left, pos, newVal);
        else            R = update(prev->right, pos, newVal);
        return newNode(prev->l, prev->r, L->val + R->val, L, R);
    }

    long long query(Node* node, int ql, int qr) const {
        if (qr < node->l || ql > node->r) return 0;
        if (ql <= node->l && node->r <= qr) return node->val;
        return query(node->left, ql, qr) + query(node->right, ql, qr);
    }

    void checkVersion(int v) const {
        if (v < 0 || v >= (int)roots.size()) throw out_of_range("version inexistente");
    }

public:
    explicit PersistentSegmentTree(const vector<long long>& a) : n((int)a.size()) {
        roots.push_back(build(a, 0, n - 1));   // version 0
    }

    // Version nueva = version v con a[pos] = newVal. Devuelve su numero.
    int update(int v, int pos, long long newVal) {
        checkVersion(v);
        if (pos < 0 || pos >= n) throw out_of_range("posicion fuera de rango");
        roots.push_back(update(roots[v], pos, newVal));
        return (int)roots.size() - 1;
    }

    // Suma de a[l..r] en la version v. Con l == r es simplemente a[l].
    long long query(int v, int l, int r) const {
        checkVersion(v);
        return query(roots[v], l, r);
    }
};


int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;
    vector<long long> a(n);
    for (auto& x : a) cin >> x;

    PersistentSegmentTree st(a);   // revision 1 = version 0

    int m;
    cin >> m;
    while (m--) {
        string op;
        int i, j;
        cin >> op >> i >> j;
        if (op == "create") {
            long long x;
            cin >> x;
            st.update(i - 1, j - 1, x);          // crea la revision siguiente
        } else {
            cout << st.query(i - 1, j - 1, j - 1) << '\n';
        }
    }
    return 0;
}
