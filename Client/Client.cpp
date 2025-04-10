    #include <winsock2.h>
    #include <iostream>
    #include <chrono>
    #include <thread>
    #include "Protocol.h"
    #include <fstream>
    #include <sstream>
    #include <atomic>

    int main ()
    {
        system("cls");

        //initialize WSA variables
        WSADATA wsadata; 
        int error; 
        WORD socketVersion = MAKEWORD(2,2); 
        error = WSAStartup(socketVersion, &wsadata);

        if (error != 0) 
        {
            std::cout << "WSAStartup failed: " << WSAGetLastError() << std::endl;
            return 1;  
        } else 
        {
            std::cout << "WSAStartup succeeded!" << std::endl;
        }

        //create socket
        SOCKET clientSocket; 
        clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP ); 

        if (clientSocket == INVALID_SOCKET) 
        {
            std::cout << "Socket not successfully created" << "\n"; 
        }
        else
        {
            std::cout << "Socket successfully created" << "\n"; 
        }
        
        //bind socket to IP adddress 
        sockaddr_in sockAddress; //instance containing socket adddress info 
        sockAddress.sin_family = AF_INET; 
        sockAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); //IP address
        sockAddress.sin_port = htons(55555); //port number

        //connect to the server
        if(connect(clientSocket, (SOCKADDR* ) &sockAddress, sizeof(sockAddress)) == SOCKET_ERROR)
        {
            std::cout << "connection failed: " << WSAGetLastError() << std::endl;
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }
        else
        { 
            std::cout << "connection successful, client can start send and receive data..." << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::seconds(2)); 
        system("cls");

        std::ifstream file("order.txt"); //open file 
        std::string line;

        char symbol[16]; 
        SIDE side;
        TYPE type;
        double price;
        double stopPrice;
        double quantity;

        std::atomic<int> orderCount; 

        //send data
        if(file)
        {
            std::cout << "file opened\n"; 

            while (std::getline(file, line))
            {
                ++orderCount;



                std::stringstream order(line); 
                std::string token; 

                //symbol 
                if(std::getline(order, token, ','))
                {
                    strncpy(symbol, token.c_str(), sizeof(symbol) - 1);
                    symbol[sizeof(symbol) - 1] = '\0';
                    std::cout << "symbol: " << symbol << "\n"; 
                }

                //side
                if(std::getline(order, token, ','))
                {
                    std::cout << "token side" << token << "\n"; 
                    if (token == "SELL" || token == "sell")
                    {
                        side = SIDE::SELL; 
                        std::cout << "side: SELL" << "\n"; 
                    }
                    else if(token == "BUY" || token == "buy") 
                    {  
                        side = SIDE::BUY; 
                        std::cout << "side: BUY" << "\n"; 
                    }
                }

                //type  
                if (std::getline(order, token, ','))
                {
                    if (token == "LIMIT" || token == "limit") 
                    {
                        type = TYPE::LIMIT;
                        std::cout << "type: LIMIT\n"; 
                    }
                    else if (token == "MARKET" || token == "market") 
                    {
                        type = TYPE::MARKET;
                        std::cout << "type: MARKET\n"; 
                    }
                    else if (token == "STOP_LIMIT" || token == "stop_limit" || token == "stop limit" || token == "STOP LIMIT") 
                    {
                        type = TYPE::STOP_LIMIT;
                        std::cout << "type: STOP_LIMIT\n"; 
                    }
                    else if (token == "STOP_MARKET" || token == "stop_market" || token == "stop market" || token == "STOP MARKET") 
                    {
                        type = TYPE::STOP_MARKET;
                        std::cout << "type: STOP_MARKET\n"; 
                    }
                }

                //price
                if (std::getline(order, token, ','))
                {
                    price = stod(token);  
                    std::cout << "price: " << price << "\n"; 
                }

                //stop price
                if (std::getline(order, token, ','))
                {
                    stopPrice = stod(token);
                    std::cout << "stop price: " << stopPrice << "\n";
                }

                //quantity
                if (std::getline(order, token, ','))
                {
                    quantity = stod(token); 
                    std::cout << "quantity: " << quantity << "\n";
                }

                //construct packet 
                OrderPacket packet(
                    orderCount,
                    symbol, 
                    side, 
                    type, 
                    price, 
                    stopPrice, 
                    quantity
                );

                std::cout << "timestamp: " << packet.timestamp << "\n"; 

                int byteCount = send(clientSocket, reinterpret_cast<const char*>(&packet), sizeof(packet), 0); //send a fied size data packet 
                
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }

            file.close(); 

            // char buffer [200]; 
            // char timeStr[100];
            // std::cout << "New Order (symbol, side, type, price, stopPrice, quantity): " << "\n"; 
            // std::cin.getline(buffer, 200); 

            // if (strcmp(buffer, "end") == 0)
            // {
            //     int byteCount = send(clientSocket, buffer, 200, 0);
            //     // Cleanup
            //     closesocket(clientSocket);
            //     WSACleanup();
            //     return 0;   
            // }
                    
            // int byteCount = send(clientSocket, buffer, 200, 0); 
        }
        else
        {
            std::cout << "error, file not opened\n";  
        }

        // Cleanup
            closesocket(clientSocket);
            WSACleanup();
            return 0; 

    }