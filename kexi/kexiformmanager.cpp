#include "kexiformmanager.h"
#include "kexiformbase.h"
#include "kexiworkspace.h"
#include <klocale.h>

#include <qstring.h>
#include <qstringlist.h>
#include <qdom.h>

KexiFormManager::KexiFormManager()
{
}

KexiFormManager::~KexiFormManager()
{
}

QStringList KexiFormManager::forms() const
{
}

bool KexiFormManager::rename(const QString& oldName, const QString& newName)
{
	Item *item=m_forms.take(oldName);
	if (!item) return false;
	if (m_forms[newName]==0) 
	{
		m_forms.insert(newName,item);
		return true;
	}
	else
	{
		m_forms.insert(oldName,item);
		return false;
	}
}

bool KexiFormManager::deleteForm(const QString& name)
{
	Item *item=m_forms.take(name);
	if (!item) return false;
	delete item;
	return true;
}

QString KexiFormManager::newForm()
{
	QString formname=i18n("New Form");
	if (!(m_forms[formname]==0))
	{
		QString tmpname=i18n("New Form <%1>");
		for (long i=2;(!(m_forms[formname=tmpname.arg(i)]==0));i++);
	}
	Item *item=new Item();
	m_forms.insert(formname,item);
	return formname;
}

void KexiFormManager::clear()
{
}

void KexiFormManager::showForm(const QString& name, Mode, QWidget *parent)
{
	Item *item=m_forms[name];
	if (!item) return;
	if (item->form.isNull())
	{
        	item->form = new KexiFormBase(parent, "form",name);
                static_cast<KexiWorkspace*>(parent->qt_cast("KexiWorkspace"))->addItem(item->form);
                item->form->show();
	}
	else
		item->form->showNormal();
}


KexiFormManager::Item::Item()
{
	doc=0;
}

KexiFormManager::Item::~Item()
{
	delete form;
	delete doc;
}

