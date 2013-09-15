#ifndef ITERATORWRAPPER_H
#define ITERATORWRAPPER_H

namespace ezc
{
	/**
	 * A wrapper to wrap iter.begin() and iter.end() together.
	 */
	template <typename StlIteratorType>
	class IteratorWrapper
	{
	public:
		IteratorWrapper(StlIteratorType begin, StlIteratorType end)
		{
			_begin = begin;
			_end = end;
		}
		~IteratorWrapper(void) { }

		StlIteratorType begin() { return _begin; }
		StlIteratorType end() { return _end; }

	private:
		StlIteratorType _begin;
		StlIteratorType _end;
	};
}
#endif