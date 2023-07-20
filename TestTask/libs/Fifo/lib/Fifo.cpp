#include "Fifo/Fifo.h"

// FifoException::FifoException(std::string const& msg) : runtime_error(msg.c_str())
//{}
//
// FifoWriteException::FifoWriteException(const std::string& msg) : FifoException(msg)
//{}
// FifoReadException::FifoReadException(const std::string& msg) : FifoException(msg)
//{}
// FifoAniException::FifoAniException(const std::string& msg) : FifoException(msg)
//{}
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/signal.h>
FifoRead::FifoRead(const Params& params) : params(params)
{
	createFifo();
}
void FifoRead::createFifo()
{
	if((mkfifo(params.addrRead.c_str(), FILE_MODE) < 0) && (errno != EEXIST)) {
		std::cout << params.addrRead.c_str() << '\n';
		throw std::runtime_error(" fail createFifo ");
	}
}

long FifoRead::openFifoRead()
{
	long fd = open(params.addrRead.c_str(), O_RDONLY, 0);
	if(-1 == fd) {
		throw std::runtime_error("fail openFifoRead");
	}
	return fd;
}

void FifoRead::startRead()
{
	runRead               = true;
	threadWaitConnectFifo = std::make_unique<std::thread>(std::thread([this]() {
		waitConnectFifo();
	}));
}
void FifoRead::waitConnectFifo()
{
	fifoReadFd = openFifoRead();
	params.connectHandler(); // соединение проиошло
	waitConnect    = true;
	threadReadFifo = std::make_unique<std::thread>(std::thread([this]() {
		readFifo();
	}));
}

FifoRead::~FifoRead()
{
	threadWaitConnectFifo->detach();
	if(waitConnect) {
		threadReadFifo->join();
	}
	unlink(params.addrRead.c_str());
}

void FifoRead::readFifo()
{
	std::vector<uint8_t> buffer(1024);
	while(runRead) {
		auto flag = read(fifoReadFd, buffer.data(), 1024);
		if(flag == 0) {
			break;
		}
		if(flag == 1024) {
			params.msgHandler(std::move(buffer));
		}
		else {
			params.msgHandler(std::vector<uint8_t>(buffer.data(), buffer.data() + flag));
		}
	}
}

void FifoRead::stopRead()
{
	runRead = false;
	close(fifoReadFd);
}

FifoWrite::FifoWrite(std::string fdFileName) : fdFileName(fdFileName)
{
	createFifo();
}
long FifoWrite::openFifoWrite()
{
	long fd = open(fdFileName.c_str(), O_WRONLY, 0);
	if(-1 == fd) {
		throw std::runtime_error("fail openFifoWrite");
	}
	return fd;
}

void FifoWrite::createFifo()
{
	if((mkfifo(fdFileName.c_str(), FILE_MODE) < 0) && (errno != EEXIST)) {
		std::cout << fdFileName.c_str() << '\n';
		throw std::runtime_error(" fail createFifo ");
	}
}

void FifoWrite::startWrite()
{
	runWrite = true;

	threadWaitConnectFifo = std::make_unique<std::thread>(std::thread([this]() {
		waitConnectFifo();
	}));
}

void FifoWrite::waitConnectFifo()
{
	fifoFd = openFifoWrite();
	// соединение проиошло

	waitConnect     = true;
	threadWriteFifo = std::make_unique<std::thread>(std::thread([this]() {
		writeFifo();
	}));
}
void FifoWrite::stopWrite()
{
	runWrite = false;
	close(fifoFd);
	threadWaitConnectFifo->detach();

	if(waitConnect) {
		threadWriteFifo->join();
	}
}
ssize_t safe_write(int fd, const void* buf, size_t bufsz)
{
	sigset_t sig_block, sig_restore, sig_pending;

	sigemptyset(&sig_block);
	sigaddset(&sig_block, SIGPIPE);

	/* Block SIGPIPE for this thread.
	 *
	 * This works since kernel sends SIGPIPE to the thread that called write(),
	 * not to the whole process.
	 */
	if(pthread_sigmask(SIG_BLOCK, &sig_block, &sig_restore) != 0) {
		return -1;
	}

	/* Check if SIGPIPE is already pending.
	 */
	int sigpipe_pending = -1;
	if(sigpending(&sig_pending) != -1) {
		sigpipe_pending = sigismember(&sig_pending, SIGPIPE);
	}

	if(sigpipe_pending == -1) {
		pthread_sigmask(SIG_SETMASK, &sig_restore, NULL);
		return -1;
	}

	ssize_t ret;
	while((ret = write(fd, buf, bufsz)) == -1) {
		if(errno != EINTR)
			break;
	}

	/* Fetch generated SIGPIPE if write() failed with EPIPE.
	 *
	 * However, if SIGPIPE was already pending before calling write(), it was also
	 * generated and blocked by caller, and caller may expect that it can fetch it
	 * later. Since signals are not queued, we don't fetch it in this case.
	 */
	if(ret == -1 && errno == EPIPE && sigpipe_pending == 0) {
		struct timespec ts;
		ts.tv_sec  = 0;
		ts.tv_nsec = 0;

		int sig;
		while((sig = sigtimedwait(&sig_block, 0, &ts)) == -1) {
			if(errno != EINTR)
				break;
		}
	}

	pthread_sigmask(SIG_SETMASK, &sig_restore, NULL);
	return ret;
}
void FifoWrite::writeFifo()
{
	while(runWrite) {
		std::lock_guard<std::mutex> mtx_0(mtx);
		if(!queue.empty()) {
			signal(SIGPIPE, SIG_IGN); // отлавливает сигнал в случае закрытия канала на чтение
			auto flag = write(fifoFd, queue.front().data(), queue.front().size());
			if(flag == -1) {
				runWrite = false;
				std::cerr << "\n принимающая сторона закрыла канал \n";
			}
			queue.pop();
		}
	}
}

void FifoWrite::pushData(void* data, size_t sizeN)
{
	if(!data) {
		std::cerr << "\n null ptr is pushData \n";
		return;
	}
	auto ptr = reinterpret_cast<uint8_t*>(data);
	std::vector<uint8_t> buffer(ptr, ptr + sizeN);
	std::lock_guard<std::mutex> mtx_0(mtx);
	queue.push(std::move(buffer));
}
FifoWrite::~FifoWrite()
{}

Fifo::Fifo(Params params) : FifoRead(params), FifoWrite(params.addrRead + "wd")
{}
