#include <gtest/gtest.h>

#include "Client/Client.h"
#include "Server/Server.h"
#include "thread"

using namespace Ipc;
class ServerClientTest : public ::testing::Test {
public:
	void clients(FifoCfg name, Config config, int nBates, int nWrite,int sizeDataServer, std::mutex& mtx)
	{
		Client client(name, config);
		std::string dataClient1 = "";

		auto getterClient1 = [&dataClient1, &mtx](FifoRead::Data&& dataq) {
			std::lock_guard<std::mutex> mtx0(mtx);
			dataClient1.insert(dataClient1.begin(), dataq.data(), dataq.data() + dataq.size());
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

		startWriteClient(client, nBates, nWrite);
		sleep(8);
		startWriteClient(client, nBates, nWrite);
		sleep(8);

		ASSERT_TRUE(clientConnection == 2);
		ASSERT_TRUE(clientDisconnection == 2);
		ASSERT_TRUE(dataClient1.size() == sizeDataServer*2);
	}
	void servers(std::list<FifoCfg> s, Config config, int nBates, int nWrite, size_t nClient,int sizeDataServer, std::mutex& mtx0)
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
			serverConnection++;
		});

		server.setDisconnectHandler([&serverDisconnection](size_t) {
			serverDisconnection++;
		});

		server.start();

		WriteServer(server, nBates, nWrite, nClient);

		sleep(25);

		server.stop();

		ASSERT_TRUE(serverConnection == nClient * 2);
		ASSERT_TRUE(serverDisconnection == nClient * 2);
		ASSERT_TRUE(dataServer.size() == sizeDataServer);
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
	void startWriteClient(Client& client, int nBates, int nWrite)
	{
		client.start();
		sleep(2);
		std::string data0(nBates, 'v');
		for(int i = 0; i != nWrite; i++) {
			client.write((void*)data0.data(), data0.size() * sizeof(std::string::value_type));
		}
		sleep(6);
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

	int sizeNClient  = 1024 * 256;
	int nWriteClient = 10;
	int sizeNServer  = 1024;
	int nWriteServer = 256 * 10;

	int sizeDataServer=sizeNServer*nWriteServer;
	int sizeDaraClient=sizeNClient*nWriteClient;

	std::mutex mtx0;
	std::thread tServer([data, &sizeNServer, nWriteServer,sizeDaraClient, this, &mtx0]() {
		servers(data, Ipc::Config::QW, sizeNServer, nWriteServer, data.size(),sizeDaraClient, mtx0);
	});
	std::mutex mtx;
	std::thread tClient1([&k1, &sizeNClient, nWriteClient,sizeDataServer, this, &mtx]() {
		clients(k1, Ipc::Config::QW, sizeNClient, nWriteClient,sizeDataServer, mtx);
	});
	sleep(1);
	std::thread tClient2([&k2, &sizeNClient, nWriteClient,sizeDataServer, this, &mtx]() {
		clients(k2, Ipc::Config::QW, sizeNClient, nWriteClient,sizeDataServer, mtx);
	});
	sleep(1);
	std::thread tClient3([&k3, &sizeNClient, nWriteClient,sizeDataServer, this, &mtx]() {
		clients(k3, Ipc::Config::QW, sizeNClient, nWriteClient,sizeDataServer, mtx);
	});

	tClient1.join();
	tClient2.join();
	tClient3.join();

	tServer.join();
}
// TEST_F(ServerClientTest, Clients3To1ServerConnectin_NQW)
//{
//	FifoCfg k1{FIFO1, FIFO1 + "_reverse"};
//	FifoCfg k2{FIFO2, FIFO2 + "_reverse"};
//	FifoCfg k3{FIFO3, FIFO3 + "_reverse"};
//
//	std::list<FifoCfg> data = {k1, k2, k3};
//
//	int sizeN = 1024 * 1024;
//	int n     = 10;
//
//	std::mutex mtx0;
//	std::thread tServer([data, &sizeN, n, this, &mtx0]() {
//		servers(data, Ipc::Config::NQW, sizeN, n, data.size(), mtx0);
//	});
//	std::mutex mtx;
//	std::thread tClient1([&k1, &sizeN, n, this, &mtx]() {
//		clients(k1, Ipc::Config::NQW, sizeN, n, mtx);
//	});
//
//	std::thread tClient2([&k2, &sizeN, n, this, &mtx]() {
//		clients(k2, Ipc::Config::QW, sizeN, n, mtx);
//	});
//
//	std::thread tClient3([&k3, &sizeN, n, this, &mtx]() {
//		clients(k3, Ipc::Config::QW, sizeN, n, mtx);
//	});
//
//	tClient1.join();
//	tClient2.join();
//	tClient3.join();
//
//	tServer.join();
// }
//
// TEST_F(ServerClientTest, Clients3To1ServerConnectin_QW_NQW)
//{
//	FifoCfg k1{FIFO1, FIFO1 + "_reverse"};
//	FifoCfg k2{FIFO2, FIFO2 + "_reverse"};
//	FifoCfg k3{FIFO3, FIFO3 + "_reverse"};
//
//	std::list<FifoCfg> data = {k1, k2, k3};
//
//	int sizeN = 1024;
//	int n     = 10;
//
//	std::mutex mtx0;
//	std::thread tServer([data, &sizeN, n, this, &mtx0]() {
//		servers(data, Ipc::Config::NQW, sizeN, n, data.size(), mtx0);
//	});
//	std::mutex mtx;
//	std::thread tClient1([&k1, &sizeN, n, this, &mtx]() {
//		clients(k1, Ipc::Config::QW, sizeN, n, mtx);
//	});
//
//	std::thread tClient2([&k2, &sizeN, n, this, &mtx]() {
//		clients(k2, Ipc::Config::QW, sizeN, n, mtx);
//	});
//
//	std::thread tClient3([&k3, &sizeN, n, this, &mtx]() {
//		clients(k3, Ipc::Config::NQW, sizeN, n, mtx);
//	});
//
//	tClient1.join();
//	tClient2.join();
//	tClient3.join();
//
//	tServer.join();
// }
