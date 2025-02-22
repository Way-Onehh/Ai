#include<AI_Agent.h>
#include<iostream>
#include<fstream>
int main(int argc,char **argv) {
    try
    {
        std::fstream fs;
        fs.open("D:/wr/Ai/key");
        
        if(!fs) 
        {
          std::cout<<"need a key"<<std::endl;
          return -1;
        }
        std::string key;
        fs>>key;
        fs.close();

        if(argc==1)
        Ai_Agent a("https://api.deepseek.com/chat/completions","deepseek-reasoner",key,
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