/*
 * BusLightingCutover.cpp
 *
 * Created: 8/27/2014 2:12:08 PM
 *  Author: Patrick
 */ 


#include <avr/io.h>
#include <avr/sfr_defs.h>
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

void LightingController::Set_5A_State(State _state)
{
	if(_state == On)
	{
		PORTB |= _BV(PINB4);
		m_5A_State = _state;
	}
	else
	{
		PORTB &= ~(_BV(PINB4));
		m_5A_State = _state;
	}
}

void LightingController::Set_40A1_State(State _state)
{
	if(_state == On)
	{
		PORTB |= _BV(PINB5);
		m_40A1_State = _state;
	}
	else
	{
		PORTB &= ~(_BV(PINB5));
		m_40A1_State = _state;
	}
}

void LightingController::Set_40A2_State(State _state)
{
	if(_state == On)
	{
		PORTB |= _BV(PINB6);
		m_40A2_State = _state;
	}
	else
	{
		PORTB &= ~(_BV(PINB6));
		m_40A2_State = _state;
	}
}

State LightingController::Get_5A_State()
{
	return m_5A_State;
}

State LightingController::Get_40A1_State()
{
	return m_40A1_State;
}

State LightingController::Get_40A2_State()
{
	return m_40A2_State;
}

size_t LightingController::Serialize(void * lc)
{
	return true;
}

	
bool LightingController::Deserialize(void * lc, size_t sz)
{
	bool successfulLoad = false;
	if(IsValidLightingController(lc, sz))
	{
		
	}
	return successfulLoad;
}

bool LightingController::IsValidLightingController(void * lc, size_t sz)
{
	return false;
}

size_t LightingController::SizeOf()
{
	size_t retVal = 0;
	retVal += sizeof(m_5A_State);
	retVal += sizeof(m_40A1_State);
	retVal += sizeof(m_40A2_State);
	return retVal;	
}

static bool SaveData(void * data, size_t sz)
{
	u_char* bytes = (u_char*)data;
	eeprom_busy_wait();
	//Size will be first byte so read is easier.
	eeprom_write_byte(0, (u_char)sz);
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
	size = eeprom_read_byte(0);
	u_char* bytes = (u_char*)data;
	for(u_int i = 1; i <= size; i++)
	{
		eeprom_busy_wait();
		*bytes = eeprom_read_byte((u_char*)i);
		++bytes;
	}
	return size;
}

static int uart_putchar(char c, FILE *stream)
{
	if (c == '\n')
	uart_putchar('\r', stream);
	loop_until_bit_is_set(UCSRA, UDRE);
	UDR = c;
	return 0;
}
