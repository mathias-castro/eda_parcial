#include <iostream>
#include <vector>
using namespace std;

// Stack con persistencia total (path copying: el camino copiado mide 1 nodo).
// Cada nodo guarda la SUMA acumulada desde el fondo, asi top(v) da la masa total en O(1).
class PersistentStack {
private:
    struct Node {
        long long sum;   // masa total de la pila hasta este nodo
        Node* next;
        Node(long long s, Node* n) : sum(s), next(n) {}
    };

    vector<Node*> roots;   // roots[v] = tope de la version v

public:
    explicit PersistentStack(int maxVersions = 0) {
        roots.reserve(maxVersions + 1);
        roots.push_back(new Node(0, nullptr));   // version 0: pila vacia (centinela, suma 0)
    }

    // Crea una version nueva = version v + un elemento de masa m. O(1).
    void push(int v, long long m) {
        roots.push_back(new Node(roots[v]->sum + m, roots[v]));
    }

    // Crea una version nueva = version v sin su tope. O(1).
    // Si v esta vacia, la nueva version tambien queda vacia.
    void pop(int v) {
        Node* r = roots[v];
        roots.push_back(r->next ? r->next : r);
    }

    // Masa total de la version v. O(1).
    long long top(int v) const {
        return roots[v]->sum;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;
    PersistentStack st(n);

    for (int i = 1; i <= n; i++) {
        int t;
        long long m;
        cin >> t >> m;
        if (m == 0) st.pop(t);
        else        st.push(t, m);
    }

    long long result = 0;
    for (int i = 1; i <= n; i++) result += st.top(i);

    cout << result << '\n';
    return 0;
}