#pragma once
#include<boost/asio.hpp>
#include<boost/beast.hpp>
#include<boost/beast/ssl.hpp>
#include<boost/beast/core.hpp>


// template<typename T>
// struct API 
// {
// std::string josn_format=template<typename T> = R"({
//         "messages": [
//           {
//             "content": "你是个ai助手",
//             "role": "system"
//           },
//           {
//             "content": "你好",
//             "role": "user"
//           }
//         ],
//         "model": null,
//         "frequency_penalty": 0,
//         "max_tokens": 2048,
//         "presence_penalty": 0,
//         "response_format": {
//           "type": "text"
//         },
//         "stop": null,
//         "stream": false,
//         "stream_options": null,
//         "temperature": 1,
//         "top_p": 1,
//         "tools": null,
//         "tool_choice": "none",
//         "logprobs": false,
//         "top_logprobs": null
// })";

// void setmodel(std::string  )
// {

// }

// void setcommit()
// {

// }

// };


class Ai_Agent
{
public:
    Ai_Agent(std::string url,std::string model,std::string key);
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
 