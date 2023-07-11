

#include "Fifo/Fifo.h"
#include <thread>

std::string FIFO1 = "/home/ilya/Загрузки/Pipe/fifo1";
std::string FIFO2 = "/home/ilya/Загрузки/Pipe/fifo2";


int main() {
    std::cout << "сервер" << std::endl << std::endl;

    Fifo client2;

    auto getter = []() {
        std::string ret;

        while (std::cin >> ret) {
            return ret;
        }
    };
    client2.setMsgGetter(getter);
    client2.start_write();


    std::thread t1([&]() {
        client2.fifoWrite(FIFO2);
    });


    client2.start_read();

    std::thread t2([&]() {
        client2.fifoRead(FIFO1, 20);
    });



    t1.join();
    t2.join();
    std::cout << "сервер окончил прием" << std::endl;
    return 1;
}