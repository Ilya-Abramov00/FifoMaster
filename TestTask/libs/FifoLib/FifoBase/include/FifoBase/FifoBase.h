#ifndef FIFO_BASE_H
#define FIFO_BASE_H
#include <functional>

// class FifoException : public std::runtime_error {
// public:
//	FifoException(std::string const& msg);
// };
//
// class FifoWriteException : FifoException {
// public:
//	FifoWriteException(std::string const& msg);
// };
//
// class FifoReadException : FifoException {
// public:
//	FifoReadException(std::string const& msg);
// };
// class FifoAniException : FifoException {
// public:
//	FifoAniException(std::string const& msg);
// };

#include <memory>
#include <unistd.h>
#include <sys/signal.h>

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IRUSR)
namespace Ipc {

using ConnectionHandler = std::function<void()>;

long openFifo(const std::string fdFileName, const char flag);

void createFifo(const std::string fdFileName);
} // namespace Ipc
#endif
