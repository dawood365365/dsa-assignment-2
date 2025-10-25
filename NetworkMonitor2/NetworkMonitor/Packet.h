#ifndef PACKET_H
#define PACKET_H

#include <string>
#include <vector>
#include <sys/time.h>
#include <sstream>
#include <iomanip>

class Packet {
public:
    int id;
    timeval timestamp;
    size_t size;
    std::vector<unsigned char> data;
    std::string srcIP;
    std::string dstIP;
    std::string protocol;
    int retryCount;
    
    Packet() : id(0), size(0), srcIP("Unknown"), dstIP("Unknown"), 
               protocol("Unknown"), retryCount(0) {
        gettimeofday(&timestamp, nullptr);
    }
    
    Packet(int id, const unsigned char* buffer, size_t size)
        : id(id), size(size), data(buffer, buffer + size),
          srcIP("Unknown"), dstIP("Unknown"), protocol("Unknown"), retryCount(0) {
        gettimeofday(&timestamp, nullptr);
    }
    
    std::string getTimestampStr() const {
        std::ostringstream oss;
        oss << timestamp.tv_sec << "." << std::setw(6)
            << std::setfill('0') << timestamp.tv_usec;
        return oss.str();
    }
    
    // Calculate estimated delay in milliseconds
    int getEstimatedDelay() const {
        return static_cast<int>(size / 1000);
    }
    
    bool canRetry() const {
        return retryCount < 2;
    }
    
    void incrementRetry() {
        retryCount++;
    }
};

#endif