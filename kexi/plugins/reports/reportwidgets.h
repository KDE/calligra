/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXIREPORTWIDGETS_H
#define KEXIREPORTWIDGETS_H

#include <qlabel.h>
#include <q3scrollview.h>
//Added by qt3to4:
#include <QPixmap>
#include <QPaintEvent>

namespace KFormDesigner
{
class Form;
class FormManager;
}

using KFormDesigner::Form;

//! A form embedded as a widget inside other form
class KexiSubReport : public Q3ScrollView
{
    Q_OBJECT
    Q_PROPERTY(QString reportName READ reportName WRITE setReportName);

public:
    KexiSubReport(QWidget *parent);
    ~KexiSubReport() {}

    //! \return the name of the subreport inside the db
    QString reportName() const {
        return m_reportName;
    }
    void setReportName(const QString &name);

private:
//  KFormDesigner::FormManager *m_manager;
    Form   *m_form;
    QWidget  *m_widget;
    QString   m_reportName;
};

//! A simple label inside a report
class Label : public QLabel
{
    Q_OBJECT

public:
    Label(const QString &text, QWidget *parent);
    ~Label() {}
};

//! A simple picture label inside a report
class PicLabel : public QLabel
{
    Q_OBJECT

public:
    PicLabel(const QPixmap &pix, QWidget *parent);
    ~PicLabel() {}

    virtual bool setProperty(const char *name, const QVariant &value);
};

//! A line
class ReportLine : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(ReportLineStyle lineStyle READ lineStyle WRITE setLineStyle)
    Q_PROPERTY(int lineWidth READ lineWidth WRITE setLineWidth)
    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_PROPERTY(CapStyle capStyle READ capStyle WRITE setCapStyle)

public:
    enum ReportLineStyle { NoLine = Qt::NoPen, Solid = Qt::SolidLine, Dash = Qt::DashLine,  Dot = Qt::DotLine,
                           DashDot = Qt::DashDotLine, DashDotDot =  Qt::DashDotDotLine
                         };
    enum CapStyle { Flat = Qt::FlatCap, Square = Qt::SquareCap, Round = Qt::RoundCap };

    ReportLine(QWidget *parent);
    ~ReportLine() {
        ;
    }

    ReportLineStyle lineStyle() const;
    void setLineStyle(ReportLineStyle style);

    int lineWidth() const;
    void setLineWidth(int width);

    QColor color() const;
    void setColor(const QColor &color);

    CapStyle capStyle() const;
    void setCapStyle(CapStyle capStyle);

protected:
    virtual void paintEvent(QPaintEvent *ev);

private:
    ReportLineStyle m_lineStyle;
    int m_lineWidth;
    CapStyle m_capStyle;
    QColor  m_color;
};


#endif

