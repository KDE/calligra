
#include "env.h"

Env::Env()
{
	setType(ELT_ENV);
}

Env::Env(const char* command)
{
	Env();
	setType(ELT_ENV);
	/* Parse the command name */
	QString pattern = QString(command);
	int pos = pattern.find("{");
	if(pos != -1)
		_name = pattern.mid(pos, pos + pattern.length() - 1);
}

Env::Env(QString name) : _name(name)
{
	Env();
}

Env::~Env()
{
}

void Env::setChildren(QPtrList<Element>* children)
{
	if(children != NULL)
		_children = *children;
}
