#include <kdebug.h>

#include "kexidialogbase.h"
#include "kexiworkspaceSDI.h"
#include "keximainwindow.h"

KexiWorkspaceSDI::KexiWorkspaceSDI(QWidget *parent, const char *name,KexiMainWindow *mw) 
	: KStatusBar(parent, name),KexiWorkspace()
{
	setSizeGripEnabled(true);
	addWidget(new QLabel("TEST",this),0,true);
	m_mainwindow=mw;
//	no = 0;
//	connect(this,SIGNAL(windowActivated(QWidget*)),this,SLOT(slotWindowActivated(QWidget*)));
}

void KexiWorkspaceSDI::addItem(KexiDialogBase *newItem)
{
#if 0
	no++;
	connect(newItem, SIGNAL(closing(KexiDialogBase *)), this, SLOT(takeItem(KexiDialogBase *)));
	kdDebug() << "item added; realnumber now: " << no << endl;
#endif
}

void KexiWorkspaceSDI::takeItem(KexiDialogBase *delItem)
{
	no--;
	kdDebug() << "took item; realnumber now" << no << endl;
}


void KexiWorkspaceSDI::slotWindowActivated(QWidget* w)
{
#if 0
	kdDebug() << "KexiWorkspace::slotWindowActivated()" << endl;
	if (!m_mainwindow) return;
	setCaption("");
	if (m_activeDialog==w) return;
	KexiDialogBase *olddialog=m_activeDialog;
	m_activeDialog=static_cast<KexiDialogBase*>(w);
	if (olddialog)
	{
		if (m_activeDialog.isNull())
		{
			olddialog->deactivateActions();
			m_mainwindow->guiFactory()->removeClient(olddialog->guiClient());
		}
		else
		{
			if (m_activeDialog->guiClient()!=olddialog->guiClient())
			{
				olddialog->deactivateActions();
				m_mainwindow->guiFactory()->removeClient(olddialog->guiClient());			
				m_mainwindow->guiFactory()->addClient(m_activeDialog->guiClient());
				m_activeDialog->activateActions();
			}
			else
			{
				olddialog->deactivateActions();
				m_activeDialog->activateActions();
			}
		}
	
	}
	else
	if (!m_activeDialog.isNull())
	{
		m_mainwindow->guiFactory()->addClient(m_activeDialog->guiClient());
		m_activeDialog->activateActions();
	}
#endif
}

KexiWorkspaceSDI::~KexiWorkspaceSDI()
{
}

#include "kexiworkspaceSDI.moc"
