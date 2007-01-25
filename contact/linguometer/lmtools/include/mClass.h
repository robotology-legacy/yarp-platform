/* Coded by M. Tavella
 * email michele@liralab.it
 * web   http://mt.homelinux.org 
 */
#ifndef MCLASS_H
#define MCLASS_H

#define MCLASS_OBJ_NAME_DEFAULT		"empty"
#define MCLASS_OBJ_NAME_SIZE		512

#include <mInclude.h>

class mClass
{
private:
	char _name [MCLASS_OBJ_NAME_SIZE];
	
public:
	mClass (void)
	{
		memset (_name, 0, MCLASS_OBJ_NAME_SIZE);
		strcpy (_name, MCLASS_OBJ_NAME_DEFAULT);
	}

	void SetName(char *name)
	{
		memset (_name, 0, MCLASS_OBJ_NAME_SIZE);
		strcpy (_name, name);
	}
	
	void GetName (char *name)
	{
		memset (name, 0, MCLASS_OBJ_NAME_SIZE);
		strcpy (name, _name);
	}
	
	const char * const GetName (void)
	{
		return _name;
	}
	
	void PrintName (void)
	{
		printf("[%s] ", _name);
	}
};

#endif
