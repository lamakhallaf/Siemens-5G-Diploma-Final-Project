#ifndef milestone1_h
#define milestone1_h

#include <vector>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>

using namespace std;

class EthernetConfigs {
    public:
    string lineRate, capSize, minIFGs, dest, source, packetSize, burstSize, burstPeriod;
    string oranSCS, MaxNrb, NrbPerPacket, PayloadType, Payload;
    int packet_time, headerSize;
    vector<string> data;
    unordered_map <string, string> map = {
        {"Eth.LineRate", lineRate}, 
        {"Eth.CaptureSizeMs", capSize}, 
        {"Eth.MinNumOfIFGsPerPacket", minIFGs}, 
        {"Eth.DestAddress", dest}, 
        {"Eth.SourceAddress", source}, 
        {"Eth.MaxPacketSize", packetSize},
        {"Eth.BurstSize", burstSize},
        {"Eth.BurstPeriodicity_us", burstPeriod},

        {"Oran.SCS", oranSCS},
        {"Oran.MaxNrb", MaxNrb},
        {"Oran.NrbPerPacket", NrbPerPacket},
        {"Oran.PayloadType", PayloadType},
        {"Oran.Payload", Payload}
    };
    
    //ideally i should take the txt file and add keys accordingly
    unordered_set<int> scs = {15, 30, 60 };
    
    EthernetConfigs(){
        packet_time = 0;
    }
};

unordered_map<string, string> readFile(string filePath);
void packets(EthernetConfigs &packet, string fileName, int m);
void burst(EthernetConfigs packet, int length, string fileName);
void writePacketsToFile(EthernetConfigs packet, int ifgBytes, int numBursts,  string fileName);
string hexa(string input);
int checkPadding(int packetSize, string ifg);
string iToHex(int value, int bytes);
string fcs (EthernetConfigs packet);
string lltoh(long long value);
long long htoll( string hexStr);
void fileOut( vector<string> data,  string fileName);
string removeSpace(string input);
int getLength(vector<string> data);

#endif