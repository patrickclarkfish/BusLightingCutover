/*
 * BusLightingCutover.cpp
 *
 * Created: 8/27/2014 2:12:08 PM
 *  Author: Patrick
 */ 


#include <avr/io.h>
#include <avr/eeprom.h>
#include "BusLightingCutover.h"

int main(void)
{
	LightingController lc = LightingController();
	
    while(1)
    {
        //TODO:: Please write your application code 
    }
}


LightingController::LightingController()
{
	u_char data[256];
	size_t size = LoadData(data);
	this->Deserialize(data, size);
}

LightingController::~LightingController()
{
	u_char data[256];
	size_t size = Serialize((void*)data);
	if(IsValidLightingController(data, size))
		SaveData(data, size);
}

size_t LightingController::Serialize(void * lc)
{
	return false;
}

	
bool LightingController::Deserialize(void * lc, size_t sz)
{
	if(IsValidLightingController(lc, sz))
	{
		
		return true;	
	}
	return false;
}

bool LightingController::IsValidLightingController(void * lc, size_t sz)
{
	return false;
}
static bool SaveData(void * data, size_t sz)
{
	u_char* bytes = (u_char*)data;
	eeprom_busy_wait();
	//Size will be first byte so read is easier.
	eeprom_write_byte(0, (u_char)size);
	for(u_int i = 1; i <= sz; i++)
	{
		eeprom_busy_wait();
		eeprom_write_byte((u_char*)i, *bytes);
		++bytes;
	}
	return false;	
}

static size_t LoadData(void * data)
{
	size_t size = 0;
	//Read size from first byte of eeprom.
	eeprom_busy_wait();
	
	return size;
}
