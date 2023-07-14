

#include "Fifo/Fifo.h"
#include <thread>

std::string FIFO1 = "/home/ilya/Fifo/fifo1";
std::string FIFO2 = "/home/ilya/Fifo/fifo2";

int main()
{
	std::cout << "сервер" << std::endl << std::endl;


	std::string a   = "";
	auto getterRead = [&a](void* data, size_t szInBytes) {
		std::cout << std::endl;

		std::cout << std::string((char*)data, (char*)data + szInBytes);
		a = a + std::string((char*)data, (char*)data + szInBytes);
	};
	Params params = {
	    FIFO2,
	    3,
	    0,
	    getterRead,
	};

	FifoRead client2(params);

	client2.startRead();
	sleep(10);
	client2.stopRead();

	std::cout << "\nсервер окончил прием\n" << std::endl;
	return 1;
}