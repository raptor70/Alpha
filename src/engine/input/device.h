#ifndef __DEVICE_H__
#define __DEVICE_H__

#include "window/window_component.h"

class Device
{
public:
	Device(){}
	virtual ~Device( void ){}

	virtual Bool Initialize() = 0;
	virtual void Update(Float _dTime, const WindowComponent & window) = 0;
	virtual void Finalize() = 0;

protected:
};

typedef RefTo<Device> DeviceRef;
typedef ArrayOf<DeviceRef> DeviceRefArray;

#endif