#include "Order.h"
#include <iostream>

using namespace std; 

//FIFO insertion 
void IntrusiveList::insert(Order* order)
{
    if ((order->prev != nullptr || order->next != nullptr) && (order != head))
        throw runtime_error("Error, the order is already inside a list"); 

    if (!head) 
    {
        head = tail = order; 
        order->next = order->prev = nullptr; 
    }
    else
    {
        tail->next = order; 
        order->prev = tail; 
        order->next = nullptr; 
        tail = order; 
    }

    ++size; 
    
}

void IntrusiveList::remove(Order* order)
{
    if (order->prev == nullptr && order->next == nullptr && head != order)
        throw runtime_error("Error, order not inside a list"); 

    if (order->prev) //if previous order exist
        order->prev->next = order->next; 
    else 
        head = order->next; 

    if (order->next) //if next order exist 
        order->next->prev = order->prev; 
    else    
        tail = order->prev; 
    
    order->prev = order->next = nullptr; //isolate the removed node 
    delete order; 
    --size; 
}


void Order::addFill(const uint64_t _filleQuantity)
{     

    if (_filleQuantity > (quantity- filledQuantity))
        throw std::runtime_error("Filled quantity exceeds remaining quantity"); 

    filledQuantity += _filleQuantity;
}