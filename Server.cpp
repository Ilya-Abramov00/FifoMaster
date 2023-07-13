

#include "Fifo/Fifo.h"
#include <thread>

std::string FIFO1 = "/home/ilya/Fifo/fifo1";
std::string FIFO2 = "/home/ilya/Fifo/fifo2";

int main()
{
	std::cout << "сервер" << std::endl << std::endl;
/*
	FifoWrite client1(FIFO1);

	auto getter = []() {
		std::string ret;

		while (std::cin >> ret) {
			return std::pair(ret,ret.size());
		}
	};*/

//	client1.setMsgGetter(getter);
	//client1.startWrite();
//	client1.writeUser();





	auto getterRead  = [](char* data, size_t szInBytes) {
		//      std::cout << std::string(dataq);

		for(int i=0;i!=szInBytes+1;i++){
			std::cout<<data[i];
		}

	};
	Params params = {
	    FIFO2,
	    6,
	    0,
	    getterRead,
	};

	FifoRead client2(params);


	client2.startRead();


	std::cout << "сервер окончил прием" << std::endl;
	return 1;
}