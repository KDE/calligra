#ifndef KEXIWORKSPACE_H
#define KEXIWORKSPACE_H

#include <qguardedptr.h>

class KexiDialogBase;
class KexiMainWindow;

class KexiWorkspace
{
	
	public:
		KexiWorkspace(){};
		virtual ~KexiWorkspace(){};
		
		virtual void addItem(KexiDialogBase *newItem)=0;
		
		virtual unsigned int count()=0;

		virtual KexiDialogBase * activeDocumentView()=0;
		
	protected:
		virtual void takeItem(KexiDialogBase *delItem)=0;
		virtual void slotWindowActivated(QWidget*)=0;
	private:
};

#endif
