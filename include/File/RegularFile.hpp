#ifndef REGULARFILE_HPP
#define REGULARFILE_HPP

#include "File/File.hpp"

namespace Hafserv
{
class RegularFile : public File
{
public:
	RegularFile();
	RegularFile(const RegularFile &other);
	RegularFile(const std::string &name);
	RegularFile &operator=(const RegularFile &other);
	virtual ~RegularFile();

	const std::string &getContents() const;
	size_t getContentsSize() const;
	const std::string &getExtension() const;

protected:
	std::string extension;
	std::string contents;
};

} // namespace Hafserv

std::ostream &operator<<(std::ostream &os, const Hafserv::RegularFile &file);

#endif
