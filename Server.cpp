

#include "Fifo/Fifo.h"
#include <thread>

std::string FIFO1 = "/home/ilya/Fifo/fifo1";
std::string FIFO2 = "/home/ilya/Fifo/fifo2";

int main()
{
	std::cout << "сервер" << std::endl << std::endl;

	int n            = 1024 * 10;
	std::string data = "";
	data.reserve(n * 10);
	auto getterRead = [&](void* dataq, size_t szInBytes) {
		data += std::string((char*)dataq, (char*)dataq + szInBytes);
		std::cout<<"получил данные\n";
		if(szInBytes != 1024) {
			std::cout << "\nне совпадает размер данных отправки и ожид приема\n";
		}
	};

	Params params = {
	    FIFO2,
	    1024,
	    0,
	    getterRead,
	};

	FifoRead client1(params);

	client1.startRead();

	sleep(20);

	client1.stopRead();

	std::cout << "получили     " << data << std::endl;

	std::cout << "\nсервер окончил прием\n" << std::endl;
	return 1;
}