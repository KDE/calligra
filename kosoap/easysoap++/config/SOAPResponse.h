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


#if !defined(AFX_SOAPRESPONSE_H__B8BDA9D2_F6F6_4312_BFAD_B34A813A3B55__INCLUDED_)
#define AFX_SOAPRESPONSE_H__B8BDA9D2_F6F6_4312_BFAD_B34A813A3B55__INCLUDED_

#include "SOAP.h"

#include "SOAPEnvelope.h"

class EASYSOAP_EXPORT SOAPResponse : public SOAPEnvelope
{
public:
	SOAPResponse();
	virtual ~SOAPResponse();

	void SetMethod(const SOAPMethod& meth);

	bool IsFault() const
	{
		return GetBody().IsFault();
	}

	const SOAPParameter& GetReturnValue(int i = 0) const
	{
		return GetBody().GetMethod().GetParameter(i);
	}

private:
};

#endif // !defined(AFX_SOAPRESPONSE_H__B8BDA9D2_F6F6_4312_BFAD_B34A813A3B55__INCLUDED_)

