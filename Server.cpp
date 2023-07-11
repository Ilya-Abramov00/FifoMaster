

#include "Fifo/Fifo.h"

std::string FIFO1 = "/home/ilya/Загрузки/Pipe/fifo1";
std::string FIFO2 = "/home/ilya/Загрузки/Pipe/fifo2";


int main() {
    std::cout << "сервер" << std::endl << std::endl;
    Fifo client2;

    client2.start_read();
    client2.fifoRead(FIFO1, 20);

    std::cout << "сервер окончил прием" << std::endl;
    return 1;
}