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
#include <qwhatsthis.h>

#include <kglobal.h>
#include <klocale.h>
#include <kmessagebox.h>

#include <myspinbox.h>
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


GUnitValidator::GUnitValidator(QWidget *parent, const char *name) :
    QValidator(parent, name) {
}

QValidator::State GUnitValidator::validate(QString &input, int &/*pos*/) const {

    QString str=input.stripWhiteSpace();
    QChar dot('.'), comma(','), m('m'), M('M'), p('p'), P('P'), t('t'), T('T'),
        i('i'), I('I'), n('n'), N('N');
    unsigned int j=1;
    bool c=false;

    if(!str.isEmpty() && !str[0].isDigit() && str[0]!=dot && str[0]!=comma && str[0]!=KGlobal::locale()->decimalSymbol()[0])
        return QValidator::Invalid;

    while(j<str.length()) {
        if(str[j].isDigit() || str[j].isSpace())
            ++j;
        else if(!c && (str[j]==dot || str[j]==comma || str[j]==KGlobal::locale()->decimalSymbol()[0])) {
            ++j;
            c=true;
        }
        else if(str[j]==m || str[j]==M) {
            if(j+1<str.length() && (str[j+1]==m || str[j+1]==M)) {
                if(j+2>=str.length())
                    return QValidator::Acceptable;
                else
                    return QValidator::Invalid;
            }
            else if(j+1>=str.length())
                return QValidator::Intermediate;
            else
                return QValidator::Invalid;
        }
        else if(str[j]==p || str[j]==P) {
            if(j+1<str.length() && (str[j+1]==t || str[j+1]==T)) {
                if(j+2>=str.length())
                    return QValidator::Acceptable;
                else
                    return QValidator::Invalid;
            }
            else if(j+1>=str.length())
                return QValidator::Intermediate;
            else
                return QValidator::Invalid;
        }
        else if(str[j]==i || str[j]==I) {
            if(j+1<str.length() && (str[j+1]==n || str[j+1]==N)) {
                if(j+2>=str.length())
                    return QValidator::Acceptable;
                else
                    return QValidator::Invalid;
            }
            else if(j+1>=str.length())
                return QValidator::Intermediate;
            else
                return QValidator::Invalid;
        }
        else
            return QValidator::Invalid;
    }
    return QValidator::Intermediate;
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
    QWhatsThis::add(previewwidget, i18n("This field shows a simplified preview of the page layout."));
    connect(this, SIGNAL(updatePreview()), previewwidget, SLOT(repaint()));
    previewlayout->addWidget(previewwidget);

    // Initialize all widgets
    connect(unit, SIGNAL(activated(int)), this, SLOT(unitChanged(int)));
    unit->setCurrentItem(static_cast<int>(doc->unit()));
    // initialize the default unit of the numinputs
    unitChanged(static_cast<int>(doc->unit()));
    // This one is needed here, because of a werid bug
    updateWH();

    if(m_layout.layout==Graphite::PageLayout::Custom) {
        format->setCurrentItem(0);
        width->setEnabled(true);
        height->setEnabled(true);
    }
    else {
        format->setCurrentItem(static_cast<int>(m_layout.size)+1);
        width->setEnabled(false);
        height->setEnabled(false);
    }
    connect(format, SIGNAL(activated(int)), this, SLOT(formatChanged(int)));

    orientation->setCurrentItem(static_cast<int>(m_layout.orientation));
    connect(orientation, SIGNAL(activated(int)), this, SLOT(orientationChanged(int)));

    connect(width, SIGNAL(valueChanged(const QString &)), this, SLOT(widthChanged()));
    width->setValidator(new GUnitValidator(width));
    connect(height, SIGNAL(valueChanged(const QString &)), this, SLOT(heightChanged()));
    height->setValidator(new GUnitValidator(height));
    connect(top, SIGNAL(valueChanged(const QString &)), this, SLOT(topBorderChanged()));
    top->setValidator(new GUnitValidator(top));
    connect(left, SIGNAL(valueChanged(const QString &)), this, SLOT(leftBorderChanged()));
    left->setValidator(new GUnitValidator(left));
    connect(right, SIGNAL(valueChanged(const QString &)), this, SLOT(rightBorderChanged()));
    right->setValidator(new GUnitValidator(right));
    connect(bottom, SIGNAL(valueChanged(const QString &)), this, SLOT(bottomBorderChanged()));
    bottom->setValidator(new GUnitValidator(bottom));

    connect(restore, SIGNAL(clicked()), this, SLOT(restoreDefaults()));
    connect(save, SIGNAL(clicked()), this, SLOT(saveAsDefault()));
    connect(buttonOk, SIGNAL(clicked()), this, SLOT(okClicked()));

    // work around a designer bug :(
    setTabOrder(orientation, width);
    setTabOrder(width, height);
    setTabOrder(height, top);
    setTabOrder(top, left);
    setTabOrder(left, right);
    setTabOrder(right, bottom);
    setTabOrder(bottom, restore);

    // initialize the boxes
    top->setValue(m_layout.borders.top);
    left->setValue(m_layout.borders.left);
    right->setValue(m_layout.borders.right);
    bottom->setValue(m_layout.borders.bottom);
}

bool PageLayoutDiaImpl::pageLayoutDia(Graphite::PageLayout &layout, GraphitePart * const doc,
                                      QWidget *parent) {

    PageLayoutDiaImpl dia(layout, doc, parent, "pagelayoutdiaimpl", true);
    if(dia.exec()==QDialog::Accepted) {
        doc->setUnit(static_cast<Graphite::Unit>(dia.unit->currentItem()));
        return true;
    }
    else
        return false;
}

void PageLayoutDiaImpl::unitChanged(int u) {

    Graphite::Unit unit=static_cast<Graphite::Unit>(u);
    width->setDefaultUnit(unit);
    height->setDefaultUnit(unit);
    top->setDefaultUnit(unit);
    left->setDefaultUnit(unit);
    right->setDefaultUnit(unit);
    bottom->setDefaultUnit(unit);
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
        updateWH();
        emit updatePreview();
    }
}

void PageLayoutDiaImpl::orientationChanged(int o) {
    m_layout.orientation=static_cast<QPrinter::Orientation>(o);
    updateWH();
    emit updatePreview();
}

void PageLayoutDiaImpl::heightChanged() {

    double h=height->currentValue();
    if(h>1501.0) {
        KMessageBox::sorry(this, i18n("You can't enter custom height values bigger\nthan 1500 mm / 59 in / 4250 pt."));
        h=1500.0;
        disconnect(height, SIGNAL(valueChanged(const QString &)), this, SLOT(heightChanged()));
        height->fixSpinbox();  // if the user used the up button we have to work around a QSpinBox oddity
        height->setValue(h);
        connect(height, SIGNAL(valueChanged(const QString &)), this, SLOT(heightChanged()));
    }
    m_layout.setHeight(h);
    emit updatePreview();
}

void PageLayoutDiaImpl::widthChanged() {

    double w=width->currentValue();
    if(w>1501.0) {
        KMessageBox::sorry(this, i18n("You can't enter custom width values bigger\nthan 1500 mm / 59 in / 4250 pt."));
        w=1500.0;
        disconnect(width, SIGNAL(valueChanged(const QString &)), this, SLOT(widthChanged()));
        width->fixSpinbox();  // if the user used the up button we have to work around a QSpinBox oddity
        width->setValue(w);
        connect(width, SIGNAL(valueChanged(const QString &)), this, SLOT(widthChanged()));
    }
    m_layout.setWidth(w);
    emit updatePreview();
}

void PageLayoutDiaImpl::topBorderChanged() {
    m_layout.borders.top=top->currentValue();
    emit updatePreview();
}

void PageLayoutDiaImpl::leftBorderChanged() {
    m_layout.borders.left=left->currentValue();
    emit updatePreview();
}

void PageLayoutDiaImpl::rightBorderChanged() {
    m_layout.borders.right=right->currentValue();
    emit updatePreview();
}

void PageLayoutDiaImpl::bottomBorderChanged() {
    m_layout.borders.bottom=bottom->currentValue();
    emit updatePreview();
}

void PageLayoutDiaImpl::saveAsDefault() {

    (void)width->currentValue();  // make sure that we have the current value
    (void)height->currentValue();
    (void)top->currentValue();
    (void)left->currentValue();
    (void)right->currentValue();
    (void)bottom->currentValue();
    m_layout.saveDefaults();
}

void PageLayoutDiaImpl::restoreDefaults() {

    m_layout.loadDefaults();
    // update the widget state
    orientation->setCurrentItem(static_cast<int>(m_layout.orientation));
    if(m_layout.layout==Graphite::PageLayout::Custom) {
        format->setCurrentItem(0);
        width->setEnabled(true);
        height->setEnabled(true);
    }
    else {
        format->setCurrentItem(static_cast<int>(m_layout.size)+1);
        width->setEnabled(false);
        height->setEnabled(false);
    }
    updateWH();
    top->setValue(m_layout.borders.top);
    left->setValue(m_layout.borders.left);
    right->setValue(m_layout.borders.right);
    bottom->setValue(m_layout.borders.bottom);

    emit updatePreview();
}

void PageLayoutDiaImpl::okClicked() {

    // sanity checks
    // These two calls just interpret the text and pop up the dia if there's any error
    (void)height->currentValue();
    (void)width->currentValue();
    if(!bordersOk()) {
        if(KMessageBox::questionYesNo(this, i18n("The border settings you specified\nare invalid (out of range).\n"
                                                 "Should I correct them?"))==KMessageBox::No)
            return;
        else
            correctBorders();
    }
    accept();
}

bool PageLayoutDiaImpl::bordersOk() {
    if(m_layout.width()<m_layout.borders.left+m_layout.borders.right+1)
        return false;
    if(m_layout.height()<m_layout.borders.top+m_layout.borders.bottom+1)
        return false;
    return true;
}

void PageLayoutDiaImpl::correctBorders() {

    double w=m_layout.width();
    double h=m_layout.height();
    double bw=m_layout.borders.left+m_layout.borders.right+1;
    double bh=m_layout.borders.top+m_layout.borders.bottom+1;
    if(w<bw) {
        if(m_layout.borders.left>w) {
            m_layout.borders.left=w;
            bw=m_layout.borders.left+m_layout.borders.right+1;
        }
        if(m_layout.borders.right>w) {
            m_layout.borders.right=w;
            bw=m_layout.borders.left+m_layout.borders.right+1;
        }
        double diff=(bw-w+6)*0.5;
        m_layout.borders.left-=diff;
        m_layout.borders.left=m_layout.borders.left < 0.0 ? 0.0 : m_layout.borders.left;
        m_layout.borders.right-=diff;
        m_layout.borders.right=m_layout.borders.right < 0.0 ? 0.0 : m_layout.borders.right;
    }
    if(h<bh) {
        if(m_layout.borders.top>h) {
            m_layout.borders.top=h;
            bh=m_layout.borders.top+m_layout.borders.bottom+1;
        }
        if(m_layout.borders.bottom>h) {
            m_layout.borders.bottom=h;
            bh=m_layout.borders.top+m_layout.borders.bottom+1;
        }
        double diff=(bh-h+6)*0.5;
        m_layout.borders.top-=diff;
        m_layout.borders.top=m_layout.borders.top < 0.0 ? 0.0 : m_layout.borders.top;
        m_layout.borders.bottom-=diff;
        m_layout.borders.bottom=m_layout.borders.bottom < 0.0 ? 0.0 : m_layout.borders.bottom;
    }
}

void PageLayoutDiaImpl::updateWH() {
    width->setValue(m_layout.width());
    height->setValue(m_layout.height());
}

#include <pagelayoutdia_impl.moc>
