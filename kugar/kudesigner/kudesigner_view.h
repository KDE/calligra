/* This file is part of the KDE project
   Copyright (C) 2002 Alexander Dymo <cloudtemple@mksat.net>

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

#ifndef KUDESIGNER_VIEW
#define KUDESIGNER_VIEW

#include <koView.h>

class KAction;
class KRadioAction;
class QPaintEvent;

class KudesignerPart;

class ReportCanvas;
class CanvasReportItem;

class KudesignerView : public KoView
{
    Q_OBJECT
public:
    KudesignerView( KudesignerPart* part, QWidget* parent = 0, const char* name = 0 );

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

    virtual void updateReadWrite( bool readwrite );

    void initActions();

private:
    ReportCanvas *rc;

    KAction* sectionsReportHeader;
    KAction* sectionsReportFooter;
    KAction* sectionsPageFooter;
    KAction* sectionsPageHeader;
    KAction* sectionsDetailHeader;
    KAction* sectionsDetail;
    KAction* sectionsDetailFooter;

    KRadioAction* itemsNothing;
    KRadioAction* itemsLabel;
    KRadioAction* itemsField;
    KRadioAction* itemsSpecial;
    KRadioAction* itemsCalculated;
    KRadioAction* itemsLine;

    KActionCollection *itemsCollection;
    KActionCollection *sectionsCollection;

};

#endif
