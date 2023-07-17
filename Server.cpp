

#include "Fifo/Fifo.h"
#include <thread>

std::string FIFO1 = "/home/ilya/Fifo/fifo1";
std::string FIFO2 = "/home/ilya/Fifo/fifo2";

int main()
{
	std::cout << "сервер" << std::endl << std::endl;

	int n            = 10;
	std::string data = "";
	data.reserve(n * 10);
	auto getterRead = [&](void* dataq, size_t szInBytes) {
		data += std::string((char*)dataq, (char*)dataq + szInBytes);
		std::cout << " \nполучил данные\n" << std::string((char*)dataq, (char*)dataq + szInBytes);
	};

	Params params = {
	    FIFO2,
	    10,
	    0,
	    getterRead,
	};

	FifoRead client1(params);

	client1.startRead();

	std::this_thread::sleep_for(std::chrono::seconds(3));

	client1.stopRead();
	std::cout << "\nсервер окончил прием\n" << std::endl;
	return 1;
}