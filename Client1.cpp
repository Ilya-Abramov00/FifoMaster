
#include <thread>
#include "Fifo/Fifo.h"

int main()
{
	std::cout << "клиент" << std::endl << std::endl;

	std::string FIFO2 = "/home/ilya/fifo2";

	int n = 1024 * 1024;

	FifoWrite client2(FIFO2);

	client2.startWrite();

	std::string a(n, 'a');

	for(int i = 0; i != 1024; i++) {
		client2.pushData((void*)a.data(), n);
	}
	sleep(5);
	client2.stopWrite();

	std::cout << "клиент завершил отправку" << std::endl;

	return 0;
}
