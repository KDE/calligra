#ifndef _KEXI_FILE_IMPORTER_
#define _KEXI_FILE_IMPORTER_

#include <qobject.h>
#include <qstringlist.h>
#include <kexiimportwizard.h>
#include <filters/kexifilter.h>
#include <filters/kexifilterwizardbase.h>

class KexiFileImportWizard: public KexiImportWizard
{
	Q_OBJECT
public:
	KexiFileImportWizard(QObject *parent, const char *name=0, const QStringList &list=QStringList());
	~KexiFileImportWizard();
	virtual QWidget* openWidget(QWidget *parent,const QString& dialogFilter);
	virtual KexiFilter *openPageLeft(QWidget *widget, const QMap<QString,QString> mimePluginMap);
};

#endif
