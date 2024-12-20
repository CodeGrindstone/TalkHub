#pragma once

#include <grpcpp/grpcpp.h>
#include "../protoc/message.grpc.pb.h"
#include "../protoc/message.pb.h"
#include "../const.h"
#include "../protoc/message.grpc.pb.h"
#include "../ConfigMgr/ConfigMgr.h"
#include "../Singleton.h"
#include <queue>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <grpcpp/grpcpp.h>

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::GetVerifyReq;
using message::GetVerifyRsp;
using message::VerifyService;

class RPConPool {
public:
    RPConPool(size_t poolSize, std::string host, std::string port)
        : poolSize_(poolSize), host_(host), port_(port), b_stop_(false) {
        for (size_t i = 0; i < poolSize_; ++i) {
            std::shared_ptr<Channel> channel = grpc::CreateChannel(host+":"+port,
                grpc::InsecureChannelCredentials());
            connections_.push(VerifyService::NewStub(channel));
        }
    }
    ~RPConPool() {
        std::lock_guard<std::mutex> lock(mutex_);
        Close();
        while (!connections_.empty()) {
            connections_.pop();
        }
    }
    std::unique_ptr<VerifyService::Stub> getConnection() {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this] {
            if (b_stop_) {
                return true;
            }
            return !connections_.empty();
            });
        //如果停止则直接返回空指针
        if (b_stop_) {
            return  nullptr;
        }
        auto context = std::move(connections_.front());
        connections_.pop();
        return context;
    }
    void returnConnection(std::unique_ptr<VerifyService::Stub> context) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (b_stop_) {
            return;
        }
        connections_.push(std::move(context));
        cond_.notify_one();
    }
    void Close() {
        b_stop_ = true;
        cond_.notify_all();
    }
private:
    std::atomic<bool> b_stop_;
    size_t poolSize_;
    std::string host_;
    std::string port_;
    std::queue<std::unique_ptr<VerifyService::Stub>> connections_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

class VerifyGrpcClient:public Singleton<VerifyGrpcClient>
{
    friend class Singleton<VerifyGrpcClient>;
public:
    GetVerifyRsp GetVerifyCode(std::string email)
    {
        ClientContext context;
        GetVerifyReq request;
        GetVerifyRsp response;

        request.set_email(email);

        auto stub = pool_->getConnection();

        Status status = stub->GetVerifyCode(&context, request, &response);

        if (status.ok())
        {
            pool_->returnConnection(std::move(stub));
            return response;
        }
        else
        {
            pool_->returnConnection(std::move(stub));
            response.set_error(ErrorCodes::ERR_SERVER_INTERNAL);
            return response;
        }
    }
private:
    VerifyGrpcClient()
    {
        std::string host = ConfigMgr::getInstance()["VerifyServer"]["Host"];
        std::string port = ConfigMgr::getInstance()["VerifyServer"]["Port"];
        pool_.reset(new RPConPool(5, host, port));
    }
    std::unique_ptr<RPConPool> pool_;
};