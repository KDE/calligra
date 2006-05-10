/***************************************************************************
*   Copyright (C) 2005 by Alexander Dymo                                  *
*   adymo@kdevelop.org                                                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU Library General Public License as       *
*   published by the Free Software Foundation; either version 2 of the    *
*   License, or (at your option) any later version.                       *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU Library General Public     *
*   License along with this program; if not, write to the                 *
*   Free Software Foundation, Inc.,                                       *
*   51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.             *
***************************************************************************/
#ifndef KUDESIGNERSTRUCTUREWIDGET_H
#define KUDESIGNERSTRUCTUREWIDGET_H

#include <k3listview.h>
#include <QMap>
//Added by qt3to4:
#include <Q3ValueList>

class KudesignerDoc;

namespace Kudesigner
{

class Box;
class Band;
class Canvas;
class StructureItem;

class StructureWidget: public K3ListView
{
    Q_OBJECT
public:
    StructureWidget( QWidget* parent = 0, const char* name = 0 );

public slots:
    void refresh();
    void selectionMade();
    void selectionClear();

    void selectItem( Q3ListViewItem *item );
    void setDocument( Kudesigner::Canvas *doc );

private:
    void refreshSection( Kudesigner::Band *section, StructureItem *root, int level = -1 );
    void refreshSectionContents( Kudesigner::Band *section, StructureItem *root );

    Kudesigner::Canvas *m_doc;
    QMap<Kudesigner::Box*, StructureItem*> m_items;
    Q3ValueList<StructureItem*> m_selected;
};

}

#endif
