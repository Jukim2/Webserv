#include "File/Directory.hpp"

#include <iomanip>

using namespace Hafserv;

const std::string Directory::htmlHeader =
	"<html lang=\"en\">"
	"<head>"
	"    <meta charset=\"UTF-8\">"
	"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
	"    <title>Index of /directory</title>"
	"    <style>"
	"        body {"
	"            font-family: Arial, sans-serif;"
	"            margin: 20px;"
	"            padding: 0;"
	"        }"
	"        h1 {"
	"            font-size: 24px;"
	"        }"
	"        table {"
	"            width: 100%;"
	"            border-collapse: collapse;"
	"        }"
	"        th, td {"
	"            padding: 8px;"
	"            text-align: left;"
	"        }"
	"        th {"
	"            background-color: #f2f2f2;"
	"        }"
	"        tr:nth-child(even) {"
	"            background-color: #f2f2f2;"
	"        }"
	"        a {"
	"            color: #007bff;"
	"            text-decoration: none;"
	"        }"
	"        a:hover {"
	"            text-decoration: underline;"
	"        }"
	"    </style>"
	"</head>";

Directory::Directory() : File(), contents() {}

Directory::Directory(const Directory &other) : File(other), contents(other.contents) {}

Directory::Directory(const std::string &name) : File(name), contents()
{
	if (error())
		return;
	else if (!isDirectory())
	{
		errorCode = FILE_TYPE_NOT_MATCHING;
		return;
	}

	DIR *dir = opendir(name.c_str());
	if (dir == NULL)
	{
		errorCode = FILE_UNKNOWN;
		return;
	}
	for (struct dirent *dp = readdir(dir); dp != NULL; dp = readdir(dir))
	{
		std::string curr = "./" + name + "/" + std::string(dp->d_name);
		File file = File(curr);
		contents.push_back(std::make_pair(dp->d_name, file.getFileStat()));
	}

	closedir(dir);
}

Directory &Directory::operator=(const Directory &other)
{
	if (this != &other)
	{
		File::operator=(other);
		name = other.name;
		contents = other.contents;
	}
	return *this;
}

Directory::~Directory() {}

const std::vector<std::pair<std::string, struct stat> > Directory::getContents() const { return contents; }

const std::string Directory::toHtml() const
{
	std::string html;
	html.append("<!DOCTYPE html>");
	html.append(htmlHeader);
	html.append("<body><h1>Index of " + name + "</h1>");
	html.append("<table><thead><tr><th>Name</th><th>Last Modified</th><th>Size</th></tr></thead><tbody>");
	for (size_t i = 1; i < contents.size(); i++)
	{
		html.append("<tr>");
		html.append("<td><a href=\"" + contents[i].first + (S_ISDIR(contents[i].second.st_mode) ? "/" : "") + "\">" +
					contents[i].first + (S_ISDIR(contents[i].second.st_mode) ? "/" : "") + "</a></td>");
		html.append("<td>January 1, 2024</td>");
		html.append("<td>" + util::string::itos((int)contents[i].second.st_size) + "bytes</td>");
		html.append("</tr>");
	}
	html.append("</tbody></table></body></html>");
	return html;
}

std::ostream &operator<<(std::ostream &os, const Directory &file)
{
	os << File(file) << std::endl;
	if (file.valid())
	{
		std::cout << "count: " << file.getContents().size() << std::endl;
		for (size_t i = 0; i < file.getContents().size(); i++)
		{
			os << std::left << std::setw(20) << file.getContents()[i].first << "\t";
			os << std::setw(10) << file.getContents()[i].second.st_size << "\t";
			os << std::setw(10) << file.getContents()[i].second.st_mode << std::endl;
		}
		return os;
	}
	return os;
}
