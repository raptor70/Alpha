#ifndef __PRIMITIVE_H__
#define __PRIMITIVE_H__

// TYPES
typedef bool				Bool;
typedef char				S8;
typedef unsigned char		U8;
typedef short				S16;
typedef unsigned short		U16;
typedef long				S32;
typedef unsigned long		U32;
typedef long long			S64;
typedef unsigned long long	U64;
typedef float				Float;
typedef double				Double;
typedef char				Char;

// VALUES
#undef TRUE
#undef FALSE

#undef NULL
#define NULL 0

#define TRUE true
#define FALSE false

#define Float_Eps             0.000001f
#define Float_Max             FLT_MAX
#define U32_Max				  -1

// CLASSES
#define BEGIN_SUPER_CLASS(_name,_parent) class _name : public _parent { typedef _parent SUPER;
#define END_SUPER_CLASS	};

// RVALUE
#define FWD(...) ::std::forward<decltype(__VA_ARGS__)>(__VA_ARGS__)

// C++
#define EXTERN extern
#ifdef WIN32
#define OVERRIDE	override
#define FINAL		sealed
#define INLINE		__forceinline
#else
#define OVERRIDE	
#define FINAL		
#endif
// HELPER
template<typename T>
void	Swap(T& _a, T& _b)
{
	T tmp = _a;
	_a = _b;
	_b = tmp;
}

// OPEARTOR ENUM FLAGS
#define DECLARE_FLAG_OPERATOR(__class__) \
INLINE constexpr __class__ operator~ (__class__ _a) { return (__class__)~(U32)_a; }											   \
INLINE constexpr __class__ operator| (__class__ _a, __class__ _b) { return (__class__)((U32)_a | (U32)_b); }		   \
INLINE constexpr __class__ operator& (__class__ _a, __class__ _b) { return (__class__)((U32)_a & (U32)_b); }		   \
INLINE constexpr __class__ operator^ (__class__ _a, __class__ _b) { return (__class__)((U32)_a ^ (U32)_b); }		   \
INLINE constexpr __class__& operator|= (__class__& _a, __class__ _b) { return (__class__&)((U32&)_a |= (U32)_b); }		   \
INLINE constexpr __class__& operator&= (__class__& _a, __class__ _b) { return (__class__&)((U32&)_a &= (U32)_b); }		   \
INLINE constexpr __class__& operator^= (__class__& _a, __class__ _b) { return (__class__&)((U32&)_a ^= (U32)_b); }

// Object Generic (to avoid use of void*)
class Object
{
	virtual void Dummy() {}
};

#endif

