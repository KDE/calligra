#include <klocale.h>

#include "kexiuseraction.h"
#include "kexiuseractionmethod.h"

KexiUserActionMethod::KexiUserActionMethod(int method, ArgTypes types, ArgNames names)
{
	m_method = method;
	m_types = types;
	m_names = names;
}

QString
KexiUserActionMethod::methodName(int method)
{
	switch(method)
	{
		case KexiUserAction::OpenObject:
			return i18n("Open Object");
		case KexiUserAction::CloseObject:
			return i18n("Close Object");
		case KexiUserAction::DeleteObject:
			return i18n("Delete Object");
		case KexiUserAction::ExecuteScript:
			return i18n("Execute Script");
		case KexiUserAction::ExitKexi:
			return i18n("Exit Main Application");
		default:
			return QString();
	}
}

