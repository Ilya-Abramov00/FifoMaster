#ifndef FIFO_BASE_H
#define FIFO_BASE_H
#include <functional>
#include <string>
#include <unistd.h>


#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IRUSR)
namespace Ipc {

using ConnectionHandler = std::function<void()>;

long openFifo(const std::string fdFileName, const char flag);

void createFifo(const std::string fdFileName);
} // namespace Ipc
#endif
