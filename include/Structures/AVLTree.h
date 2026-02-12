#ifndef AVLTREE_H
#define AVLTREE_H

#include "../Models/Visitor.h"

struct AVLNode{
    Visitor* visitor;
    AVLNode *right, *left;
    int height;

    AVLNode(Visitor* v) : visitor(v), right(nullptr), left(nullptr), height(1) {}
};

class AVLTree{
    private:

    AVLNode* root;

    int getHeight(AVLNode* n) {return n ? n->height : 0;}
    int getBalance(AVLNode* n) {
        return n ? (getHeight(n->left) - getHeight(n->right)) : 0;
    }

    AVLNode* rightRotate(AVLNode* y) {
        AVLNode* x = y->left;
        AVLNode* T2 = x->right;
        x->right = y;
        y->left = T2;
        y->height = std::max(getHeight(y->left), getHeight(y->right)) + 1;
        x->height = std::max(getHeight(x->left), getHeight(x->right)) + 1;
        return x;
    }

    AVLNode* leftRotate(AVLNode* x) {
        AVLNode* y = x->right;
        AVLNode* T2 = y->left;
        y->left = x;
        x->right = T2;
        x->height = std::max(getHeight(x->left), getHeight(x->right)) + 1;
        y->height = std::max(getHeight(y->left), getHeight(y->right)) + 1;
        return y;
    }

    AVLNode* insert(AVLNode* node, Visitor* v) {
        if (!node) return new AVLNode(v);

        if (v->id < node->visitor->id)
            node->left = insert(node->left, v);
        else if (v->id > node->visitor->id)
            node->right = insert(node->right, v);
        else return node;

        node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));

        int balance = getBalance(node);

        // 1. Left Left
        if (balance > 1 && v->id < node->left->visitor->id)
            return rightRotate(node);
        // 2. Right Right
        if (balance < -1 && v->id > node->right->visitor->id)
            return leftRotate(node);
        // 3. Left Right
        if (balance > 1 && v->id > node->left->visitor->id) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }
        // 4. right Left
        if (balance < -1 && v->id < node->right->visitor->id) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }
        return node;
    }

    Visitor* search(AVLNode* node, int id) {
        if (!node) return nullptr;
        if (node->visitor->id == id) return node->visitor;
        if (id < node->visitor->id) return search(node->left, id);
        return search(node->right, id);
    }

    // تابع کمکی برای پیدا کردن گره با کمترین مقدار (جانشین این‌اوردر)
    AVLNode* minValueNode(AVLNode* node) {
        AVLNode* current = node;
        while (current->left != nullptr) current = current->left;
        return current;
    }

    AVLNode* remove(AVLNode* root, int id) {
        if (!root) return root;

        // ۱. حذف معمولی BST
        if (id < root->visitor->id)
            root->left = remove(root->left, id);
        else if (id > root->visitor->id)
            root->right = remove(root->right, id);
        else {
            // گره پیدا شد!
            if (!root->left || !root->right) {
                AVLNode* temp = root->left ? root->left : root->right;
                if (!temp) {
                    temp = root;
                    root = nullptr;
                } else
                    *root = *temp;
                delete temp;
            } else {
                AVLNode* temp = minValueNode(root->right);
                root->visitor = temp->visitor;
                root->right = remove(root->right, temp->visitor->id);
            }
        }

        if (!root) return root;

        // ۲. آپدیت ارتفاع
        root->height = 1 + std::max(getHeight(root->left), getHeight(root->right));

        // ۳. بالانس کردن
        int balance = getBalance(root);

        // Left Left
        if (balance > 1 && getBalance(root->left) >= 0)
            return rightRotate(root);

        // Left Right
        if (balance > 1 && getBalance(root->left) < 0) {
            root->left = leftRotate(root->left);
            return rightRotate(root);
        }

        // Right Right
        if (balance < -1 && getBalance(root->right) <= 0)
            return leftRotate(root);

        // Right Left
        if (balance < -1 && getBalance(root->right) > 0) {
            root->right = rightRotate(root->right);
            return leftRotate(root);
        }

        return root;
    }

    void saveRecursive(AVLNode* node, ostream& out) {
        if (!node) return;
        // فرمت: VISITOR ID NAME PATIENCE
        out << "VISITOR " << node->visitor->id << " " << node->visitor->name 
            << " " << node->visitor->patience << endl;
        saveRecursive(node->left, out);
        saveRecursive(node->right, out);
    }
    
public:
    AVLTree() : root(nullptr) {}
    void addVisitor(Visitor* v) { root = insert(root, v); }
    Visitor* findVisitor(int id) { return search(root, id); }
    void removeFromTree(int id) { root = remove(root, id); }
    void saveToStream(ostream& out) {
        saveRecursive(root, out);
    }
};



#endif