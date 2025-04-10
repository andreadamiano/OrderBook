#ifndef PRICELEVEL_H
#define PRICELEVEL_H

#include <cstdint>
#include <list>
#include <memory>
#include "Order.h"

using namespace std; 

class PriceLevel
{
    private:
        double levelPrice; 
        IntrusiveList levelOrders; 
        double totalQuantity; 
        int64_t orderCount;     

    public:
        // PriceLevel(double _levelPrice , Order* order ) : levelPrice(_levelPrice), levelOrders(IntrusiveList(order)) , totalQuantity(0), orderCount(0) {}
        PriceLevel(double price) : levelPrice(price), totalQuantity(0), orderCount(0) {}

        void addOrder(Order& order);
        Order* removeOrder(Order& order); 

        double getQuantity () const {return totalQuantity; }
        double getCount() const {return orderCount; }
        Order* front() const {return levelOrders.front(); }
        bool empty() const {return levelOrders.empty(); }

        
        void setQuantity(const double _quantity) {totalQuantity = _quantity; }
        void setCount(const double _count) {orderCount = _count; }
}; 

#endif