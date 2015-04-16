/*
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef _SECTION_PROPERTIES_DOCK_H_
#define _SECTION_PROPERTIES_DOCK_H_

#include "ui_wdgsectionproperties.h"

class RootSection;
class Section;

#include <QDockWidget>
#include <KoDockFactoryBase.h>

class SectionPropertiesDock : public QDockWidget
{
    Q_OBJECT
public:
    SectionPropertiesDock();
    virtual ~SectionPropertiesDock();
    void setRootSection(RootSection* _rootSection);
    void setSection(Section* _section);
private Q_SLOTS:
    void reload();
    void layoutChanged(int index);
private:
    Ui::WdgSectionProperties m_wdgSectionProperties;
    Section* m_currentSection;
    RootSection* m_rootSection;
};

class SectionPropertiesDockFactory : public KoDockFactoryBase
{

public:
    SectionPropertiesDockFactory() { }

    virtual QString id() const {
        return QString("SectionProperties");
    }

    virtual QDockWidget* createDockWidget() {
        SectionPropertiesDock * dockWidget = new SectionPropertiesDock();

        dockWidget->setObjectName(id());

        return dockWidget;
    }

    DockPosition defaultDockPosition() const {
        return DockRight;
    }
};

#endif
