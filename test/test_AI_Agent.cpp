#include<AI_Agent.h>
#include<iostream>

#include<boost/regex.hpp>

int main() {
    try
    {
        Ai_Agent a("api.deepseek.com","443","Bearer sk-a2af40b9abfa4e66bbf0a09631e0b585");
    }
    catch(std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}