//
// ui.h extension file, included from the uic-generated form implementation.
//
// If you wish to add, delete or rename functions or slots use
// Qt Designer which will update this file, preserving your code. Create an
// init() function in place of a constructor, and a destroy() function in
// place of a destructor.
//

/* This file is part of the KDE project
   Copyright (C) 2004 - 2006 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation;
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>

#include <cmath>

namespace KPlato
{

/**
 * This structure describes one of the fields shown.
 */
struct FieldDescriptor
{
    // Which field is to my left, and what conversion factor separates us?
    QLineEdit *left;
    double leftScale;
    
    // Which field am I, and how am I formatted?
    QLineEdit *current;
    const char *format;
    
    // Which field is to my right, and what conversion factor separates us?
    QLineEdit *right;
    double rightScale;
    
    // If I am hidden, who else hides with me?
    QLabel *separator;
    
    // Used for calculating a correct duration
    double fullScale;
    double scale;
    
    // Used for displaying a unit behind each field
    QLabel *unit;
};

#define setField(f, l, ls, c, fmt, r, rs, s, fs, sc, u) \
do \
{ \
    m_fields[f].left = l; \
    m_fields[f].leftScale = ls; \
    m_fields[f].current = c; \
    m_fields[f].format = fmt; \
    m_fields[f].right = r; \
    m_fields[f].rightScale = rs; \
    m_fields[f].separator = s; \
    m_fields[f].fullScale = fs; \
    m_fields[f].scale = sc; \
    m_fields[f].unit = u; \
} while (0)
    
void DurationWidget::init()
{
    // Use the user's decimal point!
    m_decimalPoint = KGlobal::locale()->decimalSymbol();
    
    //NOTE: 
    //      This isn't as flexible/general as Shaheed once made it.
    //      It's now a long list of hacks.
    //      Introducing double in scales and allowing leftscale/scale/rightscale
    //      *NOT* to be multiples of each other increases the complexity and also
    //      introduces rounding errors. (eg. hour = 60 minutes, day = 7,5 hours)
    //
    //      If you know how, please make a better solution!
    //
    // Any field can be entered as an integer or a floating point value. Whatever
    // is entered is treated as follows:
    //
    //    - any fractional part is moved right one field
    //
    //    - any overflow from the integer part is carried left one field
    //
    // and the process repeated until the rightmost and leftmost fields are reached.
    QRegExp re(QString("\\d{1,10}|\\d{1,7}\\") + m_decimalPoint + 
        QString("\\d{0,10}|\\d{0,7}\\") + m_decimalPoint + 
        QString("\\d{1,3}"));
    m_validator = new QRegExpValidator(re, this);
    m_ddd->setValidator(m_validator);
    m_hh->setValidator(m_validator);
    m_mm->setValidator(m_validator);
    m_ss->setValidator(m_validator);
    m_ms->setValidator(m_validator);
    
    m_ddUnit->hide();
    m_hhUnit->hide();
    m_mmUnit->hide();
    m_ssUnit->hide();
    m_msUnit->hide();
    
    m_fields = new FieldDescriptor[5];    
    setField(0, NULL, 0, m_ddd, "%u", m_hh, 24, m_hhSpace, 24, 24, m_ddUnit);
    setField(1, m_ddd, 24, m_hh, "%02u", m_mm, 60, m_mmColon, 60, 60, m_hhUnit);
    setField(2, m_hh, 60, m_mm, "%02u", m_ss, 60, NULL, 60, 60, m_mmUnit);
    setField(3, m_mm, 60, m_ss, "%02u", m_ms, 1000, m_ssColon, 60, 60, m_ssUnit);
    setField(4, m_ss, 1000, m_ms, "%03u", NULL, 0, m_dot, 1000, 1000, m_msUnit);
}

void DurationWidget::destroy()
{
    delete m_fields;
    //delete m_validator;  //QWidget takes care of this
}

Q_INT64 DurationWidget::setValueMilliseconds(Q_INT64 milliseconds)
{
    unsigned sc = (unsigned)m_fields[4].leftScale;
    Q_INT64 secs = milliseconds / sc;
    Q_INT64 ms = milliseconds % sc;
    QString tmp;
    tmp.sprintf(m_fields[4].format, ms);
    m_fields[4].current->setText(tmp);
    return secs;
}

Q_INT64 DurationWidget::setValueSeconds(Q_INT64 seconds)
{
    unsigned sc = (unsigned)m_fields[3].leftScale;
    Q_INT64 mins = seconds / sc;
    Q_INT64 s = seconds % sc;
    QString tmp;
    tmp.sprintf(m_fields[3].format, s);
    m_fields[3].current->setText(tmp);
    return mins;
}

Q_INT64 DurationWidget::setValueMinutes(Q_INT64 mins)
{
    unsigned sc = (unsigned)m_fields[2].leftScale;
    Q_INT64 hours = mins / sc;
    Q_INT64 m = mins % sc;
    QString tmp;
    tmp.sprintf(m_fields[2].format, m);
    m_fields[2].current->setText(tmp);
    return hours;
}

// NOTE: Input is minutes and also returns minutes!
Q_INT64 DurationWidget::setValueHours(Q_INT64 mins)
{
    if (m_fields[1].current->isHidden())
        return mins;
    unsigned sc = (unsigned)m_fields[1].rightScale;
    Q_INT64 hours = (Q_INT64)(mins / sc);
    Q_INT64 m = mins - (Q_INT64)(hours * sc);
    //kdDebug()<<k_funcinfo<<"mins="<<mins<<" -> hours="<<hours<<" rem="<<m<<endl;
    QString tmp;
    tmp.sprintf(m_fields[1].format, hours);
    m_fields[1].current->setText(tmp);
    return m;
}

// NOTE: Input is minutes and also returns minutes!
Q_INT64 DurationWidget::setValueDays(Q_INT64 mins)
{
    if (m_fields[0].current->isHidden())
        return mins;
    double sc = m_fields[1].rightScale * m_fields[0].rightScale;
    Q_INT64 days = (Q_INT64)(mins / sc);
    Q_INT64 m = mins - (Q_INT64)(days * sc);
    //kdDebug()<<k_funcinfo<<"mins="<<mins<<" -> days="<<days<<" rem="<<m<<endl;
    QString tmp;
    tmp.sprintf(m_fields[0].format, days);
    m_fields[0].current->setText(tmp);
    return m;
}

void DurationWidget::setValue(const KPlato::Duration &newDuration)
{
    Q_INT64 value = newDuration.milliseconds();
    //kdDebug()<<k_funcinfo<<f<<": value="<<value<<endl;
    value = setValueMilliseconds(value); // returns seconds
    value = setValueSeconds(value); // returns minutes
    // Now call days first to allow for fractional scales
    value = setValueDays(value); // NOTE  returns minutes
    value = setValueHours(value); // NOTE  returns minutes
    value = setValueMinutes(value); // returns hours: Should be 0
    if (value > 0) kdError()<<k_funcinfo<<"Remainder > 0: "<<value<<endl;
    
    emit valueChanged();
}

Duration DurationWidget::value() const
{
    Duration d;
    int i=0;
    if (!m_fields[i].current->isHidden() &&
        m_fields[i].scale > 0 &&
        m_fields[i].scale <= m_fields[i].fullScale)
    {
        double v = m_fields[i].current->text().toDouble();
        v = v * m_fields[i].scale / m_fields[i].fullScale;;
        d.addMilliseconds((Q_INT64)(v*(1000*60*60*24)));
    }
    ++i;
    if (!m_fields[i].current->isHidden() &&
        m_fields[i].scale > 0 &&
        m_fields[i].scale <= m_fields[i].fullScale)
    {
        double v = m_fields[i].current->text().toDouble();
        v = v * m_fields[i].scale / m_fields[i].fullScale;;
        d.addMilliseconds((Q_INT64)(v*(1000*60*60)));
    }
    ++i;
    if (!m_fields[i].current->isHidden() &&
        m_fields[i].scale > 0 &&
        m_fields[i].scale <= m_fields[i].fullScale)
    {
        double v = m_fields[i].current->text().toDouble();
        v = v * m_fields[i].scale / m_fields[i].fullScale;;
        d.addMilliseconds((Q_INT64)(v*(1000*60)));
    }
    ++i;
    if (!m_fields[i].current->isHidden() &&
        m_fields[i].scale > 0 &&
        m_fields[i].scale <= m_fields[i].fullScale)
    {
        double v = m_fields[i].current->text().toDouble();
        v = v * m_fields[i].scale / m_fields[i].fullScale;;
        d.addMilliseconds((Q_INT64)(v*(1000)));
    }
    ++i;
    if (!m_fields[i].current->isHidden())
    {
        Q_INT64 v = m_fields[i].current->text().toUInt();
        d.addMilliseconds(v);
    }
    return d;
}

void DurationWidget::dddLostFocus()
{
    handleLostFocus(0);
    emit valueChanged();
}

void DurationWidget::hhLostFocus( )
{
    handleLostFocus(1);
    emit valueChanged();
}

void DurationWidget::mmLostFocus()
{
    handleLostFocus(2);
    emit valueChanged();
}

void DurationWidget::ssLostFocus()
{
    handleLostFocus(3);
    emit valueChanged();
}

void DurationWidget::msLostFocus()
{
    handleLostFocus(4);
    emit valueChanged();
}

void DurationWidget::handleLostFocus(
    int field)
{
    // Get our own info, and that of our left and right neighbours.
    QLineEdit *left = m_fields[field].left;
    double leftScale = m_fields[field].leftScale; 
    const char *leftFormat = left ? m_fields[field - 1].format : NULL; 
    QLineEdit *current = m_fields[field].current; 
    const char *currentFormat = m_fields[field].format;
    QLineEdit *right = m_fields[field].right;
    double rightScale = m_fields[field].rightScale; 
    const char *rightFormat = right ? m_fields[field + 1].format : NULL;
    
    // avoid possible crash
    if (leftScale == 0)
        leftScale = 1;
        
    // Get the text and start processing...
    QString newValue(current->text());
    double v = KGlobal::locale()->readNumber(newValue);
    unsigned currentValue = 0;
    QString tmp;
    //kdDebug()<<k_funcinfo<<field<<": value="<<v<<" v="<<v<<endl;
    if (left && v >= leftScale)
    {
        //kdDebug()<<k_funcinfo<<field<<": value="<<v<<" leftScale="<<leftScale<<endl;
        // Carry overflow, recurse as required.
        tmp.sprintf(leftFormat, (unsigned)(v / leftScale));
        left->setText(tmp);
        handleLostFocus(field - 1);

        // Get remainder.
        v = v - (tmp.toUInt() * leftScale);
        newValue = KGlobal::locale()->formatNumber(v);
    }
    int point = newValue.find(m_decimalPoint);
    if (point != -1)
    {
        //HACK doubles may be rounded(at fractions > 6 digits on my system)
        int p;
        double frac = fraction(newValue, &p);
        if (right && frac > 0.0)
        {
            //kdDebug()<<k_funcinfo<<field<<": value="<<newValue<<" rightScale="<<rightScale<<" frac="<<frac<<" ("<<newValue.mid(point)<<")"<<endl;
            // Propagate fraction
            v = rightScale * (frac*power(10.0, -p));
            frac = fraction(KGlobal::locale()->formatNumber(v, 19), 0);
            //kdDebug()<<k_funcinfo<<field<<": v="<<v<<" ("<<(unsigned)v<<") rest="<<frac<<endl;
            if (frac > 0.0)
            {
                tmp = KGlobal::locale()->formatNumber(v, 19);
                right->setText(tmp);
                handleLostFocus(field + 1);
            } else {
                tmp.sprintf(rightFormat, (unsigned)(v));
                right->setText(tmp);
            }
            
        }
        // TODO keep fraction for last shown field
        newValue = newValue.left(point);
    }
    currentValue = newValue.toUInt();
    tmp.sprintf(currentFormat, currentValue);
    current->setText(tmp);
}

// Set which fields are visible.
void DurationWidget::setVisibleFields( int fieldMask )
{
    int i;
    for (i = 0; i < 5; i++)
    {
        bool visible = ((fieldMask >> i) & 1) == 1;
        
        // Set the visibility of the fields, and of any associated separator.
        if (visible)
        {
            m_fields[i].current->show();
            if (m_fields[i].separator)
            {
                m_fields[i].separator->show();
            }
            if (m_fields[i].unit)
            {
                m_fields[i].unit->show();
     }
        }
        else
        {
            m_fields[i].current->hide();
            if (m_fields[i].separator)
            {
                m_fields[i].separator->hide();
            }
            if (m_fields[i].unit)
            {
                m_fields[i].unit->hide();
     }
        }
    }
}

// Retreive the visible fields.
int DurationWidget::visibleFields()
{
    int i;
    int fieldMask = 0;
    for (i = 0; i < 5; i++)
    {
        if (m_fields[i].current->isHidden())
        {   
            fieldMask |= (1 << i);
        }
    }
    return fieldMask;
}

void DurationWidget::setFieldLeftscale(int f, double ls)
{
    m_fields[f].leftScale = ls;
}

void DurationWidget::setFieldRightscale(int f, double rs)
{
    m_fields[f].rightScale = rs;
}

void DurationWidget::setFieldScale(int f, double scale)
{
    m_fields[f].scale = scale;
}

void DurationWidget::setFieldUnit(int f, QString unit)
{
    if (m_fields[f].unit)
    {
 m_fields[f].unit->setText(unit);
    }
}

double DurationWidget::power(double m, int e) {
    int c = e > 0 ? e : -e;
    double value = 1;
    for (int i=0; i < c; ++i) {
        value = e > 0 ? value * m : value / m;
    }
    return value;
}

double DurationWidget::fraction(QString number, int *exp) {
    int point = number.find(m_decimalPoint);
    if (point == -1) {
        return 0.0;
    }
    QString v;
    if (exp) {
        v = number.mid(point+m_decimalPoint.length());
        *exp = v.length();
        
    } else {
         v = number.mid(point);
    }
    return KGlobal::locale()->readNumber(v);
}

}  //KPlato namespace
