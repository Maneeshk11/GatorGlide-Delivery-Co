#include "AVL.h"
#include <bits/stdc++.h>
#include <thread>
using namespace std;


class OrderObj {
    public:
        int orderId;
        int currSysTime;
        int orderValue;
        int deliveryTime;
};

class printInterval {
    public:
        int start;
        int end;
};

class updateInterval {
    public:
        int orderId;
        int currSysTime;
        int newDeliveryTime;
};

Tree t;
mutex mtx;
unordered_map<int, OrderObj> orders;
unordered_map<int, OrderObj> rankOfOrders;
unordered_map<int, OrderObj> cancelOrders;
unordered_map<int, printInterval> printIntervals;
unordered_map<int, updateInterval> updateIntervals;

void processInputFile(const string& filename, Tree& t) {
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        throw std::runtime_error("Error opening file: " + filename);
    }

    string line;
    int timer = 0;
    while (getline(inFile, line)) {
        line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());
        if (line.empty() || line[0] == '/') {
            continue;
        }
        size_t openParenPos = line.find('(');
        size_t closeParenPos = line.find(')');
        if (openParenPos == string::npos || closeParenPos == string::npos || closeParenPos < openParenPos) {
            cerr << "Error: Invalid command format: " << line << std::endl;
            continue;
        }
        string command = line.substr(0, openParenPos);
        string args = line.substr(openParenPos + 1, closeParenPos - openParenPos - 1);
        istringstream argStream(args);
        vector<string> argTokens;
        string arg;
        while (getline(argStream, arg, ',')) {
            argTokens.push_back(arg);
        }
        
        if (command == "createOrder") {
            if (argTokens.size() == 4) {
                int id = stoi(argTokens[0]);
                int currSysTime = stoi(argTokens[1]);
                int orderValue = stoi(argTokens[2]);
                int deliveryTime = stoi(argTokens[3]);
                orders[currSysTime] = {id, currSysTime, orderValue, deliveryTime};
                timer = currSysTime;
            } else {
                cerr << "Error: Invalid createOrder arguments: " << args << std::endl;
            }
        } 
        else if (command == "getRankOfOrder") {
            if (argTokens.size() == 1) {
                rankOfOrders[timer] = {stoi(argTokens[0]), 0, 0, 0};
            } else {
                cerr << "Error: Invalid getRankOfOrder argument" << std::endl;
            }
        } else if (command == "print") {
            if (argTokens.size() == 2) {
                printIntervals[timer] = {stoi(argTokens[0]),stoi(argTokens[1])};
            } else {
                cerr << "Error: Invalid print arguments" << std::endl;
            }
        } else if (command == "cancelOrder") {
            if (argTokens.size() == 2) {
                cancelOrders[timer] = {stoi(argTokens[0]),stoi(argTokens[1]), 0, 0};
            } else {
                cerr << "Error: Invalid print arguments" << std::endl;
            }
        } else if (command == "updateTime") {
            if (argTokens.size() == 3) {
                updateIntervals[timer] = {stoi(argTokens[0]),stoi(argTokens[1]), stoi(argTokens[2])};
            } else {
                cerr << "Error: Invalid print arguments" << std::endl;
            }
        }
    }

}


int main() {
    
    try {
        processInputFile("input.txt", t); 
    } catch (const std::exception& e) {
        cerr << "Error: " << e.what() << std::endl;
        return 1;
    }


    thread sleepThread([](){
        int timer = 0;
        while (timer <= 20) {
            mtx.lock();
            if (orders.count(timer) > 0) {
                t.createOrder(orders[timer].orderId, orders[timer].currSysTime, orders[timer].orderValue, orders[timer].deliveryTime);
            }
            if (printIntervals.count(timer) > 0) {
                t.print(printIntervals[timer].start, printIntervals[timer].end);
            }
            if (cancelOrders.count(timer) > 0) {
                t.cancelOrder(cancelOrders[timer].orderId, cancelOrders[timer].currSysTime);
            } 
            if (updateIntervals.count(timer) > 0) {
                t.updateTime(updateIntervals[timer].orderId, updateIntervals[timer].currSysTime, updateIntervals[timer].newDeliveryTime);
            }
            if (rankOfOrders.count(timer) > 0) {
                t.getRankOfOrder(rankOfOrders[timer].orderId);
            }
            t.checkDelivery(timer);
            
            mtx.unlock();
            timer++;
            this_thread::sleep_for(chrono::seconds(1));
        }

    });

        
    sleepThread.join();

}

