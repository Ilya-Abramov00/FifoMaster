#include <gtest/gtest.h>

#include "Client/Client.h"
#include "Server/Server.h"
#include "thread"

using namespace Ipc;
class ServerClientTest : public ::testing::Test {
public:
	void SetUp() override
	{}

	void clients(FifoCfg e, int n, int k)
	{
		Client client(e);
		std::string dataClient1 = "";

		auto getterClient1 = [&dataClient1](FifoRead::Data&& dataq) {
			dataClient1.insert(dataClient1.begin(), dataq.data(), dataq.data() + dataq.size());
		};
		bool clientConnection{false};
		bool clientDisconnection{false};
		client.setReadHandler(getterClient1);
		client.setConnectHandler([&clientConnection]() {
			clientConnection = true;
		});
		client.setDisconnectHandler([&clientDisconnection]() {
			clientDisconnection = true;
		});

		startWriteClient(client, n, k);

		ASSERT_TRUE(clientConnection == true);
		ASSERT_TRUE(clientDisconnection == true);
		ASSERT_TRUE(dataClient1.size() == n * k);
	}
	void servers(std::list<FifoCfg> s, int n, int k, size_t client, std::mutex& mtx0)
	{
		std::string dataServer = "";
		auto getterServer      = [&dataServer, &mtx0](size_t id, FifoRead::Data&& dataq) {
            std::lock_guard<std::mutex> mtx(mtx0);
            dataServer.insert(dataServer.end(), dataq.data(), dataq.data() + dataq.size());
		};

		Server server(s);
		int serverConnection    = 0;
		int serverDisconnection = 0;
		server.setReadHandler(getterServer);
		server.setConnectHandler([&serverConnection](size_t) {
			serverConnection += 1;
		});
		server.setDisconnectHandler([&serverDisconnection](size_t) {
			serverDisconnection += 1;
		});

		startWriteServer(server, n, k, client);
		ASSERT_TRUE(serverConnection == client);
		ASSERT_TRUE(serverDisconnection == client);
		ASSERT_TRUE(dataServer.size() == client * n * k);
	}

	void TearDown() override
	{}

private:
	void startWriteServer(Server& server, int n, int k, size_t client)
	{
		server.start();
		std::string data0(n, 'a');
		for(int i = 0; i != k; i++) {
			for(int k = 0; k != client; k++) {
				server.write(k, (void*)data0.data(), n);
			}
		}
		sleep(20);

		server.stop();
	}
	void startWriteClient(Client& client, int n, int k)
	{
		client.start();

		std::string data0(n, 'v');
		for(int i = 0; i != k; i++) {
			client.write((void*)data0.data(), n);
		}
		sleep(15);
		client.stop();
	}
};
std::string FIFO5 = "fifo5";
std::string FIFO4 = "fifo4";
std::string FIFO3 = "fifo3";
std::string FIFO2 = "fifo2";
std::string FIFO1 = "fifo1";

TEST_F(ServerClientTest, Clients5To1ServerConnectin)
{
	FifoCfg k1{FIFO1, FIFO1 + "_reverse"};
	FifoCfg k2{FIFO2, FIFO2 + "_reverse"};
	FifoCfg k3{FIFO3, FIFO3 + "_reverse"};
	FifoCfg k4{FIFO4, FIFO4 + "_reverse"};
	FifoCfg k5{FIFO5, FIFO5 + "_reverse"};

	std::list<FifoCfg> data = {k1, k2, k3, k4, k5};

	int sizeN = 1024 * 1024;
	int n     = 10;

	std::mutex mtx;
	std::thread tServer([data, &sizeN, n, this, &mtx]() {
		servers(data, sizeN, n, data.size(), mtx);
	});

	std::thread tClient1([&k1, &sizeN, n, this]() {
		clients(k1, sizeN, n);
	});

	std::thread tClient2([&k2, &sizeN, n, this]() {
		clients(k2, sizeN, n);
	});

	std::thread tClient3([&k3, &sizeN, n, this]() {
		clients(k3, sizeN, n);
	});
	std::thread tClient4([&k4, &sizeN, n, this]() {
		clients(k4, sizeN, n);
	});
	std::thread tClient5([&k5, &sizeN, n, this]() {
		clients(k5, sizeN, n);
	});

	tClient1.join();
	tClient2.join();
	tClient3.join();
	tClient4.join();
	tClient5.join();

	tServer.join();
}
