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
#include "kudesigner_factory.h"
#include "kudesigner_doc.h"

#include <map>

#include <qpainter.h>
#include <qiconset.h>
#include <qinputdialog.h>

#include <kaction.h>
#include <kstdaction.h>
#include <klocale.h>
#include <kdebug.h>

#include <koMainWindow.h>

#include <cv.h>

#include "cfield.h"
#include "ccalcfield.h"
#include "clabel.h"
#include "cline.h"
#include "cspecialfield.h"
#include "mycanvas.h"
#include "propertyeditor.h"
#include "property.h"

KudesignerView::KudesignerView( KudesignerDoc* part, QWidget* parent, const char* name )
    : KoView( part, parent, name ),pe(0)
{
    setInstance( KudesignerFactory::global() );
    if ( !part->isReadWrite() ) // readonly case, e.g. when embedded into konqueror
        setXMLFile( "kudesigner_readonly.rc" ); // simplified GUI
    else
        setXMLFile( "kudesignerui.rc" );

    initActions();

    rc = new ReportCanvas((QCanvas *)(part->canvas()), (QWidget*)this);
    if (part->plugin())
    {
       rc->setAcceptDrops(part->plugin()->acceptsDrops());
       rc->viewport()->setAcceptDrops(part->plugin()->acceptsDrops());
       rc->setPlugin(part->plugin());
    }
    rc->viewport()->setFocusProxy(rc);
    rc->viewport()->setFocusPolicy(WheelFocus);
    rc->setFocus();

    rc->itemToInsert = 0;

    connect(rc, SIGNAL(selectedActionProcessed()), this, SLOT(unselectItemAction()));
    connect(rc, SIGNAL(modificationPerformed()), part, SLOT(setModified()));
}

void KudesignerView::paintEvent( QPaintEvent* ev )
{
    QPainter painter;
    painter.begin( this );

    // ### TODO: Scaling

    // Let the document do the drawing
    koDocument()->paintEverything( painter, ev->rect(), FALSE, this );

    painter.end();
}


void KudesignerView::initActions()
{
    sectionsReportHeader = new KAction(i18n("Report Header"), "irh", 0, this,
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
    itemsLine->setExclusiveGroup("itemsToolBar");
}


void KudesignerView::updateReadWrite( bool /*readwrite*/ )
{
#ifdef __GNUC__
#warning TODO
#endif
}

void KudesignerView::copy()
{
    kdDebug(31000) << "KudesignerView::copy(): COPY called" << endl;
}

void KudesignerView::cut()
{
    kdDebug(31000) << "KudesignerView::cut(): CUT called" << endl;
}

void KudesignerView::resizeEvent(QResizeEvent* /*_ev*/)
{
    rc->setGeometry(0, 0, width(), height());
}

void KudesignerView::slotAddReportHeader(){
    if (!(((KudesignerDoc *)(koDocument())))->canvas()->templ->reportHeader)
    {
        CanvasReportHeader *rh = new CanvasReportHeader(((KudesignerDoc *)(koDocument()))->canvas()->templ->props["LeftMargin"]->value().toInt(),
            0, ((KudesignerDoc *)(koDocument()))->canvas()->templ->width() - ((KudesignerDoc *)(koDocument()))->canvas()->templ->props["RightMargin"]->value().toInt() -
            ((KudesignerDoc *)(koDocument()))->canvas()->templ->props["LeftMargin"]->value().toInt(),
            50, ((KudesignerDoc *)(koDocument()))->canvas());
        ((KudesignerDoc *)(koDocument()))->canvas()->templ->reportHeader = rh;
        ((KudesignerDoc *)(koDocument()))->canvas()->templ->arrangeSections();
        ((KudesignerDoc *)(koDocument()))->setModified(true);
    }
}

void KudesignerView::slotAddReportFooter(){
    if (!((KudesignerDoc *)(koDocument()))->canvas()->templ->reportFooter)
    {
        CanvasReportFooter *rf = new CanvasReportFooter(((KudesignerDoc *)(koDocument()))->canvas()->templ->props["LeftMargin"]->value().toInt(),
            0, ((KudesignerDoc *)(koDocument()))->canvas()->templ->width() - ((KudesignerDoc *)(koDocument()))->canvas()->templ->props["RightMargin"]->value().toInt() -
            ((KudesignerDoc *)(koDocument()))->canvas()->templ->props["LeftMargin"]->value().toInt(),
            50, ((KudesignerDoc *)(koDocument()))->canvas());
        ((KudesignerDoc *)(koDocument()))->canvas()->templ->reportFooter = rf;
        ((KudesignerDoc *)(koDocument()))->canvas()->templ->arrangeSections();
        ((KudesignerDoc *)(koDocument()))->setModified(true);
    }
}

void KudesignerView::slotAddPageHeader(){
    if (!((KudesignerDoc *)(koDocument()))->canvas()->templ->pageHeader)
    {
        CanvasPageHeader *ph = new CanvasPageHeader(((KudesignerDoc *)(koDocument()))->canvas()->templ->props["LeftMargin"]->value().toInt(),
            0, ((KudesignerDoc *)(koDocument()))->canvas()->templ->width() - ((KudesignerDoc *)(koDocument()))->canvas()->templ->props["RightMargin"]->value().toInt() -
            ((KudesignerDoc *)(koDocument()))->canvas()->templ->props["LeftMargin"]->value().toInt(),
            50, ((KudesignerDoc *)(koDocument()))->canvas());
        ((KudesignerDoc *)(koDocument()))->canvas()->templ->pageHeader = ph;
        ((KudesignerDoc *)(koDocument()))->canvas()->templ->arrangeSections();
        ((KudesignerDoc *)(koDocument()))->setModified(true);
    }
}

void KudesignerView::slotAddPageFooter(){
    if (!((KudesignerDoc *)(koDocument()))->canvas()->templ->pageFooter)
    {
        CanvasPageFooter *pf = new CanvasPageFooter(((KudesignerDoc *)(koDocument()))->canvas()->templ->props["LeftMargin"]->value().toInt(),
            0, ((KudesignerDoc *)(koDocument()))->canvas()->templ->width() - ((KudesignerDoc *)(koDocument()))->canvas()->templ->props["RightMargin"]->value().toInt() -
            ((KudesignerDoc *)(koDocument()))->canvas()->templ->props["LeftMargin"]->value().toInt(),
            50, ((KudesignerDoc *)(koDocument()))->canvas());
        ((KudesignerDoc *)(koDocument()))->canvas()->templ->pageFooter = pf;
        ((KudesignerDoc *)(koDocument()))->canvas()->templ->arrangeSections();
        ((KudesignerDoc *)(koDocument()))->setModified(true);
    }
}

void KudesignerView::slotAddDetailHeader(){
    bool Ok = false;
    unsigned int level = QInputDialog::getInteger(i18n("Add Detail Header"), i18n("Enter detail level:"),
        0, 0, 100, 1, &Ok, this);
    if (!Ok) return;
    if (((KudesignerDoc *)(koDocument()))->canvas()->templ->detailsCount >= level)
    {
        CanvasDetailHeader *dh = new CanvasDetailHeader(((KudesignerDoc *)(koDocument()))->canvas()->templ->props["LeftMargin"]->value().toInt(),
            0, ((KudesignerDoc *)(koDocument()))->canvas()->templ->width() - ((KudesignerDoc *)(koDocument()))->canvas()->templ->props["RightMargin"]->value().toInt() -
            ((KudesignerDoc *)(koDocument()))->canvas()->templ->props["LeftMargin"]->value().toInt(),
            50, ((KudesignerDoc *)(koDocument()))->canvas());
        dh->props["Level"]->setValue(QString("%1").arg(level));
        ((KudesignerDoc *)(koDocument()))->canvas()->templ->details[level].first.first = dh;
        ((KudesignerDoc *)(koDocument()))->canvas()->templ->arrangeSections();
        ((KudesignerDoc *)(koDocument()))->setModified(true);
    }
}

void KudesignerView::slotAddDetail(){
    bool Ok = false;
    unsigned int level = QInputDialog::getInteger(i18n("Add Detail"), i18n("Enter detail level:"),
        0, 0, 100, 1, &Ok, this);
    if (!Ok) return;
    if ( ((level == 0) && (((KudesignerDoc *)(koDocument()))->canvas()->templ->detailsCount == 0))
        || (((KudesignerDoc *)(koDocument()))->canvas()->templ->detailsCount == level))
    {
        CanvasDetail *d = new CanvasDetail(((KudesignerDoc *)(koDocument()))->canvas()->templ->props["LeftMargin"]->value().toInt(),
            0, ((KudesignerDoc *)(koDocument()))->canvas()->templ->width() - ((KudesignerDoc *)(koDocument()))->canvas()->templ->props["RightMargin"]->value().toInt() -
            ((KudesignerDoc *)(koDocument()))->canvas()->templ->props["LeftMargin"]->value().toInt(),
            50, ((KudesignerDoc *)(koDocument()))->canvas());
        d->props["Level"]->setValue(QString("%1").arg(level));
        ((KudesignerDoc *)(koDocument()))->canvas()->templ->details[level].second = d;
        ((KudesignerDoc *)(koDocument()))->canvas()->templ->arrangeSections();
        ((KudesignerDoc *)(koDocument()))->canvas()->templ->detailsCount++;
        ((KudesignerDoc *)(koDocument()))->setModified(true);
    }
}

void KudesignerView::slotAddDetailFooter(){
    bool Ok = false;
    unsigned int level = QInputDialog::getInteger(i18n("Add Detail Footer"), i18n("Enter detail level:"),
        0, 0, 100, 1, &Ok, this);
    if (!Ok) return;
    if (((KudesignerDoc *)(koDocument()))->canvas()->templ->detailsCount >= level)
    {
        CanvasDetailFooter *df = new CanvasDetailFooter(((KudesignerDoc *)(koDocument()))->canvas()->templ->props["LeftMargin"]->value().toInt(),
            0, ((KudesignerDoc *)(koDocument()))->canvas()->templ->width() - ((KudesignerDoc *)(koDocument()))->canvas()->templ->props["RightMargin"]->value().toInt() -
            ((KudesignerDoc *)(koDocument()))->canvas()->templ->props["LeftMargin"]->value().toInt(),
            50, ((KudesignerDoc *)(koDocument()))->canvas());
        df->props["Level"]->setValue(QString("%1").arg(level));
        ((KudesignerDoc *)(koDocument()))->canvas()->templ->details[level].first.second = df;
        ((KudesignerDoc *)(koDocument()))->canvas()->templ->arrangeSections();
        ((KudesignerDoc *)(koDocument()))->setModified(true);
    }
}

void KudesignerView::slotAddItemNothing(){
    if (((KudesignerDoc *)(koDocument()))->canvas())
    {
        if (rc->itemToInsert)
        {
            delete rc->itemToInsert;
            rc->itemToInsert = 0;
        }
    }
}

void KudesignerView::slotAddItemLabel(){
    if (((KudesignerDoc *)(koDocument()))->canvas())
    {
        if (rc->itemToInsert)
            delete rc->itemToInsert;
        CanvasLabel *l = new CanvasLabel(0, 0, 50, 20, ((KudesignerDoc *)(koDocument()))->canvas());
        rc->itemToInsert = l;
    }
}

void KudesignerView::slotAddItemField(){
    if (((KudesignerDoc *)(koDocument()))->canvas())
    {
        if (rc->itemToInsert)
            delete rc->itemToInsert;
	    CanvasField *l = new CanvasField(0, 0, 50, 20, ((KudesignerDoc *)(koDocument()))->canvas());
    	rc->itemToInsert = l;
    }
}

void KudesignerView::slotAddItemSpecial(){
    if (((KudesignerDoc *)(koDocument()))->canvas())
    {
        if (rc->itemToInsert)
            delete rc->itemToInsert;
	    CanvasSpecialField *l = new CanvasSpecialField(0, 0, 50, 20, ((KudesignerDoc *)(koDocument()))->canvas());
    	rc->itemToInsert = l;
    }
}

void KudesignerView::slotAddItemCalculated(){
    if (((KudesignerDoc *)(koDocument()))->canvas())
    {
        if (rc->itemToInsert)
            delete rc->itemToInsert;
	    CanvasCalculatedField *l = new CanvasCalculatedField(0, 0, 50, 20, ((KudesignerDoc *)(koDocument()))->canvas());
    	rc->itemToInsert = l;
    }
}

void KudesignerView::slotAddItemLine(){
    if (((KudesignerDoc *)(koDocument()))->canvas())
    {
        if (rc->itemToInsert)
            delete rc->itemToInsert;
	    CanvasLine *l = new CanvasLine(0, 0, 50, 20, ((KudesignerDoc *)(koDocument()))->canvas());
    	rc->itemToInsert = l;
    }
}

void KudesignerView::unselectItemAction(){
    itemsNothing->setChecked(true);
}

void KudesignerView::guiActivateEvent( KParts::GUIActivateEvent *ev )
{
        if ( ev->activated() ) {
	    if ((!pe))
	    {
        	pe = new PropertyEditor(QDockWindow::OutsideDock, shell(), "propedit");
		if (((KudesignerDoc *)koDocument())->plugin()) {
			connect(pe,SIGNAL(createPluggedInEditor(QWidget*&,PropertyEditor *,
				Property*,const CanvasBox *)),
				((KudesignerDoc *)koDocument())->plugin(),
				SLOT(createPluggedInEditor(QWidget*&, PropertyEditor *,
                                Property*,const CanvasBox *)));

			kdDebug()<<"*************Property and plugin have been connected"<<endl;
		}
	        shell()->addDockWindow(pe, DockRight);
	        pe->show();
	    
	        connect(rc, SIGNAL( selectionMade(std::map<QString, PropPtr >*,const CanvasBox*) ), pe,
        	    SLOT( populateProperties(std::map<QString, PropPtr >*,const CanvasBox*) ));
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
