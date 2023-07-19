

#include "Fifo/Fifo.h"
#include <thread>




int main()
{
	std::cout << "сервер" << std::endl << std::endl;
	std::string FIFO2 = "/home/ilya/fifo2";

	int n            = 10;
	std::string data = "";
	data.reserve(n * 10);
	auto getterRead = [&](Data && dataq) {
		data += std::string(dataq.data(), dataq.data() + dataq.size());
		std::cout<<"wefg"<<std::endl;
	};

	Params params = {
	    FIFO2,
	    getterRead,
	};

	FifoRead client1(params);

	client1.startRead();

	std::this_thread::sleep_for(std::chrono::seconds(5));

	client1.stopRead();
	std::cout << "\nсервер окончил прием\n" << std::endl;
	return 1;
}