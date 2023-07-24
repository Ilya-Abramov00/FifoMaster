#include "gtest/gtest.h"

#include "Fifo/Fifo.h"

using namespace std;

TEST(big_data_FIFO, 1)
{
	std::string FIFO2 = "fifo2";
	int n             = 1024 * 1024 * 120;
	std::string data  = "";
	data.reserve(n * 12);
	auto getterRead = [&](FifoRead::Data&& dataq) {
		data += std::string(dataq.data(), dataq.data() + dataq.size());
	};
	auto connect = []() {

	};

	Fifo client2(FIFO2, FIFO2);
	client2.setReadHandler(getterRead);

	client2.setConnectionHandlerRead(connect);
	client2.setConnectionHandlerWrite(connect);

	client2.setDisConnectionHandlerRead(connect);
	client2.setDisConnectionHandlerWrite(connect);

	client2.start();

	std::string a0(n, '0');

	client2.write((void*)a0.data(), n);

	std::string a(n, '1');

	client2.write((void*)a.data(), n);

	std::string b(n, '2');

	client2.write((void*)b.data(), n);

	std::string c(n, '3');

	client2.write((void*)c.data(), n);

	std::string d(n, '4');

	client2.write((void*)d.data(), n);

	std::string e(n, '5');

	client2.write((void*)e.data(), n);

	std::string f(n, '6');

	client2.write((void*)f.data(), n);

	std::string t7(n, '7');

	client2.write((void*)t7.data(), n);

	std::string t8(n, '8');

	client2.write((void*)t8.data(), n);

	std::string t9(n, '9');

	client2.write((void*)t9.data(), n);

	std::string t10(n, '0');

	client2.write((void*)t10.data(), n);

	std::string t11(n, '1');
	client2.write((void*)t11.data(), n);

	sleep(3);

	client2.stopRead();
	client2.stopWrite();

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
}
TEST(big_data_FIFO, 2)
{
	std::string FIFO2 = "fifo2";
	int n             = 1024 * 1024;
	std::string data  = "";
	data.reserve(n * 12);
	auto getterRead = [&](FifoRead::Data&& dataq) {
		data += std::string(dataq.data(), dataq.data() + dataq.size());
	};
	auto connect = []() {

	};
	Fifo client2(FIFO2, FIFO2);
	client2.setReadHandler(getterRead);

	client2.setConnectionHandlerRead(connect);
	client2.setConnectionHandlerWrite(connect);

	client2.setDisConnectionHandlerRead(connect);
	client2.setDisConnectionHandlerWrite(connect);

	client2.start();

	std::string a0(n, '0');

	client2.write((void*)a0.data(), n);

	std::string a(n, '1');

	client2.write((void*)a.data(), n);

	std::string b(n, '2');

	client2.write((void*)b.data(), n);

	std::string c(n, '3');

	client2.write((void*)c.data(), n);

	std::string d(n, '4');

	client2.write((void*)d.data(), n);

	std::string e(n, '5');

	client2.write((void*)e.data(), n);

	std::string f(n, '6');

	client2.write((void*)f.data(), n);

	std::string t7(n, '7');

	client2.write((void*)t7.data(), n);

	std::string t8(n, '8');

	client2.write((void*)t8.data(), n);

	std::string t9(n, '9');

	client2.write((void*)t9.data(), n);

	std::string t10(n, '0');

	client2.write((void*)t10.data(), n);

	std::string t11(n, '1');
	client2.write((void*)t11.data(), n);

	sleep(3);

	client2.stopWrite();
	client2.stopRead();

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
}
