#ifndef KEXIDIALOGBASE_H
#define KEXIDIALOGBASE_H

#include <qwidget.h>

class KexiDialogBase : public QWidget
{
	Q_OBJECT
	
	public:
	
		KexiDialogBase(QWidget *parent, const char *name);
		~KexiDialogBase();
	
	signals:
		void closing(KexiDialogBase *);
	
	protected:
	
		void closeEvent(QCloseEvent *ev);
};

#endif
