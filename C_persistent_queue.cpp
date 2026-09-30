#include <iostream>
#include <vector>
using namespace std;

struct QueueNode{
    int val;
    int l,r;
    QueueNode *left;
    QueueNode *right;

    QueueNode(int l, int r, int val, QueueNode *left = nullptr, QueueNode *right = nullptr){
        this->l = l;
        this->r = r;
        this->left = left;
        this->right = right;
        this->val = val;
    }
};

struct Queue{
    QueueNode *root;
    int head;
    int tail;
    Queue(QueueNode *root, int head, int tail){
        this->root = root;
        this->head = head;
        this->tail = tail;
    }
};

struct PersistentQueue{
    vector<Queue*> root_versions;

    PersistentQueue(int N){
        auto root = new QueueNode(0,N-1,0,nullptr,nullptr);
        root_versions.emplace_back(new Queue(root,0,0));
        build(root_versions[0]->root,0,N-1);
    };

    void build(QueueNode *root, int l, int r){
        if (l == r){
            return;
        }
        int mid = (l+r)/2;

        root->left = new QueueNode(l,mid, 0, nullptr,nullptr);
        root->right = new QueueNode(mid+1,r,0,nullptr,nullptr);
        build(root->left,l,mid);
        build(root->right,mid+1,r);
    }

    void push(int version, int val){
        QueueNode* v = root_versions[version]->root;
        QueueNode* newroot = new QueueNode(v->l,v->r,v->val,v->left,v->right);
        update(newroot,root_versions[version]->tail,val);
        root_versions.emplace_back(new Queue(newroot,root_versions[version]->head ,root_versions[version]->tail+1));
    }
    void update(QueueNode *root, int pos, int val){
        if(root->l == root->r){
            root->val = val;
            return;
        }
        int mid = (root->l + root->r)/2;
        if (pos <= mid){
            root->left = new QueueNode(root->left->l, root->left->r, root->left->val, root->left->left, root->left->right);
            update(root->left,pos,val);
        }
        else{
            root->right = new QueueNode(root->right->l, root->right->r, root->right->val, root->right->left, root->right->right);
            update(root->right,pos,val);
        }
    }

    int query(QueueNode *root, int l, int r){
        if (l > root->r || r < root->l){
            return 0;
        }
        if (l <= root->l && r >= root->r){
            return root->val;
        }
        return query(root->left,l,r) + query(root->right,l,r);
    }

    int front(int version){
        Queue* q = root_versions[version];
        return query(q->root, q->head, q->head);
    }


    int pop(int version){
        Queue* q = root_versions[version];
        if (q->head == q->tail){
            throw("Error"); 
        }
        root_versions.emplace_back(new Queue(q->root, q->head+1, q->tail));
        return front(version);

}

};

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n;
    cin >>n;
    PersistentQueue queue(n);
    int op,v,x;
    for (int i = 0 ; i < n ; i++){
        cin >>op;
        if (op == 1){
            cin>>v>>x;
            queue.push(v,x);
        }
        else if (op == -1){
            cin>>v;
            int value = queue.pop(v);
            cout << value << endl;
        }
    }   
    return 0;
}
