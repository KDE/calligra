#include "kexidialogbase.h"

KexiDialogBase::KexiDialogBase(QWidget *parent, const char *name) : QWidget(parent, name)
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

void KexiDialogBase::activateActions(){;}

void KexiDialogBase::deactivateActions(){;}

#include "kexidialogbase.moc"
