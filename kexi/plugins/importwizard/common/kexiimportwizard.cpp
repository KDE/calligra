#include "kexiimportwizard.h"
#include "kexiimportwizard.moc"
#include "kexiimportwizardform.h"
#include <qlayout.h>

class KexiImportWizardPrivate
{
public:
	KexiImportWizardPrivate():widget(0){}
	~KexiImportWizardPrivate(){delete widget;}
	KexiTableImportForm *widget;
};

KexiImportWizard::KexiImportWizard(QObject *parent,
	const char *name,const QStringList &list):KexiFilterWizardBase(KEXIFILTERMANAGER(parent),name,list),d(0) {
	d=new KexiImportWizardPrivate;
}

KexiImportWizard::~KexiImportWizard() {
	delete d;
}

void KexiImportWizard::exec(const QString& dialogfilter, const QMap<QString,QString> mimePluginMap, unsigned long importTypes, bool modal){
	d->widget=new KexiTableImportForm(KEXIFILTERMANAGER(parent()),this,dialogfilter,mimePluginMap);
	d->widget->exec();
	delete d->widget;
	d->widget=0;
	deleteLater();
}

void KexiImportWizard::setMode(unsigned long mode) {
	d->widget->setMode(mode);
}
