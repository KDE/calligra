#ifndef KEXIWORKSPACE_H
#define KEXIWORKSPACE_H

#include <qworkspace.h>

class KexiDialogBase;

class KexiWorkspace : public QWorkspace
{
	Q_OBJECT
	
	public:
		KexiWorkspace(QWidget *parent=0, const char *name=0);
		~KexiWorkspace();
		
		void addItem(KexiDialogBase *newItem);
		
		unsigned int count() { return no; };
		
	protected:
		unsigned int no;

	protected slots:
		void takeItem(KexiDialogBase *delItem);
};

#endif
