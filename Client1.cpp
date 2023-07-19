
#include <thread>
#include "Fifo/Fifo.h"


int main()
{
	std::cout << "клиент" << std::endl << std::endl;

	std::string FIFO2 = "/home/ilya/fifo2";

	int n             = 10;

	FifoWrite client2(FIFO2);

	client2.startWrite();

	std::string a(n, '0');

	client2.pushData((void*)a.data(), n);


	sleep(1);


	client2.stopWrite();

	std::cout << "клиент завершил отправку" << std::endl;

	return 0;
}
