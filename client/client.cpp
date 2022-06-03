#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include "exceptions.h"
#include "hide_and_seek.grpc.pb.h"
#include "../ai/ai.h"


namespace Client {
    using grpc::Channel;
    using grpc::ClientContext;
    using grpc::Status;
    using grpc::ClientReader;
    using grpc::ClientAsyncReader;
    using grpc::CompletionQueue;

    using namespace ir::sharif::aic::hideandseek::api::grpc;

    void throw_if_error(const Status &status) {
        if (!(status).ok())
            throw Exceptions::RpcFailedException((status).error_code(), (status).error_message());
    }

    class Client {
    public:
        explicit Client(const std::string &token, const std::string &address)
                : token(token) {
            try {
                channel = grpc::CreateChannel(address, grpc::InsecureChannelCredentials());
                stub_ = GameHandler::NewStub(channel);
            } catch(...) {
                throw Exceptions::ClientCreationException(token, address);
            }
        }

        void handle_client() {
            WatchCommand request;
            ClientContext context;
            GameView gameView;

            request.set_token(token);

            std::unique_ptr<ClientReader<GameView>> reader(stub_->Watch(&context, request));
            bool first_turn = true;
            while (reader->Read(&gameView)) {
                if (first_turn) {
                    perform_initialize(gameView);
                } else if (gameView.status() == GameStatus::ONGOING) {
                    perform_send_message(gameView);
                    perform_move(gameView);
                } else if (gameView.status() == GameStatus::FINISHED) {
                    break;
                }
                first_turn = false;
            }
            Status status = reader->Finish();
            throw_if_error(status);
        }

    private:
        void DeclareReadiness(const int start_node_id) {
            DeclareReadinessCommand request;
            ClientContext context;
            ::google::protobuf::Empty reply;
            request.set_token(token);
            request.set_startnodeid(start_node_id);
            Status status = stub_->DeclareReadiness(&context, request, &reply);
            throw_if_error(status);
        }

        void SendMessage(const ChatCommand &chatCommand) {
            ClientContext context;
            ::google::protobuf::Empty reply;
            Status status = stub_->SendMessage(&context, chatCommand, &reply);
            throw_if_error(status);
        }

        void Move(const MoveCommand &moveCommand) {
            ClientContext context;
            ::google::protobuf::Empty reply;
            Status status = stub_->Move(&context, moveCommand, &reply);
            throw_if_error(status);
        }

        void perform_initialize(const GameView &gameView) {
            const auto &viewer = gameView.viewer();
            int start_node_id;
            if (viewer.type() == AgentType::THIEF) {
                start_node_id = AI::thief_move_ai(gameView);
            } else {
                start_node_id = 3; // todo dummy
            }
            DeclareReadiness(start_node_id);
        }

        void perform_send_message(const GameView &gameView) {
            const auto &viewer = gameView.viewer();
            std::string message;
            if (viewer.type() == AgentType::THIEF) {
                message = AI::thief_chat_ai(gameView);
            } else {
                message = AI::police_chat_ai(gameView);
            }
            ChatCommand chatCommand;
            chatCommand.set_token(token);
            chatCommand.set_text(message);
            SendMessage(chatCommand);
        }

        void perform_move(const GameView &gameView) {
            const auto &viewer = gameView.viewer();
            int to_node_id;
            if (viewer.type() == AgentType::THIEF) {
                to_node_id = AI::thief_move_ai(gameView);
            } else {
                to_node_id = AI::police_move_ai(gameView);
            }
            MoveCommand moveCommand;
            moveCommand.set_token(token);
            moveCommand.set_tonodeid(to_node_id);
            Move(moveCommand);
        }

        std::unique_ptr<GameHandler::Stub> stub_;
        std::shared_ptr<Channel> channel;
        const std::string token;
    };


    void handle_client(const std::string &token, const std::string &address) {
        Client client(token, address);
        client.handle_client();
    }
}