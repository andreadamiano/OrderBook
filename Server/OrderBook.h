#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#define _HAS_STD_BYTE 0 
#include <windows.h>  

#include <map>
#include <cstdint>
#include "PriceLevel.h"
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <stack>
#include <vector>


using namespace std; 

class OrderBook
{
    private:
        string symbol; 
        map<double, PriceLevel, less<double>> askLevels; //sorted in ascending order 
        map<double, PriceLevel, greater<double>> bidLevels; //sorted in descending order 

        thread matchingThread; //dedicated thread for matching orders
        thread printingThread;  //dedicated thread for printing order book

        atomic<bool> isRunning; 
        atomic<bool> match; 
        atomic<bool> newOrder; 
        atomic<bool> isPrinting;
        mutex orderMutex; //protect individual orders modification
        condition_variable newOrderCV;
        condition_variable matchCV; 

    public: 
        OrderBook(string _symbol) : isRunning(false), symbol(_symbol) , match(false), newOrder(false), isPrinting(false) {}
        ~OrderBook() { endMatching();  }

        void start(); 
        void endMatching();
        
        void addOrder(Order& order); 
        void removeOrder(Order& order, SIDE side); 
        Order* bestBid() const; 
        Order* bestAsk() const; 

        void print(int fps); 

    protected:
        void matchOrders(); 
        void printOrderBook(); 
    
}; 

#endif