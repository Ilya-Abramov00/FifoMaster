
#include <thread>
#include "Fifo/Fifo.h"

std::string FIFO1 = "/home/ilya/Fifo/fifo1";
std::string FIFO2 = "/home/ilya/Fifo/fifo2";


int main() {


    std::cout << "клиент" << std::endl << std::endl;




	FifoRead client2(FIFO2);
	client2.startRead();

	std::string data="";
	std::thread t2([&]() {
		client2.readFifo(data, 20);
	});






    FifoWrite client1(FIFO1);

    auto getter = []() {
        std::string ret;

        while (std::cin >> ret) {
            return ret;
        }
    };

    client1.setMsgGetter(getter);
	client1.startWrite();




    std::thread t1([&]() {
		client1.writeFifo();
    });







    t1.join();
    t2.join();
    std::cout << "сервер завершил отправку" << std::endl;
    return 0;
}
