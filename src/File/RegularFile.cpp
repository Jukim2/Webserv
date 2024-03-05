#include "File/RegularFile.hpp"

#include <fstream>
#include <sstream>

using namespace Hafserv;

RegularFile::RegularFile() : File(), extension(""), contents() {}

RegularFile::RegularFile(const RegularFile &other) : File(other), extension(other.extension), contents(other.contents)
{
}

RegularFile::RegularFile(const std::string &name) : File(name), extension(""), contents()
{
	if (error())
		return;
	else if (!isRegularFile())
	{
		errorCode = FILE_TYPE_NOT_MATCHING;
		return;
	}
	else if (!isReadable())
	{
		errorCode = FILE_NO_PERMISSION;
		return;
	}

	if (name.rfind('.') != std::string::npos)
		extension = name.substr(name.rfind('.') + 1, name.size() - name.rfind('.'));

	std::ifstream fs;
	fs.open(name.c_str(), std::fstream::binary);
	if (!fs.is_open())
	{
		errorCode = FILE_UNKNOWN;
		return;
	}
	std::stringstream ss;
	ss << fs.rdbuf();
	contents = ss.str();
	fs.close();
}

RegularFile &RegularFile::operator=(const RegularFile &other)
{
	if (this != &other)
	{
		File::operator=(other);
		extension = other.extension;
		contents = other.contents;
	}
	return *this;
}

RegularFile::~RegularFile() {}

const std::string &RegularFile::getContents() const { return contents; }

size_t RegularFile::getContentsSize() const { return contents.length(); }

const std::string &RegularFile::getExtension() const { return extension; }

std::ostream &operator<<(std::ostream &os, const RegularFile &file)
{
	os << File(file) << std::endl;
	if (file.valid())
	{
		std::cout << "Extension: " << file.getExtension() << std::endl;
		std::cout << "Contents-length: " << file.getContentsSize() << std::endl;
		std::cout << file.getContents();
	}
	return os;
}
