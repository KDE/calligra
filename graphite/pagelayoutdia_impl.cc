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
#include <kdebug.h>

PreviewWidget::PreviewWidget(Graphite::PageLayout &layout, QWidget *parent, const char *name) :
    QWidget(parent, name), m_layout(layout) {
    setFocusPolicy(QWidget::NoFocus);
}

void PreviewWidget::paintEvent(QPaintEvent *) {

    // First calculate some sane values...
    double w=static_cast<double>(width());
    double h, factor, aspect;
    int x, y;

    if(m_layout.layout==Graphite::PageLayout::Custom) {
        aspect=m_layout.customHeight/m_layout.customWidth;
        h=w*aspect;
        factor=w/m_layout.customWidth;
        x=0;
        y=Graphite::double2Int((static_cast<double>(height())-h)*0.5);
    }
    else {
        aspect=static_cast<double>(Graphite::pageHeight[m_layout.size])/
               static_cast<double>(Graphite::pageWidth[m_layout.size]);
        h=w*aspect;
        factor=w/static_cast<double>(Graphite::pageWidth[m_layout.size]);
        x=0;
        y=Graphite::double2Int((static_cast<double>(height())-h)*0.5);
    }

    // ...then check whether they are okay and correct if neccessary...
    if(h>static_cast<double>(height())) {
        h=static_cast<double>(height());
        w=h/aspect;
        x=Graphite::double2Int((static_cast<double>(width())-w)*0.5);
        y=0;
        if(m_layout.layout==Graphite::PageLayout::Custom)
            factor=h/m_layout.customHeight;
        else
            factor=h/static_cast<double>(Graphite::pageHeight[m_layout.size]);
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


PageLayoutDiaImpl::PageLayoutDiaImpl(Graphite::PageLayout &layout, QWidget *parent,
                  const char *name, bool modal, WFlags fl) : PageLayoutDia(parent, name, modal, fl),
    m_layout(layout) {

    // Add a preview widget
    QVBoxLayout *previewlayout=new QVBoxLayout(previewframe);
    previewlayout->setSpacing(6);  // "official" Qt defaults
    previewlayout->setMargin(11);  // "official" Qt defaults
    previewlayout->addSpacing(previewframe->fontMetrics().height()/2);
    PreviewWidget *previewwidget=new PreviewWidget(layout, previewframe, "preview");
    connect(this, SIGNAL(updatePreview()), previewwidget, SLOT(repaint()));
    previewlayout->addWidget(previewwidget);

    // Initialize all widgets
    unit->setCurrentItem(static_cast<int>(GraphiteGlobal::self()->unit()));
    connect(unit, SIGNAL(activated(int)), this, SLOT(unitChanged(int)));

    if(m_layout.layout==Graphite::PageLayout::Custom)
        format->setCurrentItem(0);
    else
        format->setCurrentItem(static_cast<int>(m_layout.size)+1);
    connect(format, SIGNAL(activated(int)), this, SLOT(formatChanged(int)));

    orientation->setCurrentItem(static_cast<int>(m_layout.orientation));
    connect(orientation, SIGNAL(activated(int)), this, SLOT(orientationChanged(int)));

    width->setRange(0.0, 1000.0, 1.0, false);
    width->setValue(m_layout.width());
    connect(width, SIGNAL(valueChanged(double)), this, SLOT(widthChanged(double)));
    height->setRange(0.0, 1000.0, 1.0, false);
    height->setValue(m_layout.height());
    connect(height, SIGNAL(valueChanged(double)), this, SLOT(heightChanged(double)));

    top->setRange(0.0, m_layout.height()-m_layout.borders.bottom, 1.0, false);
    top->setValue(m_layout.borders.top);
    connect(top, SIGNAL(valueChanged(double)), this, SLOT(topBorderChanged(double)));
    left->setRange(0.0, m_layout.width()-m_layout.borders.right, 1.0, false);
    left->setValue(m_layout.borders.left);
    connect(left, SIGNAL(valueChanged(double)), this, SLOT(leftBorderChanged(double)));
    right->setRange(0.0, m_layout.width()-m_layout.borders.left, 1.0, false);
    right->setValue(m_layout.borders.right);
    connect(right, SIGNAL(valueChanged(double)), this, SLOT(rightBorderChanged(double)));
    bottom->setRange(0.0, m_layout.height()-m_layout.borders.top, 1.0, false);
    bottom->setValue(m_layout.borders.bottom);
    connect(bottom, SIGNAL(valueChanged(double)), this, SLOT(bottomBorderChanged(double)));

    connect(restore, SIGNAL(clicked()), this, SLOT(restoreDefaults()));
    connect(save, SIGNAL(clicked()), this, SLOT(saveAsDefault()));
}

void PageLayoutDiaImpl::pageLayoutDia(Graphite::PageLayout &layout, QWidget *parent) {

    Graphite::PageLayout tmp=layout;  // store in case of a Cancel operation
    PageLayoutDiaImpl dia(layout, parent, "pagelayoutdiaimpl", true);
    if(dia.exec()==QDialog::Accepted)
        GraphiteGlobal::self()->setUnit(static_cast<GraphiteGlobal::Unit>(dia.unit->currentItem()));
    else
        layout=tmp;  // The user cancelled -> restore the original state
}

void PageLayoutDiaImpl::unitChanged(int) {
}

void PageLayoutDiaImpl::formatChanged(int) {
}

void PageLayoutDiaImpl::orientationChanged(int) {
}

void PageLayoutDiaImpl::heightChanged(double) {
}

void PageLayoutDiaImpl::widthChanged(double) {
}

void PageLayoutDiaImpl::topBorderChanged(double) {
}

void PageLayoutDiaImpl::leftBorderChanged(double) {
}

void PageLayoutDiaImpl::rightBorderChanged(double) {
}

void PageLayoutDiaImpl::bottomBorderChanged(double) {
}

void PageLayoutDiaImpl::saveAsDefault() {
}

void PageLayoutDiaImpl::restoreDefaults() {
}

#include <pagelayoutdia_impl.moc>
