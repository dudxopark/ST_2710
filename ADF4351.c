
#include "ADF4351.h"
#include "adstar.h"

#define PLL_CLK(b)		(b)?((*R_GP2OHIGH)|=(1<<3)):((*R_GP2OLOW)|=(1<<3))
#define PLL_DATA(b)		(b)?((*R_GP2OHIGH)|=(1<<2)):((*R_GP2OLOW)|=(1<<2))
#define PLL_LATCH(b)	(b)?((*R_GP2OHIGH)|=(1<<0)):((*R_GP2OLOW)|=(1<<0))

void ADF4351_WriteData(U32 uData)
{
	int i;
	PLL_LATCH(LOW);
	
	for(i = 0 ; i < 32 ; i++)
	{
		PLL_CLK(LOW);
		//delayus(1);
		PLL_DATA(uData & 0x8000);
		//delayus(1);
		PLL_CLK(HIGH);
		//delayus(1);
		uData = uData << 1;
	}
	PLL_CLK(LOW);	
	
	PLL_LATCH(HIGH);
	PLL_LATCH(HIGH);
	PLL_LATCH(LOW);
}
