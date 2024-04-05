#include "AVL.h"
#include <bits/stdc++.h>
// #include "ETATree.hpp"

using namespace std;


void Tree::createOrder(int orderId, int currSysTime, int orderValue, int deliveryTime) { // Creates an order in priority (inserts by priority) and updates the eta tree by calculating the eta
    float normalizedOrderVal = orderValue/50;
    float priority = (0.3 * normalizedOrderVal) - (0.7 * currSysTime);
    insertRecursive(root_, orderId, currSysTime, orderValue, deliveryTime, priority);
    int eta;
    if (findIOP(findNode(root_, orderId)) != nullptr) {
        eta = etaTree_->findNodeETA(findIOP(findNode(root_, orderId))->orderId) + deliveryTime;
    } else {
        eta = currSysTime + deliveryTime;
    }
    etaTree_->createOrderInETA(orderId, currSysTime, orderValue, deliveryTime, eta);
    findNode(root_, orderId)->ETA = eta;
    cout << "Order " << orderId << " has been created - ETA: " << eta << endl;
    updateTimeNecessary(orderId, currSysTime);
}

void Tree::insertRecursive(TreeNode* &node, int orderId, int cst, int ov, int dt, float priority) {
    if (node == nullptr) {
        node = new TreeNode(orderId, cst, ov, dt);
        return;
    }

    else if (priority <= node->priority) {
        insertRecursive(node->left, orderId, cst, ov, dt, priority);
        ensureBalance(node);
        return;
    } else {
        insertRecursive(node->right, orderId, cst, ov, dt, priority);
        ensureBalance(node);
        return;
    }
}

void Tree::ensureBalance(TreeNode* &node) {
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

// rotate functions

void Tree::rotateLeft(TreeNode* &node) {

    TreeNode* x = node;
    TreeNode* y = node->right;
    TreeNode* z = node->right->left;
    
    x->right = z;
    y->left = x;
    node = y;

    updateHeight(x);
    updateHeight(y);
}

void Tree::rotateRight(TreeNode* &node) {
    TreeNode* x = node;
    TreeNode* y = node->left;
    TreeNode* z = node->left->right;

    x->left = z;
    y->right = x;
    node = y;

    updateHeight(x);
    updateHeight(y);
}

void Tree::rotateLeftRight(TreeNode* &node) {
    if (!node) {
        cout << "ERROR: Node is null" << endl;
    }
    rotateLeft(node->left);
    rotateRight(node);
}

void Tree::rotateRightLeft(TreeNode* &node) {
    if (!node) {
        cout << "ERROR: Node is null" << endl;
    }
    rotateRight(node->right);
    rotateRight(node);
}


// cancel order functions

void Tree::cancelOrder(int orderId, int currSysTime) {
    if (root_ == nullptr) {
        cout << "No orders in the system" << endl;
        return;
    } 
    float priority = findNode(root_, orderId)->priority;
    int eta = findNode(root_, orderId)->ETA;
    int upcomingOrder = findIOP(findNode(root_, orderId))->orderId;
    deleteRecursive(root_, priority, currSysTime);
    etaTree_->removeOrderAfterDelivery(eta, currSysTime);
    cout << "Order " << orderId << " has been cancelled" << endl;
    updateTimeNecessary(upcomingOrder, currSysTime);
}

void Tree::removeOrderAfterDelivery(int orderId, int currSysTime) {
    if (root_ == nullptr) {
        cout << "No orders in the system" << endl;
        return;
    } 
    float priority = findNode(root_, orderId)->priority;
    int eta = findNode(root_, orderId)->ETA;
    deleteRecursive(root_, priority, currSysTime);
    etaTree_->removeOrderAfterDelivery(eta, currSysTime);
}

void Tree::deleteRecursive(TreeNode* &node, float priority, int currSysTime) {
    if (node == nullptr) {
        // cout << "order not found" << endl;
        return;
    }
    else if (node->priority == priority) {
        deleteNode(node);
        return;
    } else if (node->priority <= priority) {
        deleteRecursive(node->right, priority, currSysTime);
        ensureBalance(node);
    } else {
        deleteRecursive(node->left, priority, currSysTime);
        ensureBalance(node);
    }
}

void Tree::deleteNode(TreeNode* &node) {
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

void Tree::iopRemove(TreeNode* &node) { // calls a recursive function to find and remove the inorder predecssor
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

void Tree::iopRemove(TreeNode* &node, TreeNode* &iop, bool iniCall) { // recursive function to remove the inorder predecessor in cancel order
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

TreeNode* Tree::findIOP(TreeNode* &node) {
    if (node == nullptr) {
        return node;
    }
    if (node->left != nullptr) {
        TreeNode* temp = node->left;
        while (temp->right != nullptr) {
            temp = temp->right;
        }
        return temp;
    } else {
        TreeNode* current = root_;
        TreeNode* pred = nullptr;
        while (current != node) {
            if (node->priority <= current->priority) {
                pred = current; // Potential predecessor
                current = current->left;
            } else {
                current = current->right;
            }
        }
        return pred;
    }
}



// utils

float TreeNode::calculatePriority(int orderValue, int systemTime) {
    float valueWeight = 0.3;
    float timeWeight = 0.7;
    float normalizedOrderVal = orderValue/50;
    return (valueWeight * normalizedOrderVal) - (timeWeight * systemTime);
}

int Tree::getBalanceFactor(TreeNode* &node) {
    if (!node) return 0;
    return getHeight(node->left) - getHeight(node->right);
}

void Tree::updateHeight(TreeNode* &node) {
    if (!node) return;
    node->height = 1 + max(getHeight(node->left), getHeight(node->right));
}

int Tree::getHeight(TreeNode* &node) {
    if(!node) return -1;
    else {
        return node->height;
    }
}

void Tree::printRoot() {
    if (root_) {
        cout << root_->orderId << endl;
    } else {
        cout << "val is null" << endl;
    }
}

TreeNode*& Tree::swapNodes(TreeNode* &node1, TreeNode* &node2) {
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

TreeNode*& Tree::findNode(TreeNode* &node, int orderId) {
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

TreeNode*& Tree::findNodeByETA(TreeNode* &node, int eta) {
    if (node == nullptr) {
        return node;
    } 
    if (node->ETA == eta) {
        return node;
    }
    TreeNode*& leftResult = findNodeByETA(node->left, eta);
    if (leftResult != nullptr) {
        return leftResult;
    }
    return findNodeByETA(node->right, eta);
}

void Tree::checkDelivery(int currSysTime) {
    if (!findNodeByETA(root_, currSysTime)) {
        return;
    } else {
        int orderId = findNodeByETA(root_, currSysTime)->orderId;
        removeOrderAfterDelivery(orderId, currSysTime);
        cout << "Order " << orderId << " has been delivered at time " << currSysTime << endl;
    }
}


// functions required to implement

void Tree::print(int orderId) { // change priority to eta
    TreeNode*& temp = findNode(root_, orderId);
    if (temp == nullptr) {
        cout << "Order not found" << endl;
    } else {
        cout << "[" << temp->orderId << ", " << temp->currSysTime << ", " << temp->orderValue << ", " << temp->deliveryTime << ", " << temp->ETA << "]" << endl;
    }
}

void Tree::print(int time1, int time2) {
    printInRange(root_, time1, time2);
}

void Tree::printInRange(TreeNode* &node, int time1, int time2) {
    if (node == nullptr) {
        return;
    }
    if (node->ETA < time1) {
        printInRange(node->right, time1, time2);
        return;
    }
    if (time1 <= node->ETA && node->ETA <= time2) {
        cout << node->orderId << ", ";
    }
    printInRange(node->right, time1, time2);
}
 
void Tree::getRankOfOrder(int orderId) {
    if (findNode(root_, orderId) == nullptr) {
        cout << "Order " << orderId << " will be delivered after -1 orders" << endl;
    } else {
        cout << "Order " << orderId << " will be delivered after " << getRankCount(root_, findNode(root_, orderId)->priority) << " orders" << endl;
    }
}

int Tree::getRankCount(TreeNode* &node, float priority) {
    if (node == nullptr) {
        return 0; // No elements if node is null
    }
    int count = 0;
    count += getRankCount(node->right, priority);
    if (node->priority > priority) {
        count++;
    } else {
        return count;
    }
    count += getRankCount(node->left, priority);

    return count;
}

// updating etas    

void Tree::updateTimeNecessary(int orderId, int currSysTime) {
    vector<int> lessPriorityOrders;
    findAllLessPriority(root_, findNode(root_, orderId)->priority, lessPriorityOrders);
    int etaForNewNode = etaTree_->findNodeETA(orderId);
    int prevOrder = orderId;
    for (int i=0; i<lessPriorityOrders.size(); i++) {
        int cst = findNode(root_, lessPriorityOrders[i])->currSysTime;
        int orderValue = findNode(root_, lessPriorityOrders[i])->orderValue;
        int deliveryTime = findNode(root_, lessPriorityOrders[i])->deliveryTime;
        int eta = etaTree_->findNodeETA(prevOrder) + deliveryTime;
        etaTree_->cancelOrder(lessPriorityOrders[i], currSysTime);
        etaTree_->createOrderInETA(lessPriorityOrders[i], cst, orderValue, deliveryTime, eta);
        findNode(root_, lessPriorityOrders[i])->ETA = eta;
        prevOrder = lessPriorityOrders[i];
        cout << "Updated ETAs: [" << lessPriorityOrders[i] << ":" << eta << "] ";
    }
    if (lessPriorityOrders.size() > 0) {
        cout << endl;
    }
}

void Tree::updateTime(int orderId, int currSysTime, int newDeliveryTime) {
    int oldDeliveryTime = findNode(root_, orderId)->deliveryTime;
    findNode(root_, orderId)->deliveryTime = newDeliveryTime;
    int newETA = findNode(root_, orderId)->ETA - oldDeliveryTime + newDeliveryTime;
    findNode(root_, orderId)->ETA = newETA;
    etaTree_->findNode(root_, orderId)->deliveryTime = newDeliveryTime;
    etaTree_->findNode(root_, orderId)->ETA = newETA;
    updateTimeNecessary(orderId, currSysTime);
}

void Tree::findAllLessPriority(TreeNode* &node, float priority, vector<int>& lessPriority) {
    if (node == nullptr) {
        return;
    }
    findAllLessPriority(node->left, priority, lessPriority);
    if (node->priority < priority) {
        lessPriority.push_back(node->orderId);
    } else {
        return;
    }
}






