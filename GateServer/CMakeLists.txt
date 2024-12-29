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


# <------------ add hiredis dependency --------------->
find_path(HIREDIS_HEADER hiredis)

find_library(HIREDIS_LIB hiredis)

# <------------ add redis-plus-plus dependency -------------->
# NOTE: this should be *sw* NOT *redis++*
find_path(REDIS_PLUS_PLUS_HEADER sw)

find_library(REDIS_PLUS_PLUS_LIB redis++)

set(EXECUTABLE_OUTPUT_PATH ../)

set(ConfigMgr
../Config/ConfigMgr.h
../Config/ConfigMgr.cpp
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
../MysqlMgr/MysqlDao.h
../MysqlMgr/MysqlDao.cpp
../MysqlMgr/MysqlMgr.h
../MysqlMgr/MysqlMgr.cpp
)

set(LogicSystem
LogicSystem/LogicSystem.h
LogicSystem/LogicSystem.cpp
)

set(protoc
    ../protoc/message.grpc.pb.cc
    ../protoc/message.grpc.pb.h
    ../protoc/message.pb.h
    ../protoc/message.pb.cc
)

set(RedisMgr
    ../RedisMgr/RedisMgr.h
    ../RedisMgr/RedisMgr.cpp
)

set(StatusClient
../StatusGrpcClient/StatusGrpcClient.cpp
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
    ${StatusClient}
)

target_include_directories(GateServer PUBLIC ${HIREDIS_HEADER})
target_include_directories(GateServer PUBLIC ${HIREDIS_HEADER})
target_include_directories(GateServer PUBLIC ${REDIS_PLUS_PLUS_HEADER})

target_link_libraries(GateServer 
    Boost::filesystem 
    mysqlcppconn
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF}
    jsoncpp
    hiredis
    ${HIREDIS_LIB}
    ${REDIS_PLUS_PLUS_LIB}
)
