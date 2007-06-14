/*
 *  Copyright (C) 2007 Michele Tavella <michele@liralab.it>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
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
