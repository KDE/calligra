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


#if !defined(AFX_SOAPDISPATCHHANDLER_H__6D5D16BD_6AE8_48F6_A8FF_D7A56A9CAB34__INCLUDED_)
#define AFX_SOAPDISPATCHHANDLER_H__6D5D16BD_6AE8_48F6_A8FF_D7A56A9CAB34__INCLUDED_

#include "SOAP.h"


class EASYSOAP_EXPORT SOAPDispatchHandlerInterface
{
public:
	virtual ~SOAPDispatchHandlerInterface() {}
	virtual bool ExecuteMethod(const SOAPMethod& request, SOAPMethod& response) = 0;
};



template <typename T>
class EASYSOAP_EXPORT SOAPDispatchHandler : public SOAPDispatchHandlerInterface
{
private:
	typedef void (T::*HandlerFunction)(const SOAPMethod& request, SOAPMethod& response);
	typedef SOAPHashMap<SOAPQName, HandlerFunction> DispatchMap;

	SOAPDispatchHandler(const SOAPDispatchHandler&);
	SOAPDispatchHandler& operator=(const SOAPDispatchHandler&);

	bool ExecuteMethod(const SOAPMethod& request, SOAPMethod& response)
	{
		DispatchMap::Iterator i = m_dispatchMap.Find(request.GetName());
		if (i)
		{
			(m_target->*(*i))(request, response);
			return true;
		}
		return false;
	}

	DispatchMap	m_dispatchMap;
	T			*m_target;

protected:
	SOAPDispatchHandler(T* target = 0)
		: m_target(target)
	{
	}

	void DispatchTo(T *target)
	{
		m_target = target;
	}

	void DispatchMethod(const char *name, const char *ns, HandlerFunction func)
	{
		m_dispatchMap[SOAPQName(name, ns)] = func;
	}

	void DispatchMethod(const SOAPQName& name, HandlerFunction func)
	{
		m_dispatchMap[name] = func;
	}
};


#endif // !defined(AFX_SOAPDISPATCHHANDLER_H__6D5D16BD_6AE8_48F6_A8FF_D7A56A9CAB34__INCLUDED_)
