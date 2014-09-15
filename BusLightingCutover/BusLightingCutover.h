/*
 * BusLightingCutover.h
 *
 * Created: 8/27/2014 10:20:31 PM
 *  Author: Patrick Clark
 */ 


#ifndef BUSLIGHTINGCUTOVER_H_
#define BUSLIGHTINGCUTOVER_H_

#include <stdio.h>

// Typedefs
typedef unsigned char u_char;
typedef unsigned int u_int;

// Static Declarations
static bool SaveData(void *, size_t);
static size_t LoadData(void * );
static int uart_putchar(char c, FILE *stream);
// TODO: Fix this for C++
//static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

#define F_CPU 8000000UL // 8 MHz

enum State
{
	On,
	Off	
};

class LightingController
{

public:
	LightingController();
	~LightingController();
	
	bool IsSwitchClosed();
	bool IsArchiveSet();
	void Set_5A_State(State);
	void Set_40A1_State(State);
	void Set_40A2_State(State);
	void Archive();
	State Toggle_5A_State();
	State Toggle_40A1_State();
	State Toggle_40A2_State();
	State Get_5A_State();
	State Get_40A1_State();
	State Get_40A2_State();
		
private:
	size_t Serialize(void *);
	bool Deserialize(void *, size_t);
	bool IsValidLightingController(void*, size_t);
	size_t SizeOf();
	void SetArchive();
	
	bool m_initialized;
	bool m_archive;
	State m_5A_State;
	State m_40A1_State;
	State m_40A2_State;
};


#endif /* BUSLIGHTINGCUTOVER_H_ */