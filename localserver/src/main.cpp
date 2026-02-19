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
        server.res.SetHeader("Access-Control-Allow-Origin", "http://localhost:5000");
        server.res.SetContent("hhh", "text/plain");
        server.res.SetStatus(200);
    });

    server.Post("/save", [&server](){
        if(filesys::FileSystem::Save(server.req.GetBody())){
            server.res.SetHeader("Access-Control-Allow-Origin", "http://localhost:5000");
            server.res.SetContent("file saved succesfuly!", "text/plain");
            server.res.SetStatus(200);
        }
        else{
            server.res.SetHeader("Access-Control-Allow-Origin", "http://localhost:5000");
            server.res.SetContent("failed to save the file!", "text/plain");
            server.res.SetStatus(500);
        }
    });

    while(1){
        if(!server.Accept()){
            std::cout << server.getLastErrorMsg() << std::endl;
        }
    }
}