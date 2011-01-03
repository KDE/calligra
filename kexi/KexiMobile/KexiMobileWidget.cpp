#include "KexiMobileWidget.h"
#include "KexiMobileNavigator.h"
#include <core/KexiWindow.h>

#include <kdebug.h>

KexiMobileWidget::KexiMobileWidget(KexiProject* p) : m_project(p), m_navWidget(0), m_objectPage(0)
{
    m_navWidget = new KexiMobileNavigator();
    addWidget(m_navWidget);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

KexiMobileWidget::~KexiMobileWidget()
{

}

KexiMobileNavigator* KexiMobileWidget::navigator()
{
    return m_navWidget;
}

void KexiMobileWidget::showNavigator()
{
    if (currentWidget() != m_navWidget) {
        setCurrentWidget(m_navWidget);
    }
}


void KexiMobileWidget::databaseOpened(KexiProject *project)
{
    m_project = project;
    if (project && (project->open() == true)) {
        m_navWidget->setProject(project);
    }

    setCurrentWidget(m_navWidget);
}

KexiWindow* KexiMobileWidget::activeObject()
{
    return m_objectPage;
}

void KexiMobileWidget::setActiveObject(KexiWindow* win)
{
    removeWidget(m_objectPage);

    if (win != m_objectPage) {
        delete m_objectPage;
    }
    m_objectPage = win;
    
    addWidget(m_objectPage);
    
    setCurrentWidget(m_objectPage);
}

#include "KexiMobileWidget.moc"
