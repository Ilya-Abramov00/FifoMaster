
#include <thread>
#include "Fifo/Fifo.h"

std::string FIFO1 = "/home/ilya/Fifo/fifo1";
std::string FIFO2 = "/home/ilya/Fifo/fifo2";

int main()
{
	std::cout << "клиент" << std::endl << std::endl;

	//	auto getterRead  = [&](const char* dataq, size_t szInBytes) {
	//		//      std::cout << std::string(dataq);
	//		std::cout<<"данные пришли"<< dataq;
	//	};
	//	Params params = {
	//	    FIFO2,
	//	    111,
	//	    0,
	//	    getterRead,
	//	};
	//
	//	FifoRead client2(params);
	//	client2.startRead();
	//
	//	std::string data="";
	// ef
	//
	//

//	FifoWrite client1(FIFO2);
//	int i       = 0;
//	auto getter = [&]() {
//		std::string ret;
//		i++;
//		if(i == 3) {
//			client1.stopWrite();
//		}
//
//		while(std::cin >> ret) {
//			return std::pair(ret, ret.size());
//		}
//	};
//
//	client1.setMsgGetter(getter);
//
//	client1.startWrite();
//	client1.writeUser();
//
//	std::cout << "клиент завершил отправку" << std::endl;
	std::string FIFO2 = "/home/ilya/Fifo/fifo2";

	std::string data = "";

	auto getterRead = [&](void* dataq, size_t szInBytes) {
		data += std::string((char*)dataq, (char*)dataq + szInBytes);
	};


	Params params = {
	    FIFO2,
	    11,
	    20,
	    getterRead,
	};



	FifoRead client1(params);

	FifoWrite client2(FIFO2);

	std::string a(10, '*');
	std::thread t1([&client2,&a]() {
		client2.writeUser((void*)a.data(), 10);
	});
	sleep(0.1);
	std::string b(10, '@');
	std::thread t2([&client2,&b]() {
		client2.writeUser((void*)b.data(), 10);
	});
	sleep(0.1);
	std::string c(10, '!');
	std::thread t3([&client2,&c]() {
		client2.writeUser((void*)c.data(), 10);
	});
	client1.startRead();
	void* z= nullptr;

	sleep(0.1);
	std::thread t4([&client2,&z]() {
		client2.writeUser(z, 0);
	});


	client2.startWrite();
	sleep(1);

	client2.stopWrite();
	sleep(3);
	client1.stopRead();
	return 0;
}
