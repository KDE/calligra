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


#include "KexiMobileToolbar.h"
#include <KoIcon.h>

#include <QToolButton>
#include <QVBoxLayout>
#include <QAction>
#include <kdebug.h>
#include <QPushButton>
#include <core/KexiRecordNavigatorHandler.h>

KexiMobileToolbar::KexiMobileToolbar(QWidget* parent): QToolBar(parent),
				m_recordHandler(0)
{
    setOrientation(Qt::Vertical);

    m_gotoNavigatorAction = new QAction(koIcon("application-vnd.oasis.opendocument.database"), "Project", this);
    
    
    m_previousRecord = new QAction(koIcon("go-previous"), "Previous", this);
    m_nextRecord = new QAction(koIcon("go-next"), "Next", this);
    m_recordNumber = new QAction("0 of 0", this);

    setIconSize(QSize(48,48));
    
    addAction(m_gotoNavigatorAction);
    
    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    addWidget(spacer);

    addAction(m_previousRecord);
    addAction(m_recordNumber);
    addAction(m_nextRecord);

    connect(m_gotoNavigatorAction, SIGNAL(triggered(bool)), this, SLOT(gotoNavigatorClicked()));
    
    connect(m_nextRecord, SIGNAL(triggered(bool)), this, SLOT(recordNext()));
    connect(m_previousRecord, SIGNAL(triggered(bool)), this, SLOT(recordPrevious()));
}

KexiMobileToolbar::~KexiMobileToolbar()
{

}

void KexiMobileToolbar::gotoNavigatorClicked()
{
    qDebug() << "Goto Navigator";
    emit(pageNavigator());
}

void KexiMobileToolbar::openFileClicked()
{
    qDebug() << "Open File";
    emit(pageOpenFile());
}

void KexiMobileToolbar::recordNext()
{
	if (m_recordHandler) {
		m_recordHandler->moveToNextRecordRequested();
		updatePage();
	}
}

void KexiMobileToolbar::recordPrevious()
{
	if (m_recordHandler) {
		m_recordHandler->moveToPreviousRecordRequested();
		updatePage();
	}
}

void KexiMobileToolbar::setRecordHandler(KexiRecordNavigatorHandler* handler)
{
	qDebug() << handler;
	m_recordHandler = handler;
	updatePage();
}

void KexiMobileToolbar::updatePage()
{
	if (m_recordHandler) {
		m_recordNumber->setText(QString("%1 of %2").arg(m_recordHandler->currentRecord()).arg(m_recordHandler->recordCount()));
	}
}

#include "KexiMobileToolbar.moc"
