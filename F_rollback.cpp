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
    vector<int> positions;
    PersistentSegmentTree(int n, int m){
        root_versions.resize(n + 1);
        root_versions[n] = new SegmentTreeNode(0, n-1, 0, nullptr, nullptr);
        build(root_versions[n], 0, n-1);
        positions = vector<int>(m+1,-1);
    };

    void build(SegmentTreeNode *root,int l, int r){
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

    int query (SegmentTreeNode *root, int k){
        if (root->val < k){
            return 0;
        }
        if (root->l == root->r){
            return root->l +1;
        }
        int cant = root->left->val;
        if (k <= cant){
            return query(root->left,k);
        }
        else{
            return query(root->right, k - cant);
        }
    }

    int query(int l, int k){
        return query(root_versions[l],k);
    }

    void update(SegmentTreeNode *curr, SegmentTreeNode *prev, int pos, int newval){
            if (curr->l == curr->r){
                curr->val = newval;
                return;
            }
            int mid = (curr->l + curr->r) /2;
            if (pos <= mid){
                curr->right = prev->right;
                curr->left = new SegmentTreeNode(curr->l,mid,0,nullptr,nullptr);
                update(curr->left,prev->left, pos,  newval);
            }
            else{
                curr->left = prev->left;
                curr->right = new SegmentTreeNode(mid+1,curr->r,0,nullptr,nullptr);
                update(curr->right,prev->right, pos,  newval);
            }
            curr->val = curr->left->val + curr->right->val;
    };

    void update(int current_idx, int value, int pos){
        SegmentTreeNode *prevRoot = root_versions[current_idx + 1];
        
        SegmentTreeNode *tempRoot = new SegmentTreeNode(prevRoot->l, prevRoot->r, 0, nullptr, nullptr);
        update(tempRoot, prevRoot, pos, 1);
        
        int old_pos = positions[value];
        positions[value] = pos; 

        if (old_pos != -1) {
            SegmentTreeNode *finalRoot = new SegmentTreeNode(tempRoot->l, tempRoot->r, 0, nullptr, nullptr);
            update(finalRoot, tempRoot, old_pos, 0);
            
            root_versions[current_idx] = finalRoot; 
        } else {
            root_versions[current_idx] = tempRoot; 
        }
    }
};

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m;
    cin >> n >> m;
    PersistentSegmentTree tree(n, m);

    int x;
    vector<int> input(n);
    for(int i = 0 ; i < n ; i++){
        cin >> input[i];
    }

    for (int i = n - 1; i >= 0; i--){
        tree.update(i, input[i], i);
    }
    int q,xi,yi,li,ki;
    cin>>q;

    int p = 0;
    for (int i = 0 ; i < q; i++){
        cin>>xi>>yi;
        li = ((xi+p)%n)+1;
        ki = ((yi+p)%m)+1;
        int result = tree.query(li-1,ki);
        p = result;
        cout << result << endl;
    }
    return 0;
}
