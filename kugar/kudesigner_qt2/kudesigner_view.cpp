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

#include "kudesigner_view.h"
#include "kudesigner_doc.h"
#include "kudesigner_command.h"

#include <map>

#include <qpainter.h>
#include <qiconset.h>
#include <qinputdialog.h>
#include <qevent.h>
#include <qmainwindow.h>
#include <qaction.h>

#include <cv.h>

#define i18n tr

#include "canvdefs.h"
#include "cfield.h"
#include "ccalcfield.h"
#include "clabel.h"
#include "cline.h"
#include "cspecialfield.h"
#include "mycanvas.h"
#include "propertyeditor.h"
#include "property.h"

#include "canvkutemplate.h"
#include "canvreportheader.h"
#include "canvreportfooter.h"
#include "canvpageheader.h"
#include "canvpagefooter.h"
#include "canvdetailheader.h"
#include "canvdetailfooter.h"
#include "canvdetail.h"

KudesignerView::KudesignerView( KudesignerDoc* part, QMainWindow* parent, const char* name )
    : QWidget( parent, name ), m_doc(part)
{
    qWarning("11");
    parent->setCentralWidget(this);
    qWarning("12");
    show();
    qWarning("13");

    initActions();
    plugActions(parent);
    qWarning("14");

    rc = new ReportCanvas((QCanvas *)(part->canvas()), this);
/*    if (part->plugin())
    {
       rc->setAcceptDrops(part->plugin()->acceptsDrops());
       rc->viewport()->setAcceptDrops(part->plugin()->acceptsDrops());
       rc->setPlugin(part->plugin());
    }*/
    qWarning("15");
    rc->viewport()->setFocusProxy(rc);
    rc->viewport()->setFocusPolicy(WheelFocus);
    rc->setFocus();

    rc->itemToInsert = 0;
    qWarning("16");

    connect(rc, SIGNAL(selectedActionProcessed()), this, SLOT(unselectItemAction()));
    connect(rc, SIGNAL(modificationPerformed()), part, SLOT(setModified()));

    qWarning("17");

    pe = new PropertyEditor(QDockWindow::OutsideDock, parent, "propedit");
    if (m_doc->plugin()) {
        connect(pe,SIGNAL(createPluggedInEditor(QWidget*&,PropertyEditor *,
            Property*,CanvasBox *)),
            m_doc->plugin(),
            SLOT(createPluggedInEditor(QWidget*&, PropertyEditor *,
                            Property*,CanvasBox *)));
    }
    parent->addDockWindow(pe, m_doc->propertyPosition());
    qWarning("18");

    connect(rc, SIGNAL( selectionMade(std::map<QString, PropPtr >*,CanvasBox*) ), pe,
        SLOT( populateProperties(std::map<QString, PropPtr >*, CanvasBox*) ));
    connect(rc, SIGNAL( selectionClear() ), pe, SLOT( clearProperties() ));
    connect(pe, SIGNAL(propertyChanged(QString, QString)), rc, SLOT(updateProperty(QString, QString)));
    qWarning("19");

    pe->show();
    qWarning("199");
}

KudesignerView::~KudesignerView()
{
    delete pe;
    delete rc;
}

void KudesignerView::paintEvent( QPaintEvent* ev )
{
    QPainter painter;
    painter.begin( this );

    // ### TODO: Scaling

    // Let the document do the drawing
    m_doc->paintContent(painter, ev->rect(), FALSE);

    painter.end();
}


void KudesignerView::initActions()
{
/*    sectionsReportHeader = new KAction(i18n("Report Header"), "irh", 0, this,
        SLOT(slotAddReportHeader()), actionCollection(), "rheader");
    sectionsReportFooter = new KAction(i18n("Report Footer"), "irf", 0, this,
        SLOT(slotAddReportFooter()), actionCollection(), "rfooter");
    sectionsPageHeader = new KAction(i18n("Page Header"), "iph", 0, this,
        SLOT(slotAddPageHeader()), actionCollection(), "pheader");
    sectionsPageFooter = new KAction(i18n("Page Footer"), "ipf", 0, this,
        SLOT(slotAddPageFooter()), actionCollection(), "pfooter");
    sectionsDetailHeader = new KAction(i18n("Detail Header"), "idh", 0, this,
        SLOT(slotAddDetailHeader()), actionCollection(), "dheader");
    sectionsDetail = new KAction(i18n("Detail"), "id", 0, this,
        SLOT(slotAddDetail()), actionCollection(), "detail");
    sectionsDetailFooter = new KAction(i18n("Detail Footer"), "idf", 0, this,
        SLOT(slotAddDetailFooter()), actionCollection(), "dfooter");

    itemsNothing = new KRadioAction(i18n("Clear Selection"), "frame_edit", 0, this,
        SLOT(slotAddItemNothing()), actionCollection(), "nothing");
    itemsNothing->setExclusiveGroup("itemsToolBar");
    itemsNothing->setChecked(true);
    itemsLabel = new KRadioAction(i18n("Label"), "frame_text", 0, this,
        SLOT(slotAddItemLabel()), actionCollection(), "label");
    itemsLabel->setExclusiveGroup("itemsToolBar");
    itemsField = new KRadioAction(i18n("Field"), "frame_field", 0, this,
        SLOT(slotAddItemField()), actionCollection(), "field");
    itemsField->setExclusiveGroup("itemsToolBar");
    itemsSpecial = new KRadioAction(i18n("Special Field"), "frame_query", 0, this,
        SLOT(slotAddItemSpecial()), actionCollection(), "special");
    itemsSpecial->setExclusiveGroup("itemsToolBar");
    itemsCalculated = new KRadioAction(i18n("Calculated Field"), "frame_formula", 0, this,
        SLOT(slotAddItemCalculated()), actionCollection(), "calcfield");
    itemsCalculated->setExclusiveGroup("itemsToolBar");
    itemsLine = new KRadioAction(i18n("Line"), "frame_chart", 0, this,
        SLOT(slotAddItemLine()), actionCollection(), "line");
    itemsLine->setExclusiveGroup("itemsToolBar");*/
}

void KudesignerView::copy()
{
//    kdDebug(31000) << "KudesignerView::copy(): COPY called" << endl;
}

void KudesignerView::cut()
{
//    kdDebug(31000) << "KudesignerView::cut(): CUT called" << endl;
}

void KudesignerView::resizeEvent(QResizeEvent* /*_ev*/)
{
    rc->setGeometry(0, 0, width(), height());
}

void KudesignerView::slotAddReportHeader(){
    if (!m_doc->canvas()->templ->reportHeader)
    {
        m_doc->addCommand( new AddReportHeaderCommand(m_doc->canvas()) );
    }
}

void KudesignerView::slotAddReportFooter(){
    if (!m_doc->canvas()->templ->reportFooter)
    {
        m_doc->addCommand( new AddReportFooterCommand(m_doc->canvas()) );
    }
}

void KudesignerView::slotAddPageHeader(){
    if (!m_doc->canvas()->templ->pageHeader)
    {
        m_doc->addCommand( new AddPageHeaderCommand(m_doc->canvas()) );
    }
}

void KudesignerView::slotAddPageFooter(){
    if (!m_doc->canvas()->templ->pageFooter)
    {
        m_doc->addCommand( new AddPageFooterCommand(m_doc->canvas()) );
    }
}

void KudesignerView::slotAddDetailHeader(){
    bool Ok = false;
    unsigned int level = QInputDialog::getInteger(i18n("Add Detail Header"), i18n("Enter detail level:"),
        0, 0, 100, 1, &Ok, this);
    if (!Ok) return;
    if (m_doc->canvas()->templ->detailsCount >= level)
    {
        m_doc->addCommand( new AddDetailHeaderCommand(level, m_doc->canvas()) );
    }
}

void KudesignerView::slotAddDetail(){
    bool Ok = false;
    unsigned int level = QInputDialog::getInteger(i18n("Add Detail"), i18n("Enter detail level:"),
        0, 0, 100, 1, &Ok, this);
    if (!Ok) return;
    if ( ((level == 0) && (m_doc->canvas()->templ->detailsCount == 0))
        || (m_doc->canvas()->templ->detailsCount == level))
    {
        m_doc->addCommand( new AddDetailCommand(level, m_doc->canvas()) );
    }
}

void KudesignerView::slotAddDetailFooter(){
    bool Ok = false;
    unsigned int level = QInputDialog::getInteger(i18n("Add Detail Footer"), i18n("Enter detail level:"),
        0, 0, 100, 1, &Ok, this);
    if (!Ok) return;

    if (m_doc->canvas()->templ->detailsCount >= level)
    {
        m_doc->addCommand( new AddDetailFooterCommand(level, m_doc->canvas()) );
    }
}

void KudesignerView::slotAddItemNothing(){
    if (m_doc->canvas())
    {
        if (rc->itemToInsert)
        {
            rc->itemToInsert = 0;
        }
    }
}

void KudesignerView::slotAddItemLabel(){
    if (m_doc->canvas())
    {
        rc->itemToInsert = KuDesignerRttiCanvasLabel;
    }
}

void KudesignerView::slotAddItemField(){
    if (m_doc->canvas())
    {
        rc->itemToInsert = KuDesignerRttiCanvasField;
    }
}

void KudesignerView::slotAddItemSpecial(){
    if (m_doc->canvas())
    {
        rc->itemToInsert = KuDesignerRttiCanvasSpecial;
    }
}

void KudesignerView::slotAddItemCalculated(){
    if (m_doc->canvas())
    {
        rc->itemToInsert = KuDesignerRttiCanvasCalculated;
    }
}

void KudesignerView::slotAddItemLine(){
    if (m_doc->canvas())
    {
        rc->itemToInsert = KuDesignerRttiCanvasLine;
    }
}

void KudesignerView::unselectItemAction(){
    itemsNothing->setOn(true);
}

void KudesignerView::plugActions( QMainWindow *parent )
{
}
/*
void KudesignerView::guiActivateEvent( KParts::GUIActivateEvent *ev )
{
        if ( ev->activated() ) {
	    if ((!pe))
	    {
        	pe = new PropertyEditor(QDockWindow::OutsideDock, shell(), "propedit");
		if (m_doc->plugin()) {
			connect(pe,SIGNAL(createPluggedInEditor(QWidget*&,PropertyEditor *,
				Property*,CanvasBox *)),
				m_doc->plugin(),
				SLOT(createPluggedInEditor(QWidget*&, PropertyEditor *,
                                Property*,CanvasBox *)));

			kdDebug()<<"*************Property and plugin have been connected"<<endl;
		}
	        shell()->addDockWindow(pe, m_doc->propertyPosition());
	        pe->show();

	        connect(rc, SIGNAL( selectionMade(std::map<QString, PropPtr >*,CanvasBox*) ), pe,
        	    SLOT( populateProperties(std::map<QString, PropPtr >*, CanvasBox*) ));
	        connect(rc, SIGNAL( selectionClear() ), pe, SLOT( clearProperties() ));
    		connect(pe, SIGNAL(propertyChanged(QString, QString)), rc, SLOT(updateProperty(QString, QString)));
	    }


		pe->show();
		kdDebug()<<"pe->show()"<<endl;
        }
        else
        {
		pe->hide();
		kdDebug()<<"pe->hide()"<<endl;
        }
    KoView::guiActivateEvent( ev );
}


#include "kudesigner_view.moc"
*/
