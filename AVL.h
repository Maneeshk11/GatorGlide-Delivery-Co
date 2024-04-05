#pragma once

#include <bits/stdc++.h>
using namespace std;


class TreeNode {
    public:
        int orderId;
        int orderValue;
        int deliveryTime;
        int currSysTime;
        int ETA;
        float priority;
        TreeNode* left;
        TreeNode* right;
        int height;
        TreeNode() : left(nullptr), right(nullptr), height(0) {}
        TreeNode(int orderId, int cst, int ov, int dt) : left(nullptr), right(nullptr), height(0),
                                                        orderId(orderId), currSysTime(cst),
                                                        deliveryTime(dt), orderValue(ov) {
                                                            priority = calculatePriority(orderValue, currSysTime);
                                                        }
        float calculatePriority(int orderValue, int systemTime);
        float calculateETA(int deliveryTime, int systemTime);

};

class ETATree {
    public:
        ETATree() : root_(nullptr), currSysTime(0) {}
        void createOrderInETA(int orderId, int currSysTime, int orderValue, int deliveryTime, int eta);
        void insertRecursive(TreeNode* &node, int orderId, int cst, int ov, int dt, int eta, float priority);
        void ensureBalance(TreeNode* &node);
        int getBalanceFactor(TreeNode* &node);
        int getHeight(TreeNode* &node);
        void updateHeight(TreeNode* &node);

        void cancelOrder(int orderId, int currSysTime);
        void removeOrderAfterDelivery(int orderId, int currSysTime);
        void deleteRecursive(TreeNode* &node, int priority, int currSysTime);
        void deleteRecursiveUpdateETA(TreeNode* &node, int priority, int currSysTime, int orderId);
        void deleteNode(TreeNode* &node);
        void iopRemove(TreeNode* &node);
        void iopRemove(TreeNode* &node, TreeNode* &iop, bool iniCall);

        void rotateLeft(TreeNode* &node);
        void rotateRight(TreeNode* &node);
        void rotateLeftRight(TreeNode* &node);
        void rotateRightLeft(TreeNode* &node);

        TreeNode*& findNode(TreeNode* &node, int orderId);
        int findNodeETA(int orderId);

        TreeNode*& swapNodes(TreeNode* &node1, TreeNode* &node2);
    private:
        TreeNode* root_; 
        int currSysTime;
};

class Tree {
    public:
        Tree() : root_(nullptr), etaTree_(new ETATree())  {}
        void createOrder(int orderId, int currSysTime, int orderValue, int deliveryTime);
        void insertRecursive(TreeNode* &node, int orderId, int cst, int ov, int dt, float priority);
        void ensureBalance(TreeNode* &node);
        int getBalanceFactor(TreeNode* &node);
        int getHeight(TreeNode* &node);
        void updateHeight(TreeNode* &node);

        void cancelOrder(int orderId, int currSysTime);
        void removeOrderAfterDelivery(int orderId, int currSysTime);
        void deleteRecursive(TreeNode* &node, float priority, int currSysTime);
        void deleteNode(TreeNode* &node);
        void iopRemove(TreeNode* &node);
        void iopRemove(TreeNode* &node, TreeNode* &iop, bool iniCall);

        TreeNode* findIOP(TreeNode* &node);

        TreeNode*& swapNodes(TreeNode* &node1, TreeNode* &node2);
        TreeNode*& findNode(TreeNode* &node, int orderId);
        TreeNode*& findNodeByETA(TreeNode* &node, int orderId);

        void rotateLeft(TreeNode* &node);
        void rotateRight(TreeNode* &node);
        void rotateLeftRight(TreeNode* &node);
        void rotateRightLeft(TreeNode* &node);
        void printRoot();

        void print(int orderId);
        void print(int time1, int time2);
        void printInRange(TreeNode* &node, int time1, int time2);

        void checkDelivery(int currSysTime);

        void getRankOfOrder(int orderId);
        int getRankCount(TreeNode* &node, float priority);

        void updateTime(int orderId, int currSysTime, int newDeliveryTime);
        void updateTimeNecessary(int orderId, int currSysTime);
        void findAllLessPriority(TreeNode* &node, float priority, vector<int> &res);


    private:
        TreeNode* root_;
        ETATree* etaTree_;
};




#include "PriorityTree.hpp"
#include "ETATree.hpp"