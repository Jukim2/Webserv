#include "Http/RequestTarget.hpp"

#include <iostream>

using namespace Hafserv;

RequestTarget::RequestTarget() {}

RequestTarget::RequestTarget(std::string requestTarget)
{
    size_t idx = requestTarget.find('?');
    if (idx != std::string::npos)
    {
        targetURI = requestTarget.substr(0, idx);
        queryString = requestTarget.substr(idx + 1);
    }
    else
        targetURI = requestTarget;
}

RequestTarget::RequestTarget(const RequestTarget &other)
    : targetURI(other.targetURI), queryString(other.queryString)
{
}

RequestTarget &Hafserv::RequestTarget::operator=(const RequestTarget &rhs)
{
    if (this != &rhs)
    {
        targetURI = rhs.targetURI;
        queryString = rhs.queryString;
    }
    return *this;
}

RequestTarget::~RequestTarget() {}

const std::string &RequestTarget::getTargetURI() const { return targetURI; }
const std::string &RequestTarget::getQueryString() const { return queryString; }

std::ostream &Hafserv::operator<<(std::ostream &os,
                                  const RequestTarget &requestTarget)
{
    os << requestTarget.getTargetURI();
    if (requestTarget.getQueryString().length() > 0)
        os << "?" << requestTarget.getQueryString();
    return os;
}
