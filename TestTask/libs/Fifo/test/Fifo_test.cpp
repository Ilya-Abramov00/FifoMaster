#include <gtest/gtest.h>

#include "Fifo/Fifo.h"

using namespace std;

// TEST(FifoRead, read)
//{
//	std::string FIFO1 = "fifo1";
//	std::string FIFO2 = "fifo2";
//
//	FifoRead client2(FIFO2);
//	client2.startRead();
//
//	std::string data = "";
//
//	client2.readFifo(data, 20);
// }
// TEST(FifoWrite, write)
//{
//	std::string FIFO1 = "fifo1";
//	std::string FIFO2 = "fifo2";
//
//	std::list<string> ret = {"adwd", "defwefwef", "cdsacadcsd", "", ""};
//	auto getter           = [&]() {
//         sleep(1);
//         ret.pop_front();
//
//         return *ret.begin();
//	};
//
//	FifoWrite client2(FIFO2);
//	client2.setMsgGetter(getter);
//	client2.startWrite();
//
//	client2.writeFifo();
// }
using namespace std;
// TEST(Fifo, 3)
//{
//	const char* FIFO = "/home/ilya/Fifo/fifo";
//
//	std::string ret = "0123456";
//	auto getter     = [&ret]() {
//         return std::pair((void*)ret.data(), ret.size());
//	};
//
//	std::pair<void*, size_t> temporaryBuffer = getter();
//
//	auto ptr = reinterpret_cast<uint8_t*>(temporaryBuffer.first);
//
//	auto buffer = std::vector<uint8_t>(ptr, ptr + temporaryBuffer.second);
//
//	std::thread t2([&]() {
//		mkfifo(FIFO, FILE_MODE);
//		int fd = open(FIFO, O_WRONLY, 0);
//		write(fd, buffer.data(), temporaryBuffer.second);
//		sleep(2);
//		close(fd);
//	});
//
//	uint8_t* read_buffer = new uint8_t[temporaryBuffer.second];
//	std::thread t1([&]() {
//		int fd1 = open(FIFO, O_RDONLY, 0);
//		read(fd1, read_buffer, temporaryBuffer.second);
//	});
//
//	sleep(2);
//	vector<uint8_t> a(read_buffer, read_buffer + temporaryBuffer.second);
//
//	string b;
//	for(int i = 0; i != temporaryBuffer.second; i++) {
//		// auto c=reinterpret_cast<char *>(&read_buffer[i]);
//		b.push_back((char)read_buffer[i]);
//	}
//
//	ASSERT_TRUE(buffer == a);
//
//	ASSERT_TRUE(ret == b);
//
//	t1.join();
//	t2.join();
// }
//TEST(Fifo, 0)
//{
//	const char* FIFO = "/home/ilya/Fifo/fifo";
//
//	std::string ret = "0123456";
//	auto getter     = [&ret]() {
//        return std::pair(ret, ret.size());
//	};
//
//	std::pair<std::string, size_t> temporaryBuffer = getter();
//
//	auto ptr = reinterpret_cast<uint8_t*>(temporaryBuffer.first.data());
//
//	auto buffer = std::vector<uint8_t>(ptr, ptr + temporaryBuffer.second);
//
//	std::thread t2([&]() {
//		mkfifo(FIFO, FILE_MODE);
//		int fd = open(FIFO, O_WRONLY, 0);
//		write(fd, buffer.data(), temporaryBuffer.second);
//		sleep(2);
//		close(fd);
//	});
//
//	uint8_t* read_buffer = new uint8_t[temporaryBuffer.second];
//	std::thread t1([&]() {
//		int fd1 = open(FIFO, O_RDONLY, 0);
//		read(fd1, read_buffer, temporaryBuffer.second);
//	});
//
//	sleep(2);
//	vector<uint8_t> a(read_buffer, read_buffer + temporaryBuffer.second);
//
//	string b;
//	for(int i = 0; i != temporaryBuffer.second; i++) {
//		// auto c=reinterpret_cast<char *>(&read_buffer[i]);
//		b.push_back((char)read_buffer[i]);
//	}
//
//	ASSERT_TRUE(buffer == a);
//
//	ASSERT_TRUE(ret == b);
//
//	t1.join();
//	t2.join();
//}
TEST(Fifo, 1)
{
	std::string FIFO2 = "/home/ilya/Fifo/fifo2";

	std::string data = "";

	auto getterRead = [&](void* dataq, size_t szInBytes) {
		data += std::string((char*)dataq, (char*)dataq + szInBytes);
	};

	int i = 0;
	std::string a(10, '*');
	auto getter = [&i, &a]() {
		i++;
		return std::pair((void*)a.data(), 10);
	};

	Params params = {
	    FIFO2,
	    10,
	    20,
	    getterRead,
	};

	std::mutex mtx;

	FifoRead client1(params);

	FifoWrite client2(FIFO2,mtx);

	client2.startWrite();
	client2.writeUser(getter);
	client1.startRead();

	sleep(1);
	client2.stopWriteUser();
	sleep(1);
	client2.stopWrite();
	sleep(3);
	client1.stopRead();
	std::cout << "\n\nсчиталось  " << data.size() << endl;
	std::cout << i * 10 << endl;

	ASSERT_TRUE(data.size() == 10 * i);
	std::string b(10*i, '*');
	for(int i = 0; i != data.size(); i++) {
		ASSERT_TRUE(data[i] == b[i]);
	}
}

//TEST(Fifo, 2)
//{
//	std::string FIFO2 = "/home/ilya/Fifo/fifo2";
//
//	std::string data = "";
//
//	auto getterRead = [&](void* dataq, size_t szInBytes) {
//		data += std::string((char*)dataq, (char*)dataq + szInBytes);
//	};
//
//	int i = 0;
//	std::string a(10, '*');
//	auto getter = [&i, &a]() {
//		i++;
//		return std::pair((void*)a.data(), 10);
//	};
//
//	Params params = {
//	    FIFO2,
//	    4,
//	    10,
//	    getterRead,
//	};
//
//	std::mutex mtx;
//
//	FifoRead client1(params);
//
//	FifoWrite client2(FIFO2,mtx);
//
//	client2.setMsgGetter(getter);
//
//	client2.startWrite();
//	client1.startRead();
//
//	sleep(1);
//	client2.stopWrite();
//	sleep(3);
//	client1.stopRead();
//	std::cout << "\n\nсчиталось  " << data.size() << endl;
//	std::cout << i * 10 << endl;
//
//	ASSERT_TRUE(data.size() == 10 * i);
//	std::string b(10*i, '*');
//	for(int i = 0; i != data.size(); i++) {
//		ASSERT_TRUE(data[i] == b[i]);
//	}
//}
//
//TEST(Fifo,3)
//{
//	std::string FIFO2 = "/home/ilya/Fifo/fifo2";
//
//	std::string data = "";
//
//	auto getterRead = [&](void* dataq, size_t szInBytes) {
//		data += std::string((char*)dataq, (char*)dataq + szInBytes);
//	};
//
//	int i = 0;
//	std::string a(10, '*');
//	auto getter = [&i, &a]() {
//		i++;
//		return std::pair((void*)a.data(), 10);
//	};
//
//	Params params = {
//	    FIFO2,
//	    11,
//	    10,
//	    getterRead,
//	};
//
//	std::mutex mtx;
//
//	FifoRead client1(params);
//
//	FifoWrite client2(FIFO2,mtx);
//
//	client2.setMsgGetter(getter);
//
//	client2.startWrite();
//	client1.startRead();
//
//	sleep(1);
//	client2.stopWrite();
//	sleep(3);
//	client1.stopRead();
//	std::cout << "\n\nсчиталось  " << data.size() << endl;
//	std::cout << i * 10 << endl;
//
//	ASSERT_TRUE(data.size() == 10 * i);
//	std::string b(10*i, '*');
//	for(int i = 0; i != data.size(); i++) {
//		ASSERT_TRUE(data[i] == b[i]);
//	}
//}