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
#include "kudesigner_part.h"

#include <qpainter.h>
#include <qiconset.h>
#include <qinputdialog.h>
#include <kaction.h>
#include <kstdaction.h>
#include <klocale.h>
#include <kdebug.h>

#include <cv.h>

#include "cfield.h"
#include "ccalcfield.h"
#include "clabel.h"
#include "cline.h"
#include "cspecialfield.h"
#include "mycanvas.h"

KudesignerView::KudesignerView( KudesignerPart* part, QWidget* parent, const char* name )
    : KoView( part, parent, name )
{
    setInstance( KudesignerFactory::global() );
    if ( !part->isReadWrite() ) // readonly case, e.g. when embedded into konqueror
        setXMLFile( "kudesigner_readonly.rc" ); // simplified GUI
    else
        setXMLFile( "kudesignerui.rc" );

    initActions();

    rc = new ReportCanvas((QCanvas *)(part->canvas()), (QWidget*)this);
    rc->viewport()->setFocusProxy(rc);
    rc->viewport()->setFocusPolicy(WheelFocus);
    rc->setFocus();

    rc->itemToInsert = 0;
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

void KudesignerView::resizeEvent(QResizeEvent* _ev)
{
    rc->setGeometry(0, 0, width(), height());
}

void KudesignerView::slotAddReportHeader(){
    if (!(((KudesignerPart *)(koDocument())))->canvas()->templ->reportHeader)
    {
    	CanvasReportHeader *rh = new CanvasReportHeader(((KudesignerPart *)(koDocument()))->canvas()->templ->props["LeftMargin"].first.toInt(),
	    	0, ((KudesignerPart *)(koDocument()))->canvas()->templ->width() - ((KudesignerPart *)(koDocument()))->canvas()->templ->props["RightMargin"].first.toInt() -
		    ((KudesignerPart *)(koDocument()))->canvas()->templ->props["LeftMargin"].first.toInt(),
    		50, ((KudesignerPart *)(koDocument()))->canvas());
	    ((KudesignerPart *)(koDocument()))->canvas()->templ->reportHeader = rh;
    	((KudesignerPart *)(koDocument()))->canvas()->templ->arrangeSections();
	((KudesignerPart *)(koDocument()))->setModified(true);
    }
}

void KudesignerView::slotAddReportFooter(){
    if (!((KudesignerPart *)(koDocument()))->canvas()->templ->reportFooter)
    {
    	CanvasReportFooter *rf = new CanvasReportFooter(((KudesignerPart *)(koDocument()))->canvas()->templ->props["LeftMargin"].first.toInt(),
	    	0, ((KudesignerPart *)(koDocument()))->canvas()->templ->width() - ((KudesignerPart *)(koDocument()))->canvas()->templ->props["RightMargin"].first.toInt() -
    		((KudesignerPart *)(koDocument()))->canvas()->templ->props["LeftMargin"].first.toInt(),
	    	50, ((KudesignerPart *)(koDocument()))->canvas());
    	((KudesignerPart *)(koDocument()))->canvas()->templ->reportFooter = rf;
    	((KudesignerPart *)(koDocument()))->canvas()->templ->arrangeSections();
	((KudesignerPart *)(koDocument()))->setModified(true);
    }
}

void KudesignerView::slotAddPageHeader(){
    if (!((KudesignerPart *)(koDocument()))->canvas()->templ->pageHeader)
    {
    	CanvasPageHeader *ph = new CanvasPageHeader(((KudesignerPart *)(koDocument()))->canvas()->templ->props["LeftMargin"].first.toInt(),
	    	0, ((KudesignerPart *)(koDocument()))->canvas()->templ->width() - ((KudesignerPart *)(koDocument()))->canvas()->templ->props["RightMargin"].first.toInt() -
    		((KudesignerPart *)(koDocument()))->canvas()->templ->props["LeftMargin"].first.toInt(),
	    	50, ((KudesignerPart *)(koDocument()))->canvas());
	    ((KudesignerPart *)(koDocument()))->canvas()->templ->pageHeader = ph;
    	((KudesignerPart *)(koDocument()))->canvas()->templ->arrangeSections();
	((KudesignerPart *)(koDocument()))->setModified(true);
    }
}

void KudesignerView::slotAddPageFooter(){
    if (!((KudesignerPart *)(koDocument()))->canvas()->templ->pageFooter)
    {
    	CanvasPageFooter *pf = new CanvasPageFooter(((KudesignerPart *)(koDocument()))->canvas()->templ->props["LeftMargin"].first.toInt(),
	    	0, ((KudesignerPart *)(koDocument()))->canvas()->templ->width() - ((KudesignerPart *)(koDocument()))->canvas()->templ->props["RightMargin"].first.toInt() -
    		((KudesignerPart *)(koDocument()))->canvas()->templ->props["LeftMargin"].first.toInt(),
    		50, ((KudesignerPart *)(koDocument()))->canvas());
    	((KudesignerPart *)(koDocument()))->canvas()->templ->pageFooter = pf;
    	((KudesignerPart *)(koDocument()))->canvas()->templ->arrangeSections();
	((KudesignerPart *)(koDocument()))->setModified(true);
    }
}

void KudesignerView::slotAddDetailHeader(){
    bool Ok = false;
    unsigned int level = QInputDialog::getInteger(i18n("Add Detail Header"), i18n("Enter detail level:"),
				    0, 0, 100, 1, &Ok, this);
    if (!Ok) return;
    if (((KudesignerPart *)(koDocument()))->canvas()->templ->detailsCount >= level)
    {
    	CanvasDetailHeader *dh = new CanvasDetailHeader(((KudesignerPart *)(koDocument()))->canvas()->templ->props["LeftMargin"].first.toInt(),
	    	0, ((KudesignerPart *)(koDocument()))->canvas()->templ->width() - ((KudesignerPart *)(koDocument()))->canvas()->templ->props["RightMargin"].first.toInt() -
    		((KudesignerPart *)(koDocument()))->canvas()->templ->props["LeftMargin"].first.toInt(),
	    	50, ((KudesignerPart *)(koDocument()))->canvas());
    	dh->props["Level"].first = QString("%1").arg(level);
	    ((KudesignerPart *)(koDocument()))->canvas()->templ->details[level].first.first = dh;
    	((KudesignerPart *)(koDocument()))->canvas()->templ->arrangeSections();
	((KudesignerPart *)(koDocument()))->setModified(true);
    }
}

void KudesignerView::slotAddDetail(){
    bool Ok = false;
    unsigned int level = QInputDialog::getInteger(i18n("Add Detail"), i18n("Enter detail level:"),
				    0, 0, 100, 1, &Ok, this);
    if (!Ok) return;
    if ( ((level == 0) && (((KudesignerPart *)(koDocument()))->canvas()->templ->detailsCount == 0))
    	|| (((KudesignerPart *)(koDocument()))->canvas()->templ->detailsCount == level))
    {
    	CanvasDetail *d = new CanvasDetail(((KudesignerPart *)(koDocument()))->canvas()->templ->props["LeftMargin"].first.toInt(),
	    	0, ((KudesignerPart *)(koDocument()))->canvas()->templ->width() - ((KudesignerPart *)(koDocument()))->canvas()->templ->props["RightMargin"].first.toInt() -
        	((KudesignerPart *)(koDocument()))->canvas()->templ->props["LeftMargin"].first.toInt(),
    		50, ((KudesignerPart *)(koDocument()))->canvas());
    	d->props["Level"].first = QString("%1").arg(level);
    	((KudesignerPart *)(koDocument()))->canvas()->templ->details[level].second = d;
    	((KudesignerPart *)(koDocument()))->canvas()->templ->arrangeSections();
    	((KudesignerPart *)(koDocument()))->canvas()->templ->detailsCount++;
	((KudesignerPart *)(koDocument()))->setModified(true);
    }
}

void KudesignerView::slotAddDetailFooter(){
    bool Ok = false;
    unsigned int level = QInputDialog::getInteger(i18n("Add Detail Footer"), i18n("Enter detail level:"),
				    0, 0, 100, 1, &Ok, this);
    if (!Ok) return;
    if (((KudesignerPart *)(koDocument()))->canvas()->templ->detailsCount >= level)
    {
    	CanvasDetailFooter *df = new CanvasDetailFooter(((KudesignerPart *)(koDocument()))->canvas()->templ->props["LeftMargin"].first.toInt(),
	    	0, ((KudesignerPart *)(koDocument()))->canvas()->templ->width() - ((KudesignerPart *)(koDocument()))->canvas()->templ->props["RightMargin"].first.toInt() -
    		((KudesignerPart *)(koDocument()))->canvas()->templ->props["LeftMargin"].first.toInt(),
	    	50, ((KudesignerPart *)(koDocument()))->canvas());
    	df->props["Level"].first = QString("%1").arg(level);
	    ((KudesignerPart *)(koDocument()))->canvas()->templ->details[level].first.second = df;
    	((KudesignerPart *)(koDocument()))->canvas()->templ->arrangeSections();
	((KudesignerPart *)(koDocument()))->setModified(true);
    }
}

void KudesignerView::slotAddItemNothing(){
    if (((KudesignerPart *)(koDocument()))->canvas())
    {
        if (rc->itemToInsert)
        {
            delete rc->itemToInsert;
            rc->itemToInsert = 0;
        }
    }
}

void KudesignerView::slotAddItemLabel(){
    if (((KudesignerPart *)(koDocument()))->canvas())
    {
        if (rc->itemToInsert)
            delete rc->itemToInsert;
	    CanvasLabel *l = new CanvasLabel(0, 0, 50, 20, ((KudesignerPart *)(koDocument()))->canvas());
    	rc->itemToInsert = l;
    }
}

void KudesignerView::slotAddItemField(){
    if (((KudesignerPart *)(koDocument()))->canvas())
    {
        if (rc->itemToInsert)
            delete rc->itemToInsert;
	    CanvasField *l = new CanvasField(0, 0, 50, 20, ((KudesignerPart *)(koDocument()))->canvas());
    	rc->itemToInsert = l;
    }
}

void KudesignerView::slotAddItemSpecial(){
    if (((KudesignerPart *)(koDocument()))->canvas())
    {
        if (rc->itemToInsert)
            delete rc->itemToInsert;
	    CanvasSpecialField *l = new CanvasSpecialField(0, 0, 50, 20, ((KudesignerPart *)(koDocument()))->canvas());
    	rc->itemToInsert = l;
    }
}

void KudesignerView::slotAddItemCalculated(){
    if (((KudesignerPart *)(koDocument()))->canvas())
    {
        if (rc->itemToInsert)
            delete rc->itemToInsert;
	    CanvasCalculatedField *l = new CanvasCalculatedField(0, 0, 50, 20, ((KudesignerPart *)(koDocument()))->canvas());
    	rc->itemToInsert = l;
    }
}

void KudesignerView::slotAddItemLine(){
    if (((KudesignerPart *)(koDocument()))->canvas())
    {
        if (rc->itemToInsert)
            delete rc->itemToInsert;
	    CanvasLine *l = new CanvasLine(0, 0, 50, 20, ((KudesignerPart *)(koDocument()))->canvas());
    	rc->itemToInsert = l;
    }
}

#include "kudesigner_view.moc"
