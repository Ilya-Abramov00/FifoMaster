
#include <thread>
#include "Fifo/Fifo.h"

std::string FIFO1 = "/home/ilya/Fifo/fifo1";
std::string FIFO2 = "/home/ilya/Fifo/fifo2";

int main()
{
	std::cout << "клиент" << std::endl << std::endl;

	std::string a="";

	FifoWrite client1(FIFO2);

	std::string rf(31,'*');

	client1.writeUser((void*)rf.data(),31);
	client1.startWrite();
sleep(10);
client1.stopWrite();

	std::cout << "клиент завершил отправку" << std::endl;

	return 0;
}
