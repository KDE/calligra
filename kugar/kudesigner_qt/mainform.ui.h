/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/
/**/
#include <iostream>
/**/

void fmMain::destroy()
{
    delete canvas;
}

void fmMain::init()
{
    canvas = 0;
    rc = 0;
}

void fmMain::fileNew()
{
    dlgNew *newReport = new dlgNew;
    if ((newReport->exec()) == QDialog::Accepted)
    {
	//removing previously allocated widgets, canvas and items
	if (canvas) delete canvas;
	if (rc) delete rc;
	
	//getting the page width and height
	int height = 297;
	int width = 210;
	height = newReport->pSize.height();
	width = newReport->pSize.width();
	if (newReport->rbLandscape->isChecked())
	{
	    int temp = height;
	    height = width;
	    width = temp;
	}
		
	//zooming canvas according to screen resolution
	QPrinter* printer;

	// Set the page size
	printer = new QPrinter();
	printer->setFullPage(true);
	printer->setPageSize((QPrinter::PageSize)newReport->cbPageSize->currentItem());
	printer->setOrientation((QPrinter::Orientation)newReport->rbLandscape->isOn());
	
	// Get the page metrics and set appropriate wigth and height
	QPaintDeviceMetrics pdm(printer);
	width = pdm.width();
	height = pdm.height();

	//this is not needed anymore
	delete printer;

	//zooming canvas according to default zoom level - FitWidth;
//	int scale = this->width()/width;
//	width *= scale;
//	height *= scale;
	
     	//creating canvas
	canvas = new MyCanvas(width, height);
	canvas->setAdvancePeriod(30);

	//creating KugarTemplate object
	CanvasKugarTemplate *templ = new CanvasKugarTemplate(0, 0, width, height, canvas);
	templ->show();
	templ->props["PageSize"].first = QString("%1").arg(newReport->cbPageSize->currentItem());
	templ->props["PageOrientation"].first = QString("%1").arg(newReport->rbLandscape->isOn());
	templ->props["TopMargin"].first = QString("%1").arg(newReport->sbTop->value());
	templ->props["BottomMargin"].first = QString("%1").arg(newReport->sbBottom->value());
	templ->props["LeftMargin"].first = QString("%1").arg(newReport->sbLeft->value());
	templ->props["RightMargin"].first = QString("%1").arg(newReport->sbRight->value());
	templ->setFileName(newReport->fcReportName->text());

	//adding some canvas items
/*	QCanvasRectangle *r = new QCanvasRectangle(0, 0, width, height, canvas);
	r->show();*/
/*	QCanvasText *t = new QCanvasText(tr("Some text"), canvas);
	CanvasLabel *l = new CanvasLabel(0, 0, 100, 100, canvas);
	l->setBrush(QColor(200, 200, 200));
	l->show();
	t->show();*/
	
	//show canvas
	rc = new ReportCanvas(canvas, this);
	rc->show();
	
	//setting canvas view as central widget in form
	setCentralWidget(rc);
	AddReportSections->setEnabled(TRUE);
	reportAddReportFooter->setEnabled(TRUE);
	reportAddReportHeader->setEnabled(TRUE);
	reportAddPageFooter->setEnabled(TRUE);
	reportAddPageHeader->setEnabled(TRUE);
	reportAddDetailHeader->setEnabled(TRUE);
	reportAddDetailFooter->setEnabled(TRUE);
	reportAddDetail->setEnabled(TRUE);
	filePrintAction->setEnabled(TRUE);
	fileSaveAction->setEnabled(TRUE);
	fileSaveAsAction->setEnabled(TRUE);
	
	setCaption("Report Designer - " + (newReport->fcReportName->text() == "" ? "<unnamed>" :
				newReport->fcReportName->text()));
    }
    delete newReport;
}

void fmMain::setReportAttributes(QDomNode *report, QString fName)
{
    QDomNamedNodeMap attributes = report->attributes();
    
    //removing previously allocated widgets, canvas and items
    if (canvas) delete canvas;
    if (rc) delete rc;
	
    //getting the page width and height
    int height = 297;
    int width = 210;
    
    if (attributes.namedItem("PageOrientation").nodeValue().toInt())
    {
	int temp = height;
	height = width;
	width = temp;
    }
		
    //zooming canvas according to screen resolution
    QPrinter* printer;

    // Set the page size
    printer = new QPrinter();
    printer->setFullPage(true);
    printer->setPageSize((QPrinter::PageSize)attributes.namedItem("PageSize").nodeValue().toInt());
    printer->setOrientation((QPrinter::Orientation)attributes.namedItem("PageOrientation").nodeValue().toInt());
	
    // Get the page metrics and set appropriate wigth and height
    QPaintDeviceMetrics pdm(printer);
    width = pdm.width();
    height = pdm.height();

    //this is not needed anymore
    delete printer;

    //zooming canvas according to default zoom level - FitWidth;
//    int scale = this->width()/width;
//    width *= scale;
//    height *= scale;
	
    //creating canvas
    canvas = new MyCanvas(width, height);
    canvas->setAdvancePeriod(30);

    //creating KugarTemplate object
    CanvasKugarTemplate *templ = new CanvasKugarTemplate(0, 0, width, height, canvas);
    templ->show();
    templ->props["PageSize"].first = attributes.namedItem("PageSize").nodeValue();
    templ->props["PageOrientation"].first = attributes.namedItem("PageOrientation").nodeValue();
    templ->props["TopMargin"].first = attributes.namedItem("TopMargin").nodeValue();
    templ->props["BottomMargin"].first = attributes.namedItem("BottomMargin").nodeValue();
    templ->props["LeftMargin"].first = attributes.namedItem("LeftMargin").nodeValue();
    templ->props["RightMargin"].first = attributes.namedItem("RightMargin").nodeValue();
    templ->setFileName(fName);

    //show canvas
    rc = new ReportCanvas(canvas, this);
    rc->show();	
}

void fmMain::setReportItemAttributes(QDomNode *node, CanvasReportItem *item)
{
    QDomNamedNodeMap attributes = node->attributes();
    
    for (unsigned int i = 0; i < attributes.count(); i++)
    {
	item->props[attributes.item(i).nodeName()].first = attributes.item(i).nodeValue();
    }
}

void fmMain::addReportItems(QDomNode *node, CanvasBand *section)
{
    QDomNodeList children = node->childNodes();
    int childCount = children.length();

    for (int j = 0; j < childCount; j++)
    {
	QDomNode child = children.item(j);
	if(child.nodeType() == QDomNode::ElementNode)
	{
	    if (child.nodeName() == "Line")
	    {
		CanvasLine *line = new CanvasLine(0, 0, 50, 20, canvas);
		section->items.append(line);
		setReportItemAttributes(&child, line);
		line->setSectionUndestructive(section);
//		line->show();
	    }
	    else if (child.nodeName() == "Label")
	    {
		CanvasLabel *label = new CanvasLabel(0, 0, 50, 20, canvas);
		section->items.append(label);
		setReportItemAttributes(&child, label);
		label->setSectionUndestructive(section);
//		label->show();
	    }
	    else if (child.nodeName() == "Special")
	    {
		CanvasSpecialField *special = new CanvasSpecialField(0, 0, 50, 20, canvas);
		section->items.append(special);
		setReportItemAttributes(&child, special);
		special->setSectionUndestructive(section);
//		special->show();
	    }
	    else if (child.nodeName() == "Field")
	    {
		CanvasField *field = new CanvasField(0, 0, 50, 20, canvas);
		section->items.append(field);
		setReportItemAttributes(&child, field);
		field->setSectionUndestructive(section);
//		field->show();
	    }
	    else if (child.nodeName() == "CalculatedField")
	    {
		CanvasCalculatedField *calcField = new CanvasCalculatedField(0, 0, 50, 20, canvas);
		section->items.append(calcField);
		setReportItemAttributes(&child, calcField);
		calcField->setSectionUndestructive(section);
//		calcField->show();
	    }
	}
    }
}

void fmMain::setReportHeaderAttributes(QDomNode *node)
{
    QDomNamedNodeMap attributes = node->attributes();
    
    CanvasReportHeader *rh = new CanvasReportHeader(canvas->templ->props["LeftMargin"].first.toInt(),
		0, canvas->templ->width() - canvas->templ->props["RightMargin"].first.toInt() -
		canvas->templ->props["LeftMargin"].first.toInt(),
		attributes.namedItem("Height").nodeValue().toInt(), canvas);
    rh->props["Height"].first = attributes.namedItem("Height").nodeValue();
    canvas->templ->reportHeader = rh;
    addReportItems(node, rh);
}

void fmMain::setReportFooterAttributes(QDomNode *node)
{
    QDomNamedNodeMap attributes = node->attributes();
    
    CanvasReportFooter *rf = new CanvasReportFooter(canvas->templ->props["LeftMargin"].first.toInt(),
		0, canvas->templ->width() - canvas->templ->props["RightMargin"].first.toInt() -
		canvas->templ->props["LeftMargin"].first.toInt(),
		attributes.namedItem("Height").nodeValue().toInt(), canvas);
    rf->props["Height"].first = attributes.namedItem("Height").nodeValue();
    canvas->templ->reportFooter = rf;
    addReportItems(node, rf);
}

void fmMain::setPageHeaderAttributes(QDomNode *node)
{
    QDomNamedNodeMap attributes = node->attributes();
    
    CanvasPageHeader *ph = new CanvasPageHeader(canvas->templ->props["LeftMargin"].first.toInt(),
		0, canvas->templ->width() - canvas->templ->props["RightMargin"].first.toInt() -
		canvas->templ->props["LeftMargin"].first.toInt(),
		attributes.namedItem("Height").nodeValue().toInt(), canvas);
    ph->props["Height"].first = attributes.namedItem("Height").nodeValue();
    canvas->templ->pageHeader = ph;
    addReportItems(node, ph);
}

void fmMain::setPageFooterAttributes(QDomNode *node)
{
    QDomNamedNodeMap attributes = node->attributes();
    
    CanvasPageFooter *pf = new CanvasPageFooter(canvas->templ->props["LeftMargin"].first.toInt(),
		0, canvas->templ->width() - canvas->templ->props["RightMargin"].first.toInt() -
		canvas->templ->props["LeftMargin"].first.toInt(),
		attributes.namedItem("Height").nodeValue().toInt(), canvas);
    pf->props["Height"].first = attributes.namedItem("Height").nodeValue();
    canvas->templ->pageFooter = pf;
    addReportItems(node, pf);
}

void fmMain::setDetailHeaderAttributes(QDomNode *node)
{
    QDomNamedNodeMap attributes = node->attributes();
    
    CanvasDetailHeader *dh = new CanvasDetailHeader(canvas->templ->props["LeftMargin"].first.toInt(),
		0, canvas->templ->width() - canvas->templ->props["RightMargin"].first.toInt() -
		canvas->templ->props["LeftMargin"].first.toInt(),
		attributes.namedItem("Height").nodeValue().toInt(), canvas);
    dh->props["Level"].first = attributes.namedItem("Level").nodeValue();
    dh->props["Height"].first = attributes.namedItem("Height").nodeValue();
    canvas->templ->details[attributes.namedItem("Level").nodeValue().toInt()].first.first = dh;
    addReportItems(node, dh);
}

void fmMain::setDetailAttributes(QDomNode *node)
{
    QDomNamedNodeMap attributes = node->attributes();
    
    CanvasDetail *d = new CanvasDetail(canvas->templ->props["LeftMargin"].first.toInt(),
		0, canvas->templ->width() - canvas->templ->props["RightMargin"].first.toInt() -
		canvas->templ->props["LeftMargin"].first.toInt(),
		attributes.namedItem("Height").nodeValue().toInt(), canvas);
    d->props["Level"].first = attributes.namedItem("Level").nodeValue();
    d->props["Height"].first = attributes.namedItem("Height").nodeValue();
    canvas->templ->details[attributes.namedItem("Level").nodeValue().toInt()].second = d;
    addReportItems(node, d);
}

void fmMain::setDetailFooterAttributes(QDomNode *node)
{
    QDomNamedNodeMap attributes = node->attributes();
    
    CanvasDetailFooter *df = new CanvasDetailFooter(canvas->templ->props["LeftMargin"].first.toInt(),
		0, canvas->templ->width() - canvas->templ->props["RightMargin"].first.toInt() -
		canvas->templ->props["LeftMargin"].first.toInt(),
		attributes.namedItem("Height").nodeValue().toInt(), canvas);
    df->props["Level"].first = attributes.namedItem("Level").nodeValue();
    df->props["Height"].first = attributes.namedItem("Height").nodeValue();
    canvas->templ->details[attributes.namedItem("Level").nodeValue().toInt()].first.second = df;
    addReportItems(node, df);
}

void fmMain::fileOpen()
{
	QString fName = QFileDialog::getOpenFileName("", "*.kut", this, "Open Report Dialog",
						 "Choose a file");
    if (fName == "")
	return;

    QFile f(fName);
    if (!f.open(IO_ReadOnly))
	return;

    QDomDocument rt;
    if (!rt.setContent(&f))
	return;

    QDomNode report, rep;
    for (QDomNode report = rt.firstChild(); !report.isNull(); report = report.nextSibling())
    {
	if (report.nodeName() == "KugarTemplate")
	{
	    rep = report;
	    break;
	}
    }
    report = rep;

    //setting report Attributes
    setReportAttributes(&report, fName);

    // Get all the child report elements
    QDomNodeList children = report.childNodes();
    int childCount = children.length();
    for (int j = 0; j < childCount; j++)
    {
	QDomNode child = children.item(j);

	if (child.nodeType() == QDomNode::ElementNode)
	{
	    if (child.nodeName() == "ReportHeader")
		setReportHeaderAttributes(&child);
	    else if (child.nodeName() == "PageHeader")
		setPageHeaderAttributes(&child);
	    else if (child.nodeName() == "DetailHeader")
		setDetailHeaderAttributes(&child);
	    else if(child.nodeName() == "Detail")
		setDetailAttributes(&child);
	    else if(child.nodeName() == "DetailFooter")
		setDetailFooterAttributes(&child);
	    else if(child.nodeName() == "PageFooter")
		setPageFooterAttributes(&child);
	    else if(child.nodeName() == "ReportFooter")
		setReportFooterAttributes(&child);
	}
    }
    canvas->templ->arrangeSections(FALSE);
    QCanvasItemList l = canvas->allItems();
    for (QCanvasItemList::Iterator it = l.begin(); it != l.end(); it++)
    {
	(*it)->show();
    }
    canvas->update();
       
    //setting canvas view as central widget in form
    setCentralWidget(rc);
    AddReportSections->setEnabled(TRUE);
    reportAddReportFooter->setEnabled(TRUE);
    reportAddReportHeader->setEnabled(TRUE);
    reportAddPageFooter->setEnabled(TRUE);
    reportAddPageHeader->setEnabled(TRUE);
    reportAddDetailHeader->setEnabled(TRUE);
    reportAddDetailFooter->setEnabled(TRUE);
    reportAddDetail->setEnabled(TRUE);
    filePrintAction->setEnabled(TRUE);
    fileSaveAction->setEnabled(TRUE);
    fileSaveAsAction->setEnabled(TRUE);
	
    canvas->templ->setFileName(fName);    
    setCaption("Report Designer - " + (fName == "" ? "<unnamed>" : fName));
	
    f.close();
}

void fmMain::fileSave()
{
    if (canvas)
    {
	if (((MyCanvas*)canvas)->templ->fileName() == "")
	{
	    fileSaveAs();
	    return;
	}
	saveReport();
    }   
}

void fmMain::fileSaveAs()
{
    if (canvas)
    {
	((MyCanvas*)canvas)->templ->setFileName(QFileDialog::getSaveFileName("", "*.kut", this, "Save Report Dialog",
				     "Choose a file"));
	if (((MyCanvas*)canvas)->templ->fileName() != "")
	    saveReport();
    }
}

void fmMain::filePrint()
{   
    QString str = canvas->templ->getXml();
    QTextStream t(&str, IO_ReadOnly);
    QPrinter* printer;

    // Set the page size
    printer = new QPrinter();
    printer->setFullPage(true);
    printer->setPageSize(QPrinter::A4);
    printer->setOrientation(QPrinter::Portrait);

    if (printer->setup(this))
    {
	statusBar()->message( "Printing..." );

	QPainter p;
	if (!p.begin(printer)) 
	    return;
	QFontMetrics fm = p.fontMetrics();
	QPaintDeviceMetrics metrics(printer);
	
	QString s;
	int yPos = 0;
	int pageNo = 1;
	int Margin = 20;

	while (!t.eof())
	{
	    if ( Margin + yPos > metrics.height() - Margin ) {
		QString msg( "Printing (page " );
		msg += QString::number( ++pageNo );
		msg += ")...";
		statusBar()->message( msg );
		printer->newPage();             // no more room on this page
		yPos = 0;                       // back to top of page
	    }
	    
	    s = t.readLine();
	    p.drawText(Margin, Margin + yPos, metrics.width(), fm.lineSpacing(), 
		       AlignLeft | AlignVCenter | WordBreak | ExpandTabs, s);
	    yPos += fm.lineSpacing();
	}
	
	statusBar()->message( "Printing completed", 2000 );
	p.end();
    }
    else
	statusBar()->message( "Printing aborted", 2000 );
    
    delete printer;
}

void fmMain::fileExit()
{
    qApp->quit();
}

void fmMain::editUndo()
{

}

void fmMain::editRedo()
{

}

void fmMain::editCut()
{

}

void fmMain::editCopy()
{

}

void fmMain::editPaste()
{

}

void fmMain::editFind()
{

}

void fmMain::helpIndex()
{

}

void fmMain::helpContents()
{

}

void fmMain::helpAbout()
{
    QMessageBox::about(this, tr("About Report Designer"),
		       tr("Kugar report designer\nCopyright (C) Alexander Dymo, 2002\n\nLicensed under the terms of the GNU General Public License Version\nas published by the Free Software Foundation."));
}


void fmMain::helpAboutQT()
{
    QMessageBox::aboutQt(this);
}


void fmMain::addDetail()
{
    bool Ok = false;
    unsigned int level = QInputDialog::getInteger(tr("Add Detail"), tr("Enter detail level:"),
				    0, 0, 100, 1, &Ok, this);
    if (!Ok) return;
    if ( ((level == 0) && (canvas->templ->detailsCount == 0))
	|| (canvas->templ->detailsCount == level))
    {
	CanvasDetail *d = new CanvasDetail(canvas->templ->props["LeftMargin"].first.toInt(),
		0, canvas->templ->width() - canvas->templ->props["RightMargin"].first.toInt() -
		canvas->templ->props["LeftMargin"].first.toInt(),
		50, canvas);
	d->props["Level"].first = QString("%1").arg(level);
	canvas->templ->details[level].second = d;
	canvas->templ->arrangeSections();
	canvas->templ->detailsCount++;
    }
}

void fmMain::addDetailFooter()
{
    bool Ok = false;
    unsigned int level = QInputDialog::getInteger(tr("Add Detail Footer"), tr("Enter detail level:"),
				    0, 0, 100, 1, &Ok, this);
    if (!Ok) return;
    if (canvas->templ->detailsCount >= level)
    {
	CanvasDetailFooter *df = new CanvasDetailFooter(canvas->templ->props["LeftMargin"].first.toInt(),
		0, canvas->templ->width() - canvas->templ->props["RightMargin"].first.toInt() -
		canvas->templ->props["LeftMargin"].first.toInt(),
		50, canvas);
	df->props["Level"].first = QString("%1").arg(level);
	canvas->templ->details[level].first.second = df;
	canvas->templ->arrangeSections();
    }
}

void fmMain::addDetailHeader()
{
    bool Ok = false;
    unsigned int level = QInputDialog::getInteger(tr("Add Detail Header"), tr("Enter detail level:"),
				    0, 0, 100, 1, &Ok, this);
    if (!Ok) return;
    if (canvas->templ->detailsCount >= level)
    {
	CanvasDetailHeader *dh = new CanvasDetailHeader(canvas->templ->props["LeftMargin"].first.toInt(),
		0, canvas->templ->width() - canvas->templ->props["RightMargin"].first.toInt() -
		canvas->templ->props["LeftMargin"].first.toInt(),
		50, canvas);
	dh->props["Level"].first = QString("%1").arg(level);
	canvas->templ->details[level].first.first = dh;
	canvas->templ->arrangeSections();
    }
}

void fmMain::addPageFooter()
{
    if (!canvas->templ->pageFooter)
    {
	CanvasPageFooter *pf = new CanvasPageFooter(canvas->templ->props["LeftMargin"].first.toInt(),
		0, canvas->templ->width() - canvas->templ->props["RightMargin"].first.toInt() -
		canvas->templ->props["LeftMargin"].first.toInt(),
		50, canvas);
	canvas->templ->pageFooter = pf;
	canvas->templ->arrangeSections();
    }
}

void fmMain::addPageHeader()
{
    if (!canvas->templ->pageHeader)
    {
	CanvasPageHeader *ph = new CanvasPageHeader(canvas->templ->props["LeftMargin"].first.toInt(),
		0, canvas->templ->width() - canvas->templ->props["RightMargin"].first.toInt() -
		canvas->templ->props["LeftMargin"].first.toInt(),
		50, canvas);
	canvas->templ->pageHeader = ph;
	canvas->templ->arrangeSections();
    }
}

void fmMain::addReportFooter()
{
    if (!canvas->templ->reportFooter)
    {
	CanvasReportFooter *rf = new CanvasReportFooter(canvas->templ->props["LeftMargin"].first.toInt(),
		0, canvas->templ->width() - canvas->templ->props["RightMargin"].first.toInt() -
		canvas->templ->props["LeftMargin"].first.toInt(),
		50, canvas);
	canvas->templ->reportFooter = rf;
	canvas->templ->arrangeSections();
    }
}

void fmMain::addReportHeader()
{
    if (!canvas->templ->reportHeader)
    {
	CanvasReportHeader *rh = new CanvasReportHeader(canvas->templ->props["LeftMargin"].first.toInt(),
		0, canvas->templ->width() - canvas->templ->props["RightMargin"].first.toInt() -
		canvas->templ->props["LeftMargin"].first.toInt(),
		50, canvas);
	canvas->templ->reportHeader = rh;
	canvas->templ->arrangeSections();
    }
}

void fmMain::toggleLabel(bool t)
{
    if (t && rc)
    {
	if (rc->selectedItem)
	{
	    delete rc->selectedItem;
	}
	CanvasLabel *l = new CanvasLabel(0, 0, 50, 20, canvas);
	rc->selectedItem = l;
    }
}

void fmMain::toggleLine(bool t)
{
    if (t && rc)
    {
	if (rc->selectedItem)
	    delete rc->selectedItem;
	CanvasLine *l = new CanvasLine(0, 0, 50, 20, canvas);
	rc->selectedItem = l;
    }
}

void fmMain::toggleCalculated( bool t )
{
    if (t && rc)
    {
	if (rc->selectedItem)
	    delete rc->selectedItem;
	CanvasCalculatedField *l = new CanvasCalculatedField(0, 0, 50, 20, canvas);
	rc->selectedItem = l;
    }
}

void fmMain::toggleSpecial( bool t )
{
    if (t && rc)
    {
	if (rc->selectedItem)
	    delete rc->selectedItem;
	CanvasSpecialField *l = new CanvasSpecialField(0, 0, 50, 20, canvas);
	rc->selectedItem = l;
    }
}

void fmMain::toggleField( bool t)
{    
    if (t && rc)
    {
	if (rc->selectedItem)
	{
	    delete rc->selectedItem;
	}
	CanvasField *l = new CanvasField(0, 0, 50, 20, canvas);
	rc->selectedItem = l;
    }
}

void fmMain::toggleNothing( bool t )
{
    if (rc && t)
	if (rc->selectedItem)
	{
	    delete rc->selectedItem;
	    rc->selectedItem = 0;
	}
}

void fmMain::saveReport()
{
    setCaption("Report Designer" + (((MyCanvas*)canvas)->templ->fileName() == "" ? "<unnamed>" :
				    ((MyCanvas*)canvas)->templ->fileName()));
    QFile report(canvas->templ->fileName());
    if (report.open(IO_Raw | IO_ReadWrite | IO_Truncate))
    {
	QTextStream t(&report);
	t << canvas->templ->getXml();
    }
    report.close();
}


void fmMain::fileOpen( QString name )
{
    QString fName = name;
    if (fName == "")
	return;

    QFile f(fName);
    if (!f.open(IO_ReadOnly))
	return;

    QDomDocument rt;
    if (!rt.setContent(&f))
	return;

    QDomNode report, rep;
    for (QDomNode report = rt.firstChild(); !report.isNull(); report = report.nextSibling())
    {
	if (report.nodeName() == "KugarTemplate")
	{
	    rep = report;
	    break;
	}
    }
    report = rep;

    //setting report Attributes
    setReportAttributes(&report, fName);

    // Get all the child report elements
    QDomNodeList children = report.childNodes();
    int childCount = children.length();
    for (int j = 0; j < childCount; j++)
    {
	QDomNode child = children.item(j);

	if (child.nodeType() == QDomNode::ElementNode)
	{
	    if (child.nodeName() == "ReportHeader")
		setReportHeaderAttributes(&child);
	    else if (child.nodeName() == "PageHeader")
		setPageHeaderAttributes(&child);
	    else if (child.nodeName() == "DetailHeader")
		setDetailHeaderAttributes(&child);
	    else if(child.nodeName() == "Detail")
		setDetailAttributes(&child);
	    else if(child.nodeName() == "DetailFooter")
		setDetailFooterAttributes(&child);
	    else if(child.nodeName() == "PageFooter")
		setPageFooterAttributes(&child);
	    else if(child.nodeName() == "ReportFooter")
		setReportFooterAttributes(&child);
	}
    }
    canvas->templ->arrangeSections(FALSE);
    QCanvasItemList l = canvas->allItems();
    for (QCanvasItemList::Iterator it = l.begin(); it != l.end(); it++)
    {
	(*it)->show();
    }
    canvas->update();
       
    //setting canvas view as central widget in form
    setCentralWidget(rc);
    AddReportSections->setEnabled(TRUE);
    reportAddReportFooter->setEnabled(TRUE);
    reportAddReportHeader->setEnabled(TRUE);
    reportAddPageFooter->setEnabled(TRUE);
    reportAddPageHeader->setEnabled(TRUE);
    reportAddDetailHeader->setEnabled(TRUE);
    reportAddDetailFooter->setEnabled(TRUE);
    reportAddDetail->setEnabled(TRUE);
    filePrintAction->setEnabled(TRUE);
    fileSaveAction->setEnabled(TRUE);
    fileSaveAsAction->setEnabled(TRUE);
	
    canvas->templ->setFileName(fName);    
    setCaption("Report Designer - " + (fName == "" ? "<unnamed>" : fName));
	
    f.close();

}
