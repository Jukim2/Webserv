#ifndef FILE_HPP
#define FILE_HPP

#include <iostream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define F_ERR_MSG_OK "file is okay"
#define F_ERR_MSG_NOT_EXIST "file does not exist."
#define F_ERR_MSG_NO_PERMISSION "file has no permission"
#define F_ERR_MSG_TYPE_NOT_MATCHING "file type is not matching"
#define F_ERR_MSG_UNKNOWN "file unknown error"

namespace Hafserv
{
class File
{
public:
	enum ErrorCode
	{
		FILE_OK = 0,
		FILE_NOT_EXIST = 1,
		FILE_NO_PERMISSION = 2,
		FILE_TYPE_NOT_MATCHING = 3,
		FILE_UNKNOWN = 4,
	};

	File();
	File(const File &other);
	File(const std::string &name);
	File &operator=(const File &other);
	virtual ~File();

	bool exist() const;
	bool error() const;
	bool valid() const;

	unsigned short getErrorCode() const;
	const std::string getErrorMsg() const;
	const std::string &getName() const;
	const struct stat &getFileStat() const;
	const mode_t &getFileMode() const;
	const off_t &getFileSize() const;

	bool isDirectory() const;
	bool isRegularFile() const;
	bool isSocket() const;

	bool isReadable() const;
	bool isWritable() const;
	bool isExecutable() const;

protected:
	unsigned short errorCode;
	std::string name;
	struct stat fileStat;
};

} // namespace Hafserv

std::ostream &operator<<(std::ostream &os, const Hafserv::File &file);

#endif
