#ifndef DIRECTORY_HPP
#define DIRECTORY_HPP

#include "File/File.hpp"
#include "util/string.hpp"

#include <dirent.h>
#include <iostream>
#include <utility>
#include <vector>

namespace Hafserv
{
class Directory : public File
{
public:
	Directory();
	Directory(const Directory &other);
	Directory(const std::string &filename);
	Directory &operator=(const Directory &other);
	virtual ~Directory();

	const std::vector<std::pair<std::string, struct stat> > getContents() const;
	const std::string toHtml() const;

private:
	static const std::string htmlHeader;

	std::vector<std::pair<std::string, struct stat> > contents;
};

} // namespace Hafserv

std::ostream &operator<<(std::ostream &os, const Hafserv::Directory &file);

#endif
