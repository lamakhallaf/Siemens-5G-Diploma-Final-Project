#include "milestone1.h"
#include "milestone2.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <iomanip>
#include <cmath>
#include <cstdint>

using namespace std;

EthernetConfigs checkORAN(EthernetConfigs packet){
    unordered_set <int> SCS = {15, 30, 60};
    if (SCS.find(stoi(packet.oranSCS)) == SCS.end()){
        cout << "Invalid SCS value!";
    }
    if (stoi(packet.MaxNrb) == 0) {
        packet.MaxNrb = "273";
    }
    if (stoi(packet.NrbPerPacket) == 0) {
        packet.NrbPerPacket = "273";
    }
    return packet;
}

void fullPayload(EthernetConfigs &packet, int m, string outFile){
    //Hardcoded
    int symbolsPerSlot = 14;  
    int subPerFrame = 10;
    int frameTime = 10;

    int SCS = stoi(packet.map["Oran.SCS"]);
    int nrbPerPacket = stoi(packet.map["Oran.NrbPerPacket"]);
    int captureSizeMs = stoi(packet.map["Eth.CaptureSizeMs"]);
    int maxNRB = stoi(packet.map["Oran.MaxNrb"]);

    int packetsPerSymbol = maxNRB/nrbPerPacket;
    int slotsPerFrame = (SCS/15)*10;
    //int packetsPerFrame = packetsPerSymbol * symbolsPerSlot * slotsPerFrame; // * (stoi(config.map["Eth.CaptureSizeMs"])/10);
    int totalSymbols = captureSizeMs * slotsPerFrame/10 * symbolsPerSlot; //10ms means 10 subframes, 10*2*14 = 280 symbols total
    int totalPackets = packetsPerSymbol * totalSymbols; // 280 * 9 packets per symbol
    int frames = captureSizeMs/frameTime;
    //int iqBytes = packetsPerFrame * nrbPerPacket * 2 * 12; //2 bytes/SC * 12 SC/RB

    int frameId = 0;
    int subframeId = 0;
    int slotId = 0;
    int symbolId = 0;

    vector<string> IQ;
    IQ = readIQSamples("iq.txt");
    //ECPRI Payload (ORAN)
    for (int i = 0; i < totalPackets; i++){ 
        //Step 1: Ethernet Header
        packets(packet, "Packettt.txt", m); 
        //ECPRI Common Header
        packet.data.push_back("1E"); //1 byte for ecpriVersion, Reserved, and Concat. instead of dummy zeroes
        packet.data.push_back("00"); //00 here means IQ data message
        if ((symbolId% 14 == 0)&& (symbolId != 0)){ //When the symbol index reaches 14, I move on to the next slot/subframe/frame depending on their indices
            symbolId = 0;

            if (slotId == slotsPerFrame/10 - 1) {
                slotId = 0;
                if (subframeId == 9){
                    subframeId = 0;
                    frameId++;
                }
                else {
                    subframeId++;     
                }  
            }
            else {
                slotId++;
            }
        }
        
        packet.data.push_back("00"); //dataDirection, payloadVersion, and filterIndex
        packet.data.push_back(iToHex(frameId, 2));
        packet.data.push_back(iToHex(subframeId, 1));
        packet.data.push_back(iToHex(slotId, 2));
        packet.data.push_back(iToHex(symbolId, 1));
        packet.data.push_back("0000"); //2 bytes sectionID until symInc
        packet.data.push_back("0"); //startPrbu
        packet.data.push_back(iToHex(nrbPerPacket, 2));
        for (int j = 0; j<nrbPerPacket ; j++){
            packet.data.insert(packet.data.end(), IQ.begin() + j*12, IQ.begin() + j*12 + 12);
        }
        symbolId++;

    
    string FCS = fcs (packet);
    packet.data.push_back(FCS); // 
    int seqId = i%256; //reaches 255, then goes back to zero
    packet.data.push_back(iToHex(seqId, 4)); //ECPRI SeqId
    int padding = checkPadding(getLength(packet.data), packet.map["Eth.MinNumOfIFGsPerPacket"]);
    for (int j = 0; j<padding ; j++){
        packet.data.push_back("07");
    }
    fileOut( packet.data,  outFile);
    }
}

vector<string> readIQSamples(string fileName) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cerr << "Error: Could not open the file " << fileName << endl;
        return {};
    }

    vector<string> hexSamples;
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        int iqValue;
        while (ss >> iqValue) {

            int16_t signedValue = static_cast<int16_t>(iqValue);
            stringstream hexStream;
            hexStream << hex << setw(4) << setfill('0') << (signedValue & 0xFFFF);
            hexSamples.push_back(hexStream.str());
        }
    }

    return hexSamples;
}

void fragmentationCheck(EthernetConfigs packet, int payloadSize){
    //Calculating ORAN packet size
    int oranSize = 12*2*stoi(packet.map["Oran.NrbPerPacket"]); // 12 IQ samples * 2 bytes * nrbs per packet

    //Comparing with Ethernet Payload
    if (payloadSize < oranSize) {
        cout << "Fragmentation error! Please reduce your PRB per packet or increase your maximum packet size!" << endl;
    }
    
}

