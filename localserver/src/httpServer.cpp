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

            size_t lineEnd = fullRequest.find("\r\n", pathEnd);

            if(lineEnd != std::string::npos){
                size_t headerStart = lineEnd + 2;
                size_t headerEnd = fullRequest.find("\r\n\r\n", headerStart);

                if(headerEnd != std::string::npos){
                    std::string headers = fullRequest.substr(headerStart, headerEnd - headerStart);

                    size_t contentLengthPos = headers.find("Content-Length: ");
                    if(contentLengthPos != std::string::npos){
                        contentLengthPos += 16;
                        size_t contentLenghtEnd = headers.find("\r\n", contentLengthPos);
                        std::string contentLenghtStr = headers.substr(contentLengthPos, contentLenghtEnd - contentLengthPos);
                        int contentLength = std::stoi(contentLenghtStr);

                        if(contentLength > 0){
                            size_t bodyStart = headerEnd + 4;
                            if(bodyStart + contentLength <= fullRequest.length()){
                                body = fullRequest.substr(bodyStart, contentLength);
                            }
                        }
                    }
                }
            }
        }
        else{
            path = fullRequest.substr(pathStart);
        }
    }
    else{
        method = fullRequest;
    }

    std::cout << "method: " << method << "\npath: " << path << "\nbody\n{" << body << "\n}\n" << std::endl;
}

void Response::BuildRes(
    const std::string& cors,
    const std::string& type,
    const std::string& body,
    const uint16_t status
){
    SetCors(cors);
    SetContent(body, type);
    SetStatus(status);
}

std::string Response::GetFullResponse(){
    std::string statusText;
    switch(status) {
        case 200: statusText = "OK"; break;
        case 201: statusText = "Created"; break;
        case 204: statusText = "No Content"; break;
        case 400: statusText = "Bad Request"; break;
        case 404: statusText = "Not Found"; break;
        case 500: statusText = "Internal Server Error"; break;
        default: statusText = "Unknown"; break;
    }

    headers["Content-Length"] = std::to_string(body.length());
    std::string fullRes = "HTTP/1.1 " + std::to_string(status) + " " + statusText + "\r\n";
    for(const auto& [key,val] : headers){
        fullRes += key + ": " + val + "\r\n";
    }
    fullRes += "\r\n" + body;
    return fullRes;
}

void Response::SetHeader(const std::string& name, const std::string& val){
    headers[name] = val;
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
    const std::string& reqMethod = req.GetMethod();
    if(reqMethod == "OPTIONS"){
        res.SetHeader("Access-Control-Allow-Origin", "http://localhost:5000");
        res.SetHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.SetHeader("Access-Control-Allow-Headers", "Content-Type");
        res.SetHeader("Access-Control-Max-Age", "86400");
        res.SetContent("", "text/plain");
        res.SetStatus(204);
    }
    else if(reqMethod == "POST"){
        std::cout << "--d--d-d-d-d-dd--d-dd--dd---d\n";
        auto it = postHandlers.find(req.GetPath());
        if(it != postHandlers.end()){
            it->second();
        }
    }
}

void Server::SendCorsRes(){

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
#include<filesystem>

using namespace filesys;

bool FileSystem::Save(const std::string& jsonContent) {
    char buffer[MAX_PATH];
    if (GetModuleFileNameA(NULL, buffer, MAX_PATH) == 0) {
        std::cout << "Failed to retrieve the executable path." << std::endl;
        return false;
    }
    
    std::filesystem::path exePath = buffer;
    std::filesystem::path exeDir = exePath.parent_path();
    
    std::filesystem::path targetDir = (exeDir / "../../editor").lexically_normal();
    std::filesystem::path targetFile = targetDir / "savefile.json";

    if (!std::filesystem::exists(targetDir)) {
        std::cout << "Directory '" << targetDir.string() << "' does not exist!" << std::endl;
        return false;
    }
    if (!std::filesystem::is_directory(targetDir)) {
        std::cout << targetDir.string() << " is not a directory!" << std::endl;
        return false;
    }

    std::ofstream outFile(targetFile, std::ios::binary);
    if (!outFile.is_open()) {
        std::cout << "Could not open/create " << targetFile.string() << " for writing." << std::endl;
        return false;
    }

    outFile.write(jsonContent.c_str(), jsonContent.size());
    
    bool writeFailed = outFile.fail();
    outFile.close();

    if (writeFailed || outFile.fail()) {
        std::cout << "Failed to write JSON content to file!" << std::endl;
        return false;
    }

    std::cout << "JSON file successfully saved to: " << targetFile.string() << std::endl;
    return true;
}

#endif