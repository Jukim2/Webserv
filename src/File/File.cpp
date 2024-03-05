#include "File/File.hpp"

#include <fcntl.h>

using namespace Hafserv;

File::File() : errorCode(FILE_NOT_EXIST), name(""), fileStat() {}

File::File(const File &other) : errorCode(other.errorCode), name(other.name), fileStat(other.fileStat) {}

File::File(const std::string &name)
	: errorCode((access(name.c_str(), F_OK) == 0 ? FILE_OK : FILE_NOT_EXIST)), name(name), fileStat()
{
	if (exist())
		stat(name.c_str(), &fileStat);
}

File &File::operator=(const File &other)
{
	if (this != &other)
	{
		errorCode = other.errorCode;
		name = other.name;
		fileStat = other.fileStat;
	}
	return *this;
}

File::~File() {}

bool File::exist() const { return errorCode != FILE_NOT_EXIST; }

bool File::error() const { return errorCode != FILE_OK; }

bool File::valid() const { return !error(); }

unsigned short File::getErrorCode() const { return errorCode; }

const std::string File::getErrorMsg() const
{
	switch (errorCode)
	{
	case FILE_NOT_EXIST:
		return F_ERR_MSG_NOT_EXIST;
	case FILE_NO_PERMISSION:
		return F_ERR_MSG_NO_PERMISSION;
	case FILE_TYPE_NOT_MATCHING:
		return F_ERR_MSG_TYPE_NOT_MATCHING;
	case FILE_UNKNOWN:
		return F_ERR_MSG_UNKNOWN;
	default:
		return F_ERR_MSG_OK;
	}
}

const std::string &File::getName() const { return name; }

const struct stat &File::getFileStat() const { return fileStat; }

const mode_t &File::getFileMode() const { return fileStat.st_mode; }

const off_t &File::getFileSize() const { return fileStat.st_size; }

bool File::isDirectory() const { return exist() && S_ISDIR(fileStat.st_mode); }

bool File::isRegularFile() const { return exist() && S_ISREG(fileStat.st_mode); }

bool File::isSocket() const { return exist() && S_ISSOCK(fileStat.st_mode); }

bool File::isReadable() const { return exist() && (fileStat.st_mode & S_IRUSR); }

bool File::isWritable() const { return exist() && (fileStat.st_mode & S_IWUSR); }

bool File::isExecutable() const { return exist() && (fileStat.st_mode & S_IXUSR); }

std::ostream &operator<<(std::ostream &os, const File &file)
{
	os << "error code: " << file.getErrorMsg() << std::endl;
	os << "name: " << file.getName();
	return os;
}
