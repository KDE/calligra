/* 
 * EasySoap++ - A C++ library for SOAP (Simple Object Access Protocol)
 * Copyright (C) 2001 David Crowley; SciTegic, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#if !defined(AFX_SOAPDEBUGGER_H__2ED6E429_A5CB_4FEB_B1B1_19E83A2F8046__INCLUDED_)
#define AFX_SOAPDEBUGGER_H__2ED6E429_A5CB_4FEB_B1B1_19E83A2F8046__INCLUDED_

#include <stdio.h>
#include "SOAP.h"

class EASYSOAP_EXPORT SOAPDebugger
{
public:
	static void SetFile(const char *fname);
	static void SetMessageLevel(int level);
	static void Close();
	static void Write(int level, const char *bytes, size_t len);
	static void Print(int level, const char *str, ...);

private:
	SOAPDebugger();
	//~SOAPDebugger();

	static FILE	*m_file;
	static int	m_messageLevel;
};

#endif // !defined(AFX_SOAPDEBUGGER_H__2ED6E429_A5CB_4FEB_B1B1_19E83A2F8046__INCLUDED_)

