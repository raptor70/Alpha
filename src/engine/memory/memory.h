#ifndef __MEMORY_DECLARATION_H__
#define __MEMORY_DECLARATION_H__

	#if defined(WIN32) && defined(USE_MEMORY)
		void* operator new(size_t _size, const char* _file, int _line, const char* _name = NULL);
		void* operator new[](size_t _size, const char* _file, int _line, const char* _name = NULL);
		void operator delete(void* Ptr);
		void operator delete[](void* Ptr);
		void operator delete(void* Ptr, const char* File, int Line, const char* _name);
		void operator delete[](void* Ptr, const char* File, int Line, const char* _name);
	#endif

#endif

#ifdef MEMORY_UNDEF
	#undef MEMORY_ALLOCATOR
	#undef MEMORY_CHECK
	#undef NEW
	#undef NEW_NAMED
	#undef DELETE
	#undef MEMORY_CUSTOM
	#undef MEMORY_UNDEF
#else
	#ifndef MEMORY_CUSTOM
		#define MEMORY_CUSTOM
		#undef NEW
		#undef NEW_NAMED
		#undef DELETE
		#if defined(WIN32) && defined(USE_MEMORY)
			#define MEMORY_ALLOCATOR
			#define MEMORY_CHECK
			#define NEW new(__FILE__, __LINE__)
			#define NEW_NAMED(name) new(__FILE__, __LINE__,name)
			#define DELETE delete
		#else
			#define NEW new
			#define NEW_NAMED(name) new
			#define DELETE delete
		#endif
	#endif
#endif