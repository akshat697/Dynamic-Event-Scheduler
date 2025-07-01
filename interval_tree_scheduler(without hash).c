#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    int low, high;
    char description[100];
    int id;
    int notifyTime; // Time to send notification
    char notificationMethod[20]; // Method (e.g., email, pop-up)
    char recipient[100]; // Recipient for notification
} Interval;

typedef struct ITNode {
    Interval *i;
    int max;
    struct ITNode *left, *right;
    int height;
} ITNode;

int eventIdCounter = 1;

ITNode *newNode(Interval i) {
    ITNode *temp = (ITNode *)malloc(sizeof(ITNode));
    temp->i = (Interval *)malloc(sizeof(Interval));
    *(temp->i) = i;
    temp->max = i.high;
    temp->left = temp->right = NULL;
    temp->height = 1;
    return temp;
}

int height(ITNode *node) {
    if (node == NULL) return 0;
    return node->height;
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

int getBalance(ITNode *node) {
    if (node == NULL) return 0;
    return height(node->left) - height(node->right);
}

void updateMax(ITNode *node) {
    if (node == NULL) return;
    int max_child = 0;
    if (node->left) max_child = node->left->max;
    if (node->right && node->right->max > max_child) max_child = node->right->max;
    node->max = (max_child > node->i->high) ? max_child : node->i->high;
}

ITNode *rightRotate(ITNode *y) {
    ITNode *x = y->left;
    ITNode *T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    updateMax(y);
    updateMax(x);

    return x;
}

ITNode *leftRotate(ITNode *x) {
    ITNode *y = x->right;
    ITNode *T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    updateMax(x);
    updateMax(y);

    return y;
}

int doOverlap(Interval i1, Interval i2) {
    return (i1.low <= i2.high && i2.low <= i1.high);
}

ITNode *insertWithConflictCheck(ITNode *root, Interval i) {
    if (root == NULL) return newNode(i);

    if (doOverlap(*(root->i), i)) {
        printf("Conflict detected. Interval [%d, %d] cannot be added.\n", i.low, i.high);
        return root;
    }

    if (i.low < root->i->low)
        root->left = insertWithConflictCheck(root->left, i);
    else
        root->right = insertWithConflictCheck(root->right, i);

    root->height = max(height(root->left), height(root->right)) + 1;
    updateMax(root);

    int balance = getBalance(root);

    // if (balance > 1) {
    //     if (i.low < root->left->i->low) return rightRotate(root);
    // }
    // if (balance < -1) {
    //     if (i.low > root->right->i->low) return leftRotate(root);
    // }
    
    // Left‐Left
if (balance > 1 && i.low < root->left->i->low)
    return rightRotate(root);

// Left‐Right
if (balance > 1 && i.low > root->left->i->low) {
    root->left =  leftRotate(root->left);
    return rightRotate(root);
}

// Right‐Right
if (balance < -1 && i.low > root->right->i->low)
    return leftRotate(root);

// Right‐Left
if (balance < -1 && i.low < root->right->i->low) {
    root->right = rightRotate(root->right);
    return leftRotate(root);
}


    return root;
}

// ITNode *findEventById(ITNode *root, int id) {
//     if (root == NULL) return NULL;

//     if (id < root->i->id) {
//         return findEventById(root->left, id);
//     } else if (id > root->i->id) {
//         return findEventById(root->right, id);
//     } else {
//         return root;
//     }
// }

ITNode *findEventById(ITNode *root, int id) {
    if (root == NULL) return NULL;
    if (root->i->id == id)
        return root;
    ITNode *leftHit = findEventById(root->left, id);
    if (leftHit) return leftHit;
    return findEventById(root->right, id);
}


// ITNode *deleteEventById(ITNode *root, int id) {
//     if (root == NULL) return root;

//     if (id < root->i->id)
//         root->left = deleteEventById(root->left, id);
//     else if (id > root->i->id)
//         root->right = deleteEventById(root->right, id);
//     else {
//         if (root->left == NULL) {
//             ITNode *temp = root->right;
//             free(root->i);
//             free(root);
//             return temp;
//         } else if (root->right == NULL) {
//             ITNode *temp = root->left;
//             free(root->i);
//             free(root);
//             return temp;
//         }
//         ITNode *temp = root->right;
//         while (temp->left != NULL) temp = temp->left;

//         root->i = temp->i;
//         root->max = temp->max;
//         root->right = deleteEventById(root->right, temp->i->id);
//     }

//     root->height = max(height(root->left), height(root->right)) + 1;
//     updateMax(root);

//     int balance = getBalance(root);

//     if (balance > 1) {
//         if (getBalance(root->left) >= 0) return rightRotate(root);
//     }
//     if (balance < -1) {
//         if (getBalance(root->right) <= 0) return leftRotate(root);
//     }

//     return root;
// }

ITNode* deleteEventById(ITNode* root, int id) {
    if (root == NULL) 
        return NULL;

    // 1) If this node is the one to delete:
    if (root->i->id == id) {
        // a) Node with only one child or no child
        if (root->left == NULL || root->right == NULL) {
            ITNode *child = (root->left) ? root->left : root->right;
            if (child == NULL) {
                // No child: just delete
                free(root->i);
                free(root);
                return NULL;
            } else {
                // One child: replace root with that child
                free(root->i);
                ITNode *tmp = root;
                root = child;
                free(tmp);
            }
        } else {
            // b) Node with two children: get inorder successor (smallest in right subtree)
            ITNode *succ = root->right;
            while (succ->left)
                succ = succ->left;
            // Copy successor’s interval pointer into root
            free(root->i);
            root->i = succ->i;
            // Delete the successor node (by ID)
            root->right = deleteEventById(root->right, succ->i->id);
        }
    }
    // 2) Otherwise, recurse into both subtrees (since tree is NOT ordered by id)
    else {
        root->left  = deleteEventById(root->left,  id);
        root->right = deleteEventById(root->right, id);
    }

    // If the tree had only one node then return
    if (root == NULL)
        return NULL;

    // 3) Update height and max
    root->height = max(height(root->left), height(root->right)) + 1;
    updateMax(root);

    // 4) Get balance factor
    int balance = getBalance(root);

    // 5) Rebalance if needed

    // Left Left Case
    if (balance > 1 && getBalance(root->left) >= 0)
        return rightRotate(root);

    // Left Right Case
    if (balance > 1 && getBalance(root->left) < 0) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    // Right Right Case
    if (balance < -1 && getBalance(root->right) <= 0)
        return leftRotate(root);

    // Right Left Case
    if (balance < -1 && getBalance(root->right) > 0) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}


void scheduleNotification(ITNode *root, int eventId) {
    ITNode *eventNode = findEventById(root, eventId);
    if (eventNode == NULL) {
        printf("Event not found.\n");
        return;
    }

    printf("Enter Notification Time for Event ID %d: ", eventNode->i->id);
    scanf("%d", &(eventNode->i->notifyTime));
    printf("Enter Notification Method for Event ID %d: ", eventNode->i->id);
    scanf(" %[^\n]%*c", eventNode->i->notificationMethod);
    printf("Enter Recipient for Event ID %d: ", eventNode->i->id);
    scanf(" %[^\n]%*c", eventNode->i->recipient);
}

void checkAndDeliverNotifications(ITNode *root, int time) {
    if (root == NULL) return;
    checkAndDeliverNotifications(root->left, time);
    if (root->i->notifyTime == time) {
        printf("Notification for Event ID %d: %s - Method: %s, Recipient: %s\n",
               root->i->id, root->i->description, root->i->notificationMethod, root->i->recipient);
    }
    checkAndDeliverNotifications(root->right, time);
}

void explainEventsInTime(ITNode *root, int time) {
    if (root == NULL) return;
    if (time < root->i->low) {
        explainEventsInTime(root->left, time);
    } else if (time >= root->i->low && time <= root->i->high) {
        printf("At time %d: Event ID %d - %s\n", time, root->i->id, root->i->description);
        checkAndDeliverNotifications(root, time);
        explainEventsInTime(root->left, time);
        explainEventsInTime(root->right, time);
    } else {
        explainEventsInTime(root->right, time);
    }
}

void freeIntervalTree(ITNode *root) {
    if (root == NULL) return;

    freeIntervalTree(root->left);
    freeIntervalTree(root->right);
    free(root->i);
    free(root);
}

void inorder(ITNode *root) {
    if (root == NULL) return;

    inorder(root->left);
    printf("Interval: [%d, %d] - Event ID %d: %s\n", root->i->low, root->i->high, root->i->id, root->i->description);
    inorder(root->right);
}

int main() {
    ITNode *root = NULL;
    int choice;
    Interval i;

    while (1) {
        printf("\nInterval Tree Operations\n");
        printf("1. Insert Event\n");
        printf("2. Explain Events at Time\n");
        printf("3. Delete Event by ID\n");
        printf("4. Display Intervals (In-order)\n");
        printf("5. Schedule Notification for Event\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter Event ID: ");
                i.id = eventIdCounter++;
                printf("Enter Event Description: ");
                scanf(" %[^\n]%*c", i.description);
                printf("Enter Event Interval [low, high]: ");
                scanf("%d %d", &i.low, &i.high);
                root = insertWithConflictCheck(root, i);
                break;
            case 2:
                printf("Enter Time to Explain Events: ");
                int time;
                scanf("%d", &time);
                explainEventsInTime(root, time);
                break;
            case 3:
                printf("Enter Event ID to Delete: ");
                int eventIdToDelete;
                scanf("%d", &eventIdToDelete);
                root = deleteEventById(root, eventIdToDelete);
                break;
            case 4:
                printf("Intervals (In-order Traversal):\n");
                inorder(root);
                break;
            case 5:
                printf("Enter Event ID to Schedule Notification: ");
                int eventIdToNotify;
                scanf("%d", &eventIdToNotify);
                scheduleNotification(root, eventIdToNotify);
                break;
            case 6:
                freeIntervalTree(root);
                exit(0);
            default:
                printf("Invalid choice. Please enter a valid option.\n");
        }
    }

    return 0 ;
}