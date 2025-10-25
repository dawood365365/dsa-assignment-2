#ifndef PACKET_ANALYZER_H
#define PACKET_ANALYZER_H

#include "Packet.h"
#include "Stack.h"
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ether.h>
#include <arpa/inet.h>
#include <iostream>
#include <sstream>

class PacketAnalyzer {
public:
    // Parse all protocol layers using a stack
    void dissect(Packet& packet) {
        Stack<std::string> layers;
        
        if (packet.size < sizeof(struct ether_header)) {
            std::cout << "Packet " << packet.id << " too small for Ethernet header\n";
            return;
        }
        
        // Layer 1: Ethernet
        layers.push("Ethernet");
        const struct ether_header* eth = (struct ether_header*) packet.data.data();
        uint16_t etherType = ntohs(eth->ether_type);
        
        size_t offset = sizeof(struct ether_header);
        
        // Layer 2: IPv4 or IPv6
        if (etherType == ETHERTYPE_IP) {
            layers.push("IPv4");
            if (packet.size < offset + sizeof(struct ip)) {
                displayLayers(packet.id, layers);
                return;
            }
            
            struct ip* iph = (struct ip*)(packet.data.data() + offset);
            
            // Extract source and destination IPs
            char srcIP[INET_ADDRSTRLEN];
            char dstIP[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(iph->ip_src), srcIP, INET_ADDRSTRLEN);
            inet_ntop(AF_INET, &(iph->ip_dst), dstIP, INET_ADDRSTRLEN);
            
            packet.srcIP = srcIP;
            packet.dstIP = dstIP;
            
            int iph_len = iph->ip_hl * 4;
            offset += iph_len;
            
            // Layer 3: TCP or UDP
            if (iph->ip_p == IPPROTO_TCP) {
                layers.push("TCP");
                packet.protocol = "TCP";
            } else if (iph->ip_p == IPPROTO_UDP) {
                layers.push("UDP");
                packet.protocol = "UDP";
            }
            
        } else if (etherType == ETHERTYPE_IPV6) {
            layers.push("IPv6");
            if (packet.size < offset + sizeof(struct ip6_hdr)) {
                displayLayers(packet.id, layers);
                return;
            }
            
            struct ip6_hdr* ip6h = (struct ip6_hdr*)(packet.data.data() + offset);
            
            // Extract IPv6 source and destination
            char srcIP[INET6_ADDRSTRLEN];
            char dstIP[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, &(ip6h->ip6_src), srcIP, INET6_ADDRSTRLEN);
            inet_ntop(AF_INET6, &(ip6h->ip6_dst), dstIP, INET6_ADDRSTRLEN);
            
            packet.srcIP = srcIP;
            packet.dstIP = dstIP;
            
            offset += sizeof(struct ip6_hdr);
            
            // Layer 3: TCP or UDP for IPv6
            if (ip6h->ip6_nxt == IPPROTO_TCP) {
                layers.push("TCP");
                packet.protocol = "TCP";
            } else if (ip6h->ip6_nxt == IPPROTO_UDP) {
                layers.push("UDP");
                packet.protocol = "UDP";
            }
        }
        
        displayLayers(packet.id, layers);
    }
    
    // Display packet layer information
    void displayPacketDetails(const Packet& packet) {
        std::cout << "\n═══════════════════════════════════════════\n";
        std::cout << "  Packet ID: " << packet.id << "\n";
        std::cout << "  Timestamp: " << packet.getTimestampStr() << "\n";
        std::cout << "  Size: " << packet.size << " bytes\n";
        std::cout << "  Source IP: " << packet.srcIP << "\n";
        std::cout << "  Destination IP: " << packet.dstIP << "\n";
        std::cout << "  Protocol: " << packet.protocol << "\n";
        std::cout << "  Estimated Delay: " << packet.getEstimatedDelay() << " ms\n";
        std::cout << "═══════════════════════════════════════════\n";
    }

private:
    void displayLayers(int packetId, Stack<std::string>& layers) {
        std::cout << "Packet " << packetId << " Layers: ";
        
        // Create temporary storage for layer display
        Stack<std::string> temp;
        while (!layers.isEmpty()) {
            temp.push(layers.top());
            layers.pop();
        }
        
        bool first = true;
        while (!temp.isEmpty()) {
            if (!first) std::cout << " → ";
            std::cout << temp.top();
            temp.pop();
            first = false;
        }
        std::cout << std::endl;
    }
};

#endif