#include <iostream>
#include <chrono>
#include <amqpcpp.h>

#include <nlohmann/json.hpp>
#include "amqt.cpp/conn_handler.h"
#include "MovingMax.h"

using json = nlohmann::json;
using std::cout;
using std::endl;

static void subscribe_topic_f(std::function<void(const AMQP::Message& m, uint64_t, bool)>);
static void publish_topic_f(const json&);

int main() {
    const auto window_size = 100;
    MovingMax<int64_t> mm(window_size);
    auto handle_msg = [&mm](const AMQP::Message& m, uint64_t, bool){
        try{
            const json msg = json::parse(m.message());
            auto rand = msg["rand"].get<int64_t>();
            auto running_max = mm.process(rand);
            cout << "rand: " <<  rand<< endl;
            cout << "moving max: " << running_max << endl;

            auto msg_out = msg;
            msg_out["running_max"] = running_max;
            cout << "msg out: " << msg_out << endl;
            publish_topic_f(msg_out);
        }
        catch (std::exception& e) {
            cout << e.what() << endl;
        }
    };
    subscribe_topic_f(handle_msg);
}

void subscribe_topic_f(std::function<void(const AMQP::Message& m, uint64_t, bool)> callback){
    const std::string _exchange = "rand";
    const std::string _queue = "rand";
    const std::string _routing_key = "rand";
    const std::string _host = "localhost";
    const std::string _user = "guest";
    const std::string _pass = "guest";
    const auto _port = 5672;
    ConnHandler handler;
    AMQP::TcpConnection connection(handler,
                                   AMQP::Address(_host, _port,
                                                 AMQP::Login(_user, _pass), "/"));
    AMQP::TcpChannel channel(&connection);
    channel.onError([&handler](const char* message){
        std::cout << "Channel error: " << message << std::endl;
        handler.Stop();
    });
    channel.declareExchange(_exchange, AMQP::topic);
    channel.declareQueue(_queue);
    channel.bindQueue(_exchange, _queue, _routing_key);
    channel.consume(_queue, AMQP::noack).onReceived(callback);
    handler.Start();
    std::cout << "Closing connection." << std::endl;
    connection.close();
}

void publish_topic_f(const json& _msg){
    const std::string _exchange = "solution";
    const std::string _queue = "solution";
    const std::string _routing_key = "solution";
    const std::string _host = "localhost";
    const std::string _user = "guest";
    const std::string _pass = "guest";
    const auto _port = 5672;
    auto evbase = event_base_new();
    LibEventHandlerMyError handler(evbase);
    AMQP::TcpConnection connection(&handler,
                                   AMQP::Address(_host, _port,
                                                 AMQP::Login(_user, _pass), "/"));
    AMQP::TcpChannel channel(&connection);
    channel.onError([&evbase](const char* message){
                        std::cout << "Channel error: " << message << std::endl;
                        event_base_loopbreak(evbase);
                    });
    channel.declareExchange(_exchange, AMQP::topic).onError([&](const char* msg){
                         std::cout << "ERROR: " << msg << std::endl;
                     })
            .onSuccess([&](){
                channel.publish("solution", _routing_key, _msg.dump());
                std::cout << "Sent " << "validate_consumer" << ": '" << _msg.dump() << "'" << std::endl;
                                event_base_loopbreak(evbase);
                            }
                    );
    event_base_dispatch(evbase);
    event_base_free(evbase);
}


