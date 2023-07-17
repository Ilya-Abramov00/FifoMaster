
#include <thread>
#include "Fifo/Fifo.h"

std::string FIFO1 = "/home/ilya/Fifo/fifo1";
std::string FIFO2 = "/home/ilya/Fifo/fifo2";

int main()
{
	std::cout << "клиент" << std::endl << std::endl;

	std::string FIFO2 = "/home/ilya/Fifo/fifo2";
	int n             = 1024*10;

	FifoWrite client2(FIFO2);

	std::string a0(n, '0');
	std::thread t([&client2, &a0, &n]() {
		client2.writeUser((void*)a0.data(), n);
	});

	std::string a(n, '1');
	std::thread t1([&client2, &a, &n]() {
		client2.writeUser((void*)a.data(), n);
	});

	std::string b(n, '2');
	std::thread t2([&client2, &b, &n]() {
		client2.writeUser((void*)b.data(), n);
	});

	std::string c(n, '3');
	std::thread t3([&client2, &c, &n]() {
		client2.writeUser((void*)c.data(), n);
	});

	std::string d(n, '4');
	std::thread t4([&client2, &d, &n]() {
		client2.writeUser((void*)d.data(), n);
	});

	std::string e(n, '5');
	std::thread t5([&client2, &e, &n]() {
		client2.writeUser((void*)e.data(), n);
	});

	std::string f(n, '6');
	std::thread t6([&client2, &f, &n]() {
		client2.writeUser((void*)f.data(), n);
	});

	std::string t7(n, '7');
	std::thread t71([&client2, &t7, &n]() {
		client2.writeUser((void*)t7.data(), n);
	});

	std::string t8(n, '8');
	std::thread t81([&client2, &t8, &n]() {
		client2.writeUser((void*)t8.data(), n);
	});

	std::string t9(n, '9');
	std::thread t91([&client2, &t9, &n]() {
		client2.writeUser((void*)t9.data(), n);
	});

	client2.startWrite();

	sleep(20);

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

	t.join();
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();
	t6.join();
	t71.join();
	t81.join();
	t91.join();

	std::cout << "клиент завершил отправку" << std::endl;

	return 0;
}
