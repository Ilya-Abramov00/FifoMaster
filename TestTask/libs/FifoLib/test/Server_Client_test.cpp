#include <gtest/gtest.h>

#include "Client/Client.h"
#include "Server/Server.h"
#include "thread"

using namespace Ipc;
    class ServerClientTest : public ::testing::Test {
    public:
        void SetUp() override {}

        void startWriteClient(Client &client, int n) {
            client.start();

            std::string data0(n, 'v');
            for (int i = 0; i != 10; i++) {
                client.write((void *) data0.data(), n);
                sleep(1);
            }
            sleep(20);
            client.stop();
        }

        void startWriteServer(Server &server, int n) {
            server.start();
            std::string data0(n, 'a');
            for (int i = 0; i != 10; i++) {
                server.write(0, (void *) data0.data(), n);
                server.write(1, (void *) data0.data(), n);
                server.write(2, (void *) data0.data(), n);
                sleep(1);
            }
            sleep(30);

            server.stop();
        }

        void TearDown() override {}

    private:
    };

    std::string FIFO3 = "fifo3";
    std::string FIFO2 = "fifo2";
    std::string FIFO1 = "fifo1";

    FifoCfg k1{FIFO1, FIFO1 + "_reverse"};
    FifoCfg k2{FIFO2, FIFO2 + "_reverse"};
    FifoCfg k3{FIFO3, FIFO3 + "_reverse"};

    TEST_F(ServerClientTest, Clients3To1ServerConnectin) {

        int n = 1024 * 1024;
        std::string dataServer = "";

        std::mutex mtx0;

        auto getterServer = [&dataServer, &mtx0](size_t id, FifoRead::Data &&dataq) {
            std::lock_guard<std::mutex> mtx(mtx0);
            dataServer.insert(dataServer.end(), dataq.data(), dataq.data() + dataq.size());
        };

        Server server({k1, k2, k3});

        server.setReadHandler(getterServer);
        server.setConnectHandler([](size_t) {
        });
        server.setDisconnectHandler([](size_t) {
        });

        Client client1(k1);
        std::string dataClient1 = "";

        auto getterClient1 = [&dataClient1](FifoRead::Data &&dataq) {
            dataClient1.insert(dataClient1.begin(),dataq.data(), dataq.data() + dataq.size());
        };
        client1.setReadHandler(getterClient1);

        Client client2(k2);
        std::string dataClient2 = "";
        auto getterClient2 = [&dataClient2](FifoRead::Data &&dataq) {
            dataClient2.insert(dataClient2.begin(),dataq.data(), dataq.data() + dataq.size());
        };
        client2.setReadHandler(getterClient2);

        Client client3(k3);
        std::string dataClient3 = "";
        auto getterClient3 = [&dataClient3](FifoRead::Data &&dataq) {
            dataClient3.insert(dataClient3.begin(),dataq.data(), dataq.data() + dataq.size());
        };
        client3.setReadHandler(getterClient3);

        std::thread tServer([&server, &n, this]() {
            startWriteServer(server, n);
        });

        std::thread tClient1([&client1, &n, this]() {
            startWriteClient(client1, n);
        });

        std::thread tClient2([&client2, &n, this]() {
            startWriteClient(client2, n);
        });

        std::thread tClient3([&client3, n, this]() {
            startWriteClient(client3, n);
        });

        tClient1.join();
        tClient2.join();
        tClient3.join();
        tServer.join();

        ASSERT_TRUE(dataServer.size() == 3 * n * 10);
        ASSERT_TRUE(dataClient1.size() == n * 10);
        ASSERT_TRUE(dataClient2.size() == n * 10);
        ASSERT_TRUE(dataClient3.size() == n * 10);
    }
