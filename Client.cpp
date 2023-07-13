
#include <thread>
#include "Fifo/Fifo.h"

std::string FIFO1 = "/home/ilya/Fifo/fifo1";
std::string FIFO2 = "/home/ilya/Fifo/fifo2";


int main() {


    std::cout << "клиент" << std::endl << std::endl;

//	auto getterRead  = [&](const char* dataq, size_t szInBytes) {
//		//      std::cout << std::string(dataq);
//		std::cout<<"данные пришли"<< dataq;
//	};
//	Params params = {
//	    FIFO2,
//	    111,
//	    0,
//	    getterRead,
//	};
//
//	FifoRead client2(params);
//	client2.startRead();
//
//	std::string data="";
//
//
//


    FifoWrite client1(FIFO2);
int i=0;
    auto getter = [&]() {
        std::string ret;
i++;
if (i==3){client1.stopWrite();}

        while (std::cin >> ret) {
            return std::pair(ret,ret.size());
        }
    };

    client1.setMsgGetter(getter);


	client1.startWrite();
	client1.writeUser();



    std::cout << "клиент завершил отправку" << std::endl;
    return 0;
}
