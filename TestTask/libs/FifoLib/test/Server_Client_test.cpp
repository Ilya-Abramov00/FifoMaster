#include <gtest/gtest.h>


#include "Client/Client.h"
#include "Server/Server.h"
#include "thread"
using namespace std;
TEST(Server_Client, 1)
{

	std::string FIFO3 = "fifo3";
	std::string FIFO2 = "fifo2";
	std::string FIFO1 = "fifo1";

	int n            = 1024;
	std::string dataServer = "";
	dataServer.reserve(n * 10*3);


	FifoCfg k1{FIFO1,FIFO1+"_reverse"};
	FifoCfg k2{FIFO2,FIFO2+"_reverse"};
	FifoCfg k3{FIFO3,FIFO3+"_reverse"};
	std::mutex mtx0;
	auto getterServer = [&dataServer,&mtx0](FifoCfg name, FifoRead::Data&& dataq) {
		lock_guard<std::mutex > mtx(mtx0);
		dataServer += std::string(dataq.data(), dataq.data() + dataq.size());
	};

	Server server({k1,k2,k3});
	server.setReadHandler(getterServer);

	server.setConnectHandler([](size_t){});
	server.setDisconnectHandler([](size_t){});


	Client client1(k1);
	std::string dataClient1 = "";
	dataClient1.reserve(n * 20);
	auto getterClient1 = [&dataClient1]( FifoRead::Data&& dataq) {
		dataClient1 += std::string(dataq.data(), dataq.data() + dataq.size());
	};
	client1.setReadHandler(getterClient1);

	Client client2(k2);
	std::string dataClient2 = "";
	dataClient2.reserve(n * 20);
	auto getterClient2 = [&dataClient2]( FifoRead::Data&& dataq) {
		dataClient2 += std::string(dataq.data(), dataq.data() + dataq.size());
	};
	client2.setReadHandler(getterClient2);

	Client client3(k3);
	std::string dataClient3 = "";
	dataClient3.reserve(n * 20);
	auto getterClient3 = [&dataClient3]( FifoRead::Data&& dataq) {
		dataClient3 += std::string(dataq.data(), dataq.data() + dataq.size());
	};
	client3.setReadHandler(getterClient3);



std::thread tServer([&server, &n]() {
		server.start();
	std::string data0(n, 'a');
	for(int i = 0; i != 10; i++) {
		server.write(0, (void*)data0.data(), n);
		server.write(1, (void*)data0.data(), n);
		server.write(2, (void*)data0.data(), n);
		sleep(1);
	}

	sleep(30);

	server.stop();
	});



std::thread tClient1([&client1, &n]() {
client1.start();

std::string data0(n, 'v');
for(int i = 0; i != 10; i++) {
	client1.write((void*)data0.data(),n);
	sleep(1);
}

sleep(20);
client1.stop();

});

std::thread tClient2([&client2, &n]() {
	client2.start();

	std::string data0(n, 'v');
	for(int i = 0; i != 10; i++) {
		client2.write((void*)data0.data(),n);
		sleep(1);
	}

	sleep(20);
	client2.stop();

});

std::thread tClient3([&client3, &n]() {
	client3.start();

	std::string data0(n, 'v');
	for(int i = 0; i != 10; i++) {
		client3.write((void*)data0.data(),n);
		sleep(1);
	}
	sleep(20);

	client3.stop();

});

tClient1.join();
tClient2.join();
tClient3.join();
tServer.join();


ASSERT_TRUE(dataServer.size() == 3*n*10);
ASSERT_TRUE(dataClient1.size() == n*10);
ASSERT_TRUE(dataClient2.size() == n*10);
ASSERT_TRUE(dataClient3.size() == n*10);

}

