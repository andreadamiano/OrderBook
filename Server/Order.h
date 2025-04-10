#ifndef ORDER_H
#define ORDER_H

#include <cstdint>
#include <string>
#include <chrono>
#include "Enums.h"
#include "Protocol.h"


using namespace std; 

class IntrusiveList; 

//individual trading orders 
class Order
{
    friend class IntrusiveList; 

    private: 
        uint64_t orderID; 
        string symbol; 
        SIDE side; 
        TYPE type; 
        double price; //represent the price as integer to avoid floating point errors 
        double stopPrice; 
        double quantity; 
        double filledQuantity; 
        uint64_t timeStamp;

        //Itrusive pointers 
        Order* next; 
        Order* prev; 

    public:

        //order type checking
        bool isLimit() const {return type == TYPE::LIMIT || type == TYPE::STOP_LIMIT; }
        bool isMarket() const  {return type == TYPE::MARKET || type == TYPE::STOP_MARKET; }
        bool isStop() const {return type == TYPE::STOP_MARKET  || type == TYPE::STOP_LIMIT; }

        //constructor
        Order() = default; 
        Order(const OrderPacket& packet) : orderID(packet.orderId), symbol(packet.symbol), side(packet.side), type(packet.type), price(packet.price), stopPrice(packet.stopPrice), quantity(packet.quantity), filledQuantity(0), timeStamp(packet.timestamp) , prev(nullptr), next(nullptr)
        {}

        //retrieving methods 
        const double getPrice() const {return price; }
        const double getQuantity () const {return  quantity; }
        const uint64_t getOrderID() const {return orderID; }
        const double getFilledQuantity() const {return filledQuantity; }
        const SIDE getSide() const {return side; }
        const string getSymbol() const {return symbol; }

        //setting methods
        void addFill (const uint64_t _filledQuantity); 
        void setQuanity(const double _quantity) {quantity = _quantity; }


}; 


class IntrusiveList
{
    private: 
        Order* head; 
        Order* tail; 
        size_t size; //sie of the datastructure 

    public:
        class Iterator 
        {
            private: 
                Order* order; 

            public:
                Iterator(Order* _order) : order(_order) {}

                Iterator& operator++ () {order = order->next;  return *this; }
                Iterator& operator-- () {order = order->prev; return *this; }
                bool operator== (const Iterator& other ) const {return order == other.order;}
                bool operator!= (const Iterator& other) const {return order != other.order; }
                Order& operator* () const {return *order; }
                Order* operator->() const {return order; }
        }; 

        void insert(Order* order); 
        void remove(Order* order); 
        Order* front() const {return head; } //FIFO access
        bool empty() const {return size == 0; }
        size_t Size() const {return size; }

        Iterator begin() {return Iterator(head); }
        Iterator end() {return Iterator(nullptr);}

        IntrusiveList() : head(nullptr), tail(nullptr), size(0) {}
        IntrusiveList(Order* order): head(order) , tail(order), size(0) {}
}; 


#endif