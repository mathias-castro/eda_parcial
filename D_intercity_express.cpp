#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>
#include <climits>
#include <stdexcept>
using namespace std;

// D. Intercity Express
//
// Un ticket (c, a_i, b_i) ocupa el asiento c en los tramos a_i .. b_i - 1.
// El asiento c esta libre para viajar de a a b si ningun ticket de c se cruza
// con [a, b), o sea, si no hay ticket con  a_i < b  y  b_i > a.
//
// Truco: fijo a. De los tickets de c solo importan los que terminan despues
// de a (b_i > a); los que terminan antes ya no molestan. Sea
//     f_c(a) = el menor a_i entre los tickets de c con b_i > a   (o INF)
// Entonces c esta libre de a a b  <=>  f_c(a) >= b.
// (Si hubiera un ticket que empieza antes de b y termina despues de a, f_c
// seria menor que b.)
//
// La consulta es: el menor c con f_c(a) >= b. Con un segment tree de MAXIMOS
// sobre los asientos, bajo desde la raiz: si el maximo de la izquierda ya es
// >= b, hay un asiento valido ahi, voy a la izquierda; si no, a la derecha.
//
// Persistencia: f depende de a. Cuando a llega a b_i, ese ticket deja de
// contar y f_c sube. Ordeno esos eventos por b_i y guardo una version del
// arbol despues de cada uno. Para una consulta con a, uso la version que tiene
// aplicados todos los eventos con b_i <= a (busqueda binaria).
//
// Para saber el nuevo f_c tras quitar tickets: ordeno los tickets de cada
// asiento por b_i y precalculo el minimo de a_i de ahi en adelante (sufijo).


struct Node {
    long long val;      // MAXIMO de f_c en los asientos [l, r]
    int l, r;
    Node* left;
    Node* right;
    Node(int l, int r, long long val, Node* left = nullptr, Node* right = nullptr)
        : val(val), l(l), r(r), left(left), right(right) {}
};

class PersistentSegmentTree {
private:
    int n;
    vector<Node*> roots;

    deque<Node> pool;   // en vez de 'new' (ver B)
    Node* newNode(int l, int r, long long val, Node* left = nullptr, Node* right = nullptr) {
        pool.emplace_back(l, r, val, left, right);
        return &pool.back();
    }

    Node* build(const vector<long long>& a, int l, int r) {
        if (l == r) return newNode(l, r, a[l]);
        int mid = (l + r) / 2;
        Node* L = build(a, l, mid);
        Node* R = build(a, mid + 1, r);
        return newNode(l, r, max(L->val, R->val), L, R);   // aqui combino con max
    }

    Node* update(Node* prev, int pos, long long newVal) {
        if (prev->l == prev->r) return newNode(prev->l, prev->r, newVal);
        int mid = (prev->l + prev->r) / 2;
        Node* L = prev->left;
        Node* R = prev->right;
        if (pos <= mid) L = update(prev->left, pos, newVal);
        else            R = update(prev->right, pos, newVal);
        return newNode(prev->l, prev->r, max(L->val, R->val), L, R);
    }

    // Primer indice con valor >= b dentro del subarbol, o -1 si no hay.
    int firstAtLeast(Node* node, long long b) const {
        if (node->val < b) return -1;              // nadie aqui llega a b
        if (node->l == node->r) return node->l;
        if (node->left->val >= b) return firstAtLeast(node->left, b);
        return firstAtLeast(node->right, b);
    }

    void checkVersion(int v) const {
        if (v < 0 || v >= (int)roots.size()) throw out_of_range("version inexistente");
    }

public:
    explicit PersistentSegmentTree(const vector<long long>& a) : n((int)a.size()) {
        roots.push_back(build(a, 0, n - 1));
    }

    int update(int v, int pos, long long newVal) {
        checkVersion(v);
        if (pos < 0 || pos >= n) throw out_of_range("posicion fuera de rango");
        roots.push_back(update(roots[v], pos, newVal));
        return (int)roots.size() - 1;
    }

    int firstAtLeast(int v, long long b) const {
        checkVersion(v);
        return firstAtLeast(roots[v], b);
    }
};


int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    const long long INF = LLONG_MAX;

    long long n;
    int s, m;
    cin >> n >> s >> m;

    // Tickets de cada asiento como (b_i, a_i).
    vector<vector<pair<long long, long long>>> bySeat(s);
    for (int i = 0; i < m; i++) {
        int c;
        long long a, b;
        cin >> c >> a >> b;
        bySeat[c - 1].push_back({b, a});
    }

    // Por asiento: ordeno por b_i y calculo sufMin[j] = min a_i de j en adelante.
    // Al inicio (a muy chico) todos los tickets cuentan: f_c = sufMin[0].
    // Cuando a alcanza b del ticket j, pasa a contar desde j+1: f_c = sufMin[j+1].
    vector<long long> initial(s, INF);
    struct Event { long long coord; int seat; int order; long long newVal; };
    vector<Event> events;

    for (int c = 0; c < s; c++) {
        auto& t = bySeat[c];
        sort(t.begin(), t.end());
        int k = (int)t.size();
        vector<long long> sufMin(k + 1, INF);
        for (int j = k - 1; j >= 0; j--) sufMin[j] = min(sufMin[j + 1], t[j].second);
        initial[c] = sufMin[0];
        for (int j = 0; j < k; j++) events.push_back({t[j].first, c, j, sufMin[j + 1]});
    }

    // Ordeno por coordenada; si empatan en el mismo asiento, en el orden de j
    // (asi el ultimo aplicado deja el valor correcto).
    sort(events.begin(), events.end(), [](const Event& x, const Event& y) {
        if (x.coord != y.coord) return x.coord < y.coord;
        if (x.seat != y.seat) return x.seat < y.seat;
        return x.order < y.order;
    });

    // version k = arbol con los primeros k eventos aplicados.
    PersistentSegmentTree st(initial);
    vector<long long> coords;
    for (int k = 0; k < (int)events.size(); k++) {
        st.update(k, events[k].seat, events[k].newVal);
        coords.push_back(events[k].coord);
    }

    int q;
    cin >> q;
    long long p = 0;
    while (q--) {
        long long x, y;
        cin >> x >> y;
        long long a = x + p, b = y + p;

        // Cuantos eventos tienen b_i <= a: esa es la version que corresponde.
        int version = (int)(upper_bound(coords.begin(), coords.end(), a) - coords.begin());

        int seat = st.firstAtLeast(version, b);
        long long answer = (seat == -1) ? 0 : seat + 1;
        cout << answer << '\n';
        p = answer;
    }
    return 0;
}
