#include <gtest/gtest.h>

#include "Client/Client.h"
#include "Server/Server.h"
#include "thread"

using namespace Ipc;
class ServerClientTest : public ::testing::Test {
public:
	void SetUp() override
	{}

	void clients(FifoCfg e, Config config, int n, int k, std::mutex& mtx)
	{
		Client client(e, config);
		std::string dataClient1 = "";

		auto getterClient1 = [&dataClient1, &mtx](FifoRead::Data&& dataq) {
			std::lock_guard<std::mutex> mtx0(mtx);
			dataClient1.insert(dataClient1.begin(), dataq.data(), dataq.data() + dataq.size());
		};
		int clientConnection    = 0;
		int clientDisconnection = 0;
		client.setReadHandler(getterClient1);
		client.setConnectHandler([&clientConnection]() {
			clientConnection += 1;
		});
		client.setDisconnectHandler([&clientDisconnection]() {
			clientDisconnection += 1;
		});

		startWriteClient(client, n, k);
		sleep(2);
		startWriteClient(client, n, k);

		ASSERT_TRUE(clientConnection == 2);
		ASSERT_TRUE(clientDisconnection == 2);
		ASSERT_TRUE(dataClient1.size() == 2 * n * k);
	}
	void servers(std::list<FifoCfg> s, Config config, int n, int k, size_t client, std::mutex& mtx0)
	{
		std::string dataServer = "";
		auto getterServer      = [&dataServer, &mtx0](size_t id, FifoRead::Data&& dataq) {
            std::lock_guard<std::mutex> mtx(mtx0);
            dataServer.insert(dataServer.end(), dataq.data(), dataq.data() + dataq.size());
		};

		Server server(s, config);
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
		sleep(2);
		startWriteServer(server, n, k, client);

		ASSERT_TRUE(serverConnection == client * 2);
		ASSERT_TRUE(serverDisconnection == client * 2);
		ASSERT_TRUE(dataServer.size() == client * 2 * n * k);
	}

	void TearDown() override
	{}

private:
	void startWriteServer(Server& server, int n, int k, size_t client)
	{
		server.start();
		sleep(2);
		std::string data0(n, 'a');
		for(int i = 0; i != k; i++) {
			for(int k = 0; k != client; k++) {
				server.write(k, (void*)data0.data(), n);
			}
		}
		sleep(8);

		server.stop();
	}
	void startWriteClient(Client& client, int n, int k)
	{
		client.start();
		sleep(2);
		std::string data0(n, 'v');
		for(int i = 0; i != k; i++) {
			client.write((void*)data0.data(), data0.size() * sizeof(std::string::value_type));
		}
		sleep(8);
		client.stop();
	}
};

std::string FIFO3 = "fifo3";
std::string FIFO2 = "fifo2";
std::string FIFO1 = "fifo1";

TEST_F(ServerClientTest, Clients3To1ServerConnectin_QW)
{
	FifoCfg k1{FIFO1, FIFO1 + "_reverse"};
	FifoCfg k2{FIFO2, FIFO2 + "_reverse"};
	FifoCfg k3{FIFO3, FIFO3 + "_reverse"};

	std::list<FifoCfg> data = {k1, k2, k3};

	int sizeN = 1024 * 1024;
	int n     = 10;

	std::mutex mtx0;
	std::thread tServer([data, &sizeN, n, this, &mtx0]() {
		servers(data, Ipc::Config::QW, sizeN, n, data.size(), mtx0);
	});
	std::mutex mtx;
	std::thread tClient1([&k1, &sizeN, n, this, &mtx]() {
		clients(k1, Ipc::Config::QW, sizeN, n, mtx);
	});

	std::thread tClient2([&k2, &sizeN, n, this, &mtx]() {
		clients(k2, Ipc::Config::QW, sizeN, n, mtx);
	});

	std::thread tClient3([&k3, &sizeN, n, this, &mtx]() {
		clients(k3, Ipc::Config::QW, sizeN, n, mtx);
	});

	tClient1.join();
	tClient2.join();
	tClient3.join();

	tServer.join();
}
TEST_F(ServerClientTest, Clients3To1ServerConnectin_NQW)
{
	FifoCfg k1{FIFO1, FIFO1 + "_reverse"};
	FifoCfg k2{FIFO2, FIFO2 + "_reverse"};
	FifoCfg k3{FIFO3, FIFO3 + "_reverse"};

	std::list<FifoCfg> data = {k1, k2, k3};

	int sizeN = 1024 * 1024;
	int n     = 10;

	std::mutex mtx0;
	std::thread tServer([data, &sizeN, n, this, &mtx0]() {
		servers(data, Ipc::Config::NQW, sizeN, n, data.size(), mtx0);
	});
	std::mutex mtx;
	std::thread tClient1([&k1, &sizeN, n, this, &mtx]() {
		clients(k1, Ipc::Config::NQW, sizeN, n, mtx);
	});

	std::thread tClient2([&k2, &sizeN, n, this, &mtx]() {
		clients(k2, Ipc::Config::QW, sizeN, n, mtx);
	});

	std::thread tClient3([&k3, &sizeN, n, this, &mtx]() {
		clients(k3, Ipc::Config::QW, sizeN, n, mtx);
	});

	tClient1.join();
	tClient2.join();
	tClient3.join();

	tServer.join();
}

TEST_F(ServerClientTest, Clients3To1ServerConnectin_QW_NQW)
{
	FifoCfg k1{FIFO1, FIFO1 + "_reverse"};
	FifoCfg k2{FIFO2, FIFO2 + "_reverse"};
	FifoCfg k3{FIFO3, FIFO3 + "_reverse"};

	std::list<FifoCfg> data = {k1, k2, k3};

	int sizeN = 1024;
	int n     = 10;

	std::mutex mtx0;
	std::thread tServer([data, &sizeN, n, this, &mtx0]() {
		servers(data, Ipc::Config::NQW, sizeN, n, data.size(), mtx0);
	});
	std::mutex mtx;
	std::thread tClient1([&k1, &sizeN, n, this, &mtx]() {
		clients(k1, Ipc::Config::QW, sizeN, n, mtx);
	});

	std::thread tClient2([&k2, &sizeN, n, this, &mtx]() {
		clients(k2, Ipc::Config::QW, sizeN, n, mtx);
	});

	std::thread tClient3([&k3, &sizeN, n, this, &mtx]() {
		clients(k3, Ipc::Config::NQW, sizeN, n, mtx);
	});

	tClient1.join();
	tClient2.join();
	tClient3.join();

	tServer.join();
}
