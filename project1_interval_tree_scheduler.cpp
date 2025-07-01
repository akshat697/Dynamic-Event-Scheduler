#include <bits/stdc++.h>
using namespace std;

struct Interval {
    int low, high;
    string description;
    int id;
    int notifyTime = -1;
    string notificationMethod, recipient;
};

struct ITNode {
    Interval *i;
    int max;
    ITNode *left, *right;
    int height;
    ITNode(Interval *iv)
        : i(iv), max(iv->high), left(nullptr), right(nullptr), height(1) {}
};

static int eventIdCounter = 1;
static unordered_map<int,int> idToLow;  // maps event ID → interval.low

// Utility
int height(ITNode* n)               { return n ? n->height : 0; }
int getBalance(ITNode* n)           { return n ? height(n->left) - height(n->right) : 0; }
int updMax(ITNode* n) {
    int m = n->i->high;
    if (n->left)  m = max(m, n->left->max);
    if (n->right) m = max(m, n->right->max);
    return m;
}

// Rotations
ITNode* rightRotate(ITNode* y) {
    ITNode *x = y->left, *T2 = x->right;
    x->right = y;  y->left = T2;
    y->height = 1 + max(height(y->left), height(y->right));
    x->height = 1 + max(height(x->left), height(x->right));
    y->max = updMax(y);
    x->max = updMax(x);
    return x;
}

ITNode* leftRotate(ITNode* x) {
    ITNode *y = x->right, *T2 = y->left;
    y->left = x;  x->right = T2;
    x->height = 1 + max(height(x->left), height(x->right));
    y->height = 1 + max(height(y->left), height(y->right));
    x->max = updMax(x);
    y->max = updMax(y);
    return y;
}

// Overlap
bool doOverlap(const Interval &a, const Interval &b) {
    return (a.low <= b.high && b.low <= a.high);
}

// Insert with conflict check + AVL
ITNode* insertNode(ITNode* root, Interval* iv) {
    if (!root) {
        idToLow[iv->id] = iv->low;
        return new ITNode(iv);
    }
    if (doOverlap(*root->i, *iv)) {
        printf("Conflict detected: [%d, %d]\n", iv->low, iv->high);
        delete iv;
        return root;
    }
    if (iv->low < root->i->low)
        root->left  = insertNode(root->left,  iv);
    else
        root->right = insertNode(root->right, iv);

    root->height = 1 + max(height(root->left), height(root->right));
    root->max    = updMax(root);

    int bf = getBalance(root);
    // LL
    if (bf >  1 && iv->low < root->left->i->low)      return rightRotate(root);
    // RR
    if (bf < -1 && iv->low > root->right->i->low)     return leftRotate(root);
    // LR
    if (bf >  1 && iv->low > root->left->i->low) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }
    // RL
    if (bf < -1 && iv->low < root->right->i->low) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }
    return root;
}

// BST-delete by low+id, then AVL rebalance
ITNode* deleteByLowAndId(ITNode* root, int low, int id) {
    if (!root) return nullptr;
    if (low < root->i->low)
        root->left  = deleteByLowAndId(root->left,  low, id);
    else if (low > root->i->low)
        root->right = deleteByLowAndId(root->right, low, id);
    else {
        if (root->i->id != id) {
            root->right = deleteByLowAndId(root->right, low, id);
        } else {
            idToLow.erase(id);
            if (!root->left || !root->right) {
                ITNode* child = root->left ? root->left : root->right;
                delete root->i;
                delete root;
                return child;
            }
            ITNode* succ = root->right;
            while (succ->left) succ = succ->left;
            delete root->i;
            root->i = new Interval(*succ->i);
            idToLow[root->i->id] = root->i->low;
            root->right = deleteByLowAndId(root->right, succ->i->low, succ->i->id);
        }
    }
    if (!root) return nullptr;
    root->height = 1 + max(height(root->left), height(root->right));
    root->max    = updMax(root);

    int bf = getBalance(root);
    if (bf >  1 && getBalance(root->left) >= 0)      return rightRotate(root);
    if (bf >  1 && getBalance(root->left) <  0) { root->left = leftRotate(root->left);  return rightRotate(root); }
    if (bf < -1 && getBalance(root->right) <= 0)     return leftRotate(root);
    if (bf < -1 && getBalance(root->right) >  0) { root->right = rightRotate(root->right); return leftRotate(root); }
    return root;
}

// Find node by ID
ITNode* findEventById(ITNode* root, int id) {
    auto it = idToLow.find(id);
    if (it == idToLow.end()) return nullptr;
    int low = it->second;
    ITNode* cur = root;
    while (cur) {
        if      (low < cur->i->low)  cur = cur->left;
        else if (low > cur->i->low)  cur = cur->right;
        else if (cur->i->id == id)   return cur;
        else                          cur = cur->right;
    }
    return nullptr;
}

// Notifications & queries
void scheduleNotification(ITNode* root, int id) {
    ITNode* node = findEventById(root, id);
    if (!node) { printf("Event not found\n"); return; }
    printf("Notify time: "); scanf("%d", &node->i->notifyTime);
    printf("Method: ");    scanf(" %[^\n]%*c", &node->i->notificationMethod[0]);
    printf("Recipient: "); scanf(" %[^\n]%*c", &node->i->recipient[0]);
}

void checkAndDeliver(ITNode* root, int t) {
    if (!root) return;
    checkAndDeliver(root->left,  t);
    if (root->i->notifyTime == t)
        printf("Notification (ID %d): %s → %s for %s\n",
               root->i->id, root->i->description.c_str(),
               root->i->notificationMethod.c_str(),
               root->i->recipient.c_str());
    checkAndDeliver(root->right, t);
}

void explainEventsInTime(ITNode* root, int t) {
    if (!root || t > root->max) return;
    if (t < root->i->low)
        explainEventsInTime(root->left, t);
    else {
        if (t <= root->i->high) {
            printf("At %d: ID %d → %s\n", t, root->i->id, root->i->description.c_str());
            checkAndDeliver(root, t);
        }
        explainEventsInTime(root->left,  t);
        explainEventsInTime(root->right, t);
    }
}

void inorder(ITNode* root) {
    if (!root) return;
    inorder(root->left);
    printf("[%d,%d] ID %d: %s\n",
           root->i->low, root->i->high, root->i->id, root->i->description.c_str());
    inorder(root->right);
}

void freeTree(ITNode* root) {
    if (!root) return;
    freeTree(root->left);
    freeTree(root->right);
    delete root->i;
    delete root;
}

int main() {
    ITNode* root = nullptr;
    while (1) {
        printf("\nInterval Tree Operations\n1. Insert Event\n2. Explain Events at Time\n3. Delete Event by ID\n4. Display Intervals (In-order)\n5. Schedule Notification for Event\n6. Exit\n");
        printf("Enter your choice: ");
        int c; scanf("%d", &c);
        if      (c == 1) {
            Interval* iv = new Interval();
            iv->id = eventIdCounter++;
            printf("Desc: "); scanf(" %[^\n]%*c", &iv->description[0]);
            printf("Interval low high: "); scanf("%d %d", &iv->low, &iv->high);
            root = insertNode(root, iv);
        }
        else if (c == 2) {
            int t; printf("Time: "); scanf("%d", &t);
            explainEventsInTime(root, t);
        }
        else if (c == 3) {
            int id; printf("ID to delete: "); scanf("%d", &id);
            auto it = idToLow.find(id);
            if (it == idToLow.end()) { printf("Not found\n"); continue; }
            root = deleteByLowAndId(root, it->second, id);
        }
        else if (c == 4) {
            printf("In-order traversal:\n");
            inorder(root);
        }
        else if (c == 5) {
            int id; printf("ID to notify: "); scanf("%d", &id);
            scheduleNotification(root, id);
        }
        else if (c == 6) {
            freeTree(root);
            break;
        }
        else {
            printf("Invalid choice\n");
        }
    }
    return 0;
}
