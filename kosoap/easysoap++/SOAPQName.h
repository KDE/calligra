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


#if !defined(AFX_SOAPQNAME_H__E392FAB3_3022_11D5_B3F3_000000000000__INCLUDED_)
#define AFX_SOAPQNAME_H__E392FAB3_3022_11D5_B3F3_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class EASYSOAP_EXPORT SOAPQName  
{
public:
	SOAPQName()
	{
	}

	SOAPQName(const char *name, const char *ns = 0)
	{
		Set(name, ns);
	}

	SOAPQName(const SOAPQName& that)
	{
		m_name = that.m_name;
		m_namespace = that.m_namespace;
	}

	~SOAPQName()
	{
	}

	SOAPQName& operator=(const SOAPQName& that)
	{
		m_name = that.m_name;
		m_namespace = that.m_namespace;
		return *this;
	}

	SOAPQName& operator=(const char *name)
	{
		Set(name);
		return *this;
	}

	void Set(const char *name, const char *ns = 0);

	bool operator==(const SOAPQName& that) const
	{
		return m_name == that.m_name
			&& m_namespace == that.m_namespace;
	}

	bool operator!=(const SOAPQName& that) const
	{
		return m_name != that.m_name
			|| m_namespace != that.m_namespace;
	}

	bool operator==(const char *) const;

	SOAPString& GetName()
	{
		return m_name;
	}

	const SOAPString& GetName() const
	{
		return m_name;
	}

	const SOAPString& GetNamespace() const
	{
		return m_namespace;
	}

	void Clear();

	bool IsUndefined()
	{
		return m_name.IsEmpty() && m_namespace.IsEmpty();
	}

private:
	SOAPString	m_name;
	SOAPString	m_namespace;
};

struct SOAPHashCodeFunctor<SOAPQName>
{
	size_t operator()(const SOAPQName& val) const
	{
		return sp_hashcode(val.GetName()) ^ sp_hashcode(val.GetNamespace());
	}
};


#endif // !defined(AFX_SOAPQNAME_H__E392FAB3_3022_11D5_B3F3_000000000000__INCLUDED_)


