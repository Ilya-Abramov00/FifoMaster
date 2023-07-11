

#include "Fifo/Fifo.h"
#include <thread>

std::string FIFO1 = "/home/ilya/Fifo/fifo1";
std::string FIFO2 = "/home/ilya/Fifo/fifo2";

int main() {
    std::cout << "сервер" << std::endl << std::endl;

    FifoWrite client1;

    auto getter = []() {
        std::string ret;

        while (std::cin >> ret) {
            return ret;
        }
    };

    client1.setMsgGetter(getter);
    client1.start_write();




    std::thread t1([&]() {
        client1.fifoWrite(FIFO2);
    });

    FifoRead client2;
    client2.start_read();

    std::thread t2([&]() {
        client2.fifoRead(FIFO1, 20);
    });




    t1.join();
    t2.join();


    std::cout << "сервер окончил прием" << std::endl;
    return 1;
}