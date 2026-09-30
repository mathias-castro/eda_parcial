#include <iostream>
#include <vector>
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

    int query (SegmentTreeNode *root_l, SegmentTreeNode *root_r, int k, vector<int> &a){
        if (root_l->l == root_l->r){
            return a[root_l->l];
        }
        int cantidad = root_r->left->val - root_l->left->val;
        if (k<= cantidad){
            return query(root_l->left,root_r->left,k,a);
        }
        else{
            return query(root_l->right,root_r->right,k-cantidad,a);
        }
    }

    int query(int l, int r, int k, vector<int> &a){
        SegmentTreeNode *root_l = root_versions[l-1];
        SegmentTreeNode *root_r = root_versions[r];

        return query(root_l,root_r,k,a);
    }

    void update(SegmentTreeNode *curr, int pos){
            if (curr->l == curr->r){
                curr->val = curr->val + 1;
                return;
            }
            int mid = (curr->l + curr->r) /2;
            if (pos <= mid){
                curr->left = new SegmentTreeNode(curr->left->l,curr->left->r,curr->left->val,curr->left->left,curr->left->right);
                update(curr->left, pos);
            }
            else{
                curr->right = new SegmentTreeNode(curr->right->l,curr->right->r,curr->right->val,curr->right->left,curr->right ->right);
                update(curr->right, pos);
            }
            curr->val = curr->left->val + curr->right->val;
    };

    void update(int version, int pos){
        SegmentTreeNode *prevRoot = root_versions[version];
        SegmentTreeNode *newRoot = new SegmentTreeNode(prevRoot->l, prevRoot->r, prevRoot->val, prevRoot->left, prevRoot->right);
        update(newRoot, pos);
        root_versions.push_back(newRoot);
        return;
}
};

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n;
    cin>>n;
    long long a1,l,m;
    cin>>a1>>l>>m;
    long long r;
    vector<int> arr;

    arr.emplace_back(a1);
    for (int i = 0 ; i < n-1 ; i++){
        r = (a1 * l + m);
        a1 = r % (int)1e9;
        arr.emplace_back(a1);
    }
    vector<int> arr_compressed = arr;
    sort(arr_compressed.begin(),arr_compressed.end());
    arr_compressed.erase(unique(arr_compressed.begin(), arr_compressed.end()), arr_compressed.end());
    int n2 = arr_compressed.size();
    PersistentSegmentTree tree(0,n2-1);

    for (int i = 0; i < n; i++) {
        int valor_comprimido = lower_bound(arr_compressed.begin(), arr_compressed.end(), arr[i]) - arr_compressed.begin();
        
        tree.update(i, valor_comprimido); 
    }

    long long suma_total = 0;
    int B;
    cin >> B;

    while (B--) {
        long long G, x1, lx, mx, y1, ly, my, k1, lk, mk;
        cin >> G >> x1 >> lx >> mx >> y1 >> ly >> my >> k1 >> lk >> mk;

        for (int g = 1; g <= G; g++) {
            int ig = min(x1, y1);
            int jg = max(x1, y1);
            
            
            int respuesta = tree.query(ig, jg, k1, arr_compressed);
            suma_total += respuesta;

            if (g < G) {
                long long next_x = (((x1 - 1) * lx + mx) % n) + 1;
                long long next_y = (((y1 - 1) * ly + my) % n) + 1;
                
                int temp_ig = min(next_x, next_y);
                int temp_jg = max(next_x, next_y);
                
                long long next_k = (((k1 - 1) * lk + mk) % (temp_jg - temp_ig + 1)) + 1;

                x1 = next_x;
                y1 = next_y;
                k1 = next_k;
            }
        }
    }

    cout << suma_total << "\n";


    return 0;
}
