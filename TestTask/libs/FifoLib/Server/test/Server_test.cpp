#include <gtest/gtest.h>

#include "Server/Server.h"

using namespace std;
void c(FifoCfg name )
{
	std::cout << "CloseConnectionHandler\n";
};
void q(FifoCfg name )
{
	std::cout << "NewConnectionHandler\n";
};
TEST(Fifo, start)
{
	std::string FIFO3 = "/home/ilya/fifo3";
	std::string FIFO2 = "/home/ilya/fifo2";
	std::string FIFO1 = "/home/ilya/fifo1";

	int n            = 100;
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

	std::string eqq(n, 'a');

	a.write(k1, (void*)eqq.data(), n);//надо  уметь сигнализировать о том, что данные не отправились

	a.start();

	sleep(1);

	a.stop();

	a.start();

	sleep(1);

	a.stop();
}
TEST(Fifo, empty)
{
	std::string FIFO3 = "/home/ilya/fifo3";
	std::string FIFO2 = "/home/ilya/fifo2";
	std::string FIFO1 = "/home/ilya/fifo1";

	int n            = 100;
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


	std::string eqq(n, 'a');

	a.write(k1, (void*)eqq.data(), n);//надо  уметь сигнализировать о том, что данные не отправились

	a.start();

	sleep(1);

	a.stop();

	a.start();

	sleep(1);

	a.stop();
}

