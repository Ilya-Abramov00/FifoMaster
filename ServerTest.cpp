
//void Server::connect(std::shared_ptr<Fifo> object)
//{
//	if(object->getWaitConnectWrite() && object->getWaitConnectRead()) {
//		std::cout << "Connect " << object->getNameRead()<< std::endl;
//	}
//};
//void Server::disconnectr(std::shared_ptr<Fifo> object)
//{
//	if(object->getWaitDisconnectWrite() || object->getWaitDisconnectRead()) {
//		std::cout << "Disconnect " << object->getNameRead()<< std::endl;
//	}
//};
#include "Fifo/Fifo.h"
#include "Server/Server.h"

#include <thread>
#include <iostream>
void c(FifoCfg w)
{
	std::cout << "CloseConnectionHandler\n";
};
void q(FifoCfg w)
{
	std::cout << "NewConnectionHandler\n";
};
int main()
{
std::cout<<"Server\n\n";
	std::string FIFO3 = "/home/ilya/fifo3";
	std::string FIFO2 = "/home/ilya/fifo2";
	std::string FIFO1 = "/home/ilya/fifo1";

	int n            = 10;
	std::string data = "";
	data.reserve(n * 1024);


	FifoCfg k1{FIFO1,FIFO1+"_reverse"};
	FifoCfg k2{FIFO2,FIFO2+"_reverse"};
	FifoCfg k3{FIFO3,FIFO3+"_reverse"};

	auto e = [&](FifoCfg name, FifoRead::Data&& dataq) {

	  std::cout << "\n пришли от "<<name.directFile;
	  data += std::string(dataq.data(), dataq.data() + dataq.size());
	  std::cout<<"\n"<<std::string(dataq.data(), dataq.data() + dataq.size())<<"\n";
	};
	std::list<FifoCfg> z={k1,k2,k3};

	Server a(z);
	a.setReadHandler(e);

a.setConnectHandler([](size_t){});
a.setDisconnectHandler([](size_t){});
	a.start();

	auto x=2;
	std::string eqq(x, 'a');
	for(int i = 0; i != 4; i++) {
	a.write(0, (void*)eqq.data(), x);
	a.write(1, (void*)eqq.data(), x);
	a.write(2, (void*)eqq.data(), x);
		sleep(1);
	}




	sleep(11);

	sleep(10);

	std::cout<<"\nstopStart\n";
	a.stop();
	std::cout<<"\nstop\n";
	std::cout << data.size();
	return 1;
}