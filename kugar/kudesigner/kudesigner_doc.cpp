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

#include "kudesigner_doc.h"
#include "kudesigner_factory.h"
#include "kudesigner_view.h"

#include <canvbox.h>
#include <creportitem.h>
#include <ccalcfield.h>
#include <cfield.h>
#include <clabel.h>
#include <cline.h>
#include <cspecialfield.h>
#include <property.h>

#include <koTemplateChooseDia.h>
#include <kparts/componentfactory.h>
#include <kdebug.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kcommand.h>

#include <qpainter.h>
#include <qprinter.h>
#include <qpaintdevicemetrics.h>
#include <qfileinfo.h>
#include <qdockwindow.h>

#include <mycanvas.h>

#include <canvkutemplate.h>
#include <canvreportheader.h>
#include <canvreportfooter.h>
#include <canvpageheader.h>
#include <canvpagefooter.h>
#include <canvdetailheader.h>
#include <canvdetailfooter.h>
#include <canvdetail.h>

KudesignerDoc::KudesignerDoc( QWidget *parentWidget, const char *widgetName, QObject* parent, const char* name, bool singleViewMode )
    : KoDocument( parentWidget, widgetName, parent, name, singleViewMode ),m_plugin(0),m_propPos(DockRight)
{
    setInstance( KudesignerFactory::global(), false );
    history = new KoCommandHistory(actionCollection());

    connect( history, SIGNAL( documentRestored() ), this, SLOT( slotDocumentRestored() ) );
    connect( history, SIGNAL( commandExecuted() ), this, SLOT( slotCommandExecuted() ) );
}

KudesignerDoc::~KudesignerDoc()
{
    delete history;
}

void KudesignerDoc::addCommand(KCommand *cmd)
{
    history->addCommand(cmd);
}

bool KudesignerDoc::initDoc(InitDocFlags flags, QWidget* parentWidget)
{
    // If nothing is loaded, do initialize here
    bool ok = FALSE;

//todo use flags

    QString _template;
    KoTemplateChooseDia::DialogType dlgtype;
    if (initDocFlags() != KoDocument::InitDocFileNew)
        dlgtype = KoTemplateChooseDia::Everything;
    else
        dlgtype = KoTemplateChooseDia::OnlyTemplates;

    KoTemplateChooseDia::ReturnType ret = KoTemplateChooseDia::choose(
        KudesignerFactory::global(), _template, "application/x-kudesigner", "*.kut", i18n("Kugar Designer"),
        dlgtype, "kudesigner_template");
    if ( ret == KoTemplateChooseDia::Template ) {
        QFileInfo fileInfo( _template );
        QString fileName( fileInfo.dirPath( TRUE ) + "/" + fileInfo.baseName() + ".ktm" );
        resetURL();
        ok = loadNativeFormat( fileName );
        setEmpty();
    } else if ( ret == KoTemplateChooseDia::File ) {
        KURL url( _template);
        ok = openURL( url );
    } else if ( ret == KoTemplateChooseDia::Empty ) {
        QString fileName( locate( "kudesigner_template", "General/.source/A4.ktm", KudesignerFactory::global() ) );
        resetURL();
        ok = loadNativeFormat( fileName );
        qDebug("%d", ok);
        setEmpty();
    }
    setModified( FALSE );

    return ok;
}

KoView* KudesignerDoc::createViewInstance( QWidget* parent, const char* name )
{
    return new KudesignerView( this, parent, name );
}

bool KudesignerDoc::loadOasis( const QDomDocument&, KoOasisStyles&, const QDomDocument&, KoStore* )
{
    return false;
}

bool KudesignerDoc::saveOasis(KoStore*, KoXmlWriter*)
{
    return false;
}

bool KudesignerDoc::loadXML( QIODevice *, const QDomDocument & rt)
{
    // TODO load the document from the QDomDocument

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

    QDomNamedNodeMap attributes = report.attributes();
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

    //creating canvas
    docCanvas = new MyCanvas(width, height);
    docCanvas->setAdvancePeriod(30);

    if (m_plugin)
    {
        canvas()->setPlugin(m_plugin);
        m_plugin=0;
    }

    //creating KugarTemplate object
    CanvasKugarTemplate *templ = new CanvasKugarTemplate(0, 0, width, height, docCanvas);
    templ->show();
    templ->props["PageSize"]->setValue(attributes.namedItem("PageSize").nodeValue());
    templ->props["PageOrientation"]->setValue(attributes.namedItem("PageOrientation").nodeValue());
    templ->props["TopMargin"]->setValue(attributes.namedItem("TopMargin").nodeValue());
    templ->props["BottomMargin"]->setValue(attributes.namedItem("BottomMargin").nodeValue());
    templ->props["LeftMargin"]->setValue(attributes.namedItem("LeftMargin").nodeValue());
    templ->props["RightMargin"]->setValue(attributes.namedItem("RightMargin").nodeValue());

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
            {
                canvas()->templ->detailsCount++;
                setDetailAttributes(&child);
            }
            else if(child.nodeName() == "DetailFooter")
                setDetailFooterAttributes(&child);
            else if(child.nodeName() == "PageFooter")
                setPageFooterAttributes(&child);
            else if(child.nodeName() == "ReportFooter")
                setReportFooterAttributes(&child);
        }
    }
    docCanvas->templ->arrangeSections(FALSE);
    QCanvasItemList l = docCanvas->allItems();
    for (QCanvasItemList::Iterator it = l.begin(); it != l.end(); it++)
    {
        (*it)->show();
    }
    docCanvas->update();

    return true;
}

QDomDocument KudesignerDoc::saveXML()
{
	QDomDocument doc;
    doc.setContent(docCanvas->templ->getXml());

    return doc;
}


void KudesignerDoc::paintContent( QPainter& painter, const QRect& rect, bool /*transparent*/,
                                double /*zoomX*/, double /*zoomY*/ )
{
    // ####### handle transparency

    // Need to draw only the document rectangle described in the parameter rect.

    canvas()->drawArea(rect, &painter, TRUE);

}

void KudesignerDoc::commandExecuted()
{
    setModified(true);
}

void KudesignerDoc::documentRestored()
{
    setModified(false);
}

MyCanvas *KudesignerDoc::canvas() const
{
    return docCanvas;
}

void KudesignerDoc::setReportHeaderAttributes(QDomNode *node)
{
    QDomNamedNodeMap attributes = node->attributes();

    CanvasReportHeader *rh = new CanvasReportHeader(docCanvas->templ->props["LeftMargin"]->value().toInt(),
        0, docCanvas->templ->width() - docCanvas->templ->props["RightMargin"]->value().toInt() -
        docCanvas->templ->props["LeftMargin"]->value().toInt(),
        attributes.namedItem("Height").nodeValue().toInt(), docCanvas);
    rh->props["Height"]->setValue(attributes.namedItem("Height").nodeValue());
    docCanvas->templ->reportHeader = rh;
    addReportItems(node, rh);
}

void KudesignerDoc::setReportFooterAttributes(QDomNode *node)
{
    QDomNamedNodeMap attributes = node->attributes();

    CanvasReportFooter *rf = new CanvasReportFooter(docCanvas->templ->props["LeftMargin"]->value().toInt(),
        0, docCanvas->templ->width() - docCanvas->templ->props["RightMargin"]->value().toInt() -
        docCanvas->templ->props["LeftMargin"]->value().toInt(),
        attributes.namedItem("Height").nodeValue().toInt(), docCanvas);
    rf->props["Height"]->setValue(attributes.namedItem("Height").nodeValue());
    docCanvas->templ->reportFooter = rf;
    addReportItems(node, rf);
}

void KudesignerDoc::setPageHeaderAttributes(QDomNode *node)
{
    QDomNamedNodeMap attributes = node->attributes();

    CanvasPageHeader *ph = new CanvasPageHeader(docCanvas->templ->props["LeftMargin"]->value().toInt(),
        0, docCanvas->templ->width() - docCanvas->templ->props["RightMargin"]->value().toInt() -
        docCanvas->templ->props["LeftMargin"]->value().toInt(),
        attributes.namedItem("Height").nodeValue().toInt(), docCanvas);
    ph->props["Height"]->setValue(attributes.namedItem("Height").nodeValue());
    docCanvas->templ->pageHeader = ph;
    addReportItems(node, ph);
}

void KudesignerDoc::setPageFooterAttributes(QDomNode *node)
{
    QDomNamedNodeMap attributes = node->attributes();

    CanvasPageFooter *pf = new CanvasPageFooter(docCanvas->templ->props["LeftMargin"]->value().toInt(),
        0, docCanvas->templ->width() - docCanvas->templ->props["RightMargin"]->value().toInt() -
        docCanvas->templ->props["LeftMargin"]->value().toInt(),
        attributes.namedItem("Height").nodeValue().toInt(), docCanvas);
    pf->props["Height"]->setValue(attributes.namedItem("Height").nodeValue());
    docCanvas->templ->pageFooter = pf;
    addReportItems(node, pf);
}

void KudesignerDoc::setDetailHeaderAttributes(QDomNode *node)
{
    QDomNamedNodeMap attributes = node->attributes();

    CanvasDetailHeader *dh = new CanvasDetailHeader(docCanvas->templ->props["LeftMargin"]->value().toInt(),
        0, docCanvas->templ->width() - docCanvas->templ->props["RightMargin"]->value().toInt() -
        docCanvas->templ->props["LeftMargin"]->value().toInt(),
        attributes.namedItem("Height").nodeValue().toInt(),
        attributes.namedItem("Level").nodeValue().toInt(),
        docCanvas);
    dh->props["Level"]->setValue(attributes.namedItem("Level").nodeValue());
    dh->props["Height"]->setValue(attributes.namedItem("Height").nodeValue());
    docCanvas->templ->details[attributes.namedItem("Level").nodeValue().toInt()].first.first = dh;
    addReportItems(node, dh);
}

void KudesignerDoc::setDetailAttributes(QDomNode *node)
{
    QDomNamedNodeMap attributes = node->attributes();

    CanvasDetail *d = new CanvasDetail(docCanvas->templ->props["LeftMargin"]->value().toInt(),
        0, docCanvas->templ->width() - docCanvas->templ->props["RightMargin"]->value().toInt() -
        docCanvas->templ->props["LeftMargin"]->value().toInt(),
        attributes.namedItem("Height").nodeValue().toInt(),
        attributes.namedItem("Level").nodeValue().toInt(),
        docCanvas);
    d->props["Level"]->setValue(attributes.namedItem("Level").nodeValue());
    d->props["Height"]->setValue(attributes.namedItem("Height").nodeValue());
    docCanvas->templ->details[attributes.namedItem("Level").nodeValue().toInt()].second = d;
    addReportItems(node, d);
}

void KudesignerDoc::setDetailFooterAttributes(QDomNode *node)
{
    QDomNamedNodeMap attributes = node->attributes();

    CanvasDetailFooter *df = new CanvasDetailFooter(docCanvas->templ->props["LeftMargin"]->value().toInt(),
        0, docCanvas->templ->width() - docCanvas->templ->props["RightMargin"]->value().toInt() -
        docCanvas->templ->props["LeftMargin"]->value().toInt(),
        attributes.namedItem("Height").nodeValue().toInt(),
        attributes.namedItem("Level").nodeValue().toInt(),
        docCanvas);
    df->props["Level"]->setValue(attributes.namedItem("Level").nodeValue());
    df->props["Height"]->setValue(attributes.namedItem("Height").nodeValue());
    docCanvas->templ->details[attributes.namedItem("Level").nodeValue().toInt()].first.second = df;
    addReportItems(node, df);
}


void KudesignerDoc::addReportItems(QDomNode *node, CanvasBand *section)
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
                CanvasLine *line = new CanvasLine(0, 0, 50, 20, docCanvas);
                section->items.append(line);
                setReportItemAttributes(&child, line);
                line->setSectionUndestructive(section);
                //line->show();
            }
            else if (child.nodeName() == "Label")
            {
                CanvasLabel *label = new CanvasLabel(0, 0, 50, 20, docCanvas);
                section->items.append(label);
                setReportItemAttributes(&child, label);
                label->setSectionUndestructive(section);
                //label->show();
            }
            else if (child.nodeName() == "Special")
            {
                CanvasSpecialField *special = new CanvasSpecialField(0, 0, 50, 20, docCanvas);
                section->items.append(special);
                setReportItemAttributes(&child, special);
                special->setSectionUndestructive(section);
                //special->show();
            }
            else if (child.nodeName() == "Field")
            {
                CanvasField *field = new CanvasField(0, 0, 50, 20, docCanvas);
                section->items.append(field);
                setReportItemAttributes(&child, field);
                field->setSectionUndestructive(section);
                //field->show();
            }
            else if (child.nodeName() == "CalculatedField")
            {
                CanvasCalculatedField *calcField = new CanvasCalculatedField(0, 0, 50, 20, docCanvas);
                section->items.append(calcField);
                setReportItemAttributes(&child, calcField);
                calcField->setSectionUndestructive(section);
                //calcField->show();
            }
        }
    }
}

void KudesignerDoc::setReportItemAttributes(QDomNode *node, CanvasReportItem *item)
{
    QDomNamedNodeMap attributes = node->attributes();

    for (unsigned int i = 0; i < attributes.count(); i++)
    {
        QString propertyName=attributes.item(i).nodeName();
        QString propertyValue=attributes.item(i).nodeValue();
        if (canvas()->plugin()) canvas()->plugin()->modifyItemPropertyOnLoad(item,item->props[propertyName],propertyName,propertyValue);
        item->props[propertyName]->setValue(propertyValue);
    }
}

void KudesignerDoc::loadPlugin(const QString &name)
{
	kdDebug()<<"Trying to load plugin: "<<name<<endl;
	KuDesignerPlugin *plug=KParts::ComponentFactory::createInstanceFromLibrary<KuDesignerPlugin>(name.utf8(),this);
	m_plugin=plug;
	if (m_plugin) kdDebug()<<"plugin has been loaded"<<endl;
		else kdDebug()<<"plugin couldn't be loaded :("<<endl;
}

bool KudesignerDoc::completeSaving( KoStore* store )
{
	if (plugin()) return plugin()->store(store);
	return true;
}

bool KudesignerDoc::completeLoading( KoStore* store )
{
	if (plugin()) return plugin()->load(store);
	return true;
}

KuDesignerPlugin *KudesignerDoc::plugin()
{
	return canvas()->plugin();
}

Qt::Dock KudesignerDoc::propertyPosition()
{
	return m_propPos;
}

void KudesignerDoc::setForcedPropertyEditorPosition(Dock d)
{
	m_propPos=d;
}

#include "kudesigner_doc.moc"
