#ifndef NETWORK_MONITOR_H
#define NETWORK_MONITOR_H

#include "Packet.h"
#include "Queue.h"
#include "PacketAnalyzer.h"
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <cstring>

class NetworkMonitor {
private:
    int sock;
    std::string interface;
    Queue<Packet> packetQueue;
    Queue<Packet> filteredQueue;
    Queue<Packet> backupQueue;
    PacketAnalyzer analyzer;
    std::atomic<bool> capturing;
    int oversizedThreshold;
    int oversizedCount;
    
public:
    NetworkMonitor(const std::string& iface) 
        : interface(iface), capturing(false), oversizedThreshold(5), oversizedCount(0) {
        
        // Create raw socket
        sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
        if (sock < 0) {
            perror("Socket creation failed. Run with sudo/root privileges");
            exit(1);
        }
        
        // Bind to specific interface
        struct ifreq ifr;
        memset(&ifr, 0, sizeof(ifr));
        strncpy(ifr.ifr_name, interface.c_str(), IFNAMSIZ - 1);
        
        if (setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, &ifr, sizeof(ifr)) < 0) {
            perror("Binding to interface failed");
            close(sock);
            exit(1);
        }
        
        std::cout << "✅ Network Monitor initialized on interface: " << interface << std::endl;
        std::cout << "✅ Raw socket created successfully (requires root privileges)\n";
    }
    
    ~NetworkMonitor() { 
        capturing = false;
        close(sock); 
    }
    
    // Continuous packet capture with time limit
    void capturePackets(int duration = 60) {
        unsigned char buffer[65536];
        int id = 1;
        capturing = true;
        
        std::cout << "\n🔍 Starting continuous packet capture for " << duration << " seconds...\n";
        std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        
        auto startTime = std::chrono::steady_clock::now();
        auto endTime = startTime + std::chrono::seconds(duration);
        
        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
        
        while (capturing && std::chrono::steady_clock::now() < endTime) {
            ssize_t size = recvfrom(sock, buffer, sizeof(buffer), 0, nullptr, nullptr);
            
            if (size > 0) {
                Packet p(id++, buffer, size);
                analyzer.dissect(p);
                packetQueue.enqueue(p);
                
                if (id % 10 == 0) {
                    std::cout << "📦 Captured " << id - 1 << " packets...\r" << std::flush;
                }
            }
        }
        
        capturing = false;
        std::cout << "\n✅ Capture complete. Total packets captured: " << (id - 1) << "\n";
        std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    }
    
    // Display captured packets with detailed information
    void displayPackets() {
        if (packetQueue.isEmpty()) {
            std::cout << "\n⚠️  No packets captured yet.\n";
            return;
        }
        
        std::cout << "\n📋 Captured Packets List:\n";
        std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        std::cout << "ID\tSource IP\t\tDestination IP\t\tProtocol\tSize\tTimestamp\n";
        std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        
        Queue<Packet> temp = packetQueue;
        int count = 0;
        while (!temp.isEmpty() && count < 50) {
            Packet p = temp.front();
            temp.dequeue();
            
            std::cout << p.id << "\t"
                      << p.srcIP << "\t"
                      << p.dstIP << "\t"
                      << p.protocol << "\t\t"
                      << p.size << "\t"
                      << p.getTimestampStr() << std::endl;
            count++;
        }
        
        if (!temp.isEmpty()) {
            std::cout << "... and " << (packetQueue.size() - 50) << " more packets\n";
        }
        
        std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        std::cout << "Total packets: " << packetQueue.size() << "\n";
    }
    
    // Display detailed dissection of a specific packet
    void displayPacketDetails(int packetId) {
        Queue<Packet> temp = packetQueue;
        bool found = false;
        
        while (!temp.isEmpty()) {
            Packet p = temp.front();
            temp.dequeue();
            
            if (p.id == packetId) {
                analyzer.displayPacketDetails(p);
                found = true;
                break;
            }
        }
        
        if (!found) {
            std::cout << "⚠️  Packet with ID " << packetId << " not found.\n";
        }
    }
    
    // Filter packets by source and destination IP with size checking
    void filterPackets(const std::string& src, const std::string& dst) {
        std::cout << "\n🔎 Filtering packets: " << src << " → " << dst << "\n";
        std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        
        Queue<Packet> temp = packetQueue;
        int matchCount = 0;
        int skippedOversized = 0;
        oversizedCount = 0;
        
        // Clear previous filtered queue
        filteredQueue.clear();
        
        while (!temp.isEmpty()) {
            Packet p = temp.front();
            temp.dequeue();
            
            // Check if packet matches filter criteria
            if (p.srcIP == src && p.dstIP == dst) {
                // Skip oversized packets if threshold exceeded
                if (p.size > 1500) {
                    oversizedCount++;
                    if (oversizedCount > oversizedThreshold) {
                        skippedOversized++;
                        std::cout << "⚠️  Skipping oversized packet " << p.id 
                                  << " (Size: " << p.size << " bytes)\n";
                        continue;
                    }
                }
                
                filteredQueue.enqueue(p);
                matchCount++;
                std::cout << "✓ Matched packet " << p.id 
                          << " | Size: " << p.size 
                          << " | Protocol: " << p.protocol 
                          << " | Delay: " << p.getEstimatedDelay() << "ms\n";
            }
        }
        
        std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        std::cout << "✅ Filtered " << matchCount << " packets from " << src << " → " << dst << "\n";
        if (skippedOversized > 0) {
            std::cout << "⚠️  Skipped " << skippedOversized << " oversized packets (threshold exceeded)\n";
        }
    }
    
    // Display filtered packets with estimated delays
    void displayFilteredPackets() {
        if (filteredQueue.isEmpty()) {
            std::cout << "\n⚠️  No filtered packets available.\n";
            return;
        }
        
        std::cout << "\n📋 Filtered Packets List:\n";
        std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        std::cout << "ID\tSource IP\t\tDest IP\t\t\tSize\tDelay(ms)\n";
        std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        
        Queue<Packet> temp = filteredQueue;
        while (!temp.isEmpty()) {
            Packet p = temp.front();
            temp.dequeue();
            
            std::cout << p.id << "\t"
                      << p.srcIP << "\t"
                      << p.dstIP << "\t"
                      << p.size << "\t"
                      << p.getEstimatedDelay() << std::endl;
        }
        
        std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        std::cout << "Total filtered packets: " << filteredQueue.size() << "\n";
    }
    
    // Replay filtered packets with error handling and retry mechanism
    void replayPackets() {
        if (filteredQueue.isEmpty()) {
            std::cout << "\n⚠️  No filtered packets to replay.\n";
            return;
        }
        
        std::cout << "\n▶️  Starting packet replay...\n";
        std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        
        int successCount = 0;
        int failureCount = 0;
        
        while (!filteredQueue.isEmpty()) {
            Packet p = filteredQueue.front();
            filteredQueue.dequeue();
            
            // Calculate and apply delay
            int delay = p.getEstimatedDelay();
            std::cout << "⏳ Packet " << p.id << ": Waiting " << delay << "ms... ";
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
            
            // Attempt to replay packet
            ssize_t sent = send(sock, p.data.data(), p.size, 0);
            
            if (sent < 0 || sent != static_cast<ssize_t>(p.size)) {
                std::cout << "❌ FAILED\n";
                std::cerr << "   Error: " << strerror(errno) << "\n";
                
                // Move to backup queue for retry
                backupQueue.enqueue(p);
                failureCount++;
            } else {
                std::cout << "✅ SUCCESS (" << sent << " bytes sent)\n";
                successCount++;
            }
        }
        
        std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        std::cout << "Replay Summary:\n";
        std::cout << "  ✅ Successful: " << successCount << "\n";
        std::cout << "  ❌ Failed: " << failureCount << "\n";
        
        if (failureCount > 0) {
            std::cout << "  📦 " << failureCount << " packets moved to backup queue for retry\n";
            retryBackupPackets();
        }
    }
    
    // Retry mechanism for failed packets (max 2 retries)
    void retryBackupPackets() {
        if (backupQueue.isEmpty()) {
            std::cout << "\n✅ No packets in backup queue.\n";
            return;
        }
        
        std::cout << "\n🔄 Attempting to retry backup packets...\n";
        std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        
        Queue<Packet> tempBackup;
        
        while (!backupQueue.isEmpty()) {
            Packet p = backupQueue.front();
            backupQueue.dequeue();
            
            if (!p.canRetry()) {
                std::cout << "❌ Packet " << p.id << " exceeded max retries (2). Discarding.\n";
                continue;
            }
            
            p.incrementRetry();
            std::cout << "🔄 Retry #" << p.retryCount << " for packet " << p.id << "... ";
            
            // Consider packet size for retry
            if (p.size > 1500) {
                std::cout << "⚠️  Skipped (oversized: " << p.size << " bytes)\n";
                continue;
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            ssize_t sent = send(sock, p.data.data(), p.size, 0);
            
            if (sent < 0 || sent != static_cast<ssize_t>(p.size)) {
                std::cout << "❌ FAILED\n";
                tempBackup.enqueue(p);
            } else {
                std::cout << "✅ SUCCESS\n";
            }
        }
        
        backupQueue = tempBackup;
        
        std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        std::cout << "Remaining packets in backup: " << backupQueue.size() << "\n";
    }
    
    // Get queue statistics
    void displayStatistics() {
        std::cout << "\n📊 Network Monitor Statistics:\n";
        std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        std::cout << "  Total Captured Packets: " << packetQueue.size() << "\n";
        std::cout << "  Filtered Packets: " << filteredQueue.size() << "\n";
        std::cout << "  Backup Queue: " << backupQueue.size() << "\n";
        std::cout << "  Interface: " << interface << "\n";
        std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    }
};

#endif