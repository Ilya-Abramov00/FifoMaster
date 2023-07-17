#include "gtest/gtest.h"

#include "Fifo/Fifo.h"

using namespace std;

TEST(big_data, 1)
{
	std::string FIFO2 = "fifo2";
	int n             = 1024 * 1024;
	std::string data  = "";
	data.reserve(n * 12);
	auto getterRead = [&](void* dataq, size_t szInBytes) {
		data += std::string((char*)dataq, (char*)dataq + szInBytes);
	};

	Params params = {
	    FIFO2,
	    10,
	    0,
	    getterRead,
	};

	FifoRead client1(params);
	FifoWrite client2(FIFO2);

	client1.startRead();
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

	sleep(3);

	client2.stopWrite();
	client1.stopRead();

	a = a0 + a;
	a += b;
	a += c;
	a += d;
	a += e;
	a += f;
	a += t7;
	a += t8;
	a += t9;
	a += t10;
	a += t11;

	ASSERT_TRUE(data.size() == n * 12);
	ASSERT_TRUE(data == a);
	client1.stopRead();
}
TEST(big_data, 2)
{
	std::string FIFO2 = "fifo2";
	int n             = 1024 * 1024;
	std::string data  = "";
	data.reserve(n * 12);
	auto getterRead = [&](void* dataq, size_t szInBytes) {
		data += std::string((char*)dataq, (char*)dataq + szInBytes);
	};

	Params params = {
	    FIFO2,
	    1024,
	    0,
	    getterRead,
	};

	FifoRead client1(params);
	FifoWrite client2(FIFO2);

	client1.startRead();
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

	sleep(3);

	client2.stopWrite();
	client1.stopRead();

	a = a0 + a;
	a += b;
	a += c;
	a += d;
	a += e;
	a += f;
	a += t7;
	a += t8;
	a += t9;
	a += t10;
	a += t11;

	ASSERT_TRUE(data.size() == n * 12);
	ASSERT_TRUE(data == a);
	client1.stopRead();
}
TEST(big_data, sequential_write)
{
	std::string FIFO2 = "fifo2";
	int n             = 1024 * 1024;
	std::string data  = "";
	data.reserve(n * 12);
	auto getterRead = [&](void* dataq, size_t szInBytes) {
		data += std::string((char*)dataq, (char*)dataq + szInBytes);
	};

	Params params = {
	    FIFO2,
	    1024,
	    0,
	    getterRead,
	};

	FifoRead client1(params);
	FifoWrite client2(FIFO2);

	client1.startRead();
	client2.startWrite();

	std::string a0(n, '0');

	client2.pushData((void*)a0.data(), n);

	std::string a(n, '1');

	client2.pushData((void*)a.data(), n);

	std::string b(n, '2');

	client2.pushData((void*)b.data(), n);

	std::string c(n, '3');

	client2.pushData((void*)c.data(), n);

	std::string d(n, '4');

	client2.pushData((void*)d.data(), n);

	std::string e(n, '5');

	client2.pushData((void*)e.data(), n);

	std::string f(n, '6');

	client2.pushData((void*)f.data(), n);

	std::string t7(n, '7');

	client2.pushData((void*)t7.data(), n);

	std::string t8(n, '8');

	client2.pushData((void*)t8.data(), n);

	std::string t9(n, '9');

	client2.pushData((void*)t9.data(), n);

	std::string t10(n, '0');

	client2.pushData((void*)t10.data(), n);

	std::string t11(n, '1');
	client2.pushData((void*)t11.data(), n);

	sleep(3);

	client2.stopWrite();
	client1.stopRead();

	a = a0 + a;
	a += b;
	a += c;
	a += d;
	a += e;
	a += f;
	a += t7;
	a += t8;
	a += t9;
	a += t10;
	a += t11;

	ASSERT_TRUE(data.size() == n * 12);
	ASSERT_TRUE(data == a);
	client1.stopRead();
}