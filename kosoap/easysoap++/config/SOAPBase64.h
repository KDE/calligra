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


#if !defined(AFX_SOAPBASE64_H__8831A358_2EA1_11D5_B3F3_000000000000__INCLUDED_)
#define AFX_SOAPBASE64_H__8831A358_2EA1_11D5_B3F3_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class EASYSOAP_EXPORT SOAPBase64  
{
public:
	static void Encode(const SOAPArray<char>& bytes, SOAPString& str);
	static void Decode(const SOAPString& str, SOAPArray<char>& bytes);
private:
	SOAPBase64();
	//~SOAPBase64();

};

#endif // !defined(AFX_SOAPBASE64_H__8831A358_2EA1_11D5_B3F3_000000000000__INCLUDED_)



