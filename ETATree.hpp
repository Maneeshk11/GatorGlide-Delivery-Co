#include "AVL.h"
#include <bits/stdc++.h>
// // #include "PriorityTree.hpp"

using namespace std;


void ETATree::createOrderInETA(int orderId, int currSysTime, int orderValue, int deliveryTime, int eta) {
    
    float normalizedOrderVal = orderValue/50;
    float priority = (0.3 * normalizedOrderVal) - (0.7 * currSysTime);
    insertRecursive(root_, orderId, currSysTime, orderValue, deliveryTime, eta, priority);
}

void ETATree::insertRecursive(TreeNode* &node, int orderId, int cst, int ov, int dt, int eta, float priority) {
    
    if (node == nullptr) {
        node = new TreeNode(orderId, cst, ov, dt);
        node->ETA = eta;
        return;
    }

    else if (priority >= node->priority) {
        insertRecursive(node->left, orderId, cst, ov, dt, eta, priority);
        ensureBalance(node);
        return;
    } else {
        insertRecursive(node->right, orderId, cst, ov, dt, eta, priority);
        ensureBalance(node);
        return;
    }
}

void ETATree::ensureBalance(TreeNode* &node) {
    if (!node || node == nullptr) return;

    int initialBalance = getBalanceFactor(node);
    if (initialBalance < -2 || initialBalance > 2) {
        throw runtime_error("ERROR: Detected invalid balance factor");
    }

    if (initialBalance == -2) {
        int bal = getBalanceFactor(node->left);
        if (bal == 1 || bal == 0) {
            rotateLeft(node);
        } else if (bal == -1) {
            rotateRightLeft(node);
        }
    } else if (initialBalance == 2) {
        int bal = getBalanceFactor(node->right);
        if (bal == -1 || bal == 0) {
            rotateRight(node);
        } else if (bal == -1) {
            rotateLeftRight(node);
        }
    }
    updateHeight(node);
}


//rotate functions 

void ETATree::rotateLeft(TreeNode* &node) {

    TreeNode* x = node;
    TreeNode* y = node->right;
    TreeNode* z = node->right->left;
    
    x->right = z;
    y->left = x;
    node = y;

    updateHeight(x);
    updateHeight(y);
}

void ETATree::rotateRight(TreeNode* &node) {
    TreeNode* x = node;
    TreeNode* y = node->left;
    TreeNode* z = node->left->right;

    x->left = z;
    y->right = x;
    node = y;

    updateHeight(x);
    updateHeight(y);
}

void ETATree::rotateLeftRight(TreeNode* &node) {
    if (!node) {
        cout << "ERROR: Node is null" << endl;
    }
    rotateLeft(node->left);
    rotateRight(node);
}

void ETATree::rotateRightLeft(TreeNode* &node) {
    if (!node) {
        cout << "ERROR: Node is null" << endl;
    }
    rotateRight(node->right);
    rotateRight(node);
}

// utils

int ETATree::getBalanceFactor(TreeNode* &node) {
    if (!node) return 0;
    return getHeight(node->left) - getHeight(node->right);
}

void ETATree::updateHeight(TreeNode* &node) {
    if (!node) return;
    node->height = 1 + max(getHeight(node->left), getHeight(node->right));
}

int ETATree::getHeight(TreeNode* &node) {
    if(!node) return -1;
    else {
        return node->height;
    }
}

int ETATree::findNodeETA(int orderId) {
    if (root_ == nullptr) {
        return 0;
    } else {
        return findNode(root_, orderId)->ETA + findNode(root_, orderId)->deliveryTime;
    }
}

TreeNode*& ETATree::findNode(TreeNode* &node, int orderId) {
    if (node == nullptr) {
        return node;
    } 
    if (node->orderId == orderId) {
        return node;
    }
    TreeNode*& leftResult = findNode(node->left, orderId);
    if (leftResult != nullptr) {
        return leftResult;
    }
    return findNode(node->right, orderId);
}

// remove order functions -------------------------------------

void ETATree::cancelOrder(int orderId, int currSysTime) {
    if (root_ == nullptr) {
        cout << "No orders in the system" << endl;
        return;
    } 
    int eta = findNode(root_, orderId)->ETA;
    deleteRecursiveUpdateETA(root_, eta, currSysTime, orderId);
    // cout << "Order " << orderId << " has been cancelled" << endl;
}

void ETATree::removeOrderAfterDelivery(int eta, int currSysTime) {
    if (root_ == nullptr) {
        cout << "No orders in the system" << endl;
        return;
    } 
    deleteRecursive(root_, eta, currSysTime);
}

void ETATree::deleteRecursive(TreeNode* &node, int eta, int currSysTime) {
    if (node == nullptr) {
        // cout << "order not found" << endl;
        return;
    }
    else if (node->ETA == eta) {
        deleteNode(node);
        return;
    } else if (node->ETA <= eta) {
        deleteRecursive(node->right, eta, currSysTime);
        ensureBalance(node);
    } else {
        deleteRecursive(node->left, eta, currSysTime);
        ensureBalance(node);
    }
}

void ETATree::deleteRecursiveUpdateETA(TreeNode* &node, int eta, int currSysTime, int orderId) {
    if (node == nullptr) {
        // cout << "order not found" << endl;
        return;
    }
    else if (node->ETA == eta && node->orderId == orderId) {
        deleteNode(node);
        return;
    } else if (node->ETA <= eta) {
        deleteRecursive(node->right, eta, currSysTime);
        ensureBalance(node);
    } else {
        deleteRecursive(node->left, eta, currSysTime);
        ensureBalance(node);
    }
}

void ETATree::deleteNode(TreeNode* &node) {
    if (!node) {
        throw runtime_error("ERROR: RemoveNode called on nullptr");
    }
    if (node->left == nullptr && node->right == nullptr) {
        TreeNode* temp = node;
        node = nullptr;
        delete temp;
    } else if (node->left == nullptr) {
        TreeNode* temp = node;
        node = node->right;
        delete temp;
        temp = nullptr;
    } else if (node->right == nullptr) {
        TreeNode* temp = node;
        node = node->left;
        delete temp;
        temp = nullptr;
    } else {
        iopRemove(node);
    }
}

void ETATree::iopRemove(TreeNode* &node) { // calls a recursive function to find and remove the inorder predecssor
    if (!node) {
        throw runtime_error("No iop for the node");
    } 
    iopRemove(node, node->left, true);
    if (node->left) {               
        ensureBalance(node->left);
    }
    ensureBalance(node);
    return;   
}

void ETATree::iopRemove(TreeNode* &node, TreeNode* &iop, bool iniCall) { // recursive function to remove the inorder predecessor in cancel order
    if (!node) {
        throw runtime_error("iop remove called on nullptr: target node is null");
    } else if (!iop) {
        throw runtime_error("iop remove called on nullptr: iop is null");
    }

    if (iop->right != nullptr) {
        iopRemove(node, iop->right, false);
        if (!iniCall) {
            if (iop) {
                ensureBalance(iop);
            }
        }
    } else {
        TreeNode*& movedNode = swapNodes(node, iop);
        deleteNode(movedNode);
    }
}

TreeNode*& ETATree::swapNodes(TreeNode* &node1, TreeNode* &node2) {
    TreeNode* orig1 = node1;
    TreeNode* orig2 = node2;
    swap(node1->height, node2->height);
    if (node1->left == node2) {
        swap(node1->right, node2->right);
        node1->left = orig2->left;
        orig2->left = node1;
        node1 = orig2;
        return node1->left;
    } else if (node1->right == node2) {
        swap(node1->left, node2->left);
        node1->right = orig2->right;
        orig2->right = node1;
        node1 = orig2;
        return node1->right;
    } else if (node2->left == node1) {
        swap(node2->right, node1->right);
        node2->left = orig1->left;
        orig1->left = node2;
        node2 = orig1;
        return node2->left;
    } else if (node2->right == node1) {
        swap(node2->left, node1->left);
        node2->right = orig1->right;
        orig1->right = node2;
        node2 = orig1;
        return node2->right;
    } else {
        swap(node1->left, node2->left);
        swap(node1->right, node2->right);
        swap(node1, node2);
        return node2;
    }
}