/*******************************************************
 Windows HID simplification

 Alan Ott
 Signal 11 Software

 8/22/2009

 Copyright 2009, All Rights Reserved.
 
 This contents of this file may be used by anyone
 for any reason without any conditions and may be
 used as a starting point for your own applications
 which use HIDAPI.
********************************************************/

#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include "hidapi.h"

// Headers needed for sleeping.
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

typedef struct{
    unsigned char reportID, command, data[GENERIC_REPORT_SIZE - 1];
}Report;

typedef struct{
    unsigned char scratchpad[9];
}Scratchpad;
enum Relay { One = 4, Two, Three, Four  };

int writeToDevice(Report * buf);
int readFromDevice(Report &buf);
int RelayON(Relay number);
int RelayOFF(Relay number);
char RelayStatus();

hid_device *handle = NULL;

int main(int argc, char* argv[])
{

	
	//printf("%s\n", argv[0]);
	//printf("%s\n", argv[1]);
	//printf("%s\n", argv[2]);
	//printf("%s\n", argv[3]);

	// Open the device using the VID, PID,
	// and optionally the Serial number.
	////handle = hid_open(0x4d8, 0x3f, L"12345");
	handle = hid_open(0x03eb, 0x204f, NULL);
	if (!handle) {
		printf("unable to open device\n");
 		return 1;
	}


	int i = 0;
	//printf("%d\n", i);
	if(argc == 1)
	{
		printf("Help to used: -help\n");
		return 1;
	}
	
	while(i < argc)
	{
		if(strcmp(argv[i], "-help") == 0)
		{
			printf("Help: \n");
			printf("-relayON: relay on!\n");
			printf("-relayOFF: relay off!\n");
			printf("-status: relay status\n");
			return 0;
		}
		else if (strcmp(argv[i], "-status") == 0)
		{
			char hex[] = "0123456789ABCDEF";
			char ch[2] = { 0 }; 
			ch[0] = RelayStatus();
			ch[1] = ch[0] & 0xF0;
			ch[1] >>= 4;
			ch[1] &= 0x0F;
			ch[0] &= 0x0F;
			printf("Status: 0x%c%c\n", hex[(int)ch[1]], hex[(int)ch[0]]); 
		}
		else if(strcmp(argv[i], "-relayON") == 0)
		{
			switch(argv[i+1][0])
			{
				case '1':
				RelayON(One);
				break;
				case '2':
				RelayON(Two);
				break;
				case '3':
				RelayON(Three);
				break;
				case '4':
				RelayON(Four);
				break;
			}
		}
		else if(strcmp(argv[i], "-relayOFF") == 0)
		{
			switch(argv[i+1][0])
			{
				case '1':
				RelayOFF(One);
				break;
				case '2':
				RelayOFF(Two);
				break;
				case '3':
				RelayOFF(Three);
				break;
				case '4':
				RelayOFF(Four);
				break;
			}
		}
		i++;
	}
	


	// Set the hid_read() function to be non-blocking.
	//hid_set_nonblocking(handle, 1);
	

	
	
	//RelayON(One);
	//usleep(500*1000);
	//RelayOFF(Two);
	

	
	hid_close(handle);

	/* Free static HIDAPI objects. */
	hid_exit();


	return 0;
}

int writeToDevice(Report * buf)
{
    int res = 0;
    res = hid_write(handle, (const unsigned char *) buf, GENERIC_REPORT_SIZE + 1);
    return res;
}

int readFromDevice(Report &buf)
{
    int res = 0, counter = 0;
    unsigned char tmp_buf[GENERIC_REPORT_SIZE] = { 0 };

    while(res == 0)
    {
        res = hid_read(handle, tmp_buf, sizeof(tmp_buf));

        if(res > 0)
        {
            buf.reportID = 0x00;
            buf.command = tmp_buf[0];

            for(int i = 0; i < GENERIC_REPORT_SIZE; i++)
                buf.data[i] = tmp_buf[i + 1];
            break;
        }

        if(res < 0)
            break;

        //the magic пять попыток

        if(counter > 5)
           break;
        counter++;
    }
    return res;
}

int RelayON(Relay number)
{

    Report report = { 0x00, RELE_ON, {0x00}};
    report.data[0] = (1<<number);

    int res = 0;

    res = writeToDevice(&report);
    if(res < 0)
        return res;

    do{
        res = readFromDevice(report);
        if(res < 0)
            return res;
    }while(!(report.command == RELE_ON));

    return res;
}

int RelayOFF(Relay number)
{
    Report report = { 0x00,  RELE_OFF, {0x00}};

    report.data[0] = (1<<number);
    int res = 0;

    res = writeToDevice(&report);
    if(res < 0)
        return res;

    do{
        res = readFromDevice(report);
        if(res < 0)
            return res;
    }while(!(report.command == RELE_OFF));

    return res;
}

char RelayStatus()
{
	int res = 0;
    Report report = { 0x00, PRESS_KEY, {0x00} };

    res = writeToDevice(&report);
    if(res < 0)
        return report.data[0];

    do{
        res = readFromDevice(report);
        if(res < 0)
            return report.data[0];
    }while(!(report.command == PRESS_KEY));



    return report.data[0];
}
