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
class KexiView;

class KexiFormManager: public KexiProjectHandler
{

public:
	enum Mode{View=0,Edit=1};

        KexiQueryPart(QObject *project,const char *,const QStringList &);
	virtual ~KexiFormManager();

        virtual QString                         name();
        virtual QString                         mime();
        virtual bool                            visible();


        virtual void hookIntoView(KexiView *view);

        virtual void store (KoStore *){;}
        virtual void load  (KoStore *){;}


        virtual QPixmap                         groupPixmap();
        virtual QPixmap                         itemPixmap();

	


	QStringList forms() const;
	bool rename(const QString& oldName, const QString& newName);
	bool deleteForm(const QString& name);
	QString newForm() ;
	void clear();
	void showForm(const QString& name, Mode, KexiView *view);

protected:
	class KexiProjectHandlerItem
	{
	public:
		Item(KexiFormManager *manager,QString name, QString identifier);
		~Item();
		QGuardedPtr<KexiFormBase> form;
		QDomDocument *doc;
	};

};

#endif
