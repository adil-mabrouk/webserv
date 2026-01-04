#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

// ANSI color codes for pretty output
#define GREEN "\033[92m"
#define BLUE "\033[94m"
#define YELLOW "\033[93m"
#define RED "\033[91m"
#define RESET "\033[0m"

void printAddressInfo(struct addrinfo *info, int index)
{
    std::cout << "\n" << BLUE << "Result #" << index << ":" << RESET << "\n";
    
    // Address Family
    std::cout << "  Family:      ";
    if (info->ai_family == AF_INET)
        std::cout << "IPv4 (AF_INET)\n";
    else if (info->ai_family == AF_INET6)
        std::cout << "IPv6 (AF_INET6)\n";
    else
        std::cout << "Unknown (" << info->ai_family << ")\n";
    
    // Socket Type
    std::cout << "  Socket Type: ";
    if (info->ai_socktype == SOCK_STREAM)
        std::cout << "SOCK_STREAM (TCP)\n";
    else if (info->ai_socktype == SOCK_DGRAM)
        std::cout << "SOCK_DGRAM (UDP)\n";
    else
        std::cout << "Unknown (" << info->ai_socktype << ")\n";
    
    // Protocol
    std::cout << "  Protocol:    ";
    if (info->ai_protocol == IPPROTO_TCP)
        std::cout << "TCP\n";
    else if (info->ai_protocol == IPPROTO_UDP)
        std::cout << "UDP\n";
    else
        std::cout << info->ai_protocol << "\n";
    
    // IP Address
    void *addr;
    char ipstr[INET6_ADDRSTRLEN];
    std::string port;
    
    if (info->ai_family == AF_INET)
    {
        // IPv4
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)info->ai_addr;
        addr = &(ipv4->sin_addr);
        port = std::to_string(ntohs(ipv4->sin_port));
    }
    else
    {
        // IPv6
        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)info->ai_addr;
        addr = &(ipv6->sin6_addr);
        port = std::to_string(ntohs(ipv6->sin6_port));
    }
    
    // Convert binary address to string
    inet_ntop(info->ai_family, addr, ipstr, sizeof(ipstr));
    
    std::cout << "  IP Address:  " << GREEN << ipstr << RESET << "\n";
    std::cout << "  Port:        " << port << "\n";
    
    // Canonical name (if available)
    if (info->ai_canonname)
        std::cout << "  Canon Name:  " << info->ai_canonname << "\n";
}

void resolveAddress(const char *hostname, const char *service)
{
    struct addrinfo hints, *res, *p;
    int status;
    int count = 0;
    
    std::cout << "\n" << YELLOW << "═══════════════════════════════════════════════════" << RESET << "\n";
    std::cout << YELLOW << "Resolving: " << RESET << hostname;
    if (service)
        std::cout << ":" << service;
    std::cout << "\n";
    std::cout << YELLOW << "═══════════════════════════════════════════════════" << RESET << "\n";
    
    // Setup hints
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;      // Allow IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;  // TCP socket
    hints.ai_flags = AI_CANONNAME;    // Get canonical name
    
    // Resolve
    status = getaddrinfo(hostname, service, &hints, &res);
    if (status != 0)
    {
        std::cerr << RED << "Error: " << gai_strerror(status) << RESET << "\n\n";
        return;
    }
    
    // Print all results
    for (p = res; p != NULL; p = p->ai_next)
    {
        printAddressInfo(p, ++count);
    }
    
    std::cout << "\n" << YELLOW << "Total results: " << count << RESET << "\n\n";
    
    // Free the linked list
    freeaddrinfo(res);
}

void printUsage(const char *progName)
{
    std::cout << "Usage: " << progName << " <hostname> [port]\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << progName << " google.com\n";
    std::cout << "  " << progName << " google.com 80\n";
    std::cout << "  " << progName << " localhost 8080\n";
    std::cout << "  " << progName << " 127.0.0.1\n";
    std::cout << "  " << progName << " ::1 8080\n";
    std::cout << "  " << progName << " example.com http\n";
    std::cout << "\n";
}

int main(int argc, char *argv[])
{
    if (argc < 2 || argc > 3)
    {
        printUsage(argv[0]);
        return 1;
    }
    
    const char *hostname = argv[1];
    const char *service = (argc == 3) ? argv[2] : NULL;
    
    resolveAddress(hostname, service);
    
    return 0;
}