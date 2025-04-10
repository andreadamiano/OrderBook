#ifndef ENUMS_H
#define ENUMS_H

enum class SIDE {BUY, SELL}; 

inline std::ostream& operator << (std::ostream& os , const SIDE& side)
{
    if (side == SIDE::BUY)
        os << "BUY"; 
    
    else if (side == SIDE::SELL)
        os << "SELL"; 
    
    return os; 
}

enum class TYPE {LIMIT, MARKET, STOP_LIMIT, STOP_MARKET}; 

inline std::ostream& operator << (std::ostream& os , const TYPE& side)
{
    if (side == TYPE::MARKET)
        os << "MARKET"; 
    
    else if (side == TYPE::LIMIT)
        os << "LIMIT"; 

    else if (side == TYPE::STOP_MARKET)
        os << "STOP_MARKET"; 
    
    else if (side == TYPE::STOP_LIMIT)
        os << "STOP_LIMIT"; 
    
    return os; 
}

#endif