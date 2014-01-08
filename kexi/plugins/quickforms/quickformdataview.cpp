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
#include "kexiscriptadapterq.h"

#include <KDebug>
#include <QDomElement>

QuickFormDataView::QuickFormDataView(QWidget* parent): KexiView(parent)
{
    m_view = new QuickFormView(this);
    m_view->setBackgroundBrush(palette().brush(QPalette::Light));
    setViewWidget(m_view);  
    
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
    QDomElement qf = root.firstChildElement("quickform");
    kDebug() << root.text();

    m_view->setDeclarativeComponent(root.text().toLocal8Bit());
    
}
