/* This file is part of the KDE project
   Copyright (C) 2000 Werner Trobin <wtrobin@mandrakesoft.com>

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
#include <qstringlist.h>

#include <kdialogbase.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kcolorbtn.h>
#include <kmessagebox.h>

#include <gobject.h>
#include <graphitefactory.h>


GObjectM9r::GObjectM9r(GObject *object, const Mode &mode, GraphitePart *part,
		       const QString &type) : QObject(), m_object(object),
					      m_mode(mode), first_call(true),
					      m_dialog(0L), m_part(part),
					      m_changed(false), m_type(type),
					      m_line(0L) {
    m_handles=new QList<QRect>;
    m_handles->setAutoDelete(true);
}

GObjectM9r::~GObjectM9r() {

    if(m_dialog!=0L) {
	delete m_dialog;
	m_dialog=0L;
    }
    m_handles->clear();
    delete m_handles;
}

void GObjectM9r::ok() {

    // Ok => Apply + Cancel :)
    if(m_changed)
	apply();
    cancel();
}

void GObjectM9r::apply() {

    m_object->setName(m_line->text());
    m_dialog->enableButtonApply(false);
    m_changed=false;
}

void GObjectM9r::cancel() {

    if(!m_changed) {
	m_dialog->close(); // hide()?
	return;
    }
    if(KMessageBox::warningContinueCancel(m_dialog,
					  i18n("Your changes will be lost!"),
					  i18n("Really Cancel?"),
					  i18n("&OK"))==KMessageBox::Continue) {
	m_dialog->close(); // hide()?
	return;
    }
    apply();
    m_dialog->close(); // hide()?
}

void GObjectM9r::slotChanged(const QString &) {
    m_dialog->enableButtonApply(true);
    m_changed=true;
}

void GObjectM9r::slotChanged(int) {
    m_dialog->enableButtonApply(true);
    m_changed=true;
}

void GObjectM9r::slotChanged(const QColor &) {
    m_dialog->enableButtonApply(true);
    m_changed=true;
}

KDialogBase *GObjectM9r::createPropertyDialog(QWidget *parent) {

    if(m_dialog!=0L)
	return m_dialog;

    // Create the plain dia
    m_dialog=new KDialogBase(KDialogBase::IconList,
			     i18n("Change Properties"),
			     KDialogBase::Ok|KDialogBase::Apply|KDialogBase::Cancel,
			     KDialogBase::Ok, parent, "property dia", true, true);

    m_dialog->enableButtonOK(true);
    m_dialog->enableButtonApply(false);
    m_dialog->enableButtonCancel(true);

    connect(m_dialog, SIGNAL(okClicked()), this, SLOT(ok()));
    connect(m_dialog, SIGNAL(applyClicked()), this, SLOT(apply()));
    connect(m_dialog, SIGNAL(cancelClicked()), this, SLOT(cancel()));

    // Add an information frame
    QFrame *information=m_dialog->addPage(i18n("Information"), i18n("Information about the Object"),
					  BarIcon("exec", 32, KIcon::DefaultState, GraphiteFactory::global()));
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
    label=new QLabel(i18n("%1 rad").arg(m_object->angle()), information);
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

    return m_dialog;
}


void G1DObjectM9r::apply() {
    GObjectM9r::apply();
    // TODO
}

void G1DObjectM9r::cancel() {
    GObjectM9r::cancel();
}

KDialogBase *G1DObjectM9r::createPropertyDialog(QWidget *parent) {

    if(m_dialog)
	return m_dialog;

    GObjectM9r::createPropertyDialog(parent);
    if(!m_dialog)
	return 0L;

    // Add a pen property page
    QFrame *frame=m_dialog->addPage(i18n("Pen"), i18n("Pen Settings"),
				    BarIcon("exec", 32, KIcon::DefaultState, GraphiteFactory::global()));

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

    return m_dialog;
}


void G2DObjectM9r::apply() {
    G1DObjectM9r::apply();
    // TODO
    // brush style: index+1! (nobrush ;)
}

void G2DObjectM9r::cancel() {
    G1DObjectM9r::cancel();
}

void G2DObjectM9r::slotChanged(int x) {

    updatePage();
    GObjectM9r::slotChanged(x);
}

void G2DObjectM9r::slotChanged(const QColor &x) {

    updatePage();
    GObjectM9r::slotChanged(x);
}

KDialogBase *G2DObjectM9r::createPropertyDialog(QWidget *parent) {

    if(m_dialog)
	return m_dialog;

    G1DObjectM9r::createPropertyDialog(parent);
    if(!m_dialog)
	return 0L;

    // Wow - the fill style page :)
    QFrame *fill=m_dialog->addPage(i18n("Fill Style"), i18n("Fill Style Settings"),
				   BarIcon("exec", 32, KIcon::DefaultState, GraphiteFactory::global()));
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
    m_preview=new QWidget(previewbox);
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
    grid=new QGridLayout(widget, 2, 2, KDialog::marginHint(), KDialog::spacingHint());
    label=new QLabel(i18n("Color:"), widget);
    grid->addWidget(label, 0, 0);
    m_brushColor=new KColorButton(m_object->brush().color(), widget);
    connect(m_brushColor, SIGNAL(changed(const QColor &)), this,
	    SLOT(slotChanged(const QColor &)));
    grid->addWidget(m_brushColor, 0, 1);
    label=new QLabel(i18n("Style:"), widget);
    grid->addWidget(label, 1, 0);
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
    m_brushStyle->setCurrentItem(static_cast<int>(m_object->brush().style())-1);
    connect(m_brushStyle, SIGNAL(activated(int)),
	    this, SLOT(slotChanged(int)));
    grid->addWidget(m_brushStyle, 1, 1);
    m_stack->addWidget(widget, 1);

    // gradient
    widget=new QWidget(m_stack);
    QBoxLayout *wbox=new QVBoxLayout(widget, KDialog::marginHint(), KDialog::spacingHint());
    grid=new QGridLayout(wbox, 3, 2, KDialog::spacingHint());

    label=new QLabel(i18n("Colors:"), widget);
    grid->addWidget(label, 0, 0);
    m_gradientCA=new KColorButton(m_object->gradient().ca, widget);
    connect(m_gradientCA, SIGNAL(changed(const QColor &)), this,
	    SLOT(slotChanged(const QColor &)));
    grid->addWidget(m_gradientCA, 0, 1);
    m_gradientCB=new KColorButton(m_object->gradient().cb, widget);
    connect(m_gradientCB, SIGNAL(changed(const QColor &)), this,
	    SLOT(slotChanged(const QColor &)));
    grid->addWidget(m_gradientCB, 1, 1);
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
	    << i18n("Elliptic");
    m_gradientStyle->insertStringList(content);
    m_gradientStyle->setCurrentItem(static_cast<int>(m_object->gradient().type));
    connect(m_gradientStyle, SIGNAL(activated(int)),
	    this, SLOT(slotChanged(int)));
    grid->addWidget(m_gradientStyle, 2, 1);

    m_unbalanced=new QCheckBox(i18n("Unbalanced Gradient"), widget);
    m_unbalanced->setChecked(false);
    wbox->addWidget(m_unbalanced);
    connect(m_unbalanced, SIGNAL(clicked()), this,
	    SLOT(slotBalance()));

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

    if(m_object->brush().style()==Qt::NoBrush)
	m_stack->raiseWidget(2);
    else
	m_stack->raiseWidget(2); //static_cast<int>(m_object->fillStyle())+1);

    updatePreview(); // inititalize the pixmap

    return m_dialog;
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
    updatePreview();
}

void G2DObjectM9r::updatePage() {
    // TODO
}

void G2DObjectM9r::updatePreview() {
    // TODO
}


QDomElement GObject::save(QDomDocument &doc) const {

    // A GObject is saved to a node which is stored inside
    // its parent. That way the "tree" can be built up on
    // loading, again... Each object saves its data and
    // calls the save method of its superclass (e.g. GLine
    // would call GObject). The returned DOM Element has
    // to be saved. On loading, this Element is passed to
    // the XML CTOR of the superclass :)
    QDomElement e=doc.createElement("gobject");
    e.setAttribute("name", m_name);
    e.setAttribute("state", m_state);
    e.setAttribute("angle", m_angle);
    QDomElement format=doc.createElement("format");
    format.setAttribute("fillStyle", m_fillStyle);
    format.setAttribute("brushStyle", m_brush.style());
    format.setAttribute("brushColor", m_brush.color().name());
    QDomElement gradient=doc.createElement("gradient");
    gradient.setAttribute("colorA", m_gradient.ca.name());
    gradient.setAttribute("colorB", m_gradient.cb.name());
    gradient.setAttribute("type", m_gradient.type);
    gradient.setAttribute("xfactor", m_gradient.xfactor);
    gradient.setAttribute("yfactor", m_gradient.yfactor);
    gradient.setAttribute("ncols", m_gradient.ncols);
    format.appendChild(gradient);
    format.appendChild(doc.createElement("pen", m_pen));
    e.appendChild(format);		
    return e;
}

void GObject::setParent(GObject *parent) const {

    if(parent!=this)   // it's illegal to be oneselves parent! (parent==0L -> no parent :)
	m_parent=parent;
}

GObject::GObject(const QString &name) : m_name(name), m_state(Visible), m_parent(0L),
    m_zoom(100), m_angle(0.0), m_boundingRectDirty(true), m_fillStyle(Brush), m_ok(true) {

    m_gradient.type=KImageEffect::VerticalGradient;
    m_gradient.xfactor=1;
    m_gradient.yfactor=1;
    m_gradient.ncols=1;
}

GObject::GObject(const GObject &rhs) :  m_name(rhs.name()),
    m_state(rhs.state()), m_parent(0L), m_zoom(rhs.zoom()), m_angle(rhs.angle()),
    m_boundingRectDirty(true), m_fillStyle(rhs.fillStyle()), m_brush(rhs.brush()),
    m_gradient(rhs.gradient()), m_pen(rhs.pen()), m_ok(true) {
}

GObject::GObject(const QDomElement &element) : m_parent(0L), m_zoom(100),
					       m_boundingRectDirty(true), m_ok(false) {

    if(element.tagName()!="gobject")
	return;

    bool ok;
    static QString attrName=QString::fromLatin1("name");
    static QString attrState=QString::fromLatin1("state");
    static QString attrAngle=QString::fromLatin1("angle");
    static QString tagFormat=QString::fromLatin1("format");
    static QString attrFillStyle=QString::fromLatin1("fillStyle");
    static QString attrBrushStyle=QString::fromLatin1("brushStyle");
    static QString attrBrushColor=QString::fromLatin1("brushColor");
    static QString tagGradient=QString::fromLatin1("gradient");
    static QString attrColorA=QString::fromLatin1("colorA");
    static QString attrColorB=QString::fromLatin1("colorB");
    static QString attrType=QString::fromLatin1("type");
    static QString attrXFactor=QString::fromLatin1("xfactor");
    static QString attrYFactor=QString::fromLatin1("yfactor");
    static QString attrNCols=QString::fromLatin1("ncols");
    static QString tagPen=QString::fromLatin1("pen");

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
	    m_gradient.ncols=gradient.attribute(attrNCols).toInt(&ok);
	    if(!ok)
		m_gradient.ncols=1;	
	}
	else {
	    m_gradient.type=KImageEffect::VerticalGradient;
	    m_gradient.xfactor=1;
	    m_gradient.yfactor=1;
	    m_gradient.ncols=1;
	}
	
	QDomElement pen=format.namedItem(tagPen).toElement();
	if(!pen.isNull())
	    m_pen=pen.toPen();	
    }
    else {
	m_fillStyle=Brush;
	m_gradient.type=KImageEffect::VerticalGradient;
	m_gradient.xfactor=1;
	m_gradient.yfactor=1;
	m_gradient.ncols=1;	
    }
    m_ok=true;   // CTOR has been successful :)
}
#include <gobject.moc>
