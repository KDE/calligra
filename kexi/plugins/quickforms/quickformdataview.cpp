/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright 2014  Adam Pigg <email>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "quickformdataview.h"
#include "quickformview.h"
#include "quickrecordset.h"
#include "quickrecord.h"
#include "kexiscriptadapterq.h"

#include <KDebug>
#include <QDomElement>
#include <QLayout>
#include <widget/utils/kexirecordnavigator.h>

QuickFormDataView::QuickFormDataView(QWidget* parent): KexiView(parent), m_cursor(0)
{
    m_view = new QuickFormView(this);
    m_view->setBackgroundBrush(palette().brush(QPalette::Light));
    layout()->addWidget(m_view);  
    
    m_recordSelector = new KexiRecordNavigator(this, 0);
    layout()->addWidget(m_recordSelector);
    m_recordSelector->setRecordCount(0);
    m_recordSelector->setInsertingButtonVisible(true);
    m_recordSelector->setLabelText(i18n("Record"));
    m_recordSelector->setButtonToolTipText(KexiRecordNavigator::ButtonFirst, i18n("Go to first record"));
    m_recordSelector->setButtonWhatsThisText(KexiRecordNavigator::ButtonFirst, i18n("Goes to first record"));
    m_recordSelector->setButtonToolTipText(KexiRecordNavigator::ButtonPrevious, i18n("Go to previous record"));
    m_recordSelector->setButtonWhatsThisText(KexiRecordNavigator::ButtonPrevious, i18n("Goes to previous record"));
    m_recordSelector->setButtonToolTipText(KexiRecordNavigator::ButtonNext, i18n("Go to next record"));
    m_recordSelector->setButtonWhatsThisText(KexiRecordNavigator::ButtonNext, i18n("Goes to next record"));
    m_recordSelector->setButtonToolTipText(KexiRecordNavigator::ButtonLast, i18n("Go to last record"));
    m_recordSelector->setButtonWhatsThisText(KexiRecordNavigator::ButtonLast, i18n("Goes to last record"));
    m_recordSelector->setNumberFieldToolTips(i18n("Current record number"), i18n("Number of records"));
    m_recordSelector->setRecordHandler(this);
    
    
    m_kexi = new KexiScriptAdaptorQ();
    m_view->addContextProperty("Kexi", m_kexi);
}

QuickFormDataView::~QuickFormDataView()
{
    delete m_kexi;
}

void QuickFormDataView::setDefinition(const QString& def)
{
    kDebug() << def;
    
    QDomDocument doc;
    doc.setContent(def);
        
    QDomElement root = doc.documentElement();
    QDomElement qf = root.firstChildElement("quickform:definition");
    kDebug() << "quickform:definition" << qf.text();

    m_recordSource = root.firstChildElement("quickform:connection").attribute("record-source");
    kDebug() << "Record Source: " << m_recordSource;
    
    m_recordSet = new QuickRecordSet(m_recordSource,  KexiMainWindowIface::global()->project()->dbConnection());
    m_view->addContextProperty("RecordSet", m_recordSet);
    connect(m_recordSet, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64)));
    
    m_currentRecord = new QuickRecord(m_recordSet);
    m_view->addContextProperty("CurrentRecord", m_currentRecord);
    
    m_view->setDeclarativeComponent(root.text().toLocal8Bit());
    m_recordSelector->setRecordCount(recordCount());
}

void QuickFormDataView::addNewRecordRequested()
{
    
}

void QuickFormDataView::moveToFirstRecordRequested()
{
  m_recordSet->moveFirst();
}

void QuickFormDataView::moveToLastRecordRequested()
{
  m_recordSet->moveLast();
}

void QuickFormDataView::moveToNextRecordRequested()
{
  m_recordSet->moveNext();
}

void QuickFormDataView::moveToPreviousRecordRequested()
{
  m_recordSet->movePrevious();
}

void QuickFormDataView::moveToRecordRequested(uint r)
{
  m_recordSet->moveTo(r);
}

int QuickFormDataView::currentRecord() const
{
    return m_recordSet->at();
}

int QuickFormDataView::recordCount() const
{
    return m_recordSet->recordCount();
}

void QuickFormDataView::positionChanged(qint64 r)
{
    m_recordSelector->setCurrentRecordNumber(r + 1);
}
