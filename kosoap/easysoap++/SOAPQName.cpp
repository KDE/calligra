// SOAPQName.cpp: implementation of the SOAPQName class.
//
//////////////////////////////////////////////////////////////////////

#include "SOAP.h"
#include "SOAPQName.h"
#include "SOAPNamespaces.h"

void
SOAPQName::Set(const char *name, const char *ns)
{
	if (ns)
	{
		m_name = name;
		m_namespace = ns;
	}
	else
	{
		const char *psep = sp_strchr(name, PARSER_NS_SEP[0]);
		if (psep)
		{
			m_name = (psep + 1);
			m_namespace = "";
			m_namespace.Append(name, psep - name);
		}
		else if ((psep = sp_strchr(name, ':')))
		{
			throw SOAPException("You cannot set a QName to a value with an unexpanded namespace: %s", name);
		}
		else
		{
			m_name = name;
			m_namespace = "";
		}
	}
}

void
SOAPQName::Clear()
{
	m_name = "";
	m_namespace = "";
}

bool
SOAPQName::operator==(const char *str) const
{
	const char *sep = sp_strstr(str, PARSER_NS_SEP);
	if (sep)
	{
		return sp_strncmp(m_namespace, str, sep - str) == 0
			&& sp_strcmp(m_name, sep + 1) == 0;
	}
	else
	{
		return m_name == str;
	}
}