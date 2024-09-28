#ifndef milestone2_h
#define milestone2_h

// class ECPRI {
//     public:
//     vector<string> packet;
//     vector<string> iq_samples;
//     ECPRI(){
        
//     }
// };

EthernetConfigs checkORAN(EthernetConfigs packet);
void fullPayload(EthernetConfigs &config, int m, string outFile);
vector<string> readIQSamples(string fileName);
void fragmentationCheck(EthernetConfigs packet, int payloadSize);
#endif
