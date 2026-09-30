#include <iostream>
#include <vector>
using namespace std;

// ============================================================
// Persistent Segment Tree (suma de rango, asignacion puntual)
// Version B: estilo competitivo. Nodos en arreglos (pool) e
// indices en vez de punteros. El rango [l, r] NO se guarda:
// se pasa como parametro en la recursion.
// Nodo 0 = nodo nulo (suma 0, hijos 0): sirve de hoja vacia.
// ============================================================

struct PersistentSegmentTree {
    int n;
    vector<long long> sum;   // sum[x]  = suma del nodo x
    vector<int> L, R;        // hijos del nodo x
    vector<int> roots;       // roots[v] = raiz de la version v

    // maxUpdates: cuantos updates esperas (solo para reservar memoria)
    PersistentSegmentTree(const vector<long long>& a, int maxUpdates = 0) : n((int)a.size()) {
        int lg = 1;
        while ((1 << lg) < max(n, 1)) lg++;
        size_t cap = 2 * (size_t)max(n, 1) + (size_t)maxUpdates * (lg + 2) + 1;
        sum.reserve(cap); L.reserve(cap); R.reserve(cap);

        newNode(0, 0, 0);                              // nodo 0: nulo
        roots.push_back(n > 0 ? build(a, 0, n - 1) : 0);
    }

    int newNode(long long s, int l, int r) {
        sum.push_back(s); L.push_back(l); R.push_back(r);
        return (int)sum.size() - 1;
    }

    int build(const vector<long long>& a, int l, int r) {
        if (l == r) return newNode(a[l], 0, 0);
        int m = (l + r) / 2;
        int lc = build(a, l, m), rc = build(a, m + 1, r);
        return newNode(sum[lc] + sum[rc], lc, rc);
    }

    // Path copying: copia un nodo por nivel. O(lg n)
    int update(int prev, int l, int r, int pos, long long val) {
        if (l == r) return newNode(val, 0, 0);
        int m = (l + r) / 2;
        int lc = L[prev], rc = R[prev];                // se comparten por defecto
        if (pos <= m) lc = update(L[prev], l, m, pos, val);
        else          rc = update(R[prev], m + 1, r, pos, val);
        return newNode(sum[lc] + sum[rc], lc, rc);
    }

    long long query(int node, int l, int r, int ql, int qr) const {
        if (node == 0 || qr < l || r < ql) return 0;
        if (ql <= l && r <= qr) return sum[node];
        int m = (l + r) / 2;
        return query(L[node], l, m, ql, qr) + query(R[node], m + 1, r, ql, qr);
    }

    // Interfaz: version nueva = version v con a[pos] = val. Devuelve su numero.
    // (Estilo competitivo: se asume que v y pos son validos.)
    int update(int v, int pos, long long val) {
        roots.push_back(update(roots[v], 0, n - 1, pos, val));
        return (int)roots.size() - 1;
    }

    long long query(int v, int l, int r) const {
        if (n == 0 || l > r) return 0;
        return query(roots[v], 0, n - 1, l, r);
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    vector<long long> a = {1, 2, 3, 4, 5};
    PersistentSegmentTree st(a, 10);

    cout << st.query(0, 0, 4) << '\n';     // 15

    int v1 = st.update(0, 2, 10);          // v1: a[2] = 10
    cout << st.query(0, 0, 4) << '\n';     // 15  (version vieja intacta)
    cout << st.query(v1, 0, 4) << '\n';    // 22

    int v2 = st.update(0, 0, 100);         // rama desde v0
    cout << st.query(v2, 0, 4) << '\n';    // 114
    return 0;
}