#include "kexipart.h"

namespace KexiPart
{

Part::Part(QObject *parent, const char *name, const QStringList &)
{
	m_info = 0;
}

Part::~Part()
{
}

}

#include "kexipart.moc"

