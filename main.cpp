#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <unordered_map>
#include "milestone1.h"
#include "milestone2.h"



int main(){
    cout << "Please enter the file path. \n";
    string filePath;
    cin >> filePath;

    cout << "Please enter the name of your output file. \n";
    string outFile;
    cin >> outFile;
    
    int m;
    cout << "For milestone 1, enter 1. For milestone 2, enter anything else. \n";
    cin >> m;

    unordered_map<string, string> configurations = readFile(filePath);
    EthernetConfigs packet;
    packet.map = configurations;
    
    fullPayload(packet, m, outFile);
    
    return 0;
}
