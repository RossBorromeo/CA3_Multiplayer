#include "RoboCatPCH.hpp"


SocketAddressPtr SocketAddressFactory::CreateIPv4FromString(const string& inString)
{
	auto pos = inString.find_last_of(':');
	string host, service;

	if (pos != string::npos)
	{
		host = inString.substr(0, pos);
		service = inString.substr(pos + 1);
	}
	else
	{
		host = inString;
		service = "0"; // default port
	}

	addrinfo hint{};
	hint.ai_family = AF_INET; // IPv4

	addrinfo* result = nullptr;
	int error = getaddrinfo(host.c_str(), service.c_str(), &hint, &result);
	if (error != 0 || result == nullptr)
	{
		std::cerr << "[SocketAddressFactory] getaddrinfo failed: " << gai_strerror(error) << std::endl;
		return nullptr;
	}

	// Find first valid address
	addrinfo* iter = result;
	while (iter && !iter->ai_addr)
	{
		iter = iter->ai_next;
	}

	if (!iter || !iter->ai_addr)
	{
		freeaddrinfo(result);
		std::cerr << "[SocketAddressFactory] No valid ai_addr found.\n";
		return nullptr;
	}

	auto toRet = std::make_shared<SocketAddress>(*iter->ai_addr);
	freeaddrinfo(result);
	return toRet;
}

