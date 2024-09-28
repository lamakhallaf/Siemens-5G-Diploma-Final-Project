# Siemens-5G-Diploma-Final-Project

Welcome to my final project repository!
To run the code, please download the following files:
1) main.cpp
2) milestone1.cpp
3) milestone1.h
4) milestone2.cpp
5) milestone2.h
6) do.txt

To run the code, please copy and paste the contents of the do.txt file into the terminal of your IDE. 

Also, I've provided a sample test.txt file that shows the output for Milestone 2.

Explaining important parts of the code:
- milestone1.h
EthernetConfigs class: This class contains all of the parameters for the Ethernet packet, as well as the Oran packet inside. All of these are included in an unordered_map. I chose to set the data type of the map's values as string to make it easier for manipulating as a hex string. Other than that, this header file contains all of the declarations of the functions used in milestone1.cpp.
 
- milestone1.cpp
  1) void packets(EthernetConfigs &packet, string fileName, int m): This function starts by pushing the preamble and SFD, which are hardcoded, into a string vector that was declared in the EthernetConfigs class. Then it pushes the destination an source address. After that, I performed a fragmentation check by comparing the payload size given from the Oran specifications to the payload of the Ethernet packet. If the Oran payload is larger, I return an error message. After confirming that the sizes work fine, I then push back "00" bytes for milestone 1.
 
- milestone2.cpp
1) void fullPayload(EthernetConfigs &packet, int m, string outFile): This function calls the packets() function first to fill out the Ethernet headers, then it reads the IQ samples and pushes them according to the specifications given in the user's configuration text file.
