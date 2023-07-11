

#include "Fifo/Fifo.h"

std::string FIFO1 = "/home/ilya/Загрузки/Pipe/fifo1";
std::string FIFO2 = "/home/ilya/Загрузки/Pipe/fifo2";


int main() {


    std::cout << "клиент" << std::endl << std::endl;

    Fifo client1;
    auto getter = []() {
        std::string ret;

        while (std::cin >> ret) {
            return ret;
        }
    };
    client1.setMsgGetter(getter);
    client1.start_write();

    client1.fifoWrite(FIFO1);

    std::cout << "сервер завершил отправку" << std::endl;


    return 0;
}
