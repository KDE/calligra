#ifndef KEXIWORKSPACE_SDI_H
#define KEXIWORKSPACE_SDI_H

#include <qguardedptr.h>

#include <kstatusbar.h>
#include "kexiworkspace.h"

class KexiDialogBase;
class KexiView;

class KexiWorkspaceSDI : public KStatusBar, public KexiWorkspace
{
	Q_OBJECT
	
	public:
		KexiWorkspaceSDI(QWidget *parent=0, const char *name=0, KexiView* mw=0);
		virtual ~KexiWorkspaceSDI();
		
		virtual void addItem(KexiDialogBase *newItem);
		
		virtual unsigned int count() { return no; };

                virtual KexiDialogBase * activeDocumentView(){KexiDialogBase::activeDocumentView();}

	protected:
		unsigned int no;

	protected slots:
		virtual void takeItem(KexiDialogBase *delItem);
		virtual void slotWindowActivated(QWidget*);
	private:
	KexiView *m_mainwindow;
};

#endif
