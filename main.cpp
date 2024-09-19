#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include "generate_packets.h"

int main(){
    cout << "Please enter the file path. \n";
    string filePath;
    cin >> filePath;

    cout << "Please enter the name of your output file. \n";
    string outFile;
    cin >> outFile;
    
    unordered_map<string, string> configurations = readFile(filePath);
    EthernetConfigs packet;
    packet.map = configurations;
    int length = packets(packet, outFile);
    
    return 0;
}
