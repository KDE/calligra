#ifndef KEXIWORKSPACE_MDI_H
#define KEXIWORKSPACE_MDI_H

#include <qguardedptr.h>

#include <qworkspace.h>
#include "kexiworkspace.h"

class KexiDialogBase;
class KexiView;

class KexiWorkspaceMDI : public QWorkspace, public KexiWorkspace
{
	Q_OBJECT
	
	public:
		KexiWorkspaceMDI(QWidget *parent=0, const char *name=0, KexiView* mw=0);
		virtual ~KexiWorkspaceMDI();
		
		virtual void addItem(KexiDialogBase *newItem);
		
		virtual unsigned int count() { return no; };
                virtual KexiDialogBase * activeDocumentView() {return m_activeDialog;};		
		virtual void activateView(KexiDialogBase *kdb){slotWindowActivated(kdb);}
	protected:
		unsigned int no;

	protected slots:
		virtual void takeItem(KexiDialogBase *delItem);
		virtual void slotWindowActivated(QWidget*);
	private:
	QGuardedPtr<KexiDialogBase> m_activeDialog;
	KexiView *m_mainwindow;
};

#endif
