/*
 * BusLightingCutover.h
 *
 * Created: 8/27/2014 10:20:31 PM
 *  Author: Patrick Clark
 */ 


#ifndef BUSLIGHTINGCUTOVER_H_
#define BUSLIGHTINGCUTOVER_H_

#include <stdio.h>

typedef unsigned char u_char;
typedef unsigned int u_int;

static bool SaveData(void *, size_t);
static size_t LoadData(void * );

class LightingController
{

public:
	LightingController();
	~LightingController();
	
private:
	size_t Serialize(void *);
	bool Deserialize(void *, size_t);
	bool IsValidLightingController(void*, size_t);
};


#endif /* BUSLIGHTINGCUTOVER_H_ */