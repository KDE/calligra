#ifndef KEXIWORKSPACE_MDI_H
#define KEXIWORKSPACE_MDI_H

#include <qguardedptr.h>

#include <qworkspace.h>
#include "kexiworkspace.h"

class KexiDialogBase;
class KexiMainWindow;

class KexiWorkspaceMDI : public QWorkspace, public KexiWorkspace
{
	Q_OBJECT
	
	public:
		KexiWorkspaceMDI(QWidget *parent=0, const char *name=0, KexiMainWindow* mw=0);
		virtual ~KexiWorkspaceMDI();
		
		virtual void addItem(KexiDialogBase *newItem);
		
		virtual unsigned int count() { return no; };
                virtual KexiDialogBase * activeDocumentView() {return m_activeDialog;};		
	protected:
		unsigned int no;

	protected slots:
		virtual void takeItem(KexiDialogBase *delItem);
		virtual void slotWindowActivated(QWidget*);
	private:
	QGuardedPtr<KexiDialogBase> m_activeDialog;
	KexiMainWindow *m_mainwindow;
};

#endif
