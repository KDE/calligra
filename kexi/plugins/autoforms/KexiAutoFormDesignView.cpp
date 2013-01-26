/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) 2011  Adam Pigg <piggz1@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include "KexiAutoFormDesignView.h"
#include <QScrollArea>
#include <QLayout>
#include <KDebug>

KexiAutoFormDesignView::KexiAutoFormDesignView(QWidget* parent): KexiView(parent), m_autoformDesigner(0)
{
    kDebug();
    m_scrollArea = new QScrollArea(this);
    layout()->addWidget(m_scrollArea);
}

KexiAutoFormDesignView::~KexiAutoFormDesignView()
{

}

tristate KexiAutoFormDesignView::beforeSwitchTo(Kexi::ViewMode mode, bool& dontStore)
{
    return KexiView::beforeSwitchTo(mode, dontStore);
}

tristate KexiAutoFormDesignView::afterSwitchFrom(Kexi::ViewMode mode)
{
    Q_UNUSED(mode);
#if 0 
    if (tempData()->reportDefinition.isNull()) {
        m_reportDesigner = new KoReportDesigner(this);
    } else {
        if (m_autoformDesigner) {
            m_scrollArea->takeWidget();
            delete m_autoformDesigner;
            m_autoformDesigner = 0;
        }
        
        m_autoformDesigner = new KoReportDesigner(this, tempData()->reportDefinition);
    } 
#endif
    delete m_autoformDesigner;
    m_autoformDesigner = 0;
    m_autoformDesigner = new KexiAutoFormDesigner(this);
    m_scrollArea->setWidget(m_autoformDesigner);
    
    return true;
}

KexiDB::SchemaData* KexiAutoFormDesignView::storeNewData(const KexiDB::SchemaData& sdata,
                                                         KexiView::StoreNewDataOptions options,
                                                         bool& cancel)
{
    return KexiView::storeNewData(sdata, options, cancel);
}

tristate KexiAutoFormDesignView::storeData(bool dontAsk)
{
    return KexiView::storeData(dontAsk);
}

