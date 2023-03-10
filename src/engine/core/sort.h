#ifndef __SORT_H__
#define __SORT_H__

template <typename T, typename OP>
static Bool SortCompare(const T& _a, const T& _b, OP _op)
{
  return _op(_a,_b);
}

template<typename T>
static Bool	SortOperatorAsc(const T& _a, const T& _b)
{
	return (_a < _b);
}

template<typename T>
static Bool	SortOperatorDesc(const T& _a, const T& _b)
{
	return (_a > _b);
}

template<typename T, typename OP>
void	SortAlgo(T*	_array, S32 _first, S32 _last, OP _op)
{
	if( _first < _last )
	{
		S32 pivot = (_first + _last) / 2;

		// split
		Swap(_array[pivot],_array[_last]);
		S32 j = _first;
		for(S32 i = _first; i<=_last-1;i++)
		{
			if( SortCompare(_array[i],_array[_last],_op) )
			{
				Swap(_array[i],_array[j]);
				j++;
			}
		}
		Swap(_array[_last],_array[j]);
		pivot = j;

		SortAlgo(_array,_first,pivot-1,_op);
		SortAlgo(_array,pivot+1,_last,_op);
	}
}

template<typename T>
void	SortAsc(T*	_array, S32 _first, S32 _last)
{
	SortAlgo(_array,_first,_last,SortOperatorAsc<T>);
}

template<typename T>
void	SortAsc(ArrayOf<T>&	_array)
{
	SortAlgo(_array.GetPointer(),0,_array.GetCount()-1,SortOperatorAsc<T>);
}

template<typename T>
void	SortDesc(T*	_array, S32 _first, S32 _last)
{
	SortAlgo(_array,_first,_last,SortOperatorDesc<T>);
}

template<typename T>
void	SortDesc(ArrayOf<T>&	_array)
{
	SortAlgo(_array.GetPointer(),0,_array.GetCount()-1,SortOperatorDesc<T>);
}

#endif