#ifndef __RENDERER_DRIVER_DATA_H__
#define __RENDERER_DRIVER_DATA_H__

#define RENDERERDRIVER_DATA	\
public:\
	friend class RendererDriverGL;	\
	friend class RendererDriverVulkan;	\
	void* m_pDriverData = NULL; \
private:

#endif