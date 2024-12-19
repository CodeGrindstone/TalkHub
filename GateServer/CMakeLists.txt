cmake_minimum_required(VERSION 3.5.0)
project(GateServer VERSION 0.1.0 LANGUAGES C CXX)

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

set(EXECUTABLE_OUTPUT_PATH ../)

set(ConfigMgr
ConfigMgr/ConfigMgr.h
ConfigMgr/ConfigMgr.cpp
)

set(CServer
CServer/CServer.cpp
CServer/CServer.h
)

set(HttpConn
HttpConn/HttpConn.cpp
HttpConn/HttpConn.h
)

set(AsioIOServicePool
AsioIOServicePool/AsioIOServicePool.h
AsioIOServicePool/AsioIOServicePool.cpp
)

set(MysqlDao
MysqlMgr/MysqlDao.h
MysqlMgr/MysqlDao.cpp
MysqlMgr/MysqlMgr.h
MysqlMgr/MysqlMgr.cpp
)

set(LogicSystem
LogicSystem/LogicSystem.h
LogicSystem/LogicSystem.cpp
)

set(protoc
    protoc/message.grpc.pb.cc
    protoc/message.grpc.pb.h
    protoc/message.pb.h
    protoc/message.pb.cc
)

set(RedisMgr
    RedisMgr/RedisMgr.h
    RedisMgr/RedisMgr.cpp
)

add_executable(GateServer 
    main.cpp
    ${ConfigMgr}
    ${CServer}
    ${HttpConn}
    ${AsioIOServicePool}
    ${MysqlDao}
    ${LogicSystem}
    ${protoc}
    ${RedisMgr}
)

target_link_libraries(GateServer 
    Boost::filesystem 
    mysqlcppconn
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF}
    jsoncpp
    hiredis
)

