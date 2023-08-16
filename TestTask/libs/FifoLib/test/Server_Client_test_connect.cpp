#include <gtest/gtest.h>

#include "Client/Client.h"
#include "Server/Server.h"
#include "thread"

using namespace Ipc;
class ServerClientTestOneConnect : public ::testing::Test {
public:
	std::string FIFO3 = "fifo3";
	std::string FIFO2 = "fifo2";
	std::string FIFO1 = "fifo1";

	void clients(FifoCfg name, Config config, size_t time, int nBates, int nWrite, int nBatesServer, std::mutex& mtx,
	             size_t waitDisconnectTime)
	{
		Client client(name, config, time);
		std::string dataClient = "";

		auto getterClient1 = [&dataClient, &mtx](FifoRead::Data&& dataq) {
			std::lock_guard<std::mutex> mtx0(mtx);
			dataClient.insert(dataClient.begin(), dataq.data(), dataq.data() + dataq.size());
		};
		int clientConnection    = 0;
		int clientDisconnection = 0;
		client.setReadHandler(getterClient1);

		client.setConnectHandler([&clientConnection]() {
			clientConnection++;
		});
		client.setDisconnectHandler([&clientDisconnection]() {
			clientDisconnection++;
		});

		client.start();

		WriteClient(client, nBates, nWrite);
		sleep(waitDisconnectTime);

		client.stop();

		ASSERT_TRUE(clientConnection == 1);
		ASSERT_TRUE(clientDisconnection == 1);

		ASSERT_TRUE(dataClient.size() == nBatesServer);
	}
	void servers(std::list<FifoCfg> fifoCfg, Config config, size_t time, int nBates, int nWrite, size_t nClient,
	             int nBatesClient, std::mutex& mtx0, size_t waitDisconnectTime)
	{
		std::string dataServer = "";
		auto getterServer      = [&dataServer, &mtx0](size_t id, FifoRead::Data&& dataq) {
            std::lock_guard<std::mutex> mtx(mtx0);
            dataServer.insert(dataServer.end(), dataq.data(), dataq.data() + dataq.size());
		};

		Server server(fifoCfg);
		int serverConnection    = 0;
		int serverDisconnection = 0;
		server.setReadHandler(getterServer);

		server.setConnectHandler([&serverConnection](size_t) {
			serverConnection++;
		});

		server.setDisconnectHandler([&serverDisconnection](size_t) {
			serverDisconnection++;
		});

		server.start();

		WriteServer(server, nBates, nWrite, nClient);
		sleep(waitDisconnectTime);

		server.stop();

		ASSERT_TRUE(serverConnection == nClient * 1);
		ASSERT_TRUE(serverDisconnection == nClient * 1);
		ASSERT_TRUE(dataServer.size() == nClient * nBatesClient);
		sleep(4);
	}
	void serversDisconnect(std::list<FifoCfg> fifoCfg, Config config, size_t time, int nBates, int nWrite,
	                       size_t nClient, int nBatesClient, std::mutex& mtx0, size_t waitDisconnectTime)
	{
		std::string dataServer = "";
		auto getterServer      = [&dataServer, &mtx0](size_t id, FifoRead::Data&& dataq) {
            std::lock_guard<std::mutex> mtx(mtx0);
            dataServer.insert(dataServer.end(), dataq.data(), dataq.data() + dataq.size());
		};

		Server server(fifoCfg);
		int serverConnection    = 0;
		int serverDisconnection = 0;
		server.setReadHandler(getterServer);

		server.setConnectHandler([&serverConnection](size_t) {
			serverConnection++;
		});

		server.setDisconnectHandler([&serverDisconnection](size_t) {
			serverDisconnection++;
		});

		server.start();

		WriteServer(server, nBates, nWrite, nClient);
		sleep(waitDisconnectTime);
		server.disconnect(0);
		server.disconnect(2);
		server.stop();

		ASSERT_TRUE(serverConnection == nClient * 1);
		ASSERT_TRUE(serverDisconnection == nClient * 1);
		ASSERT_TRUE(dataServer.size() == nClient * nBatesClient);
		sleep(4);
	}

private:
	void WriteServer(Server& server, int nBates, int nWrite, size_t client)
	{
		std::string data0(nBates, 'a');
		for(int i = 0; i != nWrite; i++) {
			for(int k = 0; k != client; k++) {
				server.write(k, (void*)data0.data(), nBates);
			}
		}
	}
	void WriteClient(Client& client, int nBates, int nWrite)
	{
		std::string data0(nBates, 'v');
		for(int i = 0; i != nWrite; i++) {
			client.write((void*)data0.data(), data0.size() * sizeof(std::string::value_type));
		}
	}
};

TEST_F(ServerClientTestOneConnect, Clients3To1ServerConnectin_QW_One_Сonnect)
{
	FifoCfg k1{FIFO1, FIFO1 + "_reverse"};
	FifoCfg k2{FIFO2, FIFO2 + "_reverse"};
	FifoCfg k3{FIFO3, FIFO3 + "_reverse"};

	std::list<FifoCfg> data = {k1, k2, k3};

	int sizeNClient  = 1024 * 256;
	int nWriteClient = 10;

	int sizeNServer  = 256;
	int nWriteServer = 256;
	int dataServer   = nWriteServer * sizeNServer;
	int dataClient   = nWriteClient * sizeNClient;

	std::mutex mtx0;
	std::thread tServer([data, sizeNServer, nWriteServer, dataClient, this, &mtx0]() {
		servers(data, Ipc::Config::QW, 0, sizeNServer, nWriteServer, data.size(), dataClient, mtx0, 4);
	});
	std::mutex mtx;
	std::thread tClient1([&k1, &sizeNClient, nWriteClient, dataServer, this, &mtx]() {
		clients(k1, Ipc::Config::QW, 0, sizeNClient, nWriteClient, dataServer, mtx, 3);
	});

	std::thread tClient2([&k2, &sizeNClient, nWriteClient, dataServer, this, &mtx]() {
		clients(k2, Ipc::Config::QW, 0, sizeNClient, nWriteClient, dataServer, mtx, 6);
	});

	std::thread tClient3([&k3, &sizeNClient, nWriteClient, dataServer, this, &mtx]() {
		clients(k3, Ipc::Config::QW, 0, sizeNClient, nWriteClient, dataServer, mtx, 1);
	});

	tClient1.join();
	tClient2.join();
	tClient3.join();

	tServer.join();
}
TEST_F(ServerClientTestOneConnect, Clients3To1ServerConnectin_NQW_One_Сonnect)
{
	FifoCfg k1{FIFO1, FIFO1 + "_reverse"};
	FifoCfg k2{FIFO2, FIFO2 + "_reverse"};
	FifoCfg k3{FIFO3, FIFO3 + "_reverse"};

	std::list<FifoCfg> data = {k1, k2, k3};

	int sizeNClient  = 1024 * 256;
	int nWriteClient = 10;

	int sizeNServer  = 256;
	int nWriteServer = 256;
	int dataServer   = nWriteServer * sizeNServer;
	int dataClient   = nWriteClient * sizeNClient;

	std::mutex mtx0;
	std::thread tServer([data, sizeNServer, nWriteServer, dataClient, this, &mtx0]() {
		servers(data, Ipc::Config::NQW, 2000, sizeNServer, nWriteServer, data.size(), dataClient, mtx0, 4);
	});
	std::mutex mtx;
	std::thread tClient1([&k1, &sizeNClient, nWriteClient, dataServer, this, &mtx]() {
		clients(k1, Ipc::Config::NQW, 2000, sizeNClient, nWriteClient, dataServer, mtx, 4);
	});

	std::thread tClient2([&k2, &sizeNClient, nWriteClient, dataServer, this, &mtx]() {
		clients(k2, Ipc::Config::NQW, 2000, sizeNClient, nWriteClient, dataServer, mtx, 3);
	});

	std::thread tClient3([&k3, &sizeNClient, nWriteClient, dataServer, this, &mtx]() {
		clients(k3, Ipc::Config::NQW, 2000, sizeNClient, nWriteClient, dataServer, mtx, 2);
	});

	tClient1.join();
	tClient2.join();
	tClient3.join();

	tServer.join();
}
TEST_F(ServerClientTestOneConnect, Clients3To1ServerConnectin_QW_and_NQW_One_Сonnect)
{
	FifoCfg k1{FIFO1, FIFO1 + "_reverse"};
	FifoCfg k2{FIFO2, FIFO2 + "_reverse"};
	FifoCfg k3{FIFO3, FIFO3 + "_reverse"};

	std::list<FifoCfg> data = {k1, k2, k3};

	int sizeNClient  = 1024 * 256;
	int nWriteClient = 10;

	int sizeNServer  = 256;
	int nWriteServer = 256;
	int dataServer   = nWriteServer * sizeNServer;
	int dataClient   = nWriteClient * sizeNClient;

	std::mutex mtx0;
	std::thread tServer([data, sizeNServer, nWriteServer, dataClient, this, &mtx0]() {
		servers(data, Ipc::Config::QW, 2000, sizeNServer, nWriteServer, data.size(), dataClient, mtx0, 4);
	});
	std::mutex mtx;
	std::thread tClient1([&k1, &sizeNClient, nWriteClient, dataServer, this, &mtx]() {
		clients(k1, Ipc::Config::QW, 2000, sizeNClient, nWriteClient, dataServer, mtx, 5);
	});

	std::thread tClient2([&k2, &sizeNClient, nWriteClient, dataServer, this, &mtx]() {
		clients(k2, Ipc::Config::NQW, 2000, sizeNClient, nWriteClient, dataServer, mtx, 3);
	});

	std::thread tClient3([&k3, &sizeNClient, nWriteClient, dataServer, this, &mtx]() {
		clients(k3, Ipc::Config::QW, 2000, sizeNClient, nWriteClient, dataServer, mtx, 2);
	});

	tClient1.join();
	tClient2.join();
	tClient3.join();

	tServer.join();
}
// этот тест тестирует дисконнект сервера
TEST_F(ServerClientTestOneConnect, Clients3To1ServerConnectin_QW_and_NQW_Server_Disconnect)
{
	FifoCfg k1{FIFO1, FIFO1 + "_reverse"};
	FifoCfg k2{FIFO2, FIFO2 + "_reverse"};
	FifoCfg k3{FIFO3, FIFO3 + "_reverse"};

	std::list<FifoCfg> data = {k1, k2, k3};

	int sizeNClient  = 256;
	int nWriteClient = 10;

	int sizeNServer  = 256;
	int nWriteServer = 10;
	int dataServer   = nWriteServer * sizeNServer;
	int dataClient   = nWriteClient * sizeNClient;

	std::mutex mtx0;
	std::thread tServer([data, sizeNServer, nWriteServer, dataClient, this, &mtx0]() {
		serversDisconnect(data, Ipc::Config::QW, 2000, sizeNServer, nWriteServer, data.size(), dataClient, mtx0, 4);
	});
	std::mutex mtx;
	std::thread tClient1([&k1, &sizeNClient, nWriteClient, dataServer, this, &mtx]() {
		clients(k1, Ipc::Config::NQW, 2000, sizeNClient, nWriteClient, dataServer, mtx, 5);
	});

	std::thread tClient2([&k2, &sizeNClient, nWriteClient, dataServer, this, &mtx]() {
		clients(k2, Ipc::Config::NQW, 2000, sizeNClient, nWriteClient, dataServer, mtx, 3);
	});

	std::thread tClient3([&k3, &sizeNClient, nWriteClient, dataServer, this, &mtx]() {
		clients(k3, Ipc::Config::QW, 2000, sizeNClient, nWriteClient, dataServer, mtx, 2);
	});

	tClient1.join();
	tClient2.join();
	tClient3.join();

	tServer.join();
}
