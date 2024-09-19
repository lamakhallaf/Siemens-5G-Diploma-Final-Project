#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include "generate_packets.h"

int main(){
    cout << "Please enter the file path. \n";
    string filePath;
    cin >> filePath;
    
    unordered_map<string, string> configurations = readFile(filePath);
    EthernetConfigs packet;
    packet.map = configurations;
    int length = packets(packet);
    
    return 0;
}
