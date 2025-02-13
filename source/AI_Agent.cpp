#include<AI_Agent.h>
#include<iostream>
#include<boost/json.hpp>
#include<boost/regex.hpp>
#include<boost/beast/http.hpp>

namespace ip=boost::asio::ip;
namespace asio=boost::asio;
namespace json=boost::json;
namespace http=boost::beast::http;
namespace beast=boost::beast;

using string=std::string;
using tcp=boost::asio::ip::tcp;

class Ai_Agent::__Chatrecode
{
    using value=boost::json::value;
    using httprequest=boost::beast::http::request<boost::beast::http::string_body>;
    using parser=boost::beast::http::response_parser<http::string_body>;
    using serializer=boost::beast::http::serializer<true,http::string_body>;
private:
    string __serialize()
    {
        serializer ser(__req);
        std::stringstream ss;
        auto a= ser.get();
        ss<<a;
        return ss.str(); 
    }
    string __history;
    string __req_str;
    value __json;
    httprequest __req;
    parser __parser;
public:
    __Chatrecode(string auth)
    {
        string josn__str=R"({
        "messages": [
          {
            "content": "你是个ai助手",
            "role": "system"
          },
          {
            "content": "你好",
            "role": "user"
          }
        ],
        "model": "deepseek-chat",
        "frequency_penalty": 0,
        "max_tokens": 2048,
        "presence_penalty": 0,
        "response_format": {
          "type": "text"
        },
        "stop": null,
        "stream": false,
        "stream_options": null,
        "temperature": 1,
        "top_p": 1,
        "tools": null,
        "tool_choice": "none",
        "logprobs": false,
        "top_logprobs": null
})";
        __json=json::parse(josn__str);
        __req.method(http::verb::post);
        __req.target("/chat/completions");
        __req.version(11);
        __req.set(http::field::host,"api.deepseek.com");
        __req.set(http::field::user_agent, "AI.api");
        __req.set(http::field::accept,"*/*");
        __req.set(http::field::content_type,"application/json");
        __req.set(http::field::authorization,auth);
    }

    void commit(std::string role,std::string content)
    {
        __history.append(content);
        json::object obj;
        obj["role"]=role; 
        obj["content"]=content;
        __json.at("messages").as_array().push_back(obj);  
        //std::cout<<boost::json::serialize(json)<<std::endl;
    } 

    std::string request()
    {
        __req.body()=json::serialize(__json);
        __req.prepare_payload();
        return __serialize();
    }

    parser& Parser()
    {
        
        return __parser;
    }
};

Ai_Agent::Ai_Agent(std::string url,std::string port,std::string key) {
    //Initialization
    __chatrecode=new __Chatrecode(key);
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve(url, port);
    boost::beast::net::ssl::context cox(boost::beast::net::ssl::context::tlsv12_client);
    cox.set_default_verify_paths();
    boost::beast::ssl_stream<boost::beast::tcp_stream> ss(io_context,cox);
    boost::beast::get_lowest_layer(ss).connect(endpoints);
    ss.handshake(boost::asio::ssl::stream_base::client);

    boost::asio::async_write(ss,boost::asio::buffer(__chatrecode->request()),
        [this, &ss](const boost::system::error_code& ec, std::size_t /*length*/) {
            if (!ec) {
                __read(ss);
            } else {
                std::cerr << "Write failed: " << ec.message() << std::endl;
            }
        });

    io_context.run();
}

void Ai_Agent::__read(boost::beast::ssl_stream<boost::beast::tcp_stream>& ss) {
    //asio::buffer 对string是通过string.size()返回值来判断buffer的大小
    __buf.clear();
    //这里有留存的bug
    asio::async_read_until(ss,asio::dynamic_buffer(__buf),'\r\n',
        [this, &ss](const boost::system::error_code& ec, std::size_t length)
        {
            if (!ec) {
                std::string http;
                http+=__buf;
                __buf.clear();
                asio::read_until(ss,asio::dynamic_buffer(__buf),'\r\n\r\n');
                http+=__buf;
                __buf.clear();
                asio::read_until(ss,asio::dynamic_buffer(__buf),'\r\n\r\n');
                http+=__buf;
                __buf.clear();
                asio::read_until(ss,asio::dynamic_buffer(__buf),'\r\n\r\n');
                http+=__buf;
                __buf.clear();
                boost::regex rex("\"content\":\"(.*?)\"");
                boost::smatch match;
                boost::regex_search(http,match,rex);
                std::cout<< match[1]<<std::endl;
                __chatrecode->commit("assistant",match[1]);
                __chat(ss);
            } else {
                std::cerr << "Read failed: " << ec.message() << std::endl;
            }
        }
    );
    // __buf.clear();
    // http::message<false, boost::beast::http::string_body> message;

    // http::async_read(ss, asio::dynamic_buffer(__buf), message,
    //     [this,&message, &ss]
    //     (beast::error_code ec, size_t) {
    //         if (ec) {
    //             std::cerr << "Read failed: " << ec.message() << std::endl;
    //             return;
    //         }

    //         try {
    //             json::value json_res = json::parse(message.body());
    //             std::string content = json_res.at("content").as_string().c_str();
    //             __chatrecode->commit("assistant", content);
    //             __chat(ss); // 继续下一轮通信
    //         } catch (const std::exception& e) {
    //             std::cerr << "Error processing response: " << e.what() << std::endl;
    //         }
    //     });
}

void Ai_Agent::__chat(boost::beast::ssl_stream<boost::beast::tcp_stream>& ss) {
    __buf.clear();
    std::getline(std::cin,__buf);
    __chatrecode->commit("user",__buf);
    boost::asio::async_write(
        ss, boost::asio::buffer(__chatrecode->request()),
        [this, &ss](const boost::system::error_code& ec, std::size_t /*length*/) {
            if (!ec) {
                __read(ss);
            } else {
                std::cerr << "Write failed: " << ec.message() << std::endl;
            }
        });
}

Ai_Agent::~Ai_Agent()
{
    delete __chatrecode;
}