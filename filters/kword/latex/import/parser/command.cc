
#include "command.h"

Command::Command()
{
	_options.setAutoDelete(true);
	_params.setAutoDelete(true);
}

Command::Command(const char* name)
{
	Command::Command();
	_name = name;
}

Command::Command(const char* name, QPtrList<QPtrList<Element> >* groups)
{
	Command::Command();
	_name = name;
	if(groups != NULL)
		_elements = *groups;
}

Command::Command(const char* name, QPtrList<QPair<QString, QString> >* options,
		QPtrList<QPtrList<Element> >* groups)
{
	Command::Command();
	_name = name;
}

Command::Command(const char* name, QPtrList<QPair<QString, QString> >* options1, QPtrList<QPair<QString, QString> >* options2, QPtrList<QPtrList<Element> >* groups)
{
	Command::Command();
	_name = name;
}

Command::~Command()
{
}

void Command::addParam(const char* name)
{
	QString test = QString(name);
	QString key = test.left(test.find("="));
	QString value = test.right(test.find("="));
	addParam(key, value);
}

void Command::addOption(const char* name)
{
	QString test = QString(name);
	QString key = test.left(test.find("="));
	QString value = test.right(test.find("="));
	addOption(key, value);
}

