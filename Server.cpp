

#include "Fifo/Fifo.h"
#include <thread>

int main()
{
	std::cout << "сервер" << std::endl;

	std::string FIFO2 = "/home/ilya/fifo2";

	int n            = 10;
	std::string data = "";
	data.reserve(n * 10);
	auto getterRead = [&](Data&& dataq) {
		auto a = std::string(dataq.data(), dataq.data() + dataq.size());
		data += a;
		std::cout << a << std::endl;
		std::cout << dataq.size() << std::endl;
	};
	auto connect = []() {
		std::cout << "произошел коннект" << std::endl;
	};

	Params params = {FIFO2, getterRead, connect};

	Fifo client1(params);

	client1.startRead();
	client1.startWrite();
	std::this_thread::sleep_for(std::chrono::seconds(2));
	client1.stopWrite();
	client1.stopRead();
	std::cout << "\nсервер окончил прием\n" << std::endl;
	std::cout << data;
	return 1;
}