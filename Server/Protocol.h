#include <cstdint>
#include <chrono>
#include <string>
#include <cstring>
#include "Enums.h"

#pragma pack(push, 1)
struct OrderPacket 
{
    uint64_t orderCount;
    uint64_t orderId; 
    char symbol[16]; 
    SIDE side;
    TYPE type;
    double price;
    double stopPrice;
    double quantity;
    uint64_t timestamp; //millisecond since epoch 

    OrderPacket(uint64_t _orderCount, const char* _symbol, SIDE _side, TYPE _type, double _price, double _stopPrice, double _quantity)
    : orderCount(_orderCount), side(_side), type(_type), price(_price), stopPrice(_stopPrice), quantity(_quantity), timestamp(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count()) 
    {
        strncpy(symbol, _symbol, sizeof(_symbol)-1); 
        symbol[sizeof(symbol)-1] = '\0';    //add null termination

        orderId = generateOrderId(); 
    }

    OrderPacket() = default;  

    uint64_t generateOrderId()
    {
        return (timestamp << 24) | (orderId & 0xFFFFFF);
    }

};
#pragma pack(pop)