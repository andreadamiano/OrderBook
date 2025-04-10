#include "PriceLevel.h"


void PriceLevel::addOrder(Order& order)
{
    if (order.getPrice()!=levelPrice)
    {
        throw runtime_error("Error price doesnt match price level"); 
    }

        levelOrders.insert(&order); 
        totalQuantity += order.getQuantity(); 
        ++orderCount; 
    
}

Order* PriceLevel::removeOrder(Order& order)
{
    levelOrders.remove(&order); 
    totalQuantity -= (order.getQuantity() - order.getFilledQuantity()); //remove remaining unfilled quantity
    --orderCount; 

    return &order; 
}