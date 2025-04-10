#include "OrderBook.h"

Order* OrderBook::bestAsk() const 
{
    if(askLevels.empty())
        return nullptr; 
    
    return askLevels.begin()->second.front(); 
}

Order* OrderBook::bestBid() const 
{
    if(bidLevels.empty())
        return nullptr; 
    
    return bidLevels.begin()->second.front(); 
}

void OrderBook::addOrder(Order& order)
{   
    std::unique_lock<std::mutex> lock(orderMutex);

    if (order.getSymbol() != symbol)
        throw runtime_error("Error, different financial asset"); 

    if (order.getSide() == SIDE::BUY)
    {
        auto [it, inserted] = bidLevels.try_emplace(order.getPrice(), PriceLevel(order.getPrice())); //cretae new pricelevel if the order level doesnt exist 
        it->second.addOrder(order); 
    }
    else if (order.getSide() == SIDE::SELL)
    {
        auto [it, inserted] = askLevels.try_emplace(order.getPrice(), PriceLevel(order.getPrice())); 
        it->second.addOrder(order); 
    }
    
    lock.unlock(); 
    if(!isPrinting)
        printOrderBook(); 
    newOrder = true; 
    newOrderCV.notify_one(); 
}

void OrderBook::removeOrder(Order& order, SIDE side)
{

    if (side == SIDE::BUY)
    {
        auto& bidLevel = bidLevels.at(order.getPrice());
        double price = order.getPrice(); 
        bidLevel.removeOrder(order); 

        if (bidLevel.empty())
            bidLevels.erase(price); 
    }   
    else 
    {
        auto& askLevel = askLevels.at(order.getPrice());
        double price = order.getPrice(); 
        askLevel.removeOrder(order);  //order will be deleted from memory

        if (askLevel.empty())
            askLevels.erase(price); 
    }
}


void OrderBook::start()
{
    if(isRunning == true)
        return; 
    
    isRunning = true; 
    matchingThread = thread(&OrderBook::matchOrders, this); //new thread will start matching the orders 

    int fps = 40; 
    printingThread = thread(&OrderBook::print, this, fps); 

}


void OrderBook::endMatching() 
{
    if(isRunning == false)
        return; 

    isRunning = false; 
    newOrderCV.notify_one(); 
    
    if (matchingThread.joinable()) 
        matchingThread.join();
    
    if (printingThread.joinable())
        printingThread.join(); 

}

void OrderBook::matchOrders()
{ 
    while (isRunning)
    { 
        {
            std::unique_lock<std::mutex> lock(orderMutex); 

            newOrderCV.wait(lock, [this] {
                return !isRunning || (!bidLevels.empty() && !askLevels.empty());
            });

            if (!isRunning)
                break;           
            
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            
            Order* bid = bestBid(); 
            Order* ask = bestAsk(); 

            auto& askLevel = askLevels.at(ask->getPrice()); 
            auto& bidLevel = bidLevels.at(bid->getPrice()); 

            if (!bidLevels.empty() && !askLevels.empty() && bid->getPrice() >= ask->getPrice())   
            {
                match = true; 
                double quantity = std::min(bid->getQuantity(), ask->getQuantity());
                
                //update order quantity
                bid->setQuanity(bid->getQuantity()-quantity); 
                ask->setQuanity(ask->getQuantity()-quantity); 

                //update level quantity
                askLevel.setQuantity(askLevel.getQuantity()-quantity); 
                bidLevel.setQuantity(bidLevel.getQuantity()-quantity); 


                if (ask->getQuantity() ==0)
                {
                    removeOrder(*ask, SIDE::SELL); 
                }
                
                if (bid->getQuantity() ==0)
                {
                    removeOrder(*bid, SIDE::BUY); 
                }
            } 
        } 

        // std::this_thread::sleep_for(std::chrono::milliseconds()); 
      

    }
}


void OrderBook::print(int fps)
{
    while (isRunning)
    {
        if(!isPrinting)
        {
            printOrderBook(); 
            std::this_thread::sleep_for(std::chrono::milliseconds(1000 / fps));
        }
    }
}


void OrderBook::printOrderBook()  
{
    isPrinting = true; 
    //prefetch ladders 
    constexpr int levels = 10; //max level to be printed for each ladder 
    std::array<std::pair<double , double >, levels> askLadder; 
    std::array<std::pair<double , double >, levels> bidLadder; 
    int askCount (0); 
    int bidCount(0); 
    bool matchingHappened; 

    std::unique_lock<std::mutex> lock(orderMutex);

    //copy data to be printed 
    for (const auto& [price, level] : askLevels )
    {
        if (askCount ==levels)
            break;
        askLadder[askCount++] = {price, level.getQuantity()}; 
    }

    for (const auto& [price, level] : bidLevels )
    {
        if (bidCount ==levels)
            break;
        bidLadder[bidCount++] = {price, level.getQuantity()}; 
    }

    lock.unlock(); 


    //pre render text in memory
    std::stringstream buffer;
    
    buffer <<  "\033[?25l" << "ORDER BOOK " << symbol << "\n"; //hide cursor 
    buffer << "ASK LADDER                                                    \n";

    buffer << "\033[31m"; 
    for (int i=levels -1 ; i>=0; --i)
    {
        if(i == 0 && askLadder[0]<= bidLadder[0])
            buffer << "\033[33m" << askLadder[i].first << " liquidity: " << askLadder[i].second  << "                         \n\033[31m"; 
        else
            buffer << askLadder[i].first << " liquidity: " << askLadder[i].second  << "                         \n"; 
    }
    buffer << "\033[0m"; 

    buffer << "BUY LADDER                                                       \n";
    buffer << "\033[32m"; 
    for (int i =0; i< levels; ++i)
    {
        if (i==0 && askLadder[0]<= bidLadder[0])
            buffer << "\033[33m" << bidLadder[i].first << " liquidity: " << bidLadder[i].second << "                                   \n\033[32m"; 
        else
            buffer << bidLadder[i].first << " liquidity: " << bidLadder[i].second << "                                   \n"; 
    }
    buffer << "\033[0m"; 

    //convert stringstream into a string
    std::string output = buffer.str();

    //print 
    DWORD written;
    WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), output.c_str(), output.size(), &written, NULL);

    // Reset cursor to top left position
    COORD coord = {0, 0};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    isPrinting = false; 

    // Print match message
    // if(match)
    // {
    //     coord = {40, 0};
    //     SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    //     std::string msg = "MATCHING OCCURRED";
    //     written;
    //     WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), msg.c_str(), msg.size(), &written, NULL);
        
    //     // Briefly show the message
    //     // std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        
    //     // Clear the message
    //     std::string clear(msg.size(), ' ');
    //     SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    //     WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), clear.c_str(), clear.size(), &written, NULL);

    //     //reset cursor
    //     coord = {0, 0};
    //     SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);

    //     match = false; 
    // }
    

}