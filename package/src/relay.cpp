#include "relaycontrol.h"
#include <iostream>
#include <cstring>
// Headers needed for sleeping.
#include <unistd.h>
#include <stdint.h>

using namespace std;

string CharToString(char *chr);
int HexToBin( const char *s );


int main(int argc, char* argv[])
{

	RelayControl dev(0x03EB, 0x204F);
	//int i = 0;
	
	if(argc == 1)
	{
		cout << "Help to used: -help" << endl;
		return 1;
	}
	
	
	if(!dev.Connect())
	{
		cout << "unable to open device" << endl;;
		return 2;
	}

	dev.WakeUp();
	
	if (strcmp(argv[1], "-status") == 0)
	{
		char ch_status = dev.RelayStatus();
		cout << "0x" << CharToString(&ch_status) << endl;
			
		return 0;
	}
	
	if(strcmp(argv[1], "-owReset") == 0)
	{
		if(!dev.WireReset())
		{
			cout << "OW Reset: error" << endl;
			return 2;
		}	
		
		cout << "OW Reset: OK!" << endl;
		
		return 0;
	}
	
	if(strcmp(argv[1], "-searchROM") == 0)
	{
		int i = 1;
		while(1)
		{
			Report report = { 0x00, 0x00, { 0x00 } };

			report = dev.WireSearchRom();

			if(report.data[0] == 0x00)
			{
				break;
			}


			string stringROM;
			stringROM.clear();

			for(int is = 0; is < 8; is++)
			{
				//stringROM += ' ';
				stringROM += CharToString((char *)&report.data[is]);

			}

			//cout << i << "." << stringROM << endl;
			cout << stringROM << endl;			
			i++;
		}
	
	return 0;
	}
	
	if(argc >= 3)
	{
		if(strcmp(argv[1], "-rele") == 0)
		{
		
			if(argv[2][0] >= '1' && argv[2][0] <= '4')
				dev.RelayON((Relay) ((argv[2][0] - '1') + 4));
			else
				cout << "Relay invalid!" << endl;

			return 0;
		}
			
		/*
		if(strcmp(argv[1], "-releOFF") == 0)
		{
			if(argv[2][0] >= '1' && argv[2][0] <= '4')
				dev.RelayOFF((Relay) ((argv[2][0] - '1') + 4));
			else
				cout << "Relay invalid!" << endl;
				
			return 0;
		}
		*/
		
		if(strcmp(argv[1], "-temp") == 0)
		{
			char command = 0;				// wire command
			int counter = 0;
			Scratchpad buffer;
			
			Report report = { 0x00, 0x00, {0x00} }; 
			
			if (!(strlen(argv[2]) == 16)) // 64 bit ROM
			{
				cout << "wire ROM invalid" << endl;
				return 2;
			}

			// HEX to INT
			for(int i = 0; i < 16; i+=2)
			{
				report.data[counter] = HexToBin(&argv[2][i]);
				counter++;
			}
			
			
			if(!dev.WireMatchRom(report))
			{
				cout << "wire: match error" << endl;
				return 2;
			}
			
			
			command = 0x44;
			
			if(!dev.WireSend(command))
			{
				cout << "wire: 0x44 error" << endl;
				return 2;
			}
			
			sleep(1);
			
			if(!dev.WireMatchRom(report))
			{
				cout << "wire: match error" << endl;
				return 2;
			}
			
			command = 0xBE;
			
			if(!dev.WireSend(command))
			{
				cout << "wire: 0xBE error" << endl;
				return 2;
			}			
			
			sleep(1);
			
			buffer = dev.WireScratchpadRead();
			
			//for(int is = 0; is < 9; is++)
			//	cout << CharToString((char *)&buffer.scratchpad[is]) << " ";
			//cout << endl;
			
			
			
			int16_t temp = (buffer.scratchpad[1] << 8) | buffer.scratchpad[0];
		
			// проверка на отрицательную температуру
			
			if(buffer.scratchpad[1] & 128)
			{
				temp = ~temp + 1; 
				cout << "-";
			}
			
			temp  >>=4;
			temp &= 0x0FFF;

			int16_t o = buffer.scratchpad[0] & 0x000F;

			
			cout  << temp << "." << o << endl;
			
			
			return 0;
		}
	}

	cout << "Help:" << endl 
			<< "	-releON: 	relay on!"  << endl
			<< "	-releOFF: 	relay off!"  << endl
			<< "	-status: 	relay status" << endl
			<< "	-searchROM:	wire ROM" << endl
			<< "	-temp:		sensor temperature" << endl
			<< "	-owReset:	reset one-wire" << endl;
	return 1;
}

string CharToString(char *chr)
{
    char hex[] = "0123456789ABCDEF";
    char ch[2] = { 0 };
    string RString;
    RString.clear();

    ch[0] = *chr;
    ch[1] = ch[0] & 0xF0;
    ch[1] >>= 4;
    ch[1] &= 0x0F;
    ch[0] &= 0x0F;

    RString += hex[(int)ch[1]];
    RString += hex[(int)ch[0]];

    return RString;
}

int HexToBin( const char *s )
{
    int ret=0;
    int i;
    for( i=0; i<2; i++ )
    {
        char c = *s++;
        int n=0;
        if( '0'<=c && c<='9' )
            n = c-'0';
        else if( 'a'<=c && c<='f' )
            n = 10 + c-'a';
        else if( 'A'<=c && c<='F' )
            n = 10 + c-'A';
        ret = n + ret*16;
    }
    return ret;
}
