#ifndef KEXIWORKSPACE_H
#define KEXIWORKSPACE_H

#include <qguardedptr.h>

class KexiDialogBase;
class KexiView;

class KexiWorkspace
{
	
	public:
		KexiWorkspace(){};
		virtual ~KexiWorkspace(){};
		
		virtual void addItem(KexiDialogBase *newItem)=0;
		
		virtual unsigned int count()=0;

		virtual KexiDialogBase * activeDocumentView()=0;
		virtual void activateView(KexiDialogBase *kdb)=0;
		
	protected:
		friend class KexiDialogBase;
		virtual void takeItem(KexiDialogBase *delItem)=0;
		virtual void slotWindowActivated(QWidget*)=0;
	private:
};

#endif
