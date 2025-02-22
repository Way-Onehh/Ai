#include<AI_Agent.h>
#include<iostream>

int main(int argc,char **argv) {
    try
    {
        if(argc==1)
        Ai_Agent a("https://api.deepseek.com/chat/completions","deepseek-reasoner","sk-a2af40b9abfa4e66bbf0a09631e0b585",
        R"([
          {
            "content": "you are a helpful assistant",
            "role": "system"
          },
          {
            "content": "hi",
            "role": "user"
          }
])"
        );
        if(argc!=1)
        Ai_Agent a(argv[1],argv[2],argv[3],argv[4]);
    }
    catch(std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}