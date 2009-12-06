/*
 * OpenRPT report writer and rendering engine
 * Copyright (C) 2001-2007 by OpenMFG, LLC
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * Please contact info@openmfg.com with any questions on this license.
 */

#include "reportdesigner.h"
#include "reportsection.h"
#include "reportentities.h"
#include "reportscene.h"
#include "reportsceneview.h"
#include "reportentities.h"
#include "reportentitylabel.h"
#include "reportentityfield.h"
#include "reportentitytext.h"
#include "reportentityline.h"
#include "reportentitybarcode.h"
#include "reportentityimage.h"
#include "reportentitychart.h"
#include "reportentityshape.h"
#include "reportentitycheck.h"
#include "reportsectiondetailgroup.h"
#include "reportpropertiesbutton.h"

// dialogs
#include "sectioneditor.h"
#include "reportsectiondetail.h"

// qt
//#include <qpixmap.h>
#include <qlayout.h>
#include <qdom.h>
#include <qtextstream.h>
#include <qiodevice.h>
#include <qfile.h>
#include <QFileDialog>

#include <QCloseEvent>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <qregexp.h>
#include <qmessagebox.h>
#include <qsqlerror.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QSplitter>

#include <kexidb/connection.h>
#include <kexidb/connectiondata.h>
#include <kexidb/utils.h>
#include <kexidb/schemadata.h>
#include <kexiutils/tristate.h>
#include <kexipart.h>

#include <koproperty/EditorView.h>
#include <KoRuler.h>
#include <KoZoomHandler.h>
#include <KoDpi.h>
#include <KoPageFormat.h>
#include <kaction.h>
#include <kdebug.h>
#include <kross/core/manager.h>

//
// define and implement the ReportWriterSectionData class
// a simple class to hold/hide data in the ReportHandler class
//
class ReportWriterSectionData
{
public:
    ReportWriterSectionData() {
        selected_items_rw = 0;
        mouseAction = ReportWriterSectionData::MA_None;
        insertItem = KRObjectData::EntityNone;
    }
    virtual ~ReportWriterSectionData() {
        selected_items_rw = 0;
    }

    enum MouseAction {
        MA_None = 0,
        MA_Insert = 1,
        MA_Grab = 2,
        MA_MoveStartPoint,
        MA_MoveEndPoint,
        MA_ResizeNW = 8,
        MA_ResizeN,
        MA_ResizeNE,
        MA_ResizeE,
        MA_ResizeSE,
        MA_ResizeS,
        MA_ResizeSW,
        MA_ResizeW
    };

    int selected_x_offset;
    int selected_y_offset;

    ReportWindow * selected_items_rw;

    MouseAction mouseAction;
    KRObjectData::EntityTypes insertItem;

    // copy data
    int copy_x_pos;        // the base x position of the copy (typically the first items original pos)
    int copy_y_pos;        // the base y position of the copy (typically the first items original pos)
    QList<ReportEntity*> copy_list;
    QList<ReportEntity*> cut_list;
};

//! @internal
class ReportDesigner::Private
{
public:
    QGridLayout *grid;
    QSplitter *splitter;
    QGraphicsScene *activeScene;
    KoRuler *hruler;
    KoZoomHandler *zoom;
    QVBoxLayout *vboxlayout;
    ReportPropertiesButton *pageButton;
};

ReportDesigner::ReportDesigner(QWidget * parent)
        : QWidget(parent), d(new Private())
{
    m_conn = 0;
    m_kordata = 0;
    init();
}

void ReportDesigner::init()
{
    m_modified = false;
    m_detail = 0;
    d->hruler = 0;

    m_sectionData = new ReportWriterSectionData();
    createProperties();

    //setSizePolicy ( QSizePolicy::Expanding, QSizePolicy::Expanding );

    m_reportHeader = m_reportFooter = 0;
    m_pageHeaderFirst = m_pageHeaderOdd = m_pageHeaderEven = m_pageHeaderLast = m_pageHeaderAny = 0;
    m_pageFooterFirst = m_pageFooterOdd = m_pageFooterEven = m_pageFooterLast = m_pageFooterAny = 0;

    d->grid = new QGridLayout(this);
    d->grid->setSpacing(0);
    d->grid->setMargin(0);
    d->grid->setColumnStretch(1, 1);
    d->grid->setRowStretch(1, 1);
    d->grid->setSizeConstraint(QLayout::SetFixedSize);

    d->vboxlayout = new QVBoxLayout();
    d->vboxlayout->setSpacing(0);
    d->vboxlayout->setMargin(0);
    d->vboxlayout->setSizeConstraint(QLayout::SetFixedSize);

    //Create nice rulers
    d->zoom = new KoZoomHandler();
    d->hruler = new KoRuler(this, Qt::Horizontal, d->zoom);

    d->pageButton = new ReportPropertiesButton(this);

    //Messy, but i cant find another way
    delete d->hruler->tabChooser();
    d->hruler->setUnit(KoUnit(KoUnit::Centimeter));

    d->grid->addWidget(d->pageButton, 0, 0);
    d->grid->addWidget(d->hruler, 0, 1);
    d->grid->addLayout(d->vboxlayout, 1, 0, 1, 2);

    d->pageButton->setMaximumSize(QSize(19, 22));
    d->pageButton->setMinimumSize(QSize(19, 22));

    m_detail = new ReportSectionDetail(this);
    d->vboxlayout->insertWidget(0, m_detail);

    setLayout(d->grid);

    connect(d->pageButton, SIGNAL(released()), this, SLOT(slotPageButton_Pressed()));
    emit pagePropertyChanged(*m_set);

    connect(m_set, SIGNAL(propertyChanged(KoProperty::Set &, KoProperty::Property &)),
        this, SLOT(slotPropertyChanged(KoProperty::Set &, KoProperty::Property &)));

    changeSet(m_set);
}

ReportDesigner::ReportDesigner(QWidget *parent, QDomElement data) : QWidget(parent), d(new Private())
{
    m_conn = 0;
    m_kordata = 0;
    
    init();

    if (data.tagName() != "koreport") {
        // arg we got an xml file but not one i know of
        kDebug() << "root element was not <koreport>";;
    }

    deleteDetail();

    QDomNodeList nlist = data.childNodes();
    QDomNode it;

    for (int i = 0; i < nlist.count(); i++) {
        it = nlist.item(i);
        // at this level all the children we get should be Elements
        if (it.isElement()) {
            QString n = it.nodeName().toLower();
	    
            if (n == "title") {
                setReportTitle(it.firstChild().nodeValue());
            } else if (n == "script") {
                m_interpreter->setValue(it.toElement().attribute("Interpreter"));
                m_script->setValue(it.firstChild().nodeValue());
            } else if (n == "grid") {
                m_showGrid->setValue(it.toElement().attribute("ShowGrid").toInt() != 0);
		m_gridSnap->setValue(it.toElement().attribute("GridSnap").toInt() != 0);
		m_gridDivisions->setValue(it.toElement().attribute("GridDivisions").toInt());
		m_unit->setValue(it.toElement().attribute("PageUnit"));
	    }

            //TODO Load page options
            else if (n == "size") {
                if (it.firstChild().isText()) {
                    propertySet()->property("PageSize").setValue(it.firstChild().nodeValue());
                } else {
                    //bad code! bad code!
                    // this code doesn't check the elements and assumes they are what they should be.
                    QDomNode n1 = it.firstChild();
                    QDomNode n2 = n1.nextSibling();
                    if (n1.nodeName() == "width") {
                        propertySet()->property("CustomWidth").setValue(n1.firstChild().nodeValue().toDouble());

                        propertySet()->property("CustomHeight").setValue(n2.firstChild().nodeValue().toDouble());
                    } else {
                        propertySet()->property("CustomHeight").setValue(n1.firstChild().nodeValue().toDouble());

                        propertySet()->property("CustomWidth").setValue(n2.firstChild().nodeValue().toDouble());
                    }
                    propertySet()->property("PageSize").setValue("Custom");
                }
            } else if (n == "labeltype") {
                //TODO Labels
                //rd->pageOptions->setLabelType ( it.firstChild().nodeValue() );
            } else if (n == "orientation") {
                propertySet()->property("Orientation").setValue(it.firstChild().nodeValue());
            } else if (n == "topmargin") {
                propertySet()->property("TopMargin").setValue(it.firstChild().nodeValue().toDouble());
            } else if (n == "bottommargin") {
                propertySet()->property("BottomMargin").setValue(it.firstChild().nodeValue().toDouble());
            } else if (n == "leftmargin") {
                propertySet()->property("LeftMargin").setValue(it.firstChild().nodeValue().toDouble());
            } else if (n == "rightmargin") {
                propertySet()->property("RightMargin").setValue(it.firstChild().nodeValue().toDouble());
            } else if (n == "reportheader") {
                if (section(KRSectionData::ReportHeader) == 0) {
                    insertSection(KRSectionData::ReportHeader);
                    section(KRSectionData::ReportHeader)->initFromXML(it);
                } else {
                    kDebug() << "While loading xml tried to add more than one rpthead";
                }
            } else if (n == "reportfooter") {
                if (section(KRSectionData::ReportFooter) == 0) {
                    insertSection(KRSectionData::ReportFooter);
                    section(KRSectionData::ReportFooter)->initFromXML(it);
                } else {
                    kDebug() << "While loading xml tried to add more than one rpthead";
                }
            } else if (n == "pageheader") {
                // we need to determine which page this is for
                // firstpage | odd | even | lastpage
                // or any if none was specified
                ReportSection * rs = 0;
                if (!it.namedItem("firstpage").isNull()) {
                    if (section(KRSectionData::PageHeaderFirst) == 0) {
                        insertSection(KRSectionData::PageHeaderFirst);
                        rs = section(KRSectionData::PageHeaderFirst);
                    } else {
                        kDebug() << "tried to load more than one page head first";
                    }
                } else if (!it.namedItem("odd").isNull()) {
                    if (section(KRSectionData::PageHeaderOdd) == 0) {
                        insertSection(KRSectionData::PageHeaderOdd);
                        rs = section(KRSectionData::PageHeaderOdd);
                    } else {
                        kDebug() << "tried to load more than one page head odd";
                    }
                } else if (!it.namedItem("even").isNull()) {
                    if (section(KRSectionData::PageHeaderEven) == 0) {
                        insertSection(KRSectionData::PageHeaderEven);
                        rs = section(KRSectionData::PageHeaderEven);
                    } else {
                        kDebug() << "tried to load more than one page head even";
                    }
                } else if (!it.namedItem("lastpage").isNull()) {
                    if (section(KRSectionData::PageHeaderLast) == 0) {
                        insertSection(KRSectionData::PageHeaderLast);
                        rs = section(KRSectionData::PageHeaderLast);
                    } else {
                        kDebug() << "tried to load more than one page head last";
                    }
                } else {
                    // we have an any pghead
                    if (section(KRSectionData::PageHeaderAny) == 0) {
                        insertSection(KRSectionData::PageHeaderAny);
                        rs = section(KRSectionData::PageHeaderAny);
                    } else {
                        kDebug() << "tried to load more than one page head any";
                    }
                }
                if (rs) rs->initFromXML(it);
            } else if (n == "pagefooter") {
                // we need to determine which page this is for
                ReportSection * rs = 0;
                if (!it.namedItem("firstpage").isNull()) {
                    if (section(KRSectionData::PageFooterFirst) == 0) {
                        insertSection(KRSectionData::PageFooterFirst);
                        rs = section(KRSectionData::PageFooterFirst);
                    } else {
                        kDebug() << "tried to load more than one page foot first";
                    }
                } else if (!it.namedItem("odd").isNull()) {
                    if (section(KRSectionData::PageFooterOdd) == 0) {
                        insertSection(KRSectionData::PageFooterOdd);
                        rs = section(KRSectionData::PageFooterOdd);
                    } else {
                        kDebug() << "tried to load more than one page foot odd";
                    }
                } else if (!it.namedItem("even").isNull()) {
                    if (section(KRSectionData::PageFooterEven) == 0) {
                        insertSection(KRSectionData::PageFooterEven);
                        rs = section(KRSectionData::PageFooterEven);
                    } else {
                        kDebug() << "tried to load more than one page foot even";
                    }
                } else if (!it.namedItem("lastpage").isNull()) {
                    if (section(KRSectionData::PageFooterLast) == 0) {
                        insertSection(KRSectionData::PageFooterLast);
                        rs = section(KRSectionData::PageFooterLast);
                    } else {
                        kDebug() << "tried to load more than one page foot last";
                    }
                } else {
                    // we have the any page foot
                    if (section(KRSectionData::PageFooterAny) == 0) {
                        insertSection(KRSectionData::PageFooterAny);
                        rs = section(KRSectionData::PageFooterAny);
                    } else {
                        kDebug() << "tried to load more than one page foot any";
                    }
                }
                if (rs) rs->initFromXML(it);
            } else if (n == "section") {
                ReportSectionDetail * rsd = new ReportSectionDetail(this);
                rsd->initFromXML(it);
                setDetail(rsd);
            } else {
                kDebug() << "Encountered an unknown Element: "  << n;
            }
        } else {
            kDebug() << "Encountered a child node of root that is not an Element";
        }
    }
    this->slotPageButton_Pressed();

    setModified(false);
}

ReportDesigner::~ReportDesigner()
{
}

void ReportDesigner::closeEvent(QCloseEvent * e)
{
    if (m_modified != false) {
        switch (QMessageBox::information(this, i18n("Report Writer"), i18n("The document '%1' contains unsaved changes.\nDo you want to save the changes before closing?" , windowTitle()), i18n("Save"), i18n("Discard"), i18n("Cancel"), 0, 2)) {
        case 0:
            // save the doc...
            // if we get a not save result we'll bail so the
            // user doesn't loose any work.
//    if ( !saveToDb() ) return;
        case 1:
            // all we have to do is just accept the close event
            break;
        case 2:
            return;
        default:
            qDebug("Encountered a problem in the close event handler....");
            // should we just go ahead and close??? or should we not close???
        }
    }
    e->accept();
}

void ReportDesigner::slotSectionEditor()
{
    SectionEditor * se = new SectionEditor(this);
    se->init(this);
    se->exec();
    delete se;
}

void ReportDesigner::setReportData(KoReportData* kodata) {
    kDebug();
    m_kordata = kodata; m_conn = static_cast<KexiDB::Connection*>(kodata->connection());
    setModified(true);
}

ReportSection * ReportDesigner::section(KRSectionData::Section s) const
{
    ReportSection *sec;
    switch (s) {
    case KRSectionData::PageHeaderAny:
        sec = m_pageHeaderAny;
        break;
    case KRSectionData::PageHeaderEven:
        sec = m_pageHeaderEven;
        break;
    case KRSectionData::PageHeaderOdd:
        sec = m_pageHeaderOdd;
        break;
    case KRSectionData::PageHeaderFirst:
        sec = m_pageHeaderFirst;
        break;
    case KRSectionData::PageHeaderLast:
        sec = m_pageHeaderLast;
        break;
    case KRSectionData::PageFooterAny:
        sec = m_pageFooterAny;
        break;
    case KRSectionData::PageFooterEven:
        sec = m_pageFooterEven;
        break;
    case KRSectionData::PageFooterOdd:
        sec = m_pageFooterOdd;
        break;
    case KRSectionData::PageFooterFirst:
        sec = m_pageFooterFirst;
        break;
    case KRSectionData::PageFooterLast:
        sec = m_pageFooterLast;
        break;
    case KRSectionData::ReportHeader:
        sec = m_reportHeader;
        break;
    case KRSectionData::ReportFooter:
        sec = m_reportFooter;
        break;
    default:
        sec = NULL;
    }
    return sec;
}
void ReportDesigner::removeSection(KRSectionData::Section s)
{
    ReportSection* sec = section(s);
    if (sec != NULL) {
        delete sec;

        switch (s) {
        case KRSectionData::PageHeaderAny:
            m_pageHeaderAny = NULL;
            break;
        case KRSectionData::PageHeaderEven:
            sec = m_pageHeaderEven = NULL;
            break;
        case KRSectionData::PageHeaderOdd:
            m_pageHeaderOdd = NULL;
            break;
        case KRSectionData::PageHeaderFirst:
            m_pageHeaderFirst = NULL;
            break;
        case KRSectionData::PageHeaderLast:
            m_pageHeaderLast = NULL;
            break;
        case KRSectionData::PageFooterAny:
            m_pageFooterAny = NULL;
            break;
        case KRSectionData::PageFooterEven:
            m_pageFooterEven = NULL;
            break;
        case KRSectionData::PageFooterOdd:
            m_pageFooterOdd = NULL;
            break;
        case KRSectionData::PageFooterFirst:
            m_pageFooterFirst = NULL;
            break;
        case KRSectionData::PageFooterLast:
            m_pageFooterLast = NULL;
            break;
        case KRSectionData::ReportHeader:
            m_reportHeader = NULL;
            break;
        case KRSectionData::ReportFooter:
            m_reportFooter = NULL;
            break;
        default:
            sec = NULL;
        }

        setModified(true);
        adjustSize();
    }
}
void ReportDesigner::insertSection(KRSectionData::Section s)
{
    ReportSection* sec = section(s);
    if (sec == NULL) {
        int idx = 0;
        for (int i = 1; i <= s; ++i) {
            if (section((KRSectionData::Section)(i)) != NULL)
                idx++;
        }
        if (s > KRSectionData::ReportHeader)
            idx++;
        kDebug() << idx;
        ReportSection *rs = new ReportSection(this);
        d->vboxlayout->insertWidget(idx, rs);

        switch (s) {
        case KRSectionData::PageHeaderAny:
            rs->setTitle(i18n("Page Header (Any)"));
            m_pageHeaderAny = rs;
            break;
        case KRSectionData::PageHeaderEven:
            rs->setTitle(i18n("Page Header (Even)"));
            m_pageHeaderEven = rs;
            break;
        case KRSectionData::PageHeaderOdd:
            rs->setTitle(i18n("Page Header (Odd)"));
            m_pageHeaderOdd = rs;
            break;
        case KRSectionData::PageHeaderFirst:
            rs->setTitle(i18n("Page Header (First)"));
            m_pageHeaderFirst = rs;
            break;
        case KRSectionData::PageHeaderLast:
            rs->setTitle(i18n("Page Header (Last)"));
            m_pageHeaderLast = rs;
            break;
        case KRSectionData::PageFooterAny:
            rs->setTitle(i18n("Page Footer (Any)"));
            m_pageFooterAny = rs;
            break;
        case KRSectionData::PageFooterEven:
            rs->setTitle(i18n("Page Footer (Even)"));
            m_pageFooterEven = rs;
            break;
        case KRSectionData::PageFooterOdd:
            rs->setTitle(i18n("Page Footer (Odd)"));
            m_pageFooterOdd = rs;
            break;
        case KRSectionData::PageFooterFirst:
            rs->setTitle(i18n("Page Footer (First)"));
            m_pageFooterFirst = rs;
            break;
        case KRSectionData::PageFooterLast:
            rs->setTitle(i18n("Page Footer (Last)"));
            m_pageFooterLast = rs;
            break;
        case KRSectionData::ReportHeader:
            rs->setTitle(i18n("Report Header"));
            m_reportHeader = rs;
            break;
        case KRSectionData::ReportFooter:
            rs->setTitle(i18n("Report Footer"));
            m_reportFooter = rs;
            break;
	//These sections cannot be inserted this way
	case KRSectionData::None:
	case KRSectionData::GroupHeader:
	case KRSectionData::GroupFooter:
	case KRSectionData::Detail:
	    break;
        }

        rs->show();
        setModified(true);
        adjustSize();
        emit pagePropertyChanged(*m_set);
    }
}

static QDomElement propertyToElement(QDomDocument* d, KoProperty::Property* p)
{
  QDomElement e = d->createElement(p->name());
  e.appendChild(d->createTextNode(p->value().toString()));
  return e;
}

static void addPropertyAsAttribute(QDomElement* e, KoProperty::Property* p)
{
    switch(p->value().type()) {
	case QVariant::Int :
	    e->setAttribute(p->name(), p->value().toInt());
	    break;
	case QVariant::Double:
	    e->setAttribute(p->name(), p->value().toDouble());
	    break;
	case QVariant::Bool:
	    e->setAttribute(p->name(), p->value().toInt());
	    break;
	default:
	    e->setAttribute(p->name(), p->value().toString());
	    break;
    }
}

QDomElement ReportDesigner::document() const
{
    QDomDocument doc = QDomDocument("");
    QDomElement root = doc.createElement("koreport");
    doc.appendChild(root);

    //title
    root.appendChild(propertyToElement(&doc, m_title));

    QDomElement scr = propertyToElement(&doc, m_script);
    addPropertyAsAttribute(&scr, m_interpreter);
    root.appendChild(scr);

    QDomElement grd = doc.createElement("grid");
    addPropertyAsAttribute(&grd, m_showGrid);
    addPropertyAsAttribute(&grd, m_gridDivisions);
    addPropertyAsAttribute(&grd, m_gridSnap);
    addPropertyAsAttribute(&grd, m_unit);
    root.appendChild(grd);

    // pageOptions
    // -- size
    QDomElement size = doc.createElement("size");

    if (m_pageSize->value().toString() == "Custom") {
        QDomElement page_width = doc.createElement("width");
        page_width.appendChild(doc.createTextNode(QString::number(pageUnit().fromUserValue(m_customWidth->value().toInt()))));
        size.appendChild(page_width);
        QDomElement page_height = doc.createElement("height");
        page_height.appendChild(doc.createTextNode(QString::number(pageUnit().fromUserValue(m_customWidth->value().toInt()))));
        size.appendChild(page_height);
    } else if (m_pageSize->value().toString() == "Labels") {
        size.appendChild(doc.createTextNode("Labels"));
        QDomElement labeltype = doc.createElement("labeltype");
        labeltype.appendChild(doc.createTextNode(m_labelType->value().toString()));
        root.appendChild(labeltype);
    } else {
        size.appendChild(doc.createTextNode(m_pageSize->value().toString()));
    }
    root.appendChild(size);
    
    // -- orientation
    root.appendChild(propertyToElement(&doc, m_orientation));
    
    // -- margins
    root.appendChild(propertyToElement(&doc, m_topMargin));
    root.appendChild(propertyToElement(&doc, m_bottomMargin));
    root.appendChild(propertyToElement(&doc, m_rightMargin));
    root.appendChild(propertyToElement(&doc, m_leftMargin));

    QDomElement section;

    // report head
    if (m_reportHeader) {
        section = doc.createElement("reportheader");
        m_reportHeader->buildXML(doc, section);
        root.appendChild(section);
    }

    // page head first
    if (m_pageHeaderFirst) {
        section = doc.createElement("pageheader");
        section.appendChild(doc.createElement("firstpage"));
        m_pageHeaderFirst->buildXML(doc, section);
        root.appendChild(section);
    }
    // page head odd
    if (m_pageHeaderOdd) {
        section = doc.createElement("pageheader");
        section.appendChild(doc.createElement("odd"));
        m_pageHeaderOdd->buildXML(doc, section);
        root.appendChild(section);
    }
    // page head even
    if (m_pageHeaderEven) {
        section = doc.createElement("pageheader");
        section.appendChild(doc.createElement("even"));
        m_pageHeaderEven->buildXML(doc, section);
        root.appendChild(section);
    }
    // page head last
    if (m_pageHeaderLast) {
        section = doc.createElement("pageheader");
        section.appendChild(doc.createElement("lastpage"));
        m_pageHeaderLast->buildXML(doc, section);
        root.appendChild(section);
    }
    // page head any
    if (m_pageHeaderAny) {
        section = doc.createElement("pageheader");
        m_pageHeaderAny->buildXML(doc, section);
        root.appendChild(section);
    }

    section = doc.createElement("section");
    m_detail->buildXML(doc, section);
    root.appendChild(section);

    // page foot first
    if (m_pageFooterFirst) {
        section = doc.createElement("pagefooter");
        section.appendChild(doc.createElement("firstpage"));
        m_pageFooterFirst->buildXML(doc, section);
        root.appendChild(section);
    }
    // page foot odd
    if (m_pageFooterOdd) {
        section = doc.createElement("pagefooter");
        section.appendChild(doc.createElement("odd"));
        m_pageFooterOdd->buildXML(doc, section);
        root.appendChild(section);
    }
    // page foot even
    if (m_pageFooterEven) {
        section = doc.createElement("pagefooter");
        section.appendChild(doc.createElement("even"));
        m_pageFooterEven->buildXML(doc, section);
        root.appendChild(section);
    }
    // page foot last
    if (m_pageFooterLast) {
        section = doc.createElement("pagefooter");
        section.appendChild(doc.createElement("lastpage"));
        m_pageFooterLast->buildXML(doc, section);
        root.appendChild(section);
    }
    // page foot any
    if (m_pageFooterAny) {
        section = doc.createElement("pagefooter");
        m_pageFooterAny->buildXML(doc, section);
        root.appendChild(section);
    }

    // report foot
    if (m_reportFooter) {
        section = doc.createElement("pagefooter");
        m_reportFooter->buildXML(doc, section);
        root.appendChild(section);
    }

    return root;
}

void ReportDesigner::setReportTitle(const QString & str)
{
    if (reportTitle() != str) {
        m_title->setValue(str);
        setModified(true);
    }
}
QString ReportDesigner::reportTitle() const
{
    return m_title->value().toString();
}

bool ReportDesigner::isModified() const
{
    return m_modified;
}

void ReportDesigner::setModified(bool mod)
{
    m_modified = mod;
 
    if (m_modified) {
        emit(dirty());
    }
}

QStringList ReportDesigner::fieldList() const
{
    QStringList qs;
    qs << "";
    if (m_kordata)
        qs << m_kordata->fieldNames();

    return qs;
}

QStringList ReportDesigner::scriptList() const
{
    QStringList scripts;
    
    if (isConnected()) {
        QList<int> scriptids = m_conn->objectIds(KexiPart::ScriptObjectType);
        QStringList scriptnames = m_conn->objectNames(KexiPart::ScriptObjectType);
        QString script;

        int id, i;
        id = i = 0;

        kDebug() << scriptids << scriptnames;
        kDebug() << m_interpreter->value().toString();

        //A blank entry
        scripts << "";

        if (isConnected()) {
            foreach (id, scriptids) {
                kDebug() << "ID:" << id;
                tristate res;
                res = m_conn->loadDataBlock(id, script, QString());
                if (res == true){
                    QDomDocument domdoc;
                    bool parsed = domdoc.setContent(script, false);

                    QDomElement scriptelem = domdoc.namedItem("script").toElement();
                    if (parsed && !scriptelem.isNull()) {
                        if (m_interpreter->value().toString() == scriptelem.attribute("language") && scriptelem.attribute("scripttype") == "object") {
                            scripts << scriptnames[i];
                        }
                    }
                    else {
                        kDebug() << "Unable to parse script";
                    }
                }
                else{
                    kDebug() << "Unable to loadDataBlock";
                }
                ++i;
            }
        }
        kDebug() << scripts;
    }
    return scripts;
}

void ReportDesigner::createProperties()
{
    QStringList keys, strings;
    m_set = new KoProperty::Set(0, "Report");

    connect(m_set, SIGNAL(propertyChanged(KoProperty::Set &, KoProperty::Property &)),
        this, SLOT(slotPropertyChanged(KoProperty::Set &, KoProperty::Property &)));

    m_title = new KoProperty::Property("Title", "Report", "Title", "Report Title");

    keys.clear();
    keys = pageFormats();
    m_pageSize = new KoProperty::Property("PageSize", keys, keys, "A4", "Page Size");

    keys.clear();strings.clear();
    keys << "Portrait" << "Landscape";
    strings << i18n("Portrait") << i18n("Landscape");
    m_orientation = new KoProperty::Property("Orientation", keys, strings, "Portrait", "Page Orientation");

    keys.clear();strings.clear();
    
    strings = KoUnit::listOfUnitName();
    QString unit;
    foreach (unit, strings) {
	unit = unit.mid(unit.indexOf("(") + 1, 2);
	keys << unit;
    }

    m_unit = new KoProperty::Property("PageUnit", keys, strings, "cm", "Page Unit");

    m_showGrid = new KoProperty::Property("ShowGrid", true, "Show Grid", "Show Grid");
    m_gridSnap = new KoProperty::Property("GridSnap", true, "Grid Snap", "Grid Snap");
    m_gridDivisions = new KoProperty::Property("GridDivisions", 4, "Grid Divisions", "Grid Divisions");

    m_leftMargin = new KoProperty::Property("LeftMargin", KoUnit::unit("cm").fromUserValue(1.0), "Left Margin", "Left Margin", KoProperty::Double);
    m_rightMargin = new KoProperty::Property("RightMargin", KoUnit::unit("cm").fromUserValue(1.0), "Right Margin", "Right Margin", KoProperty::Double);
    m_topMargin = new KoProperty::Property("TopMargin", KoUnit::unit("cm").fromUserValue(1.0), "Top Margin", "Top Margin", KoProperty::Double);
    m_bottomMargin = new KoProperty::Property("BottomMargin", KoUnit::unit("cm").fromUserValue(1.0), "Bottom Margin", "Bottom Margin", KoProperty::Double);
    m_leftMargin->setOption("unit", "cm");
    m_rightMargin->setOption("unit", "cm");
    m_topMargin->setOption("unit", "cm");
    m_bottomMargin->setOption("unit", "cm");

    keys = Kross::Manager::self().interpreters();
    m_interpreter = new KoProperty::Property("Interpreter", keys, keys, keys[0], "Script Interpreter");

    keys = scriptList();
    m_script = new KoProperty::Property("Script", keys, keys, "", "Object Script");
    
    m_set->addProperty(m_title);
    m_set->addProperty(m_pageSize);
    m_set->addProperty(m_orientation);
    m_set->addProperty(m_unit);
    m_set->addProperty(m_gridSnap);
    m_set->addProperty(m_showGrid);
    m_set->addProperty(m_gridDivisions);
    m_set->addProperty(m_leftMargin);
    m_set->addProperty(m_rightMargin);
    m_set->addProperty(m_topMargin);
    m_set->addProperty(m_bottomMargin);
    m_set->addProperty(m_interpreter);
    m_set->addProperty(m_script);
    
//    KoProperty::Property* _customHeight;
//    KoProperty::Property* _customWidth;

}

/**
@brief Handle property changes
*/
void ReportDesigner::slotPropertyChanged(KoProperty::Set &s, KoProperty::Property &p)
{
    setModified(true);
    emit pagePropertyChanged(s);

    if (p.name() == "PageUnit") {
        d->hruler->setUnit(pageUnit());
        QString newstr = m_set->property("PageUnit").value().toString().mid(m_set->property("PageUnit").value().toString().indexOf("(") + 1, 2);
        
        m_set->property("LeftMargin").setOption("unit", newstr);
        m_set->property("RightMargin").setOption("unit", newstr);
        m_set->property("TopMargin").setOption("unit", newstr);
        m_set->property("BottomMargin").setOption("unit", newstr);
    }
}

/**
@brief When the 'page' button in the top left is pressed, change the property set to the reports properties.
*/
void ReportDesigner::slotPageButton_Pressed()
{
    QStringList sl = scriptList();
    
    m_script->setListData(sl, sl);
    changeSet(m_set);
}

/**
@brief Return a list of supported page formats
@return A QStringList of page formats
*/
QStringList ReportDesigner::pageFormats() const
{
    QStringList lst;
    lst << "A4" << "Letter" << "Legal" << "A3" << "A5";
    return lst;
}

QSize ReportDesigner::sizeHint() const
{
    int w = 0;
    int h = 0;

    if (m_pageFooterAny)
        h += m_pageFooterAny->sizeHint().height();
    if (m_pageFooterEven)
        h += m_pageFooterEven->sizeHint().height();
    if (m_pageFooterFirst)
        h += m_pageFooterFirst->sizeHint().height();
    if (m_pageFooterLast)
        h += m_pageFooterLast->sizeHint().height();
    if (m_pageFooterOdd)
        h += m_pageFooterOdd->sizeHint().height();
    if (m_pageHeaderAny)
        h += m_pageHeaderAny->sizeHint().height();
    if (m_pageHeaderEven)
        h += m_pageHeaderEven->sizeHint().height();
    if (m_pageHeaderFirst)
        h += m_pageHeaderFirst->sizeHint().height();
    if (m_pageHeaderLast)
        h += m_pageHeaderLast->sizeHint().height();
    if (m_pageHeaderOdd)
        h += m_pageHeaderOdd->sizeHint().height();
    if (m_reportHeader)
        h += m_reportHeader->sizeHint().height();
    if (m_reportFooter) {
        h += m_reportFooter->sizeHint().height();

    }
    if (m_detail) {
        h += m_detail->sizeHint().height();
        w += m_detail->sizeHint().width();
    }

    h += d->hruler->height();

    return QSize(w, h);
}

/**
@brief Calculate the width of the page in pixels given the paper size, orientation, dpi and margin
@return integer value of width in pixels
*/
int ReportDesigner::pageWidthPx() const
{
    int cw = 0;
    int ch = 0;
    int width = 0;

    KoPageFormat::Format pf = KoPageFormat::formatFromString(m_set->property("PageSize").value().toString());

    cw = POINT_TO_INCH(MM_TO_POINT(KoPageFormat::width(pf, KoPageFormat::Portrait))) * KoDpi::dpiX();

    ch = POINT_TO_INCH(MM_TO_POINT(KoPageFormat::height(pf, KoPageFormat::Portrait))) * KoDpi::dpiY();

    width = (m_set->property("Orientation").value().toString() == "Portrait" ? cw : ch);

    width = width - POINT_TO_INCH(m_set->property("LeftMargin").value().toDouble()) * KoDpi::dpiX();
    width = width - POINT_TO_INCH(m_set->property("RightMargin").value().toDouble()) * KoDpi::dpiX();

    return width;
}

void ReportDesigner::resizeEvent(QResizeEvent * event)
{
    Q_UNUSED(event);
    //hruler->setRulerLength ( vboxlayout->geometry().width() );
    d->hruler->setRulerLength(pageWidthPx());
}

void ReportDesigner::setDetail(ReportSectionDetail *rsd)
{
    if (m_detail == NULL) {
        int idx = 0;
        if (m_pageHeaderFirst) idx++;
        if (m_pageHeaderOdd) idx++;
        if (m_pageHeaderEven) idx++;
        if (m_pageHeaderLast) idx++;
        if (m_pageHeaderAny) idx++;
        if (m_reportHeader) idx++;
        m_detail = rsd;
        d->vboxlayout->insertWidget(idx, m_detail);
    }
}
void ReportDesigner::deleteDetail()
{
    if (m_detail != 0) {
        delete m_detail;
        m_detail = 0;
    }
}

KoUnit ReportDesigner::pageUnit() const
{
    QString u;
    bool found;

    u = m_unit->value().toString();

    KoUnit unit = KoUnit::unit(u, &found);
    if (!found) {
        unit = KoUnit::unit("cm");
    }

    return unit;
}

void ReportDesigner::setGridOptions(bool vis, int div)
{
    m_showGrid->setValue(QVariant(vis));
    m_gridDivisions->setValue(div);
}

//
// methods for the sectionMouse*Event()
//
void ReportDesigner::sectionContextMenuEvent(ReportScene * s, QGraphicsSceneContextMenuEvent * e)
{
    QMenu pop;
    QAction *popCut = 0;
    QAction *popCopy = 0;
    QAction *popPaste = 0;
    QAction* popDelete = 0;

    bool itemsSelected = selectionCount() > 0;
    if (itemsSelected) {
        popCut = pop.addAction(i18n("Cut"));
        popCopy = pop.addAction(i18n("Copy"));
    }
    if (!m_sectionData->copy_list.isEmpty())
        popPaste = pop.addAction(i18n("Paste"));

    if (itemsSelected) {
        pop.addSeparator();
        popDelete = pop.addAction(i18n("Delete"));
    }

    if (pop.actions().count() > 0) {
        QAction * ret = pop.exec(e->screenPos());
        if (ret == popCut)
            slotEditCut();
        else if (ret == popCopy)
            slotEditCopy();
        else if (ret == popPaste)
            slotEditPaste(s, e->scenePos());
        else if (ret == popDelete)
            slotEditDelete();
    }
}

void ReportDesigner::sectionMouseReleaseEvent(ReportSceneView * v, QMouseEvent * e)
{
    e->accept();
    QGraphicsItem * item = 0;
    if (e->button() == Qt::LeftButton) {
        switch (m_sectionData->mouseAction) {
        case ReportWriterSectionData::MA_Insert:
            switch (m_sectionData->insertItem) {
            case KRObjectData::EntityLabel :
                item = new ReportEntityLabel(v->designer(), v->scene());
                break;
            case KRObjectData::EntityField :
                item = new ReportEntityField(v->designer(), v->scene());
                break;
            case KRObjectData::EntityText :
                item = new ReportEntityText(v->designer(), v->scene());
                break;
            case KRObjectData::EntityBarcode :
                item = new ReportEntityBarcode(v->designer(), v->scene());
                break;
            case KRObjectData::EntityImage :
                item = new ReportEntityImage(v->designer(), v->scene());
                break;
            case KRObjectData::EntityLine :
                kDebug() << "Adding Line";
                item = new ReportEntityLine(v->designer(), v->scene());
                //dynamic_cast<QGraphicsLineItem*>(item)->setLine ( e->x()-10, e->y(), e->x()+10, e->y() );
                dynamic_cast<QGraphicsLineItem*>(item)->setLine(e->x(), e->y(), e->x() + 20, e->y());
                break;
            case KRObjectData::EntityChart :
                item = new ReportEntityChart(v->designer(), v->scene());
                break;
            case KRObjectData::EntityShape :
                item = new ReportEntityShape(v->designer(), v->scene());
                break;
            case KRObjectData::EntityCheck :
                item = new ReportEntityCheck(v->designer(), v->scene());
                break;
            default:
                kDebug() << "attempted to insert an unknown item";;
            }
            if (item) {
                if (m_sectionData->insertItem != KRObjectData::EntityLine)
                    item->setPos(e->x(), e->y());

                item->setVisible(true);
                if (v && v->designer())
                    v->designer()->setModified(true);
            }

            m_sectionData->mouseAction = ReportWriterSectionData::MA_None;
            m_sectionData->insertItem = KRObjectData::EntityNone;
            break;
        default:
            // what to do? Nothing
            // either we don't know what is going on
            // or everything has been done elsewhere
            break;
        }
    }
}

unsigned int ReportDesigner::selectionCount() const
{
  if (activeScene())
    return activeScene()->selectedItems().count();
  else
    return 0;
}

void ReportDesigner::changeSet(KoProperty::Set *s)
{
    //Set the checked state of the report proerties button
    if (s ==m_set)
      d->pageButton->setCheckState(Qt::Checked);
    else
      d->pageButton->setCheckState(Qt::Unchecked);
    
    m_itmset = s;
    emit(propertySetChanged());
}

//
// Actions
//

void ReportDesigner::slotItem(KRObjectData::EntityTypes typ)
{
    m_sectionData->mouseAction = ReportWriterSectionData::MA_Insert;
    m_sectionData->insertItem = typ;
}

void ReportDesigner::slotItem(const QString &entity)
{
    if (entity == "action-insert-label") slotItem(KRObjectData::EntityLabel);
    if (entity == "action-insert-field") slotItem(KRObjectData::EntityField);
    if (entity == "action-insert-text") slotItem(KRObjectData::EntityText);
    if (entity == "action-insert-line") slotItem(KRObjectData::EntityLine);
    if (entity == "action-insert-barcode") slotItem(KRObjectData::EntityBarcode);
    if (entity == "action-insert-chart") slotItem(KRObjectData::EntityChart);
    if (entity == "action-insert-check") slotItem(KRObjectData::EntityCheck);
    if (entity == "action-insert-image") slotItem(KRObjectData::EntityImage);
    if (entity == "action-insert-shape") slotItem(KRObjectData::EntityShape);
    
}

void ReportDesigner::slotEditDelete()
{
    QGraphicsItem * item = 0;
    while (selectionCount() > 0) {
        item = activeScene()->selectedItems()[0];
        if (item) {
            setModified(true);
            QGraphicsScene * scene = item->scene();
            delete item;
            scene->update();
            m_sectionData->mouseAction = ReportWriterSectionData::MA_None;
        }
    }
    activeScene()->selectedItems().clear();
    m_sectionData->selected_items_rw = 0;

    //TODO temporary
    //clears cut and copy lists to make sure we do not crash
    //if weve deleted something in the list
    //should really check if an item is in the list first
    //and remove it.
    m_sectionData->cut_list.clear();
    m_sectionData->copy_list.clear();
}

void ReportDesigner::slotEditCut()
{
    if (selectionCount() > 0) {
        //First delete any items that are curerntly in the list
        //so as not to leak memory
        qDeleteAll(m_sectionData->cut_list);
        m_sectionData->cut_list.clear();

        QGraphicsItem * item = activeScene()->selectedItems().first();
        if (item) {
            if (item->type() == KRObjectData::EntityLine) {
                m_sectionData->copy_x_pos = ((ReportEntityLine*) item)->line().p1().x();
                m_sectionData->copy_y_pos = ((ReportEntityLine*) item)->line().p1().y();
            } else {
                m_sectionData->copy_x_pos = (int) item->x();
                m_sectionData->copy_y_pos = (int) item->y();
            }

            m_sectionData->copy_list.clear();

            for (int i = 0; i < activeScene()->selectedItems().count(); i++) {
                QGraphicsItem *itm = activeScene()->selectedItems()[i];
                m_sectionData->cut_list.append(dynamic_cast<ReportEntity*>(itm));
                m_sectionData->copy_list.append(dynamic_cast<ReportEntity*>(itm));
            }
            int c = activeScene()->selectedItems().count();
            for (int i = 0; i < c; i++) {
                QGraphicsItem *itm = activeScene()->selectedItems()[0];
                activeScene()->removeItem(itm);
                activeScene()->update();
            }
            m_sectionData->selected_x_offset = 10;
            m_sectionData->selected_y_offset = 10;
        }
    }
}

void ReportDesigner::slotEditCopy()
{
    if (selectionCount() < 1)
        return;

    QGraphicsItem * item = activeScene()->selectedItems().first();
    if (item) {
        m_sectionData->copy_list.clear();
        if (item->type() == KRObjectData::EntityLine) {
            m_sectionData->copy_x_pos = ((ReportEntityLine*) item)->line().p1().x();
            m_sectionData->copy_y_pos = ((ReportEntityLine*) item)->line().p1().y();
        } else {
            m_sectionData->copy_x_pos = (int) item->x();
            m_sectionData->copy_y_pos = (int) item->y();
        }

        for (int i = 0; i < activeScene()->selectedItems().count(); i++) {
            m_sectionData->copy_list.append(dynamic_cast<ReportEntity*>(activeScene()->selectedItems()[i]));
        }
        m_sectionData->selected_x_offset = 10;
        m_sectionData->selected_y_offset = 10;
    }
}

void ReportDesigner::slotEditPaste()
{
    // call the editPaste function passing it a reportsection and point
    //  that make sense as defaults (same canvas / slightly offset pos of orig copy)
    QPoint p;
    p.setX(m_sectionData->copy_x_pos + m_sectionData->selected_x_offset);
    p.setY(m_sectionData->copy_y_pos + m_sectionData->selected_x_offset);
    slotEditPaste(activeScene(), p);
}

void ReportDesigner::slotEditPaste(QGraphicsScene * canvas, const QPointF & pos)
{
    Q_UNUSED(pos);
    // paste a new item of the copy we have in the specified location
    if (!m_sectionData->copy_list.isEmpty()) {
        QGraphicsItem * pasted_ent = 0;
        canvas->clearSelection();
        m_sectionData->mouseAction = ReportWriterSectionData::MA_None;

        for (int i = 0; i < m_sectionData->copy_list.count(); i++) {
            pasted_ent = 0;
            int type = dynamic_cast<KRObjectData*>(m_sectionData->copy_list[i])->type();
            kDebug() << type;
            QPointF o(m_sectionData->selected_x_offset, m_sectionData->selected_y_offset);
            if (type == KRObjectData::EntityLabel) {
                ReportEntityLabel * ent = dynamic_cast<ReportEntityLabel*>(m_sectionData->copy_list[i])->clone();
                ent->setEntityName(suggestEntityName("Label"));
                ent->setPos(ent->pos() + o);
                pasted_ent = ent;
            } else if (type == KRObjectData::EntityField) {
                ReportEntityField * ent = dynamic_cast<ReportEntityField*>(m_sectionData->copy_list[i])->clone();
                ent->setPos(ent->pos() + o);
                ent->setEntityName(suggestEntityName("Field"));
                pasted_ent = ent;
            } else if (type == KRObjectData::EntityText) {
                ReportEntityText * ent = dynamic_cast<ReportEntityText*>(m_sectionData->copy_list[i])->clone();
                ent->setPos(ent->pos() + o);
                ent->setEntityName(suggestEntityName("Text"));
                pasted_ent = ent;
            } else if (type == KRObjectData::EntityLine) {
                ReportEntityLine * ent = dynamic_cast<ReportEntityLine*>(m_sectionData->copy_list[i])->clone();
                ent->setEntityName(suggestEntityName("Line"));
                pasted_ent = ent;
            } else if (type == KRObjectData::EntityBarcode) {
                ReportEntityBarcode * ent = dynamic_cast<ReportEntityBarcode*>(m_sectionData->copy_list[i])->clone();
                ent->setPos(ent->pos() + o);
                ent->setEntityName(suggestEntityName("Barcode"));
                pasted_ent = ent;
            } else if (type == KRObjectData::EntityImage) {
                ReportEntityImage * ent = dynamic_cast<ReportEntityImage*>(m_sectionData->copy_list[i])->clone();
                ent->setPos(ent->pos() + o);
                ent->setEntityName(suggestEntityName("Image"));
                pasted_ent = ent;
            }
            //TODO add graph
            //else if(cp.copy_item == ReportWriterSectionData::GraphItem) {
            //    ReportEntityGraph * ent = new ReportEntityGraph(cp.copy_graph, rw, canvas);
            //    ent->setX(pos.x() + cp.copy_offset_x);
            //    ent->setY(pos.y() + cp.copy_offset_y);
            //    ent->setSize(cp.copy_rect.width(), cp.copy_rect.height());
            //    ent->show();
            //    pasted_ent = ent;
            //}
            else {
                kDebug() << "Tried to paste an item I don't understand.";
            }

            if (pasted_ent) {
                canvas->addItem(pasted_ent);
                pasted_ent->show();
                m_sectionData->mouseAction = ReportWriterSectionData::MA_Grab;
                setModified(true);
            }
        }
        m_sectionData->selected_x_offset += 10;
        m_sectionData->selected_y_offset += 10;
    }
}
void ReportDesigner::slotRaiseSelected()
{
    dynamic_cast<ReportScene*>(activeScene())->raiseSelected();
}

void ReportDesigner::slotLowerSelected()
{
    dynamic_cast<ReportScene*>(activeScene())->lowerSelected();
}

QGraphicsScene* ReportDesigner::activeScene() const
{
    return d->activeScene;
}

void ReportDesigner::setActiveScene(QGraphicsScene* a)
{
    if (d->activeScene && d->activeScene != a)
        d->activeScene->clearSelection();
    d->activeScene = a;
    
    //Trigger an update so that the last scene redraws its title;
    update();
}

KoZoomHandler* ReportDesigner::zoomHandler() const
{
    return d->zoom;
}

QString ReportDesigner::suggestEntityName(const QString &n) const
{
    ReportSection *sec;
    int itemCount = 0;
    //Count items in the main sections
    for (int i = 1; i <= KRSectionData::PageFooterAny; i++) {
        sec = section((KRSectionData::Section) i);
        if (sec) {
            const QGraphicsItemList l = sec->items();
            itemCount += l.count();
        }
    }

    //Count items in the group headers/footers
    for (int i = 0; i < m_detail->groupSectionCount(); i++) {
        sec = m_detail->groupSection(i)->groupHeader();
        if (sec) {
            const QGraphicsItemList l = sec->items();
            itemCount += l.count();
        }
        sec = m_detail->groupSection(i)->groupFooter();
        if (sec) {
            const QGraphicsItemList l = sec->items();
            itemCount += l.count();
        }
    }

    if (m_detail) {
        sec = m_detail->detailSection();
        if (sec) {
            const QGraphicsItemList l = sec->items();
            itemCount += l.count();
        }
    }

    while (!isEntityNameUnique(n + QString::number(itemCount))) {
        itemCount++;
    }
    return n + QString::number(itemCount);
}

bool ReportDesigner::isEntityNameUnique(const QString &n, KRObjectData* ignore) const
{
    ReportSection *sec;
    bool unique = true;

    //Check items in the main sections
    for (int i = 1; i <= KRSectionData::PageFooterAny; i++) {
        sec = section((KRSectionData::Section)i);
        if (sec) {
            const QGraphicsItemList l = sec->items();
            for (QGraphicsItemList::const_iterator it = l.constBegin(); it != l.constEnd(); ++it) {
                KRObjectData* itm = dynamic_cast<KRObjectData*>(*it);
                if (itm->entityName() == n  && itm != ignore) {
                    unique = false;
                    break;
                }
            }
            if (!unique) break;
        }
    }

    //Count items in the group headers/footers
    if (unique) {
        for (int i = 0; i < m_detail->groupSectionCount(); ++i) {
            sec = m_detail->groupSection(i)->groupHeader();
            if (sec) {
                const QGraphicsItemList l = sec->items();
                for (QGraphicsItemList::const_iterator it = l.constBegin(); it != l.constEnd(); ++it) {
                    KRObjectData* itm = dynamic_cast<KRObjectData*>(*it);
                    if (itm->entityName() == n  && itm != ignore) {
                        unique = false;
                        break;
                    }
                }

            }
            sec = m_detail->groupSection(i)->groupFooter();
            if (unique && sec) {
                const QGraphicsItemList l = sec->items();
                for (QGraphicsItemList::const_iterator it = l.constBegin(); it != l.constEnd(); ++it) {
                    KRObjectData* itm = dynamic_cast<KRObjectData*>(*it);
                    if (itm->entityName() == n  && itm != ignore) {
                        unique = false;
                        break;
                    }
                }
            }
        }
    }
    if (unique && m_detail) {
        sec = m_detail->detailSection();
        if (sec) {
            const QGraphicsItemList l = sec->items();
            for (QGraphicsItemList::const_iterator it = l.constBegin(); it != l.constEnd(); ++it) {
                KRObjectData* itm = dynamic_cast<KRObjectData*>(*it);
                if (itm->entityName() == n  && itm != ignore) {
                    unique = false;
                    break;
                }
            }
        }
    }

    return unique;
}

//static
QList<QAction*> ReportDesigner::actions()
{
    QList<QAction*> actList;
    QAction *act;

    act = new QAction(KIcon("feed-subscribe"), i18n("Label"), 0);
    act->setObjectName("action-insert-label");
    actList << act;
    
    act = new QAction(KIcon("edit-rename"), i18n("Field"), 0);
    act->setObjectName("action-insert-field");
    actList << act;

    act = new QAction(KIcon("insert-text"), i18n("Text"), 0);
    act->setObjectName("action-insert-text");
    actList << act;

    act = new QAction(KIcon("draw-freehand"), i18n("Line"), 0);
    act->setObjectName("action-insert-line");
    actList << act;
    
    act = new QAction(KIcon("insert-barcode"), i18n("Barcode"), 0);
    act->setObjectName("action-insert-barcode");
    actList << act;

    act = new QAction(KIcon("insert-image"), i18n("Image"), 0);
    act->setObjectName("action-insert-image");
    actList << act;
    
    act = new QAction(KIcon("office-chart-area"), i18n("Chart"), 0);
    act->setObjectName("action-insert-chart");
    actList << act;
    
    act = new QAction(KIcon("view-statistics"), i18n("Shape"), 0);
    act->setObjectName("action-insert-shape");
    actList << act;
    
    act = new QAction(KIcon("draw-cross"), i18n("Check"), 0);
    act->setObjectName("action-insert-check");
    actList << act;
    
    return actList;

}

