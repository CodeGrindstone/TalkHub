cmake_minimum_required(VERSION 3.5.0)
project(ChatServer VERSION 0.1.0 LANGUAGES C CXX)

# 查找 Boost 库
find_package(Boost REQUIRED COMPONENTS filesystem system)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
 
find_package(Threads REQUIRED)
 
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED) 
message(STATUS "Using protobuf ${Protobuf_VERSION}")
 
set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_REFLECTION gRPC::grpc++_reflection)
 
 
# Find gRPC installation
# Looks for gRPCConfig.cmake file installed by gRPC's cmake installation.
find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")
set(_GRPC_GRPCPP gRPC::grpc++)

# <------------ add hiredis dependency --------------->
find_path(HIREDIS_HEADER hiredis)
                                                                                                               
find_library(HIREDIS_LIB hiredis)

# <------------ add redis-plus-plus dependency -------------->
# NOTE: this should be *sw* NOT *redis++*
find_path(REDIS_PLUS_PLUS_HEADER sw)

find_library(REDIS_PLUS_PLUS_LIB redis++)


set(protoc
    ../protoc/message.pb.h
    ../protoc/message.pb.cc
    ../protoc/message.grpc.pb.h
    ../protoc/message.grpc.pb.cc
)
set(ConfigMgr
../Config/ConfigMgr.h
../Config/ConfigMgr.cpp)

set(RedisMgr
../RedisMgr/RedisMgr.h
../RedisMgr/RedisMgr.cpp
)

set(StatusGrpcClient
../StatusGrpcClient/StatusGrpcClient.h
../StatusGrpcClient/StatusGrpcClient.cpp
)

set(MessageHandler
MessageHandler/IMessageHandler.cpp
)

set(ChatGrpcClient
ChatGrpcClient/ChatGrpcClient.h
ChatGrpcClient/ChatGrpcClient.cpp
)

set(ChatServiceImpl
ChatServiceImpl/ChatServiceImpl.h
ChatServiceImpl/ChatServiceImpl.cpp
)

set(UserMgr
UserMgr/UserMgr.h
UserMgr/UserMgr.cpp
)

set(MysqlMgr
../MysqlMgr/MysqlMgr.h
../MysqlMgr/MysqlMgr.cpp
../MysqlMgr/MysqlDao.h
../MysqlMgr/MysqlDao.cpp
)


set(EXECUTABLE_OUTPUT_PATH ../)

add_executable(chatserver 
    main.cpp
    ../const.h
    ChatServer/ChatServer.cpp
    ${protoc}
    ${ConfigMgr}
    ${RedisMgr}
    ${StatusGrpcClient}
    ${MessageHandler}
    ${ChatGrpcClient}
    ${ChatServiceImpl}
    ${UserMgr}
    ${MysqlMgr}
)

target_include_directories(chatserver PUBLIC ${HIREDIS_HEADER})
target_include_directories(chatserver PUBLIC ${HIREDIS_HEADER})
target_include_directories(chatserver PUBLIC ${REDIS_PLUS_PLUS_HEADER})

target_link_libraries(chatserver
    muduo_net
    muduo_base
    pthread
    jsoncpp
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF}
    hiredis
    ${HIREDIS_LIB}
    ${REDIS_PLUS_PLUS_LIB}
    mysqlcppconn
)


