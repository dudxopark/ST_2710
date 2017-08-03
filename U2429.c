
#include "U2429.h"
#include "adstar.h"

#define VOL_CLK(b)		(b)?((*R_GP2OHIGH)|=(1<<1)):((*R_GP2OLOW)|=(1<<1))
#define VOL_DATA(b)		(b)?((*R_GP2OHIGH)|=(1<<2)):((*R_GP2OLOW)|=(1<<2))

void U2429_SetVolume(int nCh, double dVolume)
{
	int nDB;
	nDB = (int)(dVolume * 83 + .5);
	nDB -= 83;
	
	U2429_SetVolumeByNegativeDB(nCh, nDB);
}

void U2429_SetVolumeByNegativeDB(int nCh, int nDB)
{
	u8 uNegativeDB, uATT1, uATT2, uATT;
	u16 uData;
	uData = 0x0600;
	if(nCh == 1)
		uData |= 0x0002;
	else
		uData |= 0x0003;
	
//	debugprintf("nDB: %d, ", nDB);
	if(nDB <= -84)
		nDB = -83;
	uNegativeDB = nDB + 87;
	uATT1 = uNegativeDB & 0x7C;
	uATT2 = (uNegativeDB & 0x03);
	uATT = uATT1 | (uATT2 << 7);
	uData |= uATT; 
		
//	debugprintf("uData: 0x%04x\r\n", uData);
	
	U2429_WriteData(uData);
}

void U2429_WriteData(U16 uData)
{
	int i;
	for(i = 0 ; i < 11 ; i++)
	{
		VOL_CLK(LOW);
		delayus(1);
		VOL_DATA(uData & 0x01);
		delayus(1);
		VOL_CLK(HIGH);
		delayus(1);
		uData = uData >> 1;
	}
	VOL_CLK(LOW);	
}
