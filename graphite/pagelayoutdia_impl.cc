/* This file is part of the KDE project
   Copyright (C) 2001 Werner Trobin <trobin@kde.org>

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

#include <pagelayoutdia_impl.h>

#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qpushbutton.h>

#include <knuminput.h>

#include <graphitepart.h>

PreviewWidget::PreviewWidget(Graphite::PageLayout &layout, QWidget *parent, const char *name) :
    QWidget(parent, name), m_layout(layout) {
    setFocusPolicy(QWidget::NoFocus);
}

void PreviewWidget::paintEvent(QPaintEvent *) {

    // First calculate some sane values...
    double w=static_cast<double>(width());
    double h, factor, aspect;
    int x, y;

    aspect=m_layout.height()/m_layout.width();
    h=w*aspect;
    factor=w/m_layout.width();
    x=0;
    y=Graphite::double2Int((static_cast<double>(height())-h)*0.5);

    // ...then check whether they are okay and correct if neccessary...
    if(h>static_cast<double>(height())) {
        h=static_cast<double>(height());
        w=h/aspect;
        x=Graphite::double2Int((static_cast<double>(width())-w)*0.5);
        y=0;
        factor=h/m_layout.height();
    }

    // ... calculate the borders...
    int left=x+Graphite::double2Int(m_layout.borders.left*factor);
    int bw=x-left+Graphite::double2Int(w-m_layout.borders.right*factor);
    int top=y+Graphite::double2Int(m_layout.borders.top*factor);
    int bh=y-top+Graphite::double2Int(h-m_layout.borders.bottom*factor);

    // ...and paint the stuff :)
    QPainter p(this);
    p.setBrush(colorGroup().brush(QColorGroup::Light));
    p.setPen(Qt::black);
    p.drawRect(x, y, Graphite::double2Int(w), Graphite::double2Int(h));
    p.setBrush(colorGroup().brush(QColorGroup::Midlight));
    p.drawRect(left, top, bw, bh);
    p.end();
}


PageLayoutDiaImpl::PageLayoutDiaImpl(Graphite::PageLayout &layout, const GraphitePart * const doc,
                                     QWidget *parent, const char *name, bool modal, WFlags fl) :
    PageLayoutDia(parent, name, modal, fl), m_layout(layout) {

    // Add a preview widget
    QVBoxLayout *previewlayout=new QVBoxLayout(previewframe);
    previewlayout->setSpacing(6);  // "official" Qt defaults
    previewlayout->setMargin(11);  // "official" Qt defaults
    previewlayout->addSpacing(previewframe->fontMetrics().height()/2);
    PreviewWidget *previewwidget=new PreviewWidget(layout, previewframe, "preview");
    connect(this, SIGNAL(updatePreview()), previewwidget, SLOT(repaint()));
    previewlayout->addWidget(previewwidget);

    // Initialize all widgets
    unit->setCurrentItem(static_cast<int>(doc->unit()));
    connect(unit, SIGNAL(activated(int)), this, SLOT(unitChanged(int)));

    if(m_layout.layout==Graphite::PageLayout::Custom)
        format->setCurrentItem(0);
    else
        format->setCurrentItem(static_cast<int>(m_layout.size)+1);
    connect(format, SIGNAL(activated(int)), this, SLOT(formatChanged(int)));

    orientation->setCurrentItem(static_cast<int>(m_layout.orientation));
    connect(orientation, SIGNAL(activated(int)), this, SLOT(orientationChanged(int)));

    connect(width, SIGNAL(valueChanged(double)), this, SLOT(widthChanged(double)));
    connect(height, SIGNAL(valueChanged(double)), this, SLOT(heightChanged(double)));
    connect(top, SIGNAL(valueChanged(double)), this, SLOT(topBorderChanged(double)));
    connect(left, SIGNAL(valueChanged(double)), this, SLOT(leftBorderChanged(double)));
    connect(right, SIGNAL(valueChanged(double)), this, SLOT(rightBorderChanged(double)));
    connect(bottom, SIGNAL(valueChanged(double)), this, SLOT(bottomBorderChanged(double)));

    connect(restore, SIGNAL(clicked()), this, SLOT(restoreDefaults()));
    connect(save, SIGNAL(clicked()), this, SLOT(saveAsDefault()));

    // initialize the numinputs
    unitChanged(static_cast<int>(doc->unit()));

    // work around a Qt bug?
    resize(475, 480);
}

void PageLayoutDiaImpl::pageLayoutDia(Graphite::PageLayout &layout, GraphitePart * const doc,
                                      QWidget *parent) {

    Graphite::PageLayout tmp=layout;  // store in case of a Cancel operation
    PageLayoutDiaImpl dia(layout, doc, parent, "pagelayoutdiaimpl", true);
    if(dia.exec()==QDialog::Accepted)
        doc->setUnit(static_cast<Graphite::Unit>(dia.unit->currentItem()));
    else
        layout=tmp;  // The user cancelled -> restore the original state
}

void PageLayoutDiaImpl::unitChanged(int u) {

    Graphite::Unit unit=static_cast<Graphite::Unit>(u);

    if(unit==Graphite::MM) {
        width->setRange(0.0,  Graphite::pageWidth[QPrinter::B0], 1.0, false);
        width->setValue(m_layout.width());
        height->setRange(0.0,  Graphite::pageWidth[QPrinter::B0], 1.0, false); // width both times b/c of the orient.
        height->setValue(m_layout.height());
        top->setRange(0.0, m_layout.height()-m_layout.borders.bottom, 1.0, false);
        top->setValue(m_layout.borders.top);
        left->setRange(0.0, m_layout.width()-m_layout.borders.right, 1.0, false);
        left->setValue(m_layout.borders.left);
        right->setRange(0.0, m_layout.width()-m_layout.borders.left, 1.0, false);
        right->setValue(m_layout.borders.right);
        bottom->setRange(0.0, m_layout.height()-m_layout.borders.top, 1.0, false);
        bottom->setValue(m_layout.borders.bottom);
    }
    else if(unit==Graphite::Pt) {
        width->setRange(0.0,  Graphite::mm2pt(Graphite::pageWidth[QPrinter::B0]), 1.0, false);
        width->setValue(Graphite::mm2pt(m_layout.width()));
        height->setRange(0.0,  Graphite::mm2pt(Graphite::pageWidth[QPrinter::B0]), 1.0, false); // width both times b/c of the orient.
        height->setValue(Graphite::mm2pt(m_layout.height()));
        top->setRange(0.0, Graphite::mm2pt(m_layout.height()-m_layout.borders.bottom), 1.0, false);
        top->setValue(Graphite::mm2pt(m_layout.borders.top));
        left->setRange(0.0, Graphite::mm2pt(m_layout.width()-m_layout.borders.right), 1.0, false);
        left->setValue(Graphite::mm2pt(m_layout.borders.left));
        right->setRange(0.0, Graphite::mm2pt(m_layout.width()-m_layout.borders.left), 1.0, false);
        right->setValue(Graphite::mm2pt(m_layout.borders.right));
        bottom->setRange(0.0, Graphite::mm2pt(m_layout.height()-m_layout.borders.top), 1.0, false);
        bottom->setValue(Graphite::mm2pt(m_layout.borders.bottom));
    }
    else {
        width->setRange(0.0,  Graphite::mm2inch(Graphite::pageWidth[QPrinter::B0]), 1.0, false);
        width->setValue(Graphite::mm2inch(m_layout.width()));
        height->setRange(0.0,  Graphite::mm2inch(Graphite::pageWidth[QPrinter::B0]), 1.0, false); // width both times b/c of the orient.
        height->setValue(Graphite::mm2inch(m_layout.height()));
        top->setRange(0.0, Graphite::mm2inch(m_layout.height()-m_layout.borders.bottom), 1.0, false);
        top->setValue(Graphite::mm2inch(m_layout.borders.top));
        left->setRange(0.0, Graphite::mm2inch(m_layout.width()-m_layout.borders.right), 1.0, false);
        left->setValue(Graphite::mm2inch(m_layout.borders.left));
        right->setRange(0.0, Graphite::mm2inch(m_layout.width()-m_layout.borders.left), 1.0, false);
        right->setValue(Graphite::mm2inch(m_layout.borders.right));
        bottom->setRange(0.0, Graphite::mm2inch(m_layout.height()-m_layout.borders.top), 1.0, false);
        bottom->setValue(Graphite::mm2inch(m_layout.borders.bottom));
    }
}

void PageLayoutDiaImpl::formatChanged(int f) {

    if(f==0) {
        m_layout.layout=Graphite::PageLayout::Custom;
        width->setEnabled(true);
        height->setEnabled(true);
    }
    else {
        m_layout.layout=Graphite::PageLayout::Norm;
        width->setEnabled(false);
        height->setEnabled(false);
        m_layout.size=static_cast<QPrinter::PageSize>(f-1);
        correctBorders();  // guard against "overflows"
        unitChanged(unit->currentItem());
        emit updatePreview();
    }
}

void PageLayoutDiaImpl::orientationChanged(int o) {

    m_layout.orientation=static_cast<QPrinter::Orientation>(o);
    correctBorders();
    unitChanged(unit->currentItem());
    emit updatePreview();
}

void PageLayoutDiaImpl::heightChanged(double h) {
    m_layout.setHeight(h);
    correctBorders();
    unitChanged(unit->currentItem());
    emit updatePreview();
}

void PageLayoutDiaImpl::widthChanged(double w) {
    m_layout.setWidth(w);
    correctBorders();
    unitChanged(unit->currentItem());
    emit updatePreview();
}

void PageLayoutDiaImpl::topBorderChanged(double t) {
    m_layout.borders.top=t;
    correctBorders();
    unitChanged(unit->currentItem());
    emit updatePreview();
}

void PageLayoutDiaImpl::leftBorderChanged(double l) {
    m_layout.borders.left=l;
    correctBorders();
    unitChanged(unit->currentItem());
    emit updatePreview();
}

void PageLayoutDiaImpl::rightBorderChanged(double r) {
    m_layout.borders.right=r;
    correctBorders();
    unitChanged(unit->currentItem());
    emit updatePreview();
}

void PageLayoutDiaImpl::bottomBorderChanged(double b) {
    m_layout.borders.bottom=b;
    correctBorders();
    unitChanged(unit->currentItem());
    emit updatePreview();
}

void PageLayoutDiaImpl::saveAsDefault() {
}

void PageLayoutDiaImpl::restoreDefaults() {
}

void PageLayoutDiaImpl::correctBorders() {

    double bw=m_layout.borders.left+m_layout.borders.right+1;
    double bh=m_layout.borders.top+m_layout.borders.bottom+1;
    if(m_layout.width()<bw) {
        double diff=(bw-m_layout.width()+6)*0.5;
        m_layout.borders.left-=diff;
        m_layout.borders.left=m_layout.borders.left < 0.0 ? 0.0 : m_layout.borders.left;
        m_layout.borders.right-=diff;
        m_layout.borders.right=m_layout.borders.right < 0.0 ? 0.0 : m_layout.borders.right;
    }
    if(m_layout.height()<bh) {
        double diff=(bh-m_layout.height()+6)*0.5;
        m_layout.borders.top-=diff;
        m_layout.borders.top=m_layout.borders.top < 0.0 ? 0.0 : m_layout.borders.top;
        m_layout.borders.bottom-=diff;
        m_layout.borders.bottom=m_layout.borders.bottom < 0.0 ? 0.0 : m_layout.borders.bottom;
    }
}

#include <pagelayoutdia_impl.moc>
