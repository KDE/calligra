/**  This file is part of KexiMobile
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



#include "KexiMobileNavigator.h"
#include <widget/KexiProjectModel.h>
#include <widget/KexiProjectNavigator.h>
#include <kdebug.h>

KexiMobileNavigator::KexiMobileNavigator(QWidget* parent, Qt::WindowFlags f): QWidget(parent, f), m_project(0)
{
	m_formNavigator = new KexiProjectNavigator(this, KexiProjectNavigator::NoFeatures);
	m_reportNavigator = new KexiProjectNavigator(this, KexiProjectNavigator::NoFeatures);
	
	m_layout = new QHBoxLayout(this);
	m_layout->addWidget(m_formNavigator);
	m_layout->addWidget(m_reportNavigator);
	
	setLayout(m_layout);
	
    connect(m_reportNavigator, SIGNAL(openOrActivateItem(KexiPart::Item*,Kexi::ViewMode)), this, SLOT(slotOpenItem(KexiPart::Item*)));
    connect(m_formNavigator, SIGNAL(openOrActivateItem(KexiPart::Item*,Kexi::ViewMode)), this, SLOT(slotOpenItem(KexiPart::Item*)));
    
}

KexiMobileNavigator::~KexiMobileNavigator()
{

}

void KexiMobileNavigator::setProject(KexiProject* p)
{
	QString error;
	
	m_project = p;
	m_formNavigator->setProject(m_project, "org.kexi-project.autoform", &error);
	m_reportNavigator->setProject(m_project, "org.kexi-project.report", &error);
}

void KexiMobileNavigator::slotOpenItem(KexiPart::Item* item)
{
    kDebug();
    
    emit(openItem(item));
}

#include "KexiMobileNavigator.moc"
