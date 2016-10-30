#include "hidapi.h"
//#include "../../frimware/GenericHID/command.h"
//#include "../../frimware/GenericHID/Config/AppConfig.h"
#include <string>
#include <unistd.h>

#define GENERIC_REPORT_SIZE       9

#define RELE_ON 		0x01
#define RELE_OFF 		0x02
#define PRESS_KEY		0x03
#define OW_SEND			0x04
#define OW_READ			0x05
#define OW_RESET		0x06


#define READ_ROM		0x33
#define SKIP_ROM		0xCC
#define MATCH_ROM		0x55
#define SEARCH_ROM		0xF0

using namespace std;

#ifndef RELAYCONTROL_H
#define RELAYCONTROL_H

typedef struct{
    unsigned char reportID, command, data[GENERIC_REPORT_SIZE - 1];
}Report;

typedef struct{
    unsigned char scratchpad[9];
}Scratchpad;
enum Relay { One = 4, Two, Three, Four  };

class RelayControl
{
private:
    hid_device *handle;
    unsigned short vid, pid;


    int writeToDevice(Report *);
    int readFromDevice(Report &);

public:
    RelayControl(unsigned short, unsigned short);
    ~RelayControl();

    bool Connect();
    bool isConnect();
    void Disconnect();
	void WakeUp();
    string getManufacturerString();
    string getProductString();

    char RelayStatus();
	//int RelayStatus(char &);
    char RelayON(Relay);
    char RelayOFF(Relay);

    Report WireSearchRom();
    Scratchpad WireScratchpadRead();
    bool WireMatchRom(Report &);
    bool WireSkip();
	bool WireReset();
    bool WireSend(char &);
    unsigned char WireRead();



};

#endif // RELAYCONTROL_H
