add_library(AI_Agent AI_Agent.cpp)
target_include_directories(AI_Agent PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


find_package(Boost REQUIRED json http)
if(Boost_FOUND)
    target_include_directories(AI_Agent PUBLIC ${Boost_INCLUDE_DIRS})
    target_link_libraries(AI_Agent ${Boost_LIBRARIES})
endif()


find_package(OpenSSL)
if(OpenSSL_FOUND)
    target_include_directories(AI_Agent PUBLIC ${OpenSSL_INCLUDE_DIRS})
    target_link_libraries(AI_Agent ${OpenSSL_LIBRARIES})
endif()
    