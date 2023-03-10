#ifndef __PADKEY_H__
    #define __PADKEY_H__

enum PADKEY
{
	PADKEY_Dpad_Up			= (1<<0),
	PADKEY_Dpad_Down		= (1<<1),
	PADKEY_Dpad_Left		= (1<<2),
	PADKEY_Dpad_Right		= (1<<3),
	PADKEY_Start			= (1<<4),
	PADKEY_Select			= (1<<5),
	PADKEY_LeftTrigger		= (1<<6),
	PADKEY_RightTrigger		= (1<<7),
	PADKEY_LeftButton		= (1<<8),
	PADKEY_RightButton		= (1<<9),
	PADKEY_A				= (1<<10),
	PADKEY_B				= (1<<11),
	PADKEY_X				= (1<<12),
	PADKEY_Y				= (1<<13)
};

#endif