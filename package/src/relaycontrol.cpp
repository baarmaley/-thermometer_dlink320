#include "relaycontrol.h"
#include <iostream>

RelayControl::RelayControl(unsigned short vendor_id, unsigned short product_id)
{

    handle = NULL;
    vid = vendor_id;
    pid = product_id;

}

RelayControl::~RelayControl()
{
    this->Disconnect();
}

void RelayControl::Disconnect()
{
    if(this->isConnect())
    {
        hid_close(handle);
        /* Free static HIDAPI objects. */
        hid_exit();
    }
    handle = NULL;
}

bool RelayControl::Connect()
{
   if(this->isConnect())
       return true;

   handle = hid_open(vid, pid, NULL);

   if(handle)
   {
       hid_set_nonblocking(handle, 0);
       return true;
   }

   return false;

}
bool RelayControl::isConnect()
{
    if(handle)
        return true;
    return false;
}

string RelayControl::getManufacturerString()
{
    wchar_t wstr[255] = { 0 };
    int res = 0, i = 0;


    string returnString;
    res = hid_get_manufacturer_string(this->handle, wstr, 255);

    returnString.clear();

    while(wstr[i] && i < 255)
    {
        returnString += wstr[i];
        i++;
    }

    if (res < 0)
    {
        returnString.clear();
        returnString = string("Unable to read manufacturer string");
    }

    return returnString;
}

string RelayControl::getProductString()
{
    wchar_t wstr[255] = { 0 };
    int res = 0, i = 0;


    string returnString;
    res = hid_get_product_string(this->handle, wstr, 255);

    returnString.clear();

    while(wstr[i] && i < 255)
    {
        returnString += wstr[i];
        i++;
    }

    if (res < 0)
    {
        returnString.clear();
        returnString = string("Unable to read product string");
    }

    return returnString;
}

int RelayControl::writeToDevice(Report * buf)
{
    if(!isConnect())
        return -1;

    int res = 0;
    res = hid_write(handle, (const unsigned char *) buf, GENERIC_REPORT_SIZE + 1);
    return res;
}

int RelayControl::readFromDevice(Report &buf)
{
    if(!isConnect())
        return -1;

    int res = 0; //, counter = 0;
    unsigned char tmp_buf[GENERIC_REPORT_SIZE] = { 0 };

    //while(res == 0)
    //{
        //res = hid_read(handle, tmp_buf, sizeof(tmp_buf));
		res = hid_read_timeout(handle, tmp_buf, sizeof(tmp_buf), 100);
		
        if(res > 0)
        {
            buf.reportID = 0x00;
            buf.command = tmp_buf[0];

            for(int i = 0; i < GENERIC_REPORT_SIZE; i++)
                buf.data[i] = tmp_buf[i + 1];
            //break;
        }

        //if(res < 0)
          //  break;

        //the magic пять попыток

        //if(counter > 5)
        //  break;
        //counter++;
    //}
    return res;
}


char RelayControl::RelayStatus()
{
    Report report = { 0x00, PRESS_KEY, 0x00 };
    char res = 0;

    res = writeToDevice(&report);
    if(res < 0)
        return res;

    do{
        res = readFromDevice(report);
        if(res < 0)
            return res;
    }while(!(report.command == PRESS_KEY));

    return report.data[0];
}

void RelayControl::WakeUp()
{
	Report report = { 0x00, PRESS_KEY, 0x00 };
	writeToDevice(&report); 
	readFromDevice(report);
}
/*
int RelayControl::RelayStatus(char &state)
{
	Report report = { 0x00, PRESS_KEY, 0x00 };
	int res = 0, counter = 0;
	//res = readFromDevice(report);
	
	res = writeToDevice(&report); 
	
	if(res < 0)
        return res;
	
	res = 0;
	
	do{
    
    res = readFromDevice(report);
        if(res < 0)
            return res;
	}while(!(report.command == PRESS_KEY));
	
	state = report.data[0];	
	
	return res;
}
*/

char RelayControl::RelayON(Relay number)
{

    Report report = { 0x00, RELE_ON, 0x00};
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

    return report.data[0];
}

char RelayControl::RelayOFF(Relay number)
{
    Report report = { 0x00,  RELE_OFF, 0x00};

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

    return report.data[0];
}

Report RelayControl::WireSearchRom()
{

    int res = 0;
    Report report = { 0x00, SEARCH_ROM, 0x00 };

    res = writeToDevice(&report);
    if(res < 0)
        return report;

    do{
        res = readFromDevice(report);
    }while(!(report.command == SEARCH_ROM));

    return report;
}

bool RelayControl::WireMatchRom(Report &report)
{
	Report tmp_report = { 0x00, 0x00, { 0x00 } };
    report.command = MATCH_ROM;
    int res = 0;

    res = writeToDevice(&report);
    if(res < 0)
        return false;
    do{
        res = readFromDevice(tmp_report);
        if(res < 0)
            return false;
    }while(!(tmp_report.command == MATCH_ROM));


    if(tmp_report.data[0])
        return true;

    return false;

}

bool RelayControl::WireSend(char &command)
{
    int res = 0;
    Report report = { 0x00, OW_SEND, (unsigned char) command };

    res = writeToDevice(&report);
    if(res < 0)
        return false;

    return true;
}

unsigned char RelayControl::WireRead()
{
    int res = 0;
    Report report = { 0x00, OW_READ, 0x00 };

    res = writeToDevice(&report);
    if(res < 0)
        return 0;
    do{
        res = readFromDevice(report);
        if(res < 0)
            return 0;
    }while(!(report.command == OW_READ));

    return report.data[0];
}

bool RelayControl::WireSkip()
{
    int res = 0;
    Report report = { 0x00, SKIP_ROM, 0x00 };

    res = writeToDevice(&report);
    if(res < 0)
        return false;
    do{
        res = readFromDevice(report);
        if(res < 0)
            return false;
    }while(!(report.command == SKIP_ROM));


    if(report.data[0])
        return true;

    return false;
}

bool RelayControl::WireReset()
{
    int res = 0;
    Report report = { 0x00, OW_RESET, 0x00 };

    res = writeToDevice(&report);
    if(res < 0)
        return false;
    do{
        res = readFromDevice(report);
        if(res < 0)
            return false;
    }while(!(report.command == OW_RESET));


    if(report.data[0])
        return true;

    return false;
}

Scratchpad RelayControl::WireScratchpadRead()
{
    Scratchpad buffer = { 0x00 };

    for(int i = 0; i < 9; i++)
        buffer.scratchpad[i] = WireRead();

    return buffer;
}
