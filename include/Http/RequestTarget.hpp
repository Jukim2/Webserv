#ifndef REQUESTTARGET_HPP
#define REQUESTTARGET_HPP

#include <string>

namespace Hafserv
{

class RequestTarget
{
public:
	RequestTarget();
	RequestTarget(std::string requestTarget);
	RequestTarget(const RequestTarget &other);
	RequestTarget &operator=(const RequestTarget &rhs);
	~RequestTarget();

	const std::string &getTargetURI() const;
	const std::string &getQueryString() const;

private:
	std::string targetURI;
	std::string queryString;
};

std::ostream &operator<<(std::ostream &os, const RequestTarget &requestTarget);

} // namespace Hafserv

#endif
