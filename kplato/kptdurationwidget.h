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

#ifndef KPTDURATIONWIDGET_H
#define KPTDURATIONWIDGET_H

#include "ui_kptdurationwidget.h"

#include <QFrame>

#include "kptduration.h"

class QRegExpValidator;
class QLineEdit;
class QLabel;
class QString;

namespace KPlato
{

class DurationWidget : public QFrame, public Ui_DurationWidget
{
    Q_OBJECT
public:
    enum { Days = 0x1, Hours = 0x2, Minutes = 0x4, Seconds = 0x8, Milliseconds = 0x10 } Fields;

    DurationWidget(QWidget *parent=0);
    ~DurationWidget();

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

    Duration value() const;
    
    void setVisibleFields( int fieldMask );
    int visibleFields();
    void setFieldLeftscale(int f, double ls);
    void setFieldRightscale(int f, double rs);
    void setFieldScale(int f, double scale);
    void setFieldUnit(int f, QString unit);

signals:
    void valueChanged();
    
public slots:
    virtual void setValue( const KPlato::Duration & newDuration );
    virtual void handleLostFocus( int field );

private:
    QString m_decimalPoint;
    QRegExpValidator *m_validator;
    struct FieldDescriptor *m_fields;

    qint64 setValueMilliseconds( qint64 milliseconds );
    qint64 setValueSeconds( qint64 seconds );
    qint64 setValueMinutes( qint64 mins );
    qint64 setValueHours( qint64 mins );
    qint64 setValueDays( qint64 mins );
    double power( double m, int e );
    double fraction( QString number, int * exp );

private slots:
    virtual void dddLostFocus();
    virtual void hhLostFocus();
    virtual void mmLostFocus();
    virtual void ssLostFocus();
    virtual void msLostFocus();

};

}  //KPlato namespace

#endif
