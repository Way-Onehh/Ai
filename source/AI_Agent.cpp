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
    __Chatrecode(string host,string target,string model,string key )
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
        "model": null,
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
        __json.as_object().insert_or_assign("model",model);
        __req.method(http::verb::post);
        __req.target(target);
        __req.version(11);
        __req.set(http::field::host,host);
        __req.set(http::field::user_agent, "curl");
        __req.set(http::field::accept,"*/*");
        __req.set(http::field::content_type,"application/json");
        auto p_key=std::string("Bearer ")+=key;
        __req.set(http::field::authorization,p_key);
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

class Ai_Agent::__Api
{

    public:
        boost::beast::net::ssl::context cox;
        boost::beast::ssl_stream<boost::beast::tcp_stream> ss;
        boost::asio::io_context * _io_adder;
        string _host;
        __Api(boost::asio::io_context &io,string host): ss(io,cox),cox(boost::beast::net::ssl::context::tlsv13_client)
        {
            SSL_set_tlsext_host_name(ss.native_handle(), host.c_str());
            _io_adder= &io;
            _host=host;
            boost::asio::ip::tcp::resolver resolver(io);
            auto endpoints = resolver.resolve(host, "443");
            cox.set_default_verify_paths();
            cox.set_verify_mode(boost::asio::ssl::verify_peer);
            boost::beast::get_lowest_layer(ss).connect(endpoints);
            ss.handshake(boost::asio::ssl::stream_base::client);
        }
        
};

Ai_Agent::Ai_Agent(std::string url,std::string model,std::string key) {
    //Initialization
    boost::regex url_regex(R"(https://(.*?)(/.*))");
    boost::smatch match;
    boost::regex_search(url,match,url_regex);
    string host=match[1];
    string target=match[2]; 

    if(host.empty() || target.empty()) throw ;
     
    __chatrecode=new __Chatrecode(host,target,model,key);
    boost::asio::io_context io;
    __api=new __Api(io,host);
    auto& ss=__api->ss;
    boost::asio::async_write(ss,boost::asio::buffer(__chatrecode->request()),
        [this, &ss](const boost::system::error_code& ec, std::size_t /*length*/) {
            if (!ec) {
                __read(ss);
            } else {
                std::cerr << "Write failed: " << ec.message() << std::endl;
            }
        });

    io.run();
}

void Ai_Agent::__read(boost::beast::ssl_stream<boost::beast::tcp_stream>& ss) {
    __buf.clear();
    asio::async_read_until(ss,asio::dynamic_buffer(__buf),"\r\n\r\n",
        [this, &ss](const boost::system::error_code& ec, std::size_t length)
        {
            ///
            if (!ec) {
                std::string http;
                http+=__buf;
                __buf.clear();
                //std::cout<<http<<std::endl;
                asio::read_until(ss,asio::dynamic_buffer(__buf),"\r\n\r\n");
                http+=__buf;
                boost::regex rex("\"content\":\"(.*?)\"");
                boost::smatch match;
                boost::regex_search(http,match,rex);
                std::cout<< match[1]<<std::endl;
                __chatrecode->commit("assistant",match[1]);
                __chat(ss);
            } else {
                //std::cerr << "Read failed: " << ec.message() << std::endl;
                auto &io=*__api->_io_adder;
                auto host=__api->_host;
                delete __api;
                __api=new __Api(io,host);
                write(ss,boost::asio::buffer(__chatrecode->request()));
                read_until(ss,asio::dynamic_buffer(__buf),"\r\n\r\n");
                std::string http;
                http+=__buf;
                __buf.clear();
                asio::read_until(ss,asio::dynamic_buffer(__buf),"\r\n\r\n");
                http+=__buf;
                boost::regex rex("\"content\":\"(.*?)\"");
                boost::smatch match;
                boost::regex_search(http,match,rex);
                std::cout<< match[1]<<std::endl;
                __chatrecode->commit("assistant",match[1]);
                __chat(ss);
            }
        }
    );
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
    delete __api;
    delete __chatrecode;
}