/*
#include <gtest/gtest.h>

#include "Fifo/Fifo.h"

using namespace Ipc;

class WriterFactory {
public:
	static std::unique_ptr<IFifoWriter> create(std::string filename, Config conf)
	{
		switch(conf) {
		case(Config::QW):
			return std::make_unique<QWriteImpl>( (filename));
		case(Config::NQW):
			return std::make_unique<NQWriteImpl>( (filename));
		}
	}
};
	TEST(Fifo_many_potoc, 1)
	{
		using namespace std;
		using namespace Ipc;
		std::string FIFO2 = "fifo2";
		int n             = 10;
		std::string data  = "";
		data.reserve(n * 12);
		auto getterRead = [&](FifoRead::Data&& dataq) {
			data.insert(data.end(), dataq.data(), dataq.data() + dataq.size());
		};
		auto connect = []() {

		};
		Fifo client2(WriterFactory::create(FIFO2, Ipc::Config::QW), FIFO2);

		client2.setReadHandler(getterRead);

		client2.setConnectionHandlerRead(connect);
		client2.setConnectionHandlerWrite(connect);

		client2.setDisconnectionHandlerRead(connect);
		client2.setDisconnectionHandlerWrite(connect);

		client2.start();

		sleep(1);
		std::string a0(n, '0');
		std::thread t([&client2, &a0, &n]() {
			client2.write((void*)a0.data(), n);
		});

		std::string a(n, '1');
		std::thread t1([&client2, &a, &n]() {
			client2.write((void*)a.data(), n);
		});

		std::string b(n, '2');
		std::thread t2([&client2, &b, &n]() {
			client2.write((void*)b.data(), n);
		});

		std::string c(n, '3');
		std::thread t3([&client2, &c, &n]() {
			client2.write((void*)c.data(), n);
		});

		std::string d(n, '4');
		std::thread t4([&client2, &d, &n]() {
			client2.write((void*)d.data(), n);
		});

		std::string e(n, '5');
		std::thread t5([&client2, &e, &n]() {
			client2.write((void*)e.data(), n);
		});

		std::string f(n, '6');
		std::thread t6([&client2, &f, &n]() {
			client2.write((void*)f.data(), n);
		});

		std::string t7(n, '7');
		std::thread t71([&client2, &t7, &n]() {
			client2.write((void*)t7.data(), n);
		});

		std::string t8(n, '8');
		std::thread t81([&client2, &t8, &n]() {
			client2.write((void*)t8.data(), n);
		});

		std::string t9(n, '9');
		std::thread t91([&client2, &t9, &n]() {
			client2.write((void*)t9.data(), n);
		});

		std::string t10(n, '0');
		std::thread t101([&client2, &t10, &n]() {
			client2.write((void*)t10.data(), n);
		});

		std::string t11(n, '1');
		std::thread t111([&client2, &t11, &n]() {
			client2.write((void*)t11.data(), n);
		});

		sleep(10);

		client2.stop();

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
		t101.join();
		t111.join();

		ASSERT_TRUE(data.size() == n * 12);
		// вызовы вункции pushwrite перемешаны, пачки данных  перемешаться
	}

	TEST(Fifo_many_potoc, 2)
	{
		using namespace std;
		using namespace Ipc;

		std::string FIFO2 = "fifo2";
		int n             = 10 * 1024;
		std::string data  = "";
		data.reserve(n * 12);
		auto getterRead = [&](FifoRead::Data&& dataq) {
			data.insert(data.end(), dataq.data(), dataq.data() + dataq.size());
		};
		auto connect = []() {

		};
		Fifo client2(WriterFactory::create(FIFO2, Ipc::Config::NQW), FIFO2);

		client2.setReadHandler(getterRead);

		client2.setConnectionHandlerRead(connect);
		client2.setConnectionHandlerWrite(connect);

		client2.setDisconnectionHandlerRead(connect);
		client2.setDisconnectionHandlerWrite(connect);

		client2.start();

		sleep(1);
		std::string a0(n, '0');

		std::thread t([&client2, &a0, &n]() {
			client2.write((void*)a0.data(), n);
		});

		std::string a(n, '1');
		std::thread t1([&client2, &a, &n]() {
			client2.write((void*)a.data(), n);
		});

		std::string b(n, '2');
		std::thread t2([&client2, &b, &n]() {
			client2.write((void*)b.data(), n);
		});

		std::string c(n, '3');
		std::thread t3([&client2, &c, &n]() {
			client2.write((void*)c.data(), n);
		});

		std::string d(n, '4');
		std::thread t4([&client2, &d, &n]() {
			client2.write((void*)d.data(), n);
		});

		std::string e(n, '5');
		std::thread t5([&client2, &e, &n]() {
			client2.write((void*)e.data(), n);
		});

		std::string f(n, '6');
		std::thread t6([&client2, &f, &n]() {
			client2.write((void*)f.data(), n);
		});

		std::string t7(n, '7');
		std::thread t71([&client2, &t7, &n]() {
			client2.write((void*)t7.data(), n);
		});

		std::string t8(n, '8');
		std::thread t81([&client2, &t8, &n]() {
			client2.write((void*)t8.data(), n);
		});

		std::string t9(n, '9');
		std::thread t91([&client2, &t9, &n]() {
			client2.write((void*)t9.data(), n);
		});

		std::string t10(n, '0');
		std::thread t101([&client2, &t10, &n]() {
			client2.write((void*)t10.data(), n);
		});

		std::string t11(n, '1');
		std::thread t111([&client2, &t11, &n]() {
			client2.write((void*)t11.data(), n);
		});

	    sleep(10);

		client2.stop();

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
		t101.join();
		t111.join();

		ASSERT_TRUE(data.size() == n * 12);
	}
	TEST(Fifo_many_potoc, 3)
	{
		using namespace std;
		using namespace Ipc;
		std::string FIFO2 = "fifo2";
		int n             = 10 * 1024;
		std::string data  = "";
		data.reserve(n * 12);
		auto getterRead = [&](FifoRead::Data&& dataq) {
			data.insert(data.end(), dataq.data(), dataq.data() + dataq.size());
		};
		auto connect = []() {

		};
		Fifo client2(WriterFactory::create(FIFO2, Ipc::Config::NQW), FIFO2);

		client2.setReadHandler(getterRead);

		client2.setConnectionHandlerRead(connect);
		client2.setConnectionHandlerWrite(connect);

		client2.setDisconnectionHandlerRead(connect);
		client2.setDisconnectionHandlerWrite(connect);

		client2.start();

		sleep(2);
		std::string a0(n, '0');
		std::thread t([&client2, &a0, &n]() {
			client2.write((void*)a0.data(), n);
		});

		std::string a(n, '1');
		std::thread t1([&client2, &a, &n]() {
			client2.write((void*)a.data(), n);
		});

		std::string b(n, '2');
		std::thread t2([&client2, &b, &n]() {
			client2.write((void*)b.data(), n);
		});

		std::string c(n, '3');
		std::thread t3([&client2, &c, &n]() {
			client2.write((void*)c.data(), n);
		});

		std::string d(n, '4');
		std::thread t4([&client2, &d, &n]() {
			client2.write((void*)d.data(), n);
		});

		std::string e(n, '5');
		std::thread t5([&client2, &e, &n]() {
			client2.write((void*)e.data(), n);
		});

		std::string f(n, '6');
		std::thread t6([&client2, &f, &n]() {
			client2.write((void*)f.data(), n);
		});

		std::string t7(n, '7');
		std::thread t71([&client2, &t7, &n]() {
			client2.write((void*)t7.data(), n);
		});

		std::string t8(n, '8');
		std::thread t81([&client2, &t8, &n]() {
			client2.write((void*)t8.data(), n);
		});

		std::string t9(n, '9');
		std::thread t91([&client2, &t9, &n]() {
			client2.write((void*)t9.data(), n);
		});

		std::string t10(n, '0');
		std::thread t101([&client2, &t10, &n]() {
			client2.write((void*)t10.data(), n);
		});

		std::string t11(n, '1');
		std::thread t111([&client2, &t11, &n]() {
			client2.write((void*)t11.data(), n);
		});

	    sleep(10);

		client2.stop();

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
		t101.join();
		t111.join();

		ASSERT_TRUE(data.size() == n * 12);
		// вызовы вункции pushwrite перемешаны, пачки данных могли перемешаться
	}
*/
