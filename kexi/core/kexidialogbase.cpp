#include "keximainwindow.h"
#include "kexidialogbase.h"

KexiDialogBase::KexiDialogBase(KexiMainWindow *parent, const QString &title)
 : KMdiChildView(title, parent), KXMLGUIClient()
{
	parent->registerChild(this);
	parent->addWindow((KMdiChildView *)this);
}

KexiDialogBase::~KexiDialogBase()
{
}

#include "kexidialogbase.moc"

