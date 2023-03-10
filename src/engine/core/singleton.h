#ifndef __SINGLETON_H__
#define __SINGLETON_H__

#define SINGLETON_Declare( _CLASS_ ) \
    public :  \
		static INLINE _CLASS_ & GetInstance()\
		{ \
			if( !m_instance ) \
			{ \
				m_instance = NEW _CLASS_();\
			}\
			return *m_instance; \
		} \
\
		static void DestroyInstance()\
		{\
			DELETE m_instance;\
			m_instance = 0; \
		}\
\
		static Bool HasInstance()\
		{\
			return m_instance != 0;\
		}\
\
    private :  \
		static _CLASS_ *  m_instance;

// ..

#define SINGLETON_Define( _CLASS_ ) _CLASS_ * _CLASS_::m_instance = 0;

#endif