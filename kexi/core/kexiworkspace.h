#ifndef KEXIWORKSPACE_H
#define KEXIWORKSPACE_H

#include <qworkspace.h>

class KexiWorkspace : public QWorkspace
{
	Q_OBJECT
	
	public:
		KexiWorkspace(QWidget *parent=0, const char *name=0);
		~KexiWorkspace();

};

#endif
