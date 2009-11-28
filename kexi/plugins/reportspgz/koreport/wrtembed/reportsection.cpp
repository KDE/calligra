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
#include "reportentitylabel.h"
#include "reportentityfield.h"
#include "reportentitytext.h"
#include "reportentityline.h"
#include "reportentitybarcode.h"
#include "reportentityimage.h"
#include "reportentitychart.h"
#include "reportentitycheck.h"
#include "reportentityshape.h"

#include "reportscene.h"
#include "reportsceneview.h"

// qt
#include <qlabel.h>
#include <qdom.h>
#include <qlayout.h>
#include <QGridLayout>
#include <QMouseEvent>

#include <KoDpi.h>
#include <KoRuler.h>
#include <KoZoomHandler.h>
#include <koproperty/EditorView.h>
#include <KColorScheme>

#include <kdebug.h>


//
// ReportSection method implementations
//

ReportSection::ReportSection(ReportDesigner * rptdes, const char * name)
        : QWidget(rptdes)
{
    Q_UNUSED(name)
    
    m_sectionData = new KRSectionData();
    QObject::connect(m_sectionData->properties(), SIGNAL(propertyChanged(KoProperty::Set &, KoProperty::Property &)), this, SLOT(slotPropertyChanged(KoProperty::Set &, KoProperty::Property &)));
    int dpiY = KoDpi::dpiY();

    m_reportDesigner = rptdes;
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QGridLayout * glayout = new QGridLayout(this);
    glayout->setSpacing(0);
    glayout->setMargin(0);
    glayout->setColumnStretch(1, 1);
    glayout->setRowStretch(1, 1);
    glayout->setSizeConstraint(QLayout::SetFixedSize);

    // ok create the base interface
    m_title = new ReportSectionTitle(this);
    m_title->setObjectName("detail");
    m_title->setText(i18n("Detail"));

    m_sectionRuler = new KoRuler(this, Qt::Vertical, m_reportDesigner->zoomHandler());
    m_scene = new ReportScene(m_reportDesigner->pageWidthPx(), dpiY, rptdes);
    m_sceneView = new ReportSceneView(rptdes, m_scene, this, "scene view");
    m_sceneView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_resizeBar = new ReportResizeBar(this);

    QObject::connect(m_resizeBar, SIGNAL(barDragged(int)), this, SLOT(slotResizeBarDragged(int)));
    QObject::connect(m_reportDesigner, SIGNAL(pagePropertyChanged(KoProperty::Set &)), this, SLOT(slotPageOptionsChanged(KoProperty::Set &)));
    QObject::connect(m_scene, SIGNAL(clicked()), this, (SLOT(slotSceneClicked())));

    glayout->addWidget(m_title, 0, 0, 1, 2);
    glayout->addWidget(m_sectionRuler, 1, 0);
    glayout->addWidget(m_sceneView , 1, 1);
    glayout->addWidget(m_resizeBar, 2, 0, 1, 2);

    setLayout(glayout);
    slotResizeBarDragged(0);
}

ReportSection::~ReportSection()
{
    // Qt should be handling everything for us
}

void ReportSection::setTitle(const QString & s)
{
    m_title->setText(s);
}

void ReportSection::slotResizeBarDragged(int delta)
{
    if (m_sceneView->designer() && m_sceneView->designer()->propertySet()->property("PageSize").value().toString() == "Labels") {
        return; // we don't want to allow this on reports that are for labels
    }

    qreal h = m_scene->height() + delta;

    if (h < 1) h = 1;

    h = m_scene->gridPoint(QPointF(0, h)).y();

    m_sectionRuler->setRulerLength(h);

    m_scene->setSceneRect(0, 0, m_scene->width(), h);
    m_sceneView->resizeContents(QSize(m_scene->width(), h));

    m_reportDesigner->setModified(true);
}

void ReportSection::buildXML(QDomDocument & doc, QDomElement & section)
{
    int dpiY = KoDpi::dpiY();
    qreal f = INCH_TO_POINT(m_scene->height() / dpiY);
    //f = ( ( f - ( int ) f ) > .5 ? f : f + 1 );
    QDomElement height = doc.createElement("height");
    height.appendChild(doc.createTextNode(QString::number(f)));
    section.appendChild(height);

    QDomElement bgcolor = doc.createElement("bgcolor");
    bgcolor.appendChild(doc.createTextNode(m_sectionData->bgColor().name()));
    section.appendChild(bgcolor);

    // now get a list of all the QCanvasItems on this scene and output them.
    QGraphicsItemList list = m_scene->items();
    for (QGraphicsItemList::iterator it = list.begin();
            it != list.end(); it++) {
        ReportEntity::buildXML((*it), doc, section);
    }
}

void ReportSection::initFromXML(QDomNode & section)
{
    QDomNodeList nl = section.childNodes();
    QDomNode node;
    QString n;

    for (int i = 0; i < nl.count(); i++) {
        node = nl.item(i);
        n = node.nodeName();
        if (n == "height") {
            qreal h = node.firstChild().nodeValue().toDouble();
            h  = POINT_TO_INCH(h) * KoDpi::dpiY();
            kDebug() << "Section Height: " << h;
            m_scene->setSceneRect(0, 0, m_scene->width(), h);
            slotResizeBarDragged(0);
        } else if (n == "bgcolor") {
            m_sectionData->m_backgroundColor->setValue(QColor(node.firstChild().nodeValue()));
        }
        //Objects
        else if (n == "label") {
            (new ReportEntityLabel(node, m_sceneView->designer(), m_scene))->setVisible(true);
        } else if (n == "field") {
            (new ReportEntityField(node, m_sceneView->designer(), m_scene))->setVisible(true);
        } else if (n == "text") {
            (new ReportEntityText(node, m_sceneView->designer(), m_scene))->setVisible(true);
        } else if (n == "line") {
            (new ReportEntityLine(node, m_sceneView->designer(), m_scene))->setVisible(true);
        } else if (n == "barcode") {
            (new ReportEntityBarcode(node, m_sceneView->designer(), m_scene))->setVisible(true);
        } else if (n == "image") {
            (new ReportEntityImage(node, m_sceneView->designer(), m_scene))->setVisible(true);
        } else if (n == "chart") {
            (new ReportEntityChart(node, m_sceneView->designer(), m_scene))->setVisible(true);
        } else if (n == "check") {
            (new ReportEntityCheck(node, m_sceneView->designer(), m_scene))->setVisible(true);
        } else if (n == "shape") {
            (new ReportEntityShape(node, m_sceneView->designer(), m_scene))->setVisible(true);
        } else if (n == "key" || n == "firstpage" || n == "lastpage"
                   || n == "odd" || n == "even") {
            // these are all handled elsewhere but we don't want to show errors
            // because they are expected sometimes
        } else {
            kDebug() << "Encountered unknown node while parsing section: " << n;
        }
    }
}

QSize ReportSection::sizeHint() const
{
    return QSize(m_scene->width()  + m_sectionRuler->frameSize().width(), m_title->frameSize().height() + m_sceneView->sizeHint().height() + m_resizeBar->frameSize().height());;
}

void ReportSection::slotPageOptionsChanged(KoProperty::Set &set)
{
    Q_UNUSED(set)
    
    KoUnit unit = m_reportDesigner->pageUnit();

    //update items position with unit
    QList<QGraphicsItem*> itms = m_scene->items();
    for (int i = 0; i < itms.size(); ++i) {
        if (itms[i]->type() >= 65550 && itms[i]->type() <= 65555) {
            dynamic_cast<ReportRectEntity*>(itms[i])->setUnit(unit);
        }
    }

    m_scene->setSceneRect(0, 0, m_reportDesigner->pageWidthPx(), m_scene->height());
    m_title->setMinimumWidth(m_reportDesigner->pageWidthPx() + m_sectionRuler->frameSize().width());
    m_sectionRuler->setUnit(m_reportDesigner->pageUnit());

    //Trigger a redraw of the background
    m_sceneView->resetCachedContent();

    m_reportDesigner->adjustSize();
    m_reportDesigner->repaint();
}

void ReportSection::slotSceneClicked()
{
    m_reportDesigner->setActiveScene(m_scene);
    m_reportDesigner->changeSet(m_sectionData->properties());
}

void ReportSection::slotPropertyChanged(KoProperty::Set &s, KoProperty::Property &p)
{
    Q_UNUSED(s)
    
    //Handle Background Color
    if (p.name() == "BackgroundColor") {
        m_scene->setBackgroundBrush(p.value().value<QColor>());
    }

    if (m_reportDesigner) m_reportDesigner->setModified(true);

    m_scene->update();
}

//
// class ReportResizeBar
//
ReportResizeBar::ReportResizeBar(QWidget * parent, Qt::WFlags f)
        : QFrame(parent, f)
{
    //setMinimumHeight(5);
    //setMaximumHeight(5);
    setCursor(QCursor(Qt::SizeVerCursor));
    setFrameStyle(QFrame::HLine);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
}

void ReportResizeBar::mouseMoveEvent(QMouseEvent * e)
{
    e->accept();
    emit barDragged(e->y());
}

//=============================================================================

ReportSectionTitle::ReportSectionTitle(QWidget*parent) : QLabel(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setFrameStyle(QFrame::Panel | QFrame::Raised);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
}

ReportSectionTitle::~ReportSectionTitle()
{

}

void ReportSectionTitle::paintEvent(QPaintEvent * event)
{
    QPainter painter(this);
    KColorScheme colorScheme(QPalette::Active);
    
    QLinearGradient linearGrad(QPointF(0, 0), QPointF(width(), 0));
    
    ReportSection* _section = dynamic_cast<ReportSection*>(parent());

    if (_section->m_scene == _section->m_reportDesigner->activeScene()) {
      linearGrad.setColorAt(0, colorScheme.decoration(KColorScheme::HoverColor).color());
      linearGrad.setColorAt(1, colorScheme.decoration(KColorScheme::FocusColor).color());
    }
    else {
      linearGrad.setColorAt(0, colorScheme.background(KColorScheme::NormalBackground).color());
      linearGrad.setColorAt(1, colorScheme.foreground(KColorScheme::InactiveText).color());
    }
     
    painter.fillRect(rect(), linearGrad);
    QLabel::paintEvent(event);
}