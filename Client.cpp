
#include <thread>
#include "Fifo/Fifo.h"

std::string FIFO1 = "/home/ilya/Загрузки/Pipe/fifo1";
std::string FIFO2 = "/home/ilya/Загрузки/Pipe/fifo2";


int main() {


    std::cout << "клиент" << std::endl << std::endl;

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
        client1.fifoWrite(FIFO1);
    });

FifoRead client2;
    client2.start_read();

    std::thread t2([&]() {
        client2.fifoRead(FIFO2, 20);
    });




    t1.join();
    t2.join();
    std::cout << "сервер завершил отправку" << std::endl;
    return 0;
}
