#include "kexidialogbase.h"

KexiDialogBase::KexiDialogBase(QWidget *parent, const char *name) : QWidget(parent, name),KXMLGUIClient()
{
}

void KexiDialogBase::closeEvent(QCloseEvent *ev)
{
	emit closing(this);
//	close();
	ev->accept();
}

KexiDialogBase::~KexiDialogBase()
{
}

#include "kexidialogbase.moc"
