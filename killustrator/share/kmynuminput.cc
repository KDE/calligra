// -*- c-basic-offset: 4 -*-
/*
 * kmynuminput.cpp
 *
 * Initial implementation:
 *     Copyright (c) 1997 Patrick Dowler <dowler@morgul.fsh.uvic.ca>
 * Rewritten and maintained by:
 *     Copyright (c) 2000 Dirk A. Mueller <mueller@kde.org>
 *
 *  Requires the Qt widget libraries, available at no cost at
 *  http://www.troll.no/
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 */

#include <config.h>
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif
#include <math.h>

#include <qlabel.h>
#include <qlineedit.h>
#include <qsize.h>
#include <qslider.h>
#include <qspinbox.h>

#include <knumvalidator.h>
#include <kmynuminput.h>


// -----------------------------------------------------------------------------

KMyNumInput::KMyNumInput(QWidget* parent, const char* name)
    : QWidget(parent, name)
{
    init();
}


// -----------------------------------------------------------------------------

KMyNumInput::KMyNumInput(KMyNumInput* below, QWidget* parent, const char* name)
    : QWidget(parent, name)
{
    init();

    if(below) {
        m_next = below->m_next;
        m_prev = below;
        below->m_next = this;
        if(m_next)
            m_next->m_prev = this;
    }
}


// -----------------------------------------------------------------------------

void KMyNumInput::init()
{
    m_prev = m_next = 0;
    m_colw1 = m_colw2 = 0;

    m_label = 0;
    m_slider = 0;
    m_alignment = 0;
}


// -----------------------------------------------------------------------------

KMyNumInput::~KMyNumInput()
{
    if(m_prev)
        m_prev->m_next = m_next;

    if(m_next)
        m_next->m_prev = m_prev;
}


// -----------------------------------------------------------------------------

void KMyNumInput::setLabel(QString label, int a)
{
    if(label.isEmpty()) {
        delete m_label;
        m_label = 0;
        m_alignment = 0;
    }
    else {
        m_label = new QLabel(label, this, "KMyNumInput::QLabel");
        m_label->setAlignment((a & (~(AlignTop|AlignBottom|AlignVCenter))) | AlignVCenter);
        // if no vertical alignment set, use Top alignment
        if(!(a & (AlignTop|AlignBottom|AlignVCenter)))
           a |= AlignTop;
        m_alignment = a;
    }

    layout(true);
}


// -----------------------------------------------------------------------------

void KMyNumInput::layout(bool deep)
{
    int w1 = m_colw1;
    int w2 = m_colw2;

    // label sizeHint
    m_sizeLabel = (m_label ? m_label->sizeHint() : QSize(0,0));

    if(m_label && (m_alignment & AlignVCenter))
        m_colw1 = m_sizeLabel.width() + 4;
    else
        m_colw1 = 0;

    // slider sizeHint
    m_sizeSlider = (m_slider ? m_slider->sizeHint() : QSize(0, 0));

    doLayout();

    if(!deep) {
        m_colw1 = w1;
        m_colw2 = w2;
        return;
    }

    KMyNumInput* p = this;
    while(p) {
        p->doLayout();
        w1 = QMAX(w1, p->m_colw1);
        w2 = QMAX(w2, p->m_colw2);
        p = p->m_prev;
    }

    p = m_next;
    while(p) {
        p->doLayout();
        w1 = QMAX(w1, p->m_colw1);
        w2 = QMAX(w2, p->m_colw2);
        p = p->m_next;
    }

    p = this;
    while(p) {
        p->m_colw1 = w1;
        p->m_colw2 = w2;
        p = p->m_prev;
    }

    p = m_next;
    while(p) {
        p->m_colw1 = w1;
        p->m_colw2 = w2;
        p = p->m_next;
    }

//    kdDebug() << "w1 " << w1 << " w2 " << w2 << endl;
}


// -----------------------------------------------------------------------------

QSizePolicy KMyNumInput::sizePolicy() const
{
    return QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed );
}


// -----------------------------------------------------------------------------

QSize KMyNumInput::sizeHint() const
{
    return minimumSizeHint();
}


// -----------------------------------------------------------------------------

void KMyNumInput::setSteps(int minor, int major)
{
    if(m_slider)
        m_slider->setSteps( minor, major );
}

// -----------------------------------------------------------------------------

/**
 *
 * This is just a small wrapper mainly to enable mouseWheel support
 *
 */

class KMyDoubleLine : public QLineEdit
{
public:
    KMyDoubleLine(KMyDoubleNumInput* parent, const char* name)
        : QLineEdit(parent, name)
        { };

    void interpretText();
protected:
    virtual void wheelEvent(QWheelEvent* e);
};

void KMyDoubleLine::wheelEvent(QWheelEvent* e)
{
    KMyDoubleNumInput* w = static_cast<KMyDoubleNumInput*>(parent());

    if(edited())
        interpretText();

    if(w->m_range) {
        w->setValue(w->value() + (e->delta()/120)*w->m_step);
        e->accept();
    }
    else
        e->ignore();
}


void KMyDoubleLine::interpretText()
{
    KMyDoubleNumInput* w = static_cast<KMyDoubleNumInput*>(parent());

    QString s = QString(text()).stripWhiteSpace();
    if ( !w->m_prefix.isEmpty() ) {
        QString px = QString(w->m_prefix).stripWhiteSpace();
        int len = px.length();
        if ( len && s.left(len) == px )
            s.remove( 0, len );
    }
    if ( !w->m_suffix.isEmpty() ) {
        QString sx = QString(w->m_suffix).stripWhiteSpace();
        int len = sx.length();
        if ( len && s.right(len) == sx )
            s.truncate( s.length() - len );
    }

    s = s.stripWhiteSpace();

    if(edited()) {
        bool ok;
        double value = s.toDouble(&ok);
        if(ok) {
            w->m_value = value;
            setEdited(false);
        }

    }
}


// -----------------------------------------------------------------------------

KMyDoubleNumInput::KMyDoubleNumInput(QWidget *parent, const char *name)
    : KMyNumInput(parent, name)
{
    init(0.0);
}


KMyDoubleNumInput::KMyDoubleNumInput(double value, QWidget *parent, const char *name)
    : KMyNumInput(parent, name)
{
    init(value);
}


// -----------------------------------------------------------------------------

KMyDoubleNumInput::KMyDoubleNumInput(KMyNumInput* below, double value, QWidget* parent,
                                 const char* name)
    : KMyNumInput(below, parent, name)
{
    init(value);
}


// -----------------------------------------------------------------------------

KMyDoubleNumInput::~KMyDoubleNumInput()
{
    delete m_format;
}


// -----------------------------------------------------------------------------

void KMyDoubleNumInput::init(double value)
{
    m_value = value;
    m_format = qstrdup("%.2f");

    edit = new KMyDoubleLine(this, "KMyDoubleNumInput::QLineEdit");
    edit->setAlignment(AlignRight);
    edit->setValidator(new KFloatValidator(this,
                                                                                   "KMyDoubleNumInput::KFloatValidator"));
    edit->installEventFilter( this );
    //setFocusProxy(edit);

    m_suffix = m_prefix = "";
    layout(true);

    resetEditBox();
}


// -----------------------------------------------------------------------------

void KMyDoubleNumInput::sliderMoved(int val)
{
    m_value = m_lower + (double)val * (m_upper - m_lower)/m_slider->maxValue();

    resetEditBox();
    emit valueChanged(m_value);
}


// -----------------------------------------------------------------------------

QSize KMyDoubleNumInput::minimumSizeHint() const
{
    constPolish();

    int w = 0;
    int h = 0;

    // if in extra row, then count it here
    if(m_label && (m_alignment & (AlignBottom|AlignTop)))
        h += 4 + m_sizeLabel.height();
    else
        // no extra frame space
        h += m_sizeLabel.height();

    h += 2 + QMAX(m_sizeEdit.height(), m_sizeSlider.height());

    w += m_slider ? m_slider->sizeHint().width() + 8 : 0;
    w += m_colw1 + m_colw2;

    if(m_alignment & (AlignTop|AlignBottom))
        w = QMAX(w, m_sizeLabel.width() + 4);

    return QSize(w, h);
}


// -----------------------------------------------------------------------------

void KMyDoubleNumInput::resizeEvent(QResizeEvent* e)
{
    int w = m_colw1;
    int h = 0;

    if(m_label && (m_alignment & AlignTop)) {
        m_label->setGeometry(0, 0, e->size().width(), m_sizeLabel.height());
        h += m_sizeLabel.height() + 4;
    }

    if(m_label && (m_alignment & AlignVCenter))
        m_label->setGeometry(0, 0, w, m_sizeEdit.height());

    edit->setGeometry(w, h, m_slider ? m_colw2
                                          : e->size().width() - w, m_sizeEdit.height());
    w += m_colw2 + 8;

    if(m_slider)
        m_slider->setGeometry(w, h, e->size().width() - w, m_sizeEdit.height());

    h += m_sizeEdit.height() + 2;

    if(m_label && (m_alignment & AlignBottom))
        m_label->setGeometry(0, h, m_sizeLabel.width(), m_sizeLabel.height());
}


// -----------------------------------------------------------------------------

void KMyDoubleNumInput::doLayout()
{
    // edit sizeHint
    edit->constPolish();
    QFontMetrics fm( edit->font() );
    QString s;
    int h = fm.height();
    s.sprintf(m_format, m_value);
    int w = fm.width(m_prefix) + fm.width(s) + fm.width(m_suffix);
    w = QMAX(w, fm.width(m_specialvalue));
    if(m_range) {
        s.sprintf(m_format, m_lower);
        w = QMAX(w, fm.width(s));
        s.sprintf(m_format, m_upper);
        w = QMAX(w, fm.width(s));
        // something inbetween
        s.sprintf(m_format, m_lower + m_step);
        w = QMAX(w, fm.width(s));
    }

    if ( edit->frame() ) {
        h += 8;
        if ( edit->style().guiStyle() == WindowsStyle && h < 26 )
            h = 22;
        m_sizeEdit.setWidth(w + 8);
        m_sizeEdit.setHeight(h);
    } else {
        m_sizeEdit.setWidth(w + 4);
        m_sizeEdit.setHeight(h + 4);
    }

    m_colw2 = m_sizeEdit.width();
}


// -----------------------------------------------------------------------------

void KMyDoubleNumInput::setValue(double val)
{
    m_value = val;

    if(m_value < m_lower) m_value = m_lower;
    if(m_upper < m_value) m_value = m_upper;

    if(m_slider) {
        int slvalue = (int)(m_slider->maxValue()
                                                        * (m_value - m_lower)/(m_upper - m_lower));
        m_slider->setValue(slvalue);
    }

    resetEditBox();
}


// -----------------------------------------------------------------------------

void KMyDoubleNumInput::setRange(double lower, double upper, double step,
                                                           bool slider)
{
    m_lower = lower;
    m_upper = upper;
    m_step  = step;
    m_range = (m_lower < m_upper);

    // bounds checking the values
    if(m_value < m_lower)  m_value = m_lower;
    if(m_upper < m_lower)  m_upper = m_value;
    if(m_upper < m_value)  m_value = m_upper;

    // make m_value a multiple of step
    m_value = floor(m_value / m_step) * m_step;

    if(slider) {
                int slmax = QMIN(INT_MAX, (int)((m_upper - m_lower)/m_step));
        int slvalue = (int)(slmax * (m_value - m_lower) / (m_upper - m_lower));
                if (m_slider) {
                        m_slider->setRange(0, slmax);
                        m_slider->setValue(slvalue);
                } else {
                        m_slider = new QSlider(0, slmax, 1, slvalue,
                                                                   QSlider::Horizontal, this);
                        m_slider->setTickmarks(QSlider::Below);
                        connect(m_slider, SIGNAL(valueChanged(int)),
                                        SLOT(sliderMoved(int)));
                }
                m_slider->setTickInterval(slmax / 10);
    } else {
        delete m_slider;
        m_slider = 0;
    }

    resetEditBox();
    layout(true);
}


// -----------------------------------------------------------------------------

double  KMyDoubleNumInput::value(bool forceUpdate) const
{
    if(forceUpdate)
        edit->interpretText();
    return m_value;
}


QString KMyDoubleNumInput::suffix() const
{
    return m_suffix;
}


QString KMyDoubleNumInput::prefix() const
{
    return m_prefix;
}


const char *KMyDoubleNumInput::format() const
{
    return m_format;
}

// -----------------------------------------------------------------------------

void KMyDoubleNumInput::setSuffix(const QString &suffix)
{
    m_suffix = suffix;

    resetEditBox();
    layout(true);
}

// -----------------------------------------------------------------------------

void KMyDoubleNumInput::setPrefix(const QString &prefix)
{
    m_prefix = prefix;

    resetEditBox();
    layout(true);
}

// -----------------------------------------------------------------------------

void KMyDoubleNumInput::setFormat(const char* fmt)
{
    m_format = qstrdup(fmt);

    resetEditBox();
    layout(true);
}

// -----------------------------------------------------------------------------

void KMyDoubleNumInput::resetEditBox()
{
    if(!m_specialvalue.isEmpty() && (fabs(m_value - m_lower) < 1e-10))
        edit->setText(m_specialvalue);
    else {
        QString s;
        s.sprintf(m_format, m_value);
        edit->setText(m_prefix + s + m_suffix);
    }
}


// -----------------------------------------------------------------------------

void KMyDoubleNumInput::setSpecialValueText(const QString& text)
{
    m_specialvalue = text;

    resetEditBox();
    layout(true);
};


// -----------------------------------------------------------------------------

void KMyDoubleNumInput::setLabel(QString label, int a)
{
    KMyNumInput::setLabel(label, a);

    if(m_label)
        m_label->setBuddy(edit);

}

// -----------------------------------------------------------------------------

bool KMyDoubleNumInput::eventFilter( QObject* obj, QEvent* ev )
{
    if ( obj != edit )
        return false;

    bool revalue = false;
    double old_value = m_value;

    if (ev->type() == QEvent::FocusOut || ev->type() == QEvent::Leave || ev->type() == QEvent::Hide) {
        edit->interpretText();
        revalue = false;
    } else if ( ev->type() == QEvent::KeyPress ) {
        QKeyEvent* k = (QKeyEvent*)ev;
        if ( k->key() == Key_Up || k->text() == "+" ) {
            if(m_range)
                m_value += m_step;
            revalue = true;
        } else if ( k->key() == Key_Down || k->text() == "-" ) {
            if(m_range)
                m_value -= m_step;
            revalue = true;
        } else if ( k->key() == Key_Return ) {
            edit->interpretText();
            revalue = false;
        }
    }

    if(m_value != old_value) {
        setValue(m_value);
        emit valueChanged(m_value);
    }

    return revalue;
}


// -----------------------------------------------------------------------------

#include <kmynuminput.moc>
