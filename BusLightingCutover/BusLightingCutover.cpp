/*
 * BusLightingCutover.cpp
 *
 * Created: 8/27/2014 2:12:08 PM
 *  Author: Patrick
 */ 


#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include "BusLightingCutover.h"
//Delay comes after BusLightingCutover.h to define clock freq.
#include <util/delay.h>

EMPTY_INTERRUPT(BADISR_vect);

LightingController * lc = NULL;
u_char g_archive;

ISR(PCINT1_vect, ISR_NAKED)
{
	if(lc != NULL)
	{	
		if(lc->IsSwitchClosed())
			lc->Set_5A_State(On);
		else
			lc->Set_5A_State(Off);
	}
	reti();
}

ISR(TIMER0_OVF_vect)
{
	if(g_archive++ == 0)
		lc->Archive();
}

int main(void)
{
	PCMSK1 |= _BV(PCINT8);
	GIMSK |= _BV(PCIE1);
	LightingController m_lc = LightingController();
	lc = &m_lc;
    _delay_ms(1000);
	sei();
	while(1);
}


LightingController::LightingController()
{
	m_initialized = false;
	m_archive = false;
	DDRA &= ~(_BV(PINA0));
	DDRB = _BV(PINB4) | _BV(PINB3) | _BV(PINB2);
	TCCR0B |= _BV(CS02) | _BV(CS00);
	TIMSK |= _BV(TOIE0);
	//u_char data[256];
	//size_t size = LoadData(data);
	//this->Deserialize(data, size);
	m_initialized = true;
}

LightingController::~LightingController()
{
	u_char data[256];
	size_t size = Serialize((void*)data);
	if(IsValidLightingController(data, size))
		SaveData(data, size);
}

void LightingController::Archive()
{
	if(lc->IsArchiveSet())
	{
		u_char data[5] = {0xFF, 0xFA, 0xD5, 0x04, 0x3E};
		size_t sz = 3;//lc->Serialize((void*) data);
		SaveData(data, sz);
		m_archive = false;
	}
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
	SetArchive();
}

void LightingController::Set_40A1_State(State _state)
{
	if(_state == On)
	{
		PORTB |= _BV(PINB3);
		m_40A1_State = _state;
	}
	else
	{
		PORTB &= ~(_BV(PINB3));
		m_40A1_State = _state;
	}
	SetArchive();
}

void LightingController::Set_40A2_State(State _state)
{
	if(_state == On)
	{
		PORTB |= _BV(PINB2);
		m_40A2_State = _state;
	}
	else
	{
		PORTB &= ~(_BV(PINB2));
		m_40A2_State = _state;
	}
	SetArchive();
}

State LightingController::Toggle_5A_State()
{
	if(Get_5A_State() == On)
	{
		Set_5A_State(Off);
		return Off;
	}
	Set_5A_State(On);
	return On;
}

State LightingController::Toggle_40A1_State()
{
	if(Get_40A1_State() == On)
	{
		Set_40A1_State(Off);
		return Off;
	}
	Set_40A1_State(On);
	return On;
}

State LightingController::Toggle_40A2_State()
{
	if(Get_40A2_State() == On)
	{
		Set_40A2_State(Off);
		return Off;
	}
	Set_40A2_State(On);
	return On;
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
	u_char * pos = (u_char*) lc;
	if(sizeof(lc) >= SizeOf())
	{
		pos[0] = (u_char)Get_5A_State();
		pos[1] = (u_char)Get_40A1_State();
		pos[2] = (u_char)Get_40A2_State();
		return true;
	}
	return false;
}

	
bool LightingController::Deserialize(void * lc, size_t sz)
{
	bool successfulLoad = false;
	if(IsValidLightingController(lc, sz))
	{
		u_char * pos = (u_char*)lc;
			Set_5A_State((State)pos[0]);
			Set_40A1_State((State)pos[1]);
			Set_40A2_State((State)pos[2]);
		successfulLoad = true;
	}	
	return successfulLoad;
}

bool LightingController::IsValidLightingController(void * lc, size_t sz)
{
	if(sz == this->SizeOf())
	{
		u_char * pos = (u_char*)lc;
		for(u_int i = 0; i < sz; i++)
		{
			if(pos[i] < 0 || pos[i] > 1)
				return false;
		}
		return true;
	}
	return false;
}

bool LightingController::IsSwitchClosed()
{
	return bit_is_set(PINA, PINA0);
}

bool LightingController::IsArchiveSet()
{
	return m_archive;
}

size_t LightingController::SizeOf()
{
	size_t retVal = 0;
	retVal += sizeof(m_5A_State);
	retVal += sizeof(m_40A1_State);
	retVal += sizeof(m_40A2_State);
	return retVal;	
}

void LightingController::SetArchive()
{
	if(m_initialized)
		m_archive = true;
}

static bool SaveData(void * data, size_t sz)
{
	u_char* bytes = (u_char*)data;
	eeprom_busy_wait();
	//Size will be first byte so read is easier.
	eeprom_write_byte((u_char*)5, (u_char)sz);
	for(u_int i = 6; i <= sz; i++)
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
	size = eeprom_read_byte((u_char*)5);
	u_char* bytes = (u_char*)data;
	for(u_int i = 6; i <= size; i++)
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
