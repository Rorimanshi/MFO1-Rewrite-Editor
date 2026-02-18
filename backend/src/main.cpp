#include"Server.hpp"

#include<iostream>

int main(){
    httpserv::Server server;

    if(server.getLastErrorMsg() != ""){
        return 1;
    }

    if(!server.Listen(27015)){
        std::cout << server.getLastErrorMsg() << std::endl;
        return 1;
    }

    server.Post("/h", [&server](){
        server.res.SetCors("127.0.0.1:5000");
        server.res.SetContent("hhh", "text/plain");
        server.res.SetStatus(200);
    });

    server.Post("/save", [](){
        //todo
    });

    while(1){
        if(!server.Accept()){
            std::cout << server.getLastErrorMsg() << std::endl;
        }
    }
}