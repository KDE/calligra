#ifndef KEXIWORKSPACE_SDI_H
#define KEXIWORKSPACE_SDI_H

#include <qguardedptr.h>

#include <kstatusbar.h>
#include "kexiworkspace.h"

class KexiDialogBase;
class KexiMainWindow;

class KexiWorkspaceSDI : public KStatusBar, public KexiWorkspace
{
	Q_OBJECT
	
	public:
		KexiWorkspaceSDI(QWidget *parent=0, const char *name=0, KexiMainWindow* mw=0);
		virtual ~KexiWorkspaceSDI();
		
		virtual void addItem(KexiDialogBase *newItem);
		
		virtual unsigned int count() { return no; };

                virtual KexiDialogBase * activeDocumentView(){return 0;}

	protected:
		unsigned int no;

	protected slots:
		virtual void takeItem(KexiDialogBase *delItem);
		virtual void slotWindowActivated(QWidget*);
	private:
	KexiMainWindow *m_mainwindow;
};

#endif
