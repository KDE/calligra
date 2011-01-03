#ifndef KEXIMOBILEWIDHET_H
#define KEXIMOBILEWIDHET_H

#include <QStackedWidget>
#include <core/kexiproject.h>

class KexiMobileOpenFileWidget;
class KexiMobileNavigator;
class KexiProject;

class KexiMobileWidget : public QStackedWidget
{
	Q_OBJECT
public:
	KexiMobileWidget(KexiProject *p);
	virtual ~KexiMobileWidget();
	void databaseOpened(KexiProject*);
	
        KexiMobileNavigator *navigator();
        void setActiveObject(KexiWindow* win);
        KexiWindow* activeObject();
        
public slots:
        void showNavigator();

private:
	KexiProject *m_project;
	
	KexiMobileNavigator *m_navWidget;
        KexiWindow* m_objectPage;
};

#endif
