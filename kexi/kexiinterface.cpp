

#include "kexiinterfacetemplate.h"
#include "kexiinterface.h"

#include <klibloader.h>
#include <kdebug.h>

KexiInterface::KexiInterface(QString libname, QString driver, QString description, QObject *parent, KLibLoader *libloader) : QObject(parent, libname)
{
	m_libLoader = libloader;
	m_description = description;
	m_libName = libname;
	loaded = false;
	
}

bool KexiInterface::load()
{
	if(!loaded)
	{
		kdDebug() << "go load the lib!" << endl;
		m_lib = m_libLoader->library(m_libName);
	
		if(m_lib)
		{
			kdDebug() << "library '" << m_libName << "' loaded" << endl;
			if(m_lib->symbol("plugin_info"))
			{
				loaded = true;
				kdDebug() << "  library is valid" << endl;
				m_info = static_cast<pluginInfo *>(m_lib->symbol("plugin_info"));
				kdDebug() << "sizeof(m_info): " << sizeof(m_info) << endl;
				kdDebug() << "  lib: autor = " << m_info->author << endl;
				if(m_info->init_func)
				{
					m_db = m_info->init_func(this);
				}
			
			}
			else
			{
				kdDebug() << "  invalide library" << endl;
				return false;
			}
		}
		else
		{
//		delete m_lib;
			kdDebug() << "library '" << m_libName << "' doesn't exists, or isn't ready!" << endl;
			return false;
		}
	}

	return true;
}

bool KexiInterface::unload()
{
	if(loaded)
	{
		delete m_info;
		m_lib->unload();
		loaded = false;
	}
	
	return true;
}

/*DBLocation KexiInterface::location()
{
	if(loaded)
	{
		return m_info->location;
	}
	else
	{
		return None;
	}
}*/

KexiInterface::~KexiInterface()
{
}

//#include "kexiinterface.moc"

#include "kexiinterface.moc"
