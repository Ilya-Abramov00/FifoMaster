

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
	// client1.startWrite();
	//	client1.writeUser();
std::string a="";
	auto getterRead = [& a](std::string data, size_t szInBytes) {

		std::cout << std::endl;
		if(data.size()!=szInBytes){throw ;}
		std::cout<<data;
		a=a+ data;
	};
	Params params = {
	    FIFO2,
	    2,
	    0,
	    getterRead,
	};

	FifoRead client2(params);

	client2.startRead();
	std::cout << "\nсервер окончил прием\n" << std::endl;
	std::cout<<a;

	return 1;
}