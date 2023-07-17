
#include <thread>
#include "Fifo/Fifo.h"


std::string FIFO2 = "/home/ilya/Fifo/fifo2";

int main()
{
	std::cout << "клиент" << std::endl << std::endl;

	std::string FIFO2 = "/home/ilya/Fifo/fifo2";
	int n             = 10;

	FifoWrite client2(FIFO2);

	client2.startWrite();

	std::string a0(n, '0');
	std::thread t([&client2, &a0, &n]() {
		client2.pushData((void*)a0.data(), n);
	});
	t.join();
	std::string a(n, '1');
	std::thread t1([&client2, &a, &n]() {
		client2.pushData((void*)a.data(), n);
	});
	t1.join();
	std::string b(n, '2');
	std::thread t2([&client2, &b, &n]() {
		client2.pushData((void*)b.data(), n);
	});
	t2.join();
	std::string c(n, '3');
	std::thread t3([&client2, &c, &n]() {
		client2.pushData((void*)c.data(), n);
	});
	t3.join();
	std::string d(n, '4');
	std::thread t4([&client2, &d, &n]() {
		client2.pushData((void*)d.data(), n);
	});
	t4.join();
	std::string e(n, '5');
	std::thread t5([&client2, &e, &n]() {
		client2.pushData((void*)e.data(), n);
	});
	t5.join();
	std::string f(n, '6');
	std::thread t6([&client2, &f, &n]() {
		client2.pushData((void*)f.data(), n);
	});
	t6.join();
	std::string t7(n, '7');
	std::thread t71([&client2, &t7, &n]() {
		client2.pushData((void*)t7.data(), n);
	});
	t71.join();
	std::string t8(n, '8');
	std::thread t81([&client2, &t8, &n]() {
		client2.pushData((void*)t8.data(), n);
	});
	t81.join();

	std::string t9(n, '9');
	std::thread t91([&client2, &t9, &n]() {
		client2.pushData((void*)t9.data(), n);
	});
	t91.join();

	std::string t10(n, '0');
	std::thread t101([&client2, &t10, &n]() {
		client2.pushData((void*)t10.data(), n);
	});

	t101.join();
	std::string t11(n, '1');
	std::thread t111([&client2, &t11, &n]() {
		client2.pushData((void*)t11.data(), n);
	});
	t111.join();


	std::this_thread::sleep_for(std::chrono::seconds(9));


	client2.stopWrite();

	a += a0;
	a += b;
	a += c;
	a += d;
	a += e;
	a += f;
	a += t7;
	a += t8;
	a += t9;



	std::cout << "клиент завершил отправку" << std::endl;

	return 0;
}
