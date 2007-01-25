/* Coded by M. Tavella
 * email michele@liralab.it
 * web   http://mt.homelinux.org 
 */
#ifndef MCONTAINER_H
#define MCONTAINER_H

#include "mInclude.h"
#include "mClass.h"

template <typename mType> 
class mContainer : public mClass	
{
	public:
		mContainer (unsigned const int size = 1024);
		~mContainer (void);
		void Init (void);
		bool Set (mType element, unsigned int position = -1);
		bool Get (mType *element, unsigned int position = -1);
		bool Empty (void);
		bool Full  (void);
		unsigned int Last (void);
	
	private:
		void Alloc (void);
		void Dealloc (void);

		/* Remember: I need to learn how to use inheritance 
		 * with templates. 
		 * This new skill should allow me to declare _data as
		 * protected and then write a new class where _data is public.
		 */

	public:
		mType *_data;
		bool *_status;

	private:
		unsigned int _size;
		unsigned int _last;
};


template <typename mType> 
mContainer<mType>::mContainer (unsigned const int size) 
{
	SetName ("mContainer");
	
	_data = NULL;
	_size = size;

	PrintName ();
	printf ("Starting %s with %d elements\n", GetName (), _size);

	Alloc ();
}


template <typename mType> 
mContainer<mType>::~mContainer (void) 
{
	PrintName ();
	printf ("Destroying %s\n", GetName ());
	Dealloc ();	
}

	
template <typename mType> 
void mContainer<mType>::Init (void) 
{
	PrintName ();
	printf ("Init procedure called\n");
	Dealloc ();	
	Alloc ();	
}

	
template <typename mType> 
bool mContainer<mType>::Set (mType element, unsigned int position) 
{
	if (position < 0 || position >= _size)
		return false;
	else if (memcpy (_data + position, &element, sizeof (mType)) != NULL)
	{
		_status [position] = true;
		_last = position;
		return true;
	}
	
	return false;
}

	
template <typename mType> 
bool mContainer<mType>::Get (mType *element, unsigned int position) 
{
	if (position < 0 || position >= _size)
		return false;
	else if (memcpy (element, _data + position, sizeof (mType)) != NULL)
	{
		return true;
	}
	
	return true;
}


template <typename mType> 
bool mContainer<mType>::Empty (void) 
{
	for (unsigned int i = 0; i < _size; i++)
		if (_status [i] == true)
			return false;

	return true;
}


template <typename mType> 
unsigned int mContainer<mType>::Last (void) 
{
	return _last;
}


template <typename mType> 
bool mContainer<mType>::Full (void) 
{
	unsigned int elements = 0;
	for (unsigned int i = 0; i < _size; i++)
		if (_status [i] == true)
			elements++;

	return (elements == _size);
}

	
template <typename mType> 
void mContainer<mType>::Alloc (void) 
{
	_data   = new mType [_size];
	_status = new bool [_size]; 
	
	for (unsigned int i = 0; i < _size; i++)
		_status [i] = false;
}


template <typename mType> 
void mContainer<mType>::Dealloc (void) 
{
	_last = 0;

	if (_data != NULL)
		delete [] _data;
	
	if (_status != NULL)
		delete _status;
}

#endif
