#include "NetworkMonitor.h"
#include <iostream>
#include <string>

void displayMenu() {
    std::cout << "\n╔════════════════════════════════════════════╗\n";
    std::cout << "║     NETWORK PACKET MONITOR SYSTEM          ║\n";
    std::cout << "╠════════════════════════════════════════════╣\n";
    std::cout << "║  1. Capture Packets (Continuous)           ║\n";
    std::cout << "║  2. Display Captured Packets               ║\n";
    std::cout << "║  3. Display Packet Details                 ║\n";
    std::cout << "║  4. Filter Packets by IP                   ║\n";
    std::cout << "║  5. Display Filtered Packets               ║\n";
    std::cout << "║  6. Replay Filtered Packets                ║\n";
    std::cout << "║  7. Retry Backup Packets                   ║\n";
    std::cout << "║  8. Display Statistics                     ║\n";
    std::cout << "║  9. Run Complete Demo (1 minute)           ║\n";
    std::cout << "║  0. Exit                                   ║\n";
    std::cout << "╚════════════════════════════════════════════╝\n";
}

void runComprehensiveDemo(NetworkMonitor& monitor) {
    std::cout << "\n╔════════════════════════════════════════════════════════╗\n";
    std::cout << "║        COMPREHENSIVE NETWORK MONITOR DEMO              ║\n";
    std::cout << "║  Demonstrating all functionalities for 1 minute        ║\n";
    std::cout << "╚════════════════════════════════════════════════════════╝\n";
    
    // Step 1: Capture packets for 60 seconds
    std::cout << "\n[STEP 1] Continuous Packet Capture\n";
    monitor.capturePackets(60);
    
    // Step 2: Display captured packets
    std::cout << "\n[STEP 2] Displaying Captured Packets\n";
    monitor.displayPackets();
    
    // Step 3: Display statistics
    std::cout << "\n[STEP 3] System Statistics\n";
    monitor.displayStatistics();
    
    // Step 4: Filter packets (example IPs - adjust based on your network)
    std::cout << "\n[STEP 4] Filtering Packets\n";
    std::cout << "Enter Source IP for filtering: ";
    std::string srcIP, dstIP;
    std::cin >> srcIP;
    std::cout << "Enter Destination IP for filtering: ";
    std::cin >> dstIP;
    
    monitor.filterPackets(srcIP, dstIP);
    
    // Step 5: Display filtered packets
    std::cout << "\n[STEP 5] Displaying Filtered Packets\n";
    monitor.displayFilteredPackets();
    
    // Step 6: Replay filtered packets
    std::cout << "\n[STEP 6] Replaying Filtered Packets\n";
    monitor.replayPackets();
    
    // Step 7: Final statistics
    std::cout << "\n[STEP 7] Final Statistics\n";
    monitor.displayStatistics();
    
    std::cout << "\n╔════════════════════════════════════════════════════════╗\n";
    std::cout << "║           DEMO COMPLETED SUCCESSFULLY                  ║\n";
    std::cout << "╚════════════════════════════════════════════════════════╝\n";
}

int main() {
    std::cout << "╔════════════════════════════════════════════════════════╗\n";
    std::cout << "║     NETWORK PACKET MONITOR - INITIALIZATION            ║\n";
    std::cout << "╚════════════════════════════════════════════════════════╝\n";
    std::cout << "\n⚠️  This application requires ROOT privileges!\n";
    std::cout << "Run with: sudo ./network_monitor\n\n";
    
    std::string iface;
    std::cout << "Enter network interface name (e.g., eth0, wlan0, ens33): ";
    std::cin >> iface;
    
    try {
        NetworkMonitor monitor(iface);
        
        int choice;
        bool running = true;
        
        while (running) {
            displayMenu();
            std::cout << "Enter your choice: ";
            std::cin >> choice;
            
            switch (choice) {
                case 1: {
                    int duration;
                    std::cout << "Enter capture duration in seconds (default 60): ";
                    std::cin >> duration;
                    if (duration <= 0) duration = 60;
                    monitor.capturePackets(duration);
                    break;
                }
                
                case 2:
                    monitor.displayPackets();
                    break;
                
                case 3: {
                    int packetId;
                    std::cout << "Enter Packet ID to view details: ";
                    std::cin >> packetId;
                    monitor.displayPacketDetails(packetId);
                    break;
                }
                
                case 4: {
                    std::string src, dst;
                    std::cout << "Enter Source IP: ";
                    std::cin >> src;
                    std::cout << "Enter Destination IP: ";
                    std::cin >> dst;
                    monitor.filterPackets(src, dst);
                    break;
                }
                
                case 5:
                    monitor.displayFilteredPackets();
                    break;
                
                case 6:
                    monitor.replayPackets();
                    break;
                
                case 7:
                    monitor.retryBackupPackets();
                    break;
                
                case 8:
                    monitor.displayStatistics();
                    break;
                
                case 9:
                    runComprehensiveDemo(monitor);
                    break;
                
                case 0:
                    running = false;
                    std::cout << "\n✅ Shutting down Network Monitor...\n";
                    break;
                
                default:
                    std::cout << "❌ Invalid choice! Please try again.\n";
            }
        }
        
        std::cout << "\n╔════════════════════════════════════════════════════════╗\n";
        std::cout << "║     Thank you for using Network Packet Monitor!        ║\n";
        std::cout << "╚════════════════════════════════════════════════════════╝\n";
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}