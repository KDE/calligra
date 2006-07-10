/* This file is part of the KDE project
  Copyright (C) 2003-2004 Alexander Dymo <cloudtemple@mksat.net>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MEm_viewHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/
#ifndef KUDESIGNER_VIEW_H
#define KUDESIGNER_VIEW_H

#include <KoView.h>
#include <kparts/event.h>

#include <qdom.h>
//Added by qt3to4:
#include <QLabel>
#include <QResizeEvent>
#include <QPaintEvent>

#include <set>

class QLabel;
class QSpinBox;
class QAction;
class QPaintEvent;
class Q3DockWindow;
class Q3ToolBar;

class KAction;
class KToggleAction;

class KudesignerDoc;


namespace KoProperty
{
class Editor;
class Buffer;
}
using namespace KoProperty;

namespace Kudesigner
{
class View;
class StructureWidget;
class ReportItem;
}

class KudesignerView: public KoView
{
    Q_OBJECT
    friend class Kudesigner::View;

public:
    KudesignerView( KudesignerDoc* part, QWidget* parent = 0, const char* name = 0 );
    virtual ~KudesignerView();

    Kudesigner::View *view()
    {
        return m_view;
    }

protected slots:
    void populateProperties( Buffer *buf );
    void cut();
    void copy();
    void paste();
    void deleteItems();
    void selectAll();
    void slotAddItemNothing();
    void slotAddDetailFooter();
    void slotAddDetail();
    void slotAddDetailHeader();
    void slotAddPageFooter();
    void slotAddPageHeader();
    void slotAddReportFooter();
    void slotAddReportHeader();
    void slotAddItemLine();
    void slotAddItemCalculated();
    void slotAddItemSpecial();
    void slotAddItemField();
    void slotAddItemLabel();
    void unselectItemAction();
    void placeItem( int x, int y, int band, int bandLevel );

protected:
    void initActions();

    void paintEvent( QPaintEvent* );
    virtual void resizeEvent( QResizeEvent* _ev );
    virtual void updateReadWrite( bool readwrite );
    virtual void guiActivateEvent( KParts::GUIActivateEvent *ev );

private:
    Kudesigner::View *m_view;
    Kudesigner::StructureWidget *m_structure;
    KoProperty::Editor *m_propertyEditor;
    KoProperty::Buffer *m_buffer;

    KudesignerDoc *m_doc;

    KAction* sectionsReportHeader;
    KAction* sectionsReportFooter;
    KAction* sectionsPageFooter;
    KAction* sectionsPageHeader;
    KAction* sectionsDetailHeader;
    KAction* sectionsDetail;
    KAction* sectionsDetailFooter;

    KToggleAction* itemsNothing;
    KToggleAction* itemsLabel;
    KToggleAction* itemsField;
    KToggleAction* itemsSpecial;
    KToggleAction* itemsCalculated;
    KToggleAction* itemsLine;

    KAction* cutAction;
    KAction* copyAction;
    KAction* pasteAction;
    KAction* selectAllAction;
    KAction* deleteAction;

    KAction *gridActionLabel;
    KAction *gridAction;
    QLabel *gridLabel;
    QSpinBox *gridBox;

    KActionCollection *itemsCollection;
    KActionCollection *sectionsCollection;
};

#endif
