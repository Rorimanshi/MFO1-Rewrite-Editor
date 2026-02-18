#ifndef _HTTPSERVER
#define _HTTPSERVER

#include<iostream>
#include<cctype>
#include<algorithm>

#include"Server.hpp"

using namespace httpserv;

void Request::Parse(const char* req){
    if(!req) return;

    const std::string strReq(req);
    fullRequest = strReq;

    size_t methodEnd = fullRequest.find(' ');
    if(methodEnd != std::string::npos){
        method = fullRequest.substr(0, methodEnd);

        size_t pathStart = methodEnd + 1;
        size_t pathEnd = fullRequest.find(' ', pathStart);
        if(pathEnd != std::string::npos){
            path = fullRequest.substr(pathStart, pathEnd - pathStart);
        }
        else{
            path = fullRequest.substr(pathStart);
        }
    }
    else{
        method = fullRequest;
    }

    std::cout << "method: " << method << "\npath: " << path << std::endl;
}



std::string Response::GetFullResponse(){
    const size_t bodyLen = body.length();
    const std::string fullRes = 
        "HTTP/1.1 " + std::to_string(status) + "\r\n" +
        "Content-Type: " + type + "\r\n" +
        "Access-Control-Allow-Origin: " + cors + "\r\n" +
        "Content-Length: " + std::to_string(bodyLen) + "\r\n" +
        "Connection: close\r\n" +
        "\r\n" +
        body;

    return fullRes;
}

void Response::SetCors(const std::string& _cors){
    cors = _cors;
}
void Response::SetStatus(const uint16_t _status){
    status = _status;
}
void Response::SetContent(const std::string& _body, const std::string& _type){
    type = _type;
    body = _body;
}



Server::Server(){
    InitSocketAPI();
}

bool Server::Listen(const u_short port, const char* addr){
    mainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(mainSocket == INVALID_SOCKET){
        lastErrorMsg = "Failed to create a socket!";
        std::cout << lastErrorMsg;
        return false;
    }
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr(addr);
    service.sin_port = htons(port);

    if(bind(mainSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR){
        lastErrorMsg = "Socket binding failed!";
        std::cout << lastErrorMsg;
        return false;
    }

    if(listen(mainSocket, 1) == SOCKET_ERROR){
        lastErrorMsg = "Error listening on socket: " + std::to_string(mainSocket) + "!";
        std::cout << lastErrorMsg;
        return false;
    }

    return true;
}

bool Server::Accept(){
    if(clientSocket == SOCKET_ERROR){
        std::cout << "Listening for connection..." << std::endl;
        while(clientSocket == SOCKET_ERROR){
            clientSocket = accept(mainSocket, NULL, NULL);
        }
        std::cout << "Client connected" << std::endl;
        if(!HandleConnection()){
            return false;
        }
        closesocket(clientSocket);
        clientSocket = SOCKET_ERROR;
        std::cout << "Connection closed" << std::endl;
    }
    return true;
}

bool Server::Send(){
    const std::string fullRes = res.GetFullResponse();
    const int bytesSent = send(clientSocket, fullRes.c_str(), fullRes.length(), 0);

    if(bytesSent == SOCKET_ERROR){
        lastErrorMsg = "Failed to send data to the client";
        std::cout << lastErrorMsg;
        return false;
    }

    std::cout << "Text sent:" << std::endl;
    std::cout << "---------------------\n" << fullRes << "\n---------------------" << std::endl;

    return true;
}

void Server::Cleanup(){
    closesocket(mainSocket);
    closesocket(clientSocket);
    WSACleanup();
}

bool Server::InitSocketAPI(){
    int res = WSAStartup(MAKEWORD(2,2), &wsaData);
    if(res != NO_ERROR){
        lastErrorMsg = "Initialization error!";
        std::cout << lastErrorMsg;
        return false;
    }
    return true;
}

bool Server::HandleConnection(){
    int bytesRecv;
    char bufferRecv[4096] = {0};
    bytesRecv = recv(clientSocket, bufferRecv, sizeof(bufferRecv) - 1, 0);
    if(bytesRecv > 0){
        std::cout << "Bytes recieved: " << bytesRecv << std::endl;
        std::cout << "Text recieved: \n{\n" << bufferRecv << "\n}" << std::endl;
        bufferRecv[bytesRecv] = '\0';
        req.Parse(bufferRecv);
        HandleRequest();
        if(!Send()){
            return false;
        }
    }
    return true;
}

void Server::HandleRequest(){
    if(req.GetMethod() == "POST"){
        std::cout << "--d--d-d-d-d-dd--d-dd--dd---d\n";
        auto it = postHandlers.find(req.GetPath());
        if(it != postHandlers.end()){
            it->second();
        }
    }
}

const char* Server::BuildResponse(
    const char* body,
    const char* status,
    const char* type,
    const char* cors
) {
    
    memset(resBuffer, 0, sizeof(resBuffer));
    snprintf(resBuffer, sizeof(resBuffer),
        "HTTP/1.1 %s\r\n"
        "Content-Type: %s\r\n"
        "Access-Control-Allow-Origin: %s\r\n"
        "Content-Length: %d"
        "\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        status,
        type,
        cors,
        strlen(body),
        body
    );
    return resBuffer;
}

void Server::Post(const std::string& path, std::function<void()> handler){
    postHandlers[path] = handler;
}

#include<fstream>
#include"sys/stat.h"

using namespace filesys;
bool FileSystem::Save(const std::string& jsonContent){
    const std::string targetDir = "../../editor";
    const std::string targetFile = targetDir + "/savefile.json";

    DWORD attrib = GetFileAttributesA(targetDir.c_str());
    if(attrib == INVALID_FILE_ATTRIBUTES){
        std::cout << "Directory '" << targetDir << " does not exist!" << std::endl;
        return false;
    }
    if(!(attrib & FILE_ATTRIBUTE_DIRECTORY)){
        std::cout << targetDir << " is not a directory!" << std::endl;
        return false;
    }

    std::ofstream outFile(targetFile, std::ios::binary);
    if(!outFile.is_open()){
        std::cout << "Could not open/create " << targetFile << " for writting." << std::endl;
        return false;
    }
    outFile.write(jsonContent.c_str(), jsonContent.size());
    outFile.close();

    if(outFile.fail()){
        std::cout << "Failed to write JSON content to file";
        return false;
    }

    std::cout << "JSON file succesfully saved to: " << targetFile << std::endl;
    return true;
}

#endif