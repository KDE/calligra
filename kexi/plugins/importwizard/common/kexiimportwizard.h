#ifndef _KEXI_TABLE_IMPORTER_
#define _KEXI_TABLE_IMPORTER_

#include <qobject.h>
#include <qstringlist.h>
#include "filters/kexifilterwizardbase.h"

class QWidget;

class KexiImportWizardPrivate;
class KexiFilter;

class KexiImportWizard: public KexiFilterWizardBase
{
	Q_OBJECT
public:
	KexiImportWizard(QObject *parent, const char *name=0, const QStringList &list=QStringList());
	~KexiImportWizard();
	virtual void exec(const QString& dialogfilter, const QMap<QString,QString> mimePluginMap,unsigned long importTypes,bool modal);
	virtual void setMode(unsigned long mode);

	virtual QWidget* openWidget(QWidget *parent,const QString& dialogFilter)=0;
	virtual KexiFilter *openPageLeft(QWidget *widget, const QMap<QString,QString> mimePluginMap)=0;
private:
	KexiImportWizardPrivate *d;
signals:
};

#endif
