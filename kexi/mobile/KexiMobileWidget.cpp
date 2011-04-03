/**  This file is part of the KDE project
 * 
 *  Copyright (C) 2011 Adam Pigg <adam@piggz.co.uk>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#include "KexiMobileWidget.h"
#include "KexiMobileNavigator.h"

#include <core/KexiWindow.h>
#include <QDebug>


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
