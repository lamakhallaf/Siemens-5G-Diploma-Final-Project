#include "milestone1.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <iomanip>
#include <cmath>
#include "milestone2.h"

// #include <chrono>
// #include <thread>
// #include <stdexcept>

using namespace std;

string removeComments (string line);
int checkPadding(int packetSize);

unordered_map<string, string> readFile(string filePath){
    ifstream inputFile(filePath);
    string line;
    EthernetConfigs configs;

    while (getline(inputFile, line)){
        line = removeComments(line);
        for (auto p = configs.map.begin(); p != configs.map.end(); p++){
            if (line.find(p->first) != string::npos) {
                int position = line.find("=");
                string temp = line.substr(position + 1 );
                temp = hexa(temp);
                while (temp.find(" ") != string::npos){
                    temp = removeSpace(temp);
                }

                p->second = temp;
            }
        }
    }

    return configs.map;
}

//I added this function because otherwise it would read the 0x in any comment
string removeComments (string line) {
    int pos = line.find("//"); 
    if (pos != string::npos) {
        return line.substr(0, pos);
    }
    return line; 
}

void packets(EthernetConfigs &packet, string fileName, int m){
    
    packet.data.push_back("FB555555"); //Preamble and SFD 8 bytes
    int length = getLength(packet.data);
    packet.data.push_back("555555D5");
    packet.data.push_back(packet.map["Eth.DestAddress"]); //6 bytes
    packet.data.push_back(packet.map["Eth.SourceAddress"]);
    packet.headerSize = 0.5*(packet.map["Eth.DestAddress"].length() + packet.map["Eth.SourceAddress"].length() + 28); //28 is the preamble, SFD, ethertype, and FCS. Div by 2 and it gives you bytes.
    int payloadSize = stoi(packet.map["Eth.MaxPacketSize"]) - packet.headerSize;
    int oranPayload = payloadSize - 14; // 6 bytes for ECPRI Header, 8 bytes for ORAN header + section header
    fragmentationCheck(packet, oranPayload);
    packet.data.push_back(iToHex(payloadSize, 4));
    if (m==1){
        for (int i = 0; i< payloadSize ; i++){
            packet.data.push_back("00");
        } 
        packet.data.push_back("00");
        string FCS = fcs (packet);
    packet.data.push_back(FCS); // 

    int padding = checkPadding(getLength(packet.data), packet.map["Eth.MinNumOfIFGsPerPacket"]);
        for (int j = 0; j<padding ; j++){
            packet.data.push_back("07");
        }
    
    burst(packet, length, fileName);
    }
}

void burst(EthernetConfigs packet, int packetLength, string fileName){
    float lineRateBpms = float(stoi(packet.map["Eth.LineRate"])*(pow(10, 9))/8); // Bytes per second
    int bytesToSend = stoi(packet.map["Eth.BurstSize"]) * packetLength; //packet length including ifgs
    float burstPeriod = float(stoi(packet.map["Eth.BurstPeriodicity_us"])*pow(10, -6)); // in seconds
    int bytesPerBurst = burstPeriod*lineRateBpms;
    int burstSize = stoi(packet.map["Eth.BurstSize"]);
    int totalBytesToSend = stoi(packet.map["Eth.CaptureSizeMs"]) * lineRateBpms * pow(10, -3);

    int bytes = stoi(packet.map["Eth.LineRate"])*(pow(10, 9))/8 * stoi(packet.map["Eth.CaptureSizeMs"]);
    int numBursts = float(stoi(packet.map["Eth.CaptureSizeMs"])*pow(10, -3)/burstPeriod);

    vector<string> stream; 
    float transmissionTime = (bytesToSend) / (lineRateBpms); // time in microseconds
    float packetTime = (packetLength) / (lineRateBpms);
    float ifgTime = burstPeriod - transmissionTime;
    int ifgBytes = (ifgTime * lineRateBpms) / (8*2);

    if (ifgTime < 0) {
        cout << "Error: Transmission time exceeds burst periodicity!" << endl;
        return;
    }
    //for loop to push everything
    bool done = false;
    int pack = 0;

    int totalBytes = bytesPerBurst * numBursts;
    int k = 0;
    writePacketsToFile(packet,  ifgBytes,  numBursts,  fileName);

}
void writePacketsToFile(EthernetConfigs packet, int ifgBytes, int numBursts, string fileName) {
    ofstream outputFile(fileName);

    if (!outputFile.is_open()) {
        cerr << "Error: Could not open the file " << fileName << " for writing." << endl;
        return;
    }

    // Ensure packet.data is populated
    if (packet.data.empty()) {
        cerr << "Error: Packet data is empty." << endl;
        return;
    }

    // Combine all packet data into a single string
    string combinedData;
    for (const auto& hexStr : packet.data) {
        combinedData += hexStr;
    }
    string combinedIFG;
    for (int i = 0; i<ifgBytes ; i++) {
        combinedIFG += "07";
    }

    // Write the data and IFG bytes to the file
    for (int i = 0; i < numBursts; i++) {
        // Write the combined data in 8-byte chunks
        for (int j = 0; j < combinedData.length(); j += 8) {  // 16 hex digits = 8 bytes
            outputFile << combinedData.substr(j, 8) << endl;
        }
        for (int j = 0; j < combinedIFG.length(); j += 8) {  // 16 hex digits = 8 bytes
            outputFile << combinedIFG.substr(j, 8) << endl;
        }

    }

    outputFile.close();
}



int checkPadding(int packetSize, string ifg){
    //packetSize = packetSize/2;
    int totalSize = packetSize + stoi(ifg);
    int remainder = totalSize % 4;
    if (remainder == 0){
        return 0 + stoi(ifg);  
    } 
    return 4  + stoi(ifg) - remainder;  
}

string hexa(string input) {
    int pos = input.find("0x");
    if (pos == string::npos) {
        pos = input.find("0X");
    }

    if (pos != string::npos) {
        input.erase(pos, 2);  
    }
    return input;
}

string iToHex(int value, int bytes) {
    stringstream stream;
    stream << hex << uppercase << setfill('0') << setw(bytes) << value;
    return stream.str();
}

long long htoll( string hexStr) {
    long long value;
    stringstream ss;
    ss << hex << hexStr; 
    ss >> value;
    return value;
}

string lltoh(long long value) {
    stringstream ss;
    ss << hex << value;  
    return ss.str();
}

string fcs (EthernetConfigs packet){
    long long sum = 0;
    sum = sum + htoll(packet.map["Eth.DestAddress"]) + htoll(packet.map["Eth.SourceAddress"]);  
    string result = lltoh(sum);
    return result;
}

void fileOut(vector<string> data, string fileName) {
    ofstream outputFile(fileName);

    if (!outputFile.is_open()) {
        cerr << "Error: Could not open the file " << fileName << " for writing." << endl;
        return;
    }

    string combinedData;
    for (auto hexStr : data) {
        combinedData += hexStr;
    }

    for (int i = 0; i < combinedData.length(); i += 8) {
        outputFile << combinedData.substr(i, 8) << endl;  // Write 8 bytes per line
    }

    outputFile.close();
}


string removeSpace(string input){
    int pos = input.find(" ");
    if (pos != string::npos) {
        input.erase(pos, 1);  
    }
    return input;
}

int getLength(vector<string> data) {
    int totalLength = 0;
    for ( auto& str : data) {
        totalLength += str.length();
    }
    return totalLength/2;
}