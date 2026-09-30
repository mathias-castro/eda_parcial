#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;


struct SegmentTreeNode{
    int val;
    int l,r;
    SegmentTreeNode *left;
    SegmentTreeNode *right;

    SegmentTreeNode(int l, int r, int val, SegmentTreeNode *left = nullptr, SegmentTreeNode *right = nullptr){
        this->l = l;
        this->r = r;
        this->left = left;
        this->right = right;
        this->val = val;
    }
};

struct PersistentSegmentTree{
    vector<SegmentTreeNode*> root_versions;

    PersistentSegmentTree(int l, int r){
        root_versions.emplace_back(new SegmentTreeNode(l,r,0,nullptr,nullptr));
        build(root_versions[0],l,r);
    };

    void build(SegmentTreeNode *root, int l, int r){
        if (l == r){
            root->val = 0;
            return;
        }
        int mid = (l+r)/2;

        root->left = new SegmentTreeNode(l,mid, 0, nullptr,nullptr);
        root->right = new SegmentTreeNode(mid+1,r,0,nullptr,nullptr);

        build(root->left,l,mid);
        build(root->right,mid+1,r);

        root->val = root->left->val + root->right->val;
    }

    int query (SegmentTreeNode *root, int l, int r){
        if (l > root->r || r < root->l){
            return 0;
        }
        if (l <= root->l && r >= root->r){
            return root->val;
        }
        return query(root->left,l,r) + query(root->right,l,r);
    }

    // Suma de las posiciones [l, r] en la version 'version'.
    int query(int version, int l, int r){
        return query(root_versions[version], l, r);
    }

    // Igual que antes, pero ahora sumo 'delta' en la hoja (+1 o -1).
    void update(SegmentTreeNode *curr, SegmentTreeNode *prev, int pos, int delta){
            if (curr->l == curr->r){
                curr->val = curr->val + delta;
                return;
            }
            int mid = (curr->l + curr->r) /2;
            if (pos <= mid){
                curr->right = prev->right;
                curr->left = new SegmentTreeNode(curr->l,mid,prev->left->val,prev->left->left,prev->left->right);
                update(curr->left,prev->left,pos,delta);
            }
            else{
                curr->left = prev->left;
                curr->right = new SegmentTreeNode(mid+1,curr->r,prev->right->val,prev->right->left,prev->right->right);
                update(curr->right,prev->right,pos,delta);
            }
            curr->val = curr->left->val + curr->right->val;
    };

    void update(int version, int pos, int delta){
        SegmentTreeNode *prevRoot = root_versions[version];
        SegmentTreeNode *newRoot = new SegmentTreeNode(prevRoot->l, prevRoot->r, prevRoot->val, prevRoot->left, prevRoot->right);
        update(newRoot,prevRoot, pos, delta);
        root_versions.push_back(newRoot);
        return;
}
};

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n,x;
    cin>>n;
    vector<int> data(n+1);
    for (int i = 1 ; i <= n ; i++){
        cin>>x;
        data[i] = x;
    }

    PersistentSegmentTree tree(1,n);

    // last[v] = ultima posicion donde vi el valor v (0 si todavia no aparece).
    vector<int> last(1000001, 0);

    // versionOf[j] = version del arbol con las ultimas apariciones de a[1..j].
    vector<int> versionOf(n+1, 0);
    int current = 0;

    for (int j = 1 ; j <= n ; j++){
        int v = data[j];
        if (last[v] != 0){
            // Su aparicion anterior ya no es la ultima.
            tree.update(current, last[v], -1);
            current = tree.root_versions.size() - 1;
        }
        tree.update(current, j, +1);
        current = tree.root_versions.size() - 1;

        versionOf[j] = current;
        last[v] = j;
    }

    int q;
    cin>>q;
    for (int p = 0 ; p < q ; p++){
        int i, j;
        cin >> i >> j;
        cout << tree.query(versionOf[j], i, j) << "\n";
    }
    return 0;
}