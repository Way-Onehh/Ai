#pragma once
#include<boost/asio.hpp>
#include<boost/beast.hpp>
#include<boost/beast/ssl.hpp>
#include<boost/beast/core.hpp>

class Ai_Agent
{
public:
    Ai_Agent(std::string url,std::string model,std::string key,std::string prompt =R"([
          {
            "content": "你是个ai助手",
            "role": "system"
          },
          {
            "content": "你好",
            "role": "user"
          }
])");
    ~Ai_Agent();
private:
    void __chat(boost::beast::ssl_stream<boost::beast::tcp_stream> &ss);
    void __read(boost::beast::ssl_stream<boost::beast::tcp_stream> &ss);
    std::string __buf; 
    class __Chatrecode;
    __Chatrecode* __chatrecode;
    class __Api;
    __Api * __api;
 };
 