/* This file is part of the KDE project
   Copyright (C) 2003 Alexander Dymo <cloudtemple@mksat.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/
#ifndef KUDESIGNER_VIEW_H
#define KUDESIGNER_VIEW_H

#include <qwidget.h>

class QAction;
class QPaintEvent;

class KudesignerDoc;

class ReportCanvas;
class CanvasReportItem;
class PropertyEditor;
class QMainWindow;

class KudesignerView : public QWidget
{
    Q_OBJECT
    friend class ReportCanvas;
public:
    KudesignerView( KudesignerDoc* part, QMainWindow* parent, const char* name = 0 );
    virtual ~KudesignerView();
protected slots:
    void cut();
    void copy();
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


protected:
    void paintEvent( QPaintEvent* );
    virtual void resizeEvent(QResizeEvent* _ev);

//    virtual void guiActivateEvent( KParts::GUIActivateEvent *ev );

    void initActions();
    void plugActions(QMainWindow *parent);

private:
    ReportCanvas *rc;
    PropertyEditor *pe;
    KudesignerDoc *m_doc;

    QAction* sectionsReportHeader;
    QAction* sectionsReportFooter;
    QAction* sectionsPageFooter;
    QAction* sectionsPageHeader;
    QAction* sectionsDetailHeader;
    QAction* sectionsDetail;
    QAction* sectionsDetailFooter;

    QAction* itemsNothing;
    QAction* itemsLabel;
    QAction* itemsField;
    QAction* itemsSpecial;
    QAction* itemsCalculated;
    QAction* itemsLine;

/*    QActionCollection *itemsCollection;
    QActionCollection *sectionsCollection;*/

};

#endif
