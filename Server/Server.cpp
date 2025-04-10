#include <iostream>
#include <winsock2.h>  // Use winsock2.h, not winsock.h
#include <sstream>
#include "OrderBook.h"



int main() 
{
    system("cls");

    // Initialize Winsock
    WSADATA wsadata;
    int error;
    WORD socketVersion = MAKEWORD(2, 2);  //request winsock 2.2

    error = WSAStartup(socketVersion, &wsadata);

    if (error != 0) 
    {
        std::cout << "WSAStartup failed: " << WSAGetLastError() << std::endl;
        return 1;  
    } 
    else 
    {
        std::cout << "WSAStartup succeeded!" << std::endl;
    }

    //create socket
    SOCKET serverSocket; 
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP ); 

    if (serverSocket == INVALID_SOCKET) 
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

    if (bind(serverSocket, (SOCKADDR*)&sockAddress, sizeof(sockAddress)) == SOCKET_ERROR)
    {
        std::cout << "Bind failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    else
    { 
        std::cout << "Successful bind" << std::endl;
    }

    //listen to connection 
    if (listen(serverSocket, 1) == SOCKET_ERROR)
    {
        std::cout << "error, not listening" << "\n"; 
    }
    else
    {
        std::cout << "listening..." << "\n"; 
    }

    //accept connection
    SOCKET acceptSocket; 
    acceptSocket = accept(serverSocket, nullptr, nullptr); 

    if(acceptSocket == INVALID_SOCKET)
    {
        std::cout << "accept failed" << WSAGetLastError() << "\n";
        WSACleanup();
        return -1; 
    }
    else
    {
        std::cout << "connection accepted" << "\n"; 
    }

    std::this_thread::sleep_for(std::chrono::seconds(2)); 
    system("cls");

    //initialize order book
    OrderBook book("AAPL"); 
    book.start(); //start matching orders and print on the console 

    //receive data 
    while(true)
    {
        OrderPacket receivedPacket;
        int byteCount = recv(acceptSocket, reinterpret_cast<char*>(&receivedPacket), sizeof(receivedPacket), 0);
        short y(0); 

        if (byteCount > 0)
        {
            // std::stringstream buffer; 

            // // Reset cursor to top right position
            // if (y ==20)
            //     y=0; 
            // COORD coord = {25, y++};
            // SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);

            // buffer << "packet: "; 
            // buffer << "orderID: " << receivedPacket.orderId << " ";
            // buffer << "symbol: " << receivedPacket.symbol << " ";
            // buffer << "price: " << receivedPacket.price << " ";
            // buffer << "side: " << receivedPacket.side << " ";
            // buffer << "type: " << receivedPacket.type << " ";
            // buffer << "stopPrice: " << receivedPacket.stopPrice << " ";
            // buffer << "quantity: " << receivedPacket.quantity << " ";

            // //convert stringstream into a string
            // std::string output = buffer.str();

            // //print 
            // DWORD written;
            // WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), output.c_str(), output.size(), &written, NULL);

            //add order to orderbook
            Order* order = new Order(receivedPacket); 
            book.addOrder(*order); 

            // //print order book
            // book.printOrderBook(); 

        }




    }

    // Cleanup
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}

