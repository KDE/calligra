/* This file is part of the KDE project
   Copyright (C) 2000 Werner Trobin <trobin@kde.org>

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

#include <qdom.h>
#include <qlayout.h>
#include <qvbuttongroup.h>
#include <qradiobutton.h>
#include <qvgroupbox.h>
#include <qwidgetstack.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qslider.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qimage.h>
#include <qstringlist.h>
#include <qsizepolicy.h>

#include <kdialogbase.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kcolorbtn.h>
#include <kimageeffect.h>
#include <kmessagebox.h>
#include <kwin.h>
#include <kapp.h>

#include <gobject.h>
#include <graphitefactory.h>
#include <graphiteview.h>

#include <kdebug.h>


GObjectM9r::GObjectM9r(GObject *object, const Mode &mode, GraphitePart *part,
                       GraphiteView *view, const QString &type) :
    KDialogBase(KDialogBase::IconList, i18n("Change Properties"),
                KDialogBase::Ok|KDialogBase::Apply|KDialogBase::Cancel,
                KDialogBase::Ok, view->canvas(), "property dia", true, true),
    m_object(object), m_mode(mode), first_call(true), m_part(part),
    m_pressed(false), m_changed(false), m_created(false), m_type(type),
    m_line(0L), m_view(view) {

    m_handles=new QList<QRect>;
    m_handles->setAutoDelete(true);
}

GObjectM9r::~GObjectM9r() {

    m_handles->clear();
    delete m_handles;
}

void GObjectM9r::slotChanged(const QString &) {

    enableButtonApply(true);
    m_changed=true;
}

void GObjectM9r::slotChanged(int) {

    enableButtonApply(true);
    m_changed=true;
}

void GObjectM9r::slotChanged(const QColor &) {

    enableButtonApply(true);
    m_changed=true;
}

void GObjectM9r::slotOk() {

    slotApply();
    KDialogBase::slotOk();
}

void GObjectM9r::slotApply() {

    m_object->setName(m_line->text());
    m_changed=false;
    enableButtonApply(false);
    KDialogBase::slotApply();
}

void GObjectM9r::slotCancel() {

    if(!m_changed) {
        KDialogBase::slotCancel();
        return;
    }
    if(KMessageBox::warningContinueCancel(this,
                                          i18n("You'll loose the last changes you made!"),
                                          i18n("Really Cancel?"),
                                          i18n("Continue"))==KMessageBox::Continue)
        KDialogBase::slotCancel();
}

void GObjectM9r::createPropertyDialog() {

    if(m_created)
        return;

    // This should set the app icon for the dialog, too... broken?
    KWin kwin;
    kwin.setIcons(this->winId(), kapp->icon(), kapp->miniIcon());
    m_created=true;
    enableButtonOK(true);
    enableButtonApply(false);
    enableButtonCancel(true);

    // Add an information frame
    QFrame *information=addPage(i18n("Information"), i18n("Information about the Object"),
                                BarIcon(QString::fromLatin1("exec"), 32, KIcon::DefaultState, GraphiteFactory::global()));
    QGridLayout *grid=new QGridLayout(information, 9, 5, KDialogBase::marginHint(), KDialogBase::spacingHint());

    QLabel *label=new QLabel(i18n("Name:"), information);
    grid->addWidget(label, 0, 0);
    m_line=new QLineEdit(information);
    m_line->setText(m_object->name());
    connect(m_line, SIGNAL(textChanged(const QString &)),
            this, SLOT(slotChanged(const QString &)));
    grid->addWidget(m_line, 0, 2);

    label=new QLabel(i18n("Type:"), information);
    grid->addWidget(label, 1, 0);
    label=new QLabel(m_type, information);
    grid->addWidget(label, 1, 2);

    label=new QLabel(i18n("Origin:"), information);
    grid->addWidget(label, 2, 0);
    label=new QLabel(i18n("x=%1, y=%2").arg(m_object->origin().x()).
                     arg(m_object->origin().y()), information);
    grid->addWidget(label, 2, 2);

    label=new QLabel(i18n("Angle:"), information);
    grid->addWidget(label, 3, 0);
    QString rad=QString::number(m_object->angle());
    QString deg=QString::number(Graphite::rad2deg(m_object->angle()));
    label=new QLabel(i18n("%1 rad (%2 degrees)").arg(rad).arg(deg), information);
    grid->addWidget(label, 3, 2);

    grid->setRowStretch(4, 1);

    label=new QLabel(i18n("Bounding Rectangle:"), information);
    grid->addMultiCellWidget(label, 5, 5, 0, 3);

    label=new QLabel(i18n("Top-Left:"), information);
    grid->addWidget(label, 6, 0);

    label=new QLabel(i18n("x=%1, y=%2").arg(m_object->boundingRect().x()).
                     arg(m_object->boundingRect().y()), information);
    grid->addWidget(label, 6, 2);

    label=new QLabel(i18n("Bottom-Right:"), information);
    grid->addWidget(label, 7, 0);

    label=new QLabel(i18n("x=%1, y=%2").arg(m_object->boundingRect().bottomRight().x()).
                     arg(m_object->boundingRect().bottomRight().y()), information);
    grid->addWidget(label, 7, 2);
    grid->setColStretch(1, 1);
    grid->setColStretch(3, 5);
    grid->setRowStretch(8, 5);
}


void G1DObjectM9r::slotApply() {

    m_object->setPen(QPen(m_color->color(),
                          m_width->value(),
                          static_cast<Qt::PenStyle>(m_style->currentItem())));
    GObjectM9r::slotApply();
}

void G1DObjectM9r::createPropertyDialog() {

    if(m_created)
        return;

    GObjectM9r::createPropertyDialog();

    // Add a pen property page
    QFrame *frame=addPage(i18n("Pen"), i18n("Pen Settings"),
                          BarIcon(QString::fromLatin1("exec"), 32, KIcon::DefaultState, GraphiteFactory::global()));

    QGridLayout *grid=new QGridLayout(frame, 4, 4, KDialog::marginHint(), KDialog::spacingHint());

    QLabel *label=new QLabel(i18n("Width:"), frame);
    grid->addWidget(label, 0, 0);

    m_width=new QSpinBox(1, 100, 1, frame);
    m_width->setValue(m_object->pen().width());
    connect(m_width, SIGNAL(valueChanged(int)), this, SLOT(slotChanged(int)));
    grid->addWidget(m_width, 0, 2);

    label=new QLabel(i18n("Color:"), frame);
    grid->addWidget(label, 1, 0);

    m_color=new KColorButton(m_object->pen().color(), frame);
    connect(m_color, SIGNAL(changed(const QColor &)), this,
            SLOT(slotChanged(const QColor &)));
    grid->addWidget(m_color, 1, 2);

    label=new QLabel(i18n("Style:"), frame);
    grid->addWidget(label, 2, 0);

    m_style=new QComboBox(frame);
    QPainter painter;
    QPixmap *pm;
    for(int i=0; i<6; ++i) {
        pm=new QPixmap(70, 15);
        pm->fill();
        painter.begin(pm);
        painter.setPen(QPen(QColor(Qt::black),
                            static_cast<unsigned int>(2),
                            static_cast<Qt::PenStyle>(i)));
        painter.drawLine(0, 7, 70, 7);
        painter.end();
        m_style->insertItem(*pm);
    }
    connect(m_style, SIGNAL(activated(int)), this, SLOT(slotChanged(int)));
    m_style->setCurrentItem(m_object->pen().style());
    grid->addWidget(m_style, 2, 2);
    grid->setColStretch(1, 1);
    grid->setColStretch(3, 10);
    grid->setRowStretch(3, 1);
}


void G2DObjectM9r::slotChanged(int x) {

    updatePage();
    GObjectM9r::slotChanged(x);
}

void G2DObjectM9r::slotChanged(const QColor &x) {

    updatePage();
    GObjectM9r::slotChanged(x);
}

void G2DObjectM9r::slotApply() {

    int id=m_style->id(m_style->selected());

    if(id==0) {
        m_object->setBrush(QBrush());
        m_object->setFillStyle(GObject::Brush);
    }
    else if(id==1) {
        m_object->setBrush(QBrush(m_brushColor->color(),
                                  static_cast<Qt::BrushStyle>
                                  (m_brushStyle->currentItem()+1)));
        m_object->setFillStyle(GObject::Brush);
    }
    else {
        Gradient g;
        g.ca=m_gradientCA->color();
        g.cb=m_gradientCB->color();
        g.type=static_cast<KImageEffect::GradientType>
               (m_gradientStyle->currentItem());
        g.xfactor=m_xfactor->value();
        g.yfactor=m_yfactor->value();
        m_object->setGradient(g);
        m_object->setFillStyle(GObject::GradientFilled);
    }
    G1DObjectM9r::slotApply();
}

void G2DObjectM9r::resizeEvent(QResizeEvent *e) {

    updatePage();
    KDialogBase::resizeEvent(e);
}

void G2DObjectM9r::createPropertyDialog() {

    if(m_created)
        return;

    G1DObjectM9r::createPropertyDialog();

    // Wow - the fill style page :)
    QFrame *fill=addPage(i18n("Fill Style"), i18n("Fill Style Settings"),
                         BarIcon(QString::fromLatin1("exec"), 32, KIcon::DefaultState, GraphiteFactory::global()));
    QBoxLayout *mainbox=new QHBoxLayout(fill, KDialog::marginHint(), KDialog::spacingHint());
    QBoxLayout *leftbox=new QVBoxLayout(mainbox, 0, 0);
    mainbox->setStretchFactor(leftbox, 1);

    m_style=new QVButtonGroup(i18n("Fill Style:"), fill);
    QRadioButton *r=new QRadioButton(i18n("None"), m_style);
    r=new QRadioButton(i18n("Brush"), m_style);
    r=new QRadioButton(i18n("Gradient"), m_style);
    leftbox->addWidget(m_style);
    connect(m_style, SIGNAL(clicked(int)),
            this, SLOT(slotChanged(int)));
    if(m_object->brush().style()==Qt::NoBrush)
        m_style->setButton(0);
    else
        m_style->setButton(static_cast<int>(m_object->fillStyle())+1);
    m_style->setExclusive(true);

    QVGroupBox *previewbox=new QVGroupBox(i18n("Preview:"), fill);
    m_preview=new PWidget(previewbox, this);
    m_preview->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    leftbox->addWidget(previewbox);
    leftbox->setStretchFactor(previewbox, 1);

    QBoxLayout *rightbox=new QVBoxLayout(mainbox);
    rightbox->addSpacing(previewbox->fontMetrics().height()/2);
    m_stack=new QWidgetStack(fill);
    m_stack->setFrameStyle(QFrame::Box | QFrame::Sunken);
    rightbox->addWidget(m_stack);

    mainbox->setStretchFactor(rightbox, 1);

    // none
    QWidget *widget=new QWidget(m_stack);
    QGridLayout *grid=new QGridLayout(widget, 1, 1, KDialog::marginHint(), KDialog::spacingHint());
    QLabel *label=new QLabel(i18n("Not configurable."), widget);
    grid->addWidget(label, 0, 0);
    m_stack->addWidget(widget, 0);

    // brush
    widget=new QWidget(m_stack);
    grid=new QGridLayout(widget, 5, 4, KDialog::marginHint(), KDialog::spacingHint());
    label=new QLabel(i18n("Color:"), widget);
    grid->addWidget(label, 1, 0);
    m_brushColor=new KColorButton(m_object->brush().color(), widget);
    connect(m_brushColor, SIGNAL(changed(const QColor &)), this,
            SLOT(slotChanged(const QColor &)));
    grid->addWidget(m_brushColor, 1, 2);
    label=new QLabel(i18n("Style:"), widget);
    grid->addWidget(label, 3, 0);
    m_brushStyle=new QComboBox(widget);
    QStringList content;
    // Note: index+1 => fill style (NoBrush is missing!)
    content << i18n("100% filled (solid)")
            << i18n("94% filled")
            << i18n("88% filled")
            << i18n("63% filled")
            << i18n("50% filled")
            << i18n("37% filled")
            << i18n("12% filled")
            << i18n("6% filled")
            << i18n("Horizontal Lines")
            << i18n("Vertical Lines")
            << i18n("Crossing Lines")
            << i18n("Diagonal Lines (/)")
            << i18n("Diagonal Lines (\\)")
            << i18n("Diagonal Crossing Lines");
    m_brushStyle->insertStringList(content);
    int current=static_cast<int>(m_object->brush().style());
    if(current==0) ++current;
    m_brushStyle->setCurrentItem(current-1);
    connect(m_brushStyle, SIGNAL(activated(int)),
            this, SLOT(slotChanged(int)));
    grid->addWidget(m_brushStyle, 3, 2);
    grid->setRowStretch(0, 1);
    grid->setRowStretch(2, 1);
    grid->setRowStretch(4, 10);
    grid->setColStretch(1, 1);
    grid->setColStretch(3, 10);
    m_stack->addWidget(widget, 1);

    // gradient
    widget=new QWidget(m_stack);
    QBoxLayout *wbox=new QVBoxLayout(widget, KDialog::marginHint(), KDialog::spacingHint());
    wbox->addStretch(1);
    grid=new QGridLayout(wbox, 3, 4, KDialog::spacingHint());

    label=new QLabel(i18n("Colors:"), widget);
    grid->addWidget(label, 0, 0);
    m_gradientCA=new KColorButton(m_object->gradient().ca, widget);
    connect(m_gradientCA, SIGNAL(changed(const QColor &)), this,
            SLOT(slotChanged(const QColor &)));
    grid->addWidget(m_gradientCA, 0, 2);
    m_gradientCB=new KColorButton(m_object->gradient().cb, widget);
    connect(m_gradientCB, SIGNAL(changed(const QColor &)), this,
            SLOT(slotChanged(const QColor &)));
    grid->addWidget(m_gradientCB, 1, 2);
    label=new QLabel(i18n("Style:"), widget);
    grid->addWidget(label, 2, 0);
    m_gradientStyle=new QComboBox(widget);
    content.clear();
    content << i18n("Vertical")
            << i18n("Horizontal")
            << i18n("Diagonal (/)")
            << i18n("Diagonal (\\)")
            << i18n("Pyramid")
            << i18n("Rectangle")
            << i18n("PipeCross")
            << i18n("Elliptical");
    m_gradientStyle->insertStringList(content);
    m_gradientStyle->setCurrentItem(static_cast<int>(m_object->gradient().type));
    connect(m_gradientStyle, SIGNAL(activated(int)),
            this, SLOT(slotChanged(int)));
    grid->addWidget(m_gradientStyle, 2, 2);
    grid->setColStretch(1, 1);
    grid->setColStretch(3, 10);

    wbox->addStretch(10);
    m_unbalanced=new QCheckBox(i18n("Unbalanced Gradient"), widget);
    m_unbalanced->setChecked(false);
    wbox->addWidget(m_unbalanced);
    connect(m_unbalanced, SIGNAL(clicked()), this,
            SLOT(slotBalance()));

    wbox->addStretch(1);
    QGridLayout *factorgrid=new QGridLayout(wbox, 2, 2, KDialog::spacingHint());
    label=new QLabel(i18n("X-Factor:"), widget);
    factorgrid->addWidget(label, 0, 0);
    m_xfactor=new QSlider(-200, 200, 10, m_object->gradient().xfactor,
                          Qt::Horizontal, widget);
    connect(m_xfactor, SIGNAL(valueChanged(int)),
            this, SLOT(slotChanged(int)));
    m_xfactor->setEnabled(false);
    factorgrid->addWidget(m_xfactor, 0, 1);

    label=new QLabel(i18n("Y-Factor:"), widget);
    factorgrid->addWidget(label, 1, 0);
    m_yfactor=new QSlider(-200, 200, 10, m_object->gradient().yfactor,
                          Qt::Horizontal, widget);
    connect(m_yfactor, SIGNAL(valueChanged(int)),
            this, SLOT(slotChanged(int)));
    m_yfactor->setEnabled(false);
    factorgrid->addWidget(m_yfactor, 1, 1);
    m_stack->addWidget(widget, 2);
    wbox->addStretch(1);

    int id;
    if(m_object->brush().style()==Qt::NoBrush)
        id=0;
    else
        id=static_cast<int>(m_object->fillStyle())+1;

    m_stack->raiseWidget(id);
    updatePreview(id); // inititalize the pixmap
}

void G2DObjectM9r::slotBalance() {

    if(m_unbalanced->isChecked()) {
        m_xfactor->setEnabled(true);
        m_yfactor->setEnabled(true);
    }
    else {
        m_xfactor->setEnabled(false);
        m_yfactor->setEnabled(false);
    }
    updatePreview(2);
}

void G2DObjectM9r::updatePage() {

    int b=m_style->id(m_style->selected());
    m_stack->raiseWidget(b);
    updatePreview(b);
}

void G2DObjectM9r::updatePreview(int btn) {

    QPainter painter;
    QPixmap *pm=new QPixmap(m_preview->size());

    pm->fill();
    if(btn==1) {
        painter.begin(pm);
        painter.fillRect(QRect(QPoint(0, 0), m_preview->size()),
                         QBrush(m_brushColor->color(),
                                static_cast<Qt::BrushStyle>(m_brushStyle->currentItem()+1)));
        painter.end();
    }
    else if(btn==2) {
        painter.begin(pm);
        if(!m_unbalanced->isChecked()) {
            painter.drawImage(QPoint(0, 0),
                              KImageEffect::gradient(m_preview->size(),
                                                     m_gradientCA->color(),
                                                     m_gradientCB->color(),
                                                     static_cast<KImageEffect::GradientType>
                                                     (m_gradientStyle->currentItem())));
        }
        else {
            painter.drawImage(QPoint(0, 0),
                              KImageEffect::unbalancedGradient(m_preview->size(),
                                                               m_gradientCA->color(),
                                                               m_gradientCB->color(),
                                                               static_cast<KImageEffect::GradientType>
                                                               (m_gradientStyle->currentItem()),
                                                               m_xfactor->value(),
                                                               m_yfactor->value()));
        }
        painter.end();
    }
    m_preview->setBackgroundPixmap(*pm);
}


QDomElement GObject::save(QDomDocument &doc) const {

    static const QString &tagObject=KGlobal::staticQString("gobject");
    static const QString &attrName=KGlobal::staticQString("name");
    static const QString &attrState=KGlobal::staticQString("state");
    static const QString &attrAngle=KGlobal::staticQString("angle");
    static const QString &tagFormat=KGlobal::staticQString("format");
    static const QString &attrFillStyle=KGlobal::staticQString("fillStyle");
    static const QString &attrBrushStyle=KGlobal::staticQString("brushStyle");
    static const QString &attrBrushColor=KGlobal::staticQString("brushColor");
    static const QString &tagGradient=KGlobal::staticQString("gradient");
    static const QString &attrColorA=KGlobal::staticQString("colorA");
    static const QString &attrColorB=KGlobal::staticQString("colorB");
    static const QString &attrType=KGlobal::staticQString("type");
    static const QString &attrXFactor=KGlobal::staticQString("xfactor");
    static const QString &attrYFactor=KGlobal::staticQString("yfactor");
    static const QString &tagPen=KGlobal::staticQString("pen");

    // A GObject is saved to a node which is stored inside
    // its parent. That way the "tree" can be built up on
    // loading, again... Each object saves its data and
    // calls the save method of its superclass (e.g. GLine
    // would call GObject). The returned DOM Element has
    // to be saved. On loading, this Element is passed to
    // the XML CTOR of the superclass :)
    QDomElement e=doc.createElement(tagObject);
    e.setAttribute(attrName, m_name);
    e.setAttribute(attrState, m_state);
    e.setAttribute(attrAngle, m_angle);
    QDomElement format=doc.createElement(tagFormat);
    format.setAttribute(attrFillStyle, m_fillStyle);
    format.setAttribute(attrBrushStyle, m_brush.style());
    format.setAttribute(attrBrushColor, m_brush.color().name());
    QDomElement gradient=doc.createElement(tagGradient);
    gradient.setAttribute(attrColorA, m_gradient.ca.name());
    gradient.setAttribute(attrColorB, m_gradient.cb.name());
    gradient.setAttribute(attrType, m_gradient.type);
    gradient.setAttribute(attrXFactor, m_gradient.xfactor);
    gradient.setAttribute(attrYFactor, m_gradient.yfactor);
    format.appendChild(gradient);
    format.appendChild(GraphiteGlobal::self()->
                       createElement(tagPen, m_pen, doc));
    e.appendChild(format);
    return e;
}

void GObject::drawHandles(QPainter &p, QList<QRect> *handles) {

    p.save();
    p.setPen(Qt::black);
    p.setBrush(Qt::black);
    // TODO: Test RasterOp Not???

    int size;
    if(m_state==Handles)
        size=GraphiteGlobal::self()->handleSize();
    else if(m_state==Rot_Handles)
        size=GraphiteGlobal::self()->rotHandleSize();
    else
        return; // no need to draw handles - shouldn't happen

    int offset=GraphiteGlobal::self()->offset();

    QRect *lt=new QRect(boundingRect().left()-offset,
                        boundingRect().top()-offset, size, size);
    QRect *t=0L;
    QRect *rt=new QRect(boundingRect().right()-offset,
                        boundingRect().top()-offset, size, size);

    QRect *lb=new QRect(boundingRect().left()-offset,
                        boundingRect().bottom()-offset, size, size);
    QRect *b=0L;
    QRect *rb=new QRect(boundingRect().right()-offset,
                        boundingRect().bottom()-offset, size, size);

    QRect *l=0L;
    QRect *r=0L;

    if(boundingRect().width()>GraphiteGlobal::self()->thirdHandleTrigger()) {
        int dx=Graphite::double2Int(static_cast<double>(boundingRect().width())*0.5)-offset;
        t=new QRect(boundingRect().left()+dx, boundingRect().top(), size, size);
        b=new QRect(boundingRect().left()+dx, boundingRect().bottom(), size, size);
    }
    if(boundingRect().height()>GraphiteGlobal::self()->thirdHandleTrigger()) {
        int dy=Graphite::double2Int(static_cast<double>(boundingRect().height())*0.5)-offset;
        l=new QRect(boundingRect().left(), boundingRect().top()+dy, size, size);
        r=new QRect(boundingRect().right(), boundingRect().top()+dy, size, size);
    }

    if(m_state==Handles) {
        p.drawRect(*lt);
        p.drawRect(*rt);
        p.drawRect(*lb);
        p.drawRect(*rb);
        if(t)
            p.drawRect(*t);
        if(b)
            p.drawRect(*b);
        if(l)
            p.drawRect(*l);
        if(r)
            p.drawRect(*r);
    }
    else {
        p.drawEllipse(*lt);
        p.drawEllipse(*rt);
        p.drawEllipse(*lb);
        p.drawEllipse(*rb);
        if(t)
            p.drawEllipse(*t);
        if(b)
            p.drawEllipse(*b);
        if(l)
            p.drawEllipse(*l);
        if(r)
            p.drawEllipse(*r);
    }

    if(handles) {
        handles->clear();
        handles->append(lt);
        handles->append(rt);
        handles->append(lb);
        handles->append(rb);
        if(t)
            handles->append(t);
        if(b)
            handles->append(b);
        if(l)
            handles->append(l);
        if(r)
            handles->append(r);
    }
    else {
        delete lt;
        delete rt;
        delete lb;
        delete rb;
        delete t;
        delete b;
        delete l;
        delete r;
    }
    p.restore();
}

void GObject::setParent(GObject *parent) const {

    if(parent!=this)   // it's illegal to be oneselves parent! (parent==0L -> no parent :)
        m_parent=parent;
}

GObject::GObject(const QString &name) : m_name(name), m_state(Visible), m_parent(0L),
    m_angle(0.0), m_boundingRectDirty(true), m_fillStyle(Brush), m_ok(true) {

    m_gradient.type=KImageEffect::VerticalGradient;
    m_gradient.xfactor=1;
    m_gradient.yfactor=1;
}

GObject::GObject(const GObject &rhs) :  m_name(rhs.name()),
    m_state(rhs.state()), m_parent(0L), m_angle(rhs.angle()),
    m_boundingRectDirty(true), m_fillStyle(rhs.fillStyle()), m_brush(rhs.brush()),
    m_gradient(rhs.gradient()), m_pen(rhs.pen()), m_ok(true) {
}

GObject::GObject(const QDomElement &element) : m_parent(0L), m_boundingRectDirty(true),
                                               m_ok(false) {

    static const QString &tagObject=KGlobal::staticQString("gobject");
    static const QString &attrName=KGlobal::staticQString("name");
    static const QString &attrState=KGlobal::staticQString("state");
    static const QString &attrAngle=KGlobal::staticQString("angle");
    static const QString &tagFormat=KGlobal::staticQString("format");
    static const QString &attrFillStyle=KGlobal::staticQString("fillStyle");
    static const QString &attrBrushStyle=KGlobal::staticQString("brushStyle");
    static const QString &attrBrushColor=KGlobal::staticQString("brushColor");
    static const QString &tagGradient=KGlobal::staticQString("gradient");
    static const QString &attrColorA=KGlobal::staticQString("colorA");
    static const QString &attrColorB=KGlobal::staticQString("colorB");
    static const QString &attrType=KGlobal::staticQString("type");
    static const QString &attrXFactor=KGlobal::staticQString("xfactor");
    static const QString &attrYFactor=KGlobal::staticQString("yfactor");
    static const QString &tagPen=KGlobal::staticQString("pen");

    if(element.tagName()!=tagObject )
        return;

    bool ok;

    if(element.hasAttribute(attrName))
        m_name=element.attribute(attrName);
    else
        m_name="no name";

    m_state=static_cast<State>(element.attribute(attrState).toInt(&ok));
    if(!ok || m_state==Handles || m_state==Rot_Handles)
        m_state=Visible;

    m_angle=element.attribute(attrAngle).toDouble(&ok);
    if(!ok)
        m_angle=0.0;

    QDomElement format=element.namedItem(tagFormat).toElement();
    if(!format.isNull()) {
        m_fillStyle=static_cast<FillStyle>(format.attribute(attrFillStyle).toInt(&ok));
        if(!ok)
            m_fillStyle=Brush;

        int tmp=format.attribute(attrBrushStyle).toInt(&ok);
        if(!ok)
            tmp=0;
        m_brush.setStyle(static_cast<QBrush::BrushStyle>(tmp));
        if(format.hasAttribute(attrBrushColor))
            m_brush.setColor(QColor(format.attribute(attrBrushColor)));

        QDomElement gradient=format.namedItem(tagGradient).toElement();
        if(!gradient.isNull()) {
            if(gradient.hasAttribute(attrColorA))
                m_gradient.ca=QColor(gradient.attribute(attrColorA));
            if(gradient.hasAttribute(attrColorB))
                m_gradient.cb=QColor(gradient.attribute(attrColorB));

            m_gradient.type=static_cast<KImageEffect::GradientType>(gradient.attribute(attrType).toInt(&ok));
            if(!ok)
                m_gradient.type=static_cast<KImageEffect::GradientType>(0);

            m_gradient.xfactor=gradient.attribute(attrXFactor).toInt(&ok);
            if(!ok)
                m_gradient.xfactor=1;
            m_gradient.yfactor=gradient.attribute(attrYFactor).toInt(&ok);
            if(!ok)
                m_gradient.yfactor=1;
        }
        else {
            m_gradient.type=KImageEffect::VerticalGradient;
            m_gradient.xfactor=1;
            m_gradient.yfactor=1;
        }

        QDomElement pen=format.namedItem(tagPen).toElement();
        if(!pen.isNull())
            m_pen=GraphiteGlobal::self()->toPen(pen);
    }
    else {
        m_fillStyle=Brush;
        m_gradient.type=KImageEffect::VerticalGradient;
        m_gradient.xfactor=1;
        m_gradient.yfactor=1;
    }
    m_ok=true;   // CTOR has been completed successfully :)
}

#include <gobject.moc>
