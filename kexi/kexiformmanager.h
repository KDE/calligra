#ifndef _KEXI_FORM_MANAGER_H_
#define _KEXI_FORM_MANAGER_H_

#include <qdict.h>
#include <qguardedptr.h>
class QString;
class QStringList;
class QDomDocument;
class KexiFormBase;
class KexiWorkspace;
class QWidget;

class KexiProject;

class KexiFormManager
{

public:
	enum Mode{View=0,Edit=1};

	KexiFormManager(KexiProject *project);
	virtual ~KexiFormManager();
	QStringList forms() const;
	bool rename(const QString& oldName, const QString& newName);
	bool deleteForm(const QString& name);
	QString newForm() ;
	void clear();
	void showForm(const QString& name, Mode, QWidget *parent);

protected:
	class Item
	{
	public:
		Item();
		~Item();
		QGuardedPtr<KexiFormBase> form;
		QDomDocument *doc;
	};

private:
	QDict<Item> m_forms;
	KexiProject *m_project;
};

#endif
