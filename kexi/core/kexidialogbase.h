#ifndef KEXIDIALOGBASE_H
#define KEXIDIALOGBASE_H

#include <qwidget.h>
#include <kxmlguiclient.h>

class KexiDialogBase : public QWidget
{
	Q_OBJECT
	
	public:

		KexiDialogBase(QWidget *parent, const char *name);
		~KexiDialogBase();
	
		virtual KXMLGUIClient *guiClient()=0;
		virtual void activateActions();
		virtual void deactivateActions();
	signals:
		void closing(KexiDialogBase *);
	
	protected:
	
		void closeEvent(QCloseEvent *ev);
};

#endif
