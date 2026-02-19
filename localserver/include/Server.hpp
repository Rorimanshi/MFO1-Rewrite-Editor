#include<string>
#include<unordered_map>
#include<functional>
#include<cstdint>

#include<WinSock2.h>

namespace httpserv {

class Request {
public:
    Request() = default;

    void Parse(const char* req);

    std::string GetMethod() const { return method; }
    std::string GetPath() const { return path; }
    std::string GetBody() const { return body; }
private:
    std::string method = "";
    std::string path = "";
    std::string body = "";
    std::string fullRequest = "";
};

class Response {
public:
    Response() = default;

    void BuildRes(
        const std::string& cors = "BLOCK",
        const std::string& type = "text/plain",
        const std::string& body = "internal server error",
        const uint16_t status = 500
    );

    std::string GetFullResponse();

    void SetHeader(const std::string& name, const std::string& val);
    void SetCors(const std::string& _cors);
    void SetStatus(const uint16_t _status);
    void SetContent(const std::string& _body, const std::string& _type);
private:
    std::string method = "GET";
    std::string cors = "BLOCK";
    std::string type = "text/plain";
    std::string body = "internal server error";
    uint16_t status = 500;
    std::unordered_map<std::string, std::string> headers;
};

class Server {
public:
    Request req;
    Response res;
public:
    Server();
    
    bool Listen(const u_short port, const char* addr = "127.0.0.1");
    bool Accept();
    bool Send();
    
    void Cleanup();

    void Post(const std::string& path, std::function<void()> handler);

    std::string getLastErrorMsg() const { return lastErrorMsg; }
    SOCKET getMainSockDescript() const { return mainSocket; }
private:
    WSADATA wsaData;
    SOCKET mainSocket = INVALID_SOCKET;
    SOCKET clientSocket = INVALID_SOCKET;
    struct sockaddr_in service = {0};
    char resBuffer[512] = {0};
    std::unordered_map<std::string, std::function<void()>> postHandlers;
    std::string lastErrorMsg;
private:
    bool InitSocketAPI();
    bool HandleConnection();
    void HandleRequest();
    void ParseRequest();
    void SendCorsRes();
    const char* BuildResponse(
        const char* body,
        const char* status,
        const char* type,
        const char* cors = "http://localhost:5000"
    );
};

}

namespace filesys {

class FileSystem {
public:
    FileSystem() = default;

    static bool Save(const std::string& jsonContent);
};

}