#include "FifoBase/FifoBase.h"

// FifoException::FifoException(std::string const& msg) : runtime_error(msg.c_str())
//{}
//
// FifoWriteException::FifoWriteException(const std::string& msg) : FifoException(msg)
//{}
// FifoReadException::FifoReadException(const std::string& msg) : FifoException(msg)
//{}
// FifoAniException::FifoAniException(const std::string& msg) : FifoException(msg)
#include <string>
#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>


long FifoBase::openFifo(const std::string fdFileName, const char flag)
{
	long fd = -1;
	if(flag == 'W') {
		fd = open(fdFileName.c_str(), O_WRONLY, 0);
	}
	else if(flag == 'R') {
		fd = open(fdFileName.c_str(), O_RDONLY, 0);
	}
	return fd;
}

void FifoBase::createFifo(const std::string fdFileName)
{
	if((mkfifo(fdFileName.c_str(), FILE_MODE) < 0) && (errno != EEXIST)) {
		std::cout << fdFileName.c_str() << '\n';
		throw std::runtime_error(" fail createFifo ");
	}
}
