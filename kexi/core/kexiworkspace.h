#ifndef KEXIWORKSPACE_H
#define KEXIWORKSPACE_H

#include <qworkspace.h>
#include <qguardedptr.h>

class KexiDialogBase;
class KexiMainWindow;

class KexiWorkspace : public QWidget
{
	Q_OBJECT
	
	public:
		KexiWorkspace(QWidget *parent=0, const char *name=0, KexiMainWindow* mw=0);
		~KexiWorkspace();
		
		void addItem(KexiDialogBase *newItem);
		
		unsigned int count() { return no; };

		virtual QSize sizeHint() const;
		virtual QSize minimumSizeHint() const;
		
	protected:
		unsigned int no;

	protected slots:
		void takeItem(KexiDialogBase *delItem);
		void slotWindowActivated(QWidget*);
	private:
	QGuardedPtr<KexiDialogBase> m_activeDialog;
	KexiMainWindow *m_mainwindow;
};

#endif
