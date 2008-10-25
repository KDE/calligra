/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Alexander Dymo <cloudtemple@mskat.net>
   Copyright (C) 2008 Jarosław Staniek <staniek@kde.org>

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

#ifndef KPROPERTY_SPINBOX_H
#define KPROPERTY_SPINBOX_H

#include "Factory.h"
#include <KNumInput>

class IntSpinBox : public KIntNumInput
{
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue USER true)

public:
    IntSpinBox(const KoProperty::Property* prop, QWidget *parent = 0);
    virtual ~IntSpinBox();

    virtual int value() const { return KIntNumInput::value(); }

//    virtual void setProperty(const KoProperty::Property *prop);
    
//    virtual void drawViewer(QPainter *p, const QColorGroup &cg, const QRect &r, const QVariant &value);

public slots:
    virtual void setValue(int value) { KIntNumInput::setValue(value); }

//todo?    virtual bool eventFilter(QObject *o, QEvent *e);
/*    QLineEdit * lineEdit() const {
        return KIntSpinBox::lineEdit();
    }*/
};

/*class KOPROPERTY_EXPORT IntEdit : public Widget
{
    Q_OBJECT

public:
    IntEdit(Property *property, QWidget *parent = 0);
    virtual ~IntEdit();

    virtual QVariant value() const;
    virtual void setValue(const QVariant &value, bool emitChange = true);
    virtual void drawViewer(QPainter *p, const QColorGroup &cg, const QRect &r, const QVariant &value);

protected:
    virtual void setReadOnlyInternal(bool readOnly);
    void updateSpinWidgets();

protected slots:
    void slotValueChanged(int value);

private:
    IntSpinBox  *m_edit;
};*/

// Double editor

class DoubleSpinBox : public KDoubleNumInput
{
    Q_OBJECT
    Q_PROPERTY(double value READ value WRITE setValue USER true)

public:
//! @todo Support setting precision limits, step, etc.
    DoubleSpinBox(const KoProperty::Property* prop, QWidget *parent = 0);
    virtual ~DoubleSpinBox();

//    virtual bool eventFilter(QObject *o, QEvent *e);
/*    QLineEdit * lineEdit() const {
        return QDoubleSpinBox::lineEdit();
    }*/

    virtual double value() const { return KDoubleNumInput::value(); }

public slots:
    virtual void setValue(double value) { KDoubleNumInput::setValue(value); }

protected:
    //! Used to fix height of the internal spin box
    virtual void resizeEvent( QResizeEvent * event );
};

/*
class KOPROPERTY_EXPORT DoubleEdit : public Widget
{
    Q_OBJECT

public:
    DoubleEdit(Property *property, QWidget *parent = 0);
    virtual ~DoubleEdit();

    virtual QVariant value() const;
    virtual void setValue(const QVariant &value, bool emitChange = true);
    virtual void drawViewer(QPainter *p, const QColorGroup &cg, const QRect &r, const QVariant &value);

protected:
    virtual void setReadOnlyInternal(bool readOnly);
    void updateSpinWidgets();

protected slots:
    void slotValueChanged(double value);

private:
    DoubleSpinBox  *m_edit;
};*/

class IntSpinBoxDelegate : public KoProperty::EditorCreatorInterface, 
                           public KoProperty::ValueDisplayInterface
{
public:
    IntSpinBoxDelegate();
    
    virtual QString displayText( const KoProperty::Property* prop ) const;

    virtual QWidget * createEditor( int type, QWidget *parent, 
        const QStyleOptionViewItem & option, const QModelIndex & index ) const;
};

class DoubleSpinBoxDelegate : public KoProperty::EditorCreatorInterface, 
                              public KoProperty::ValueDisplayInterface
{
public:
    DoubleSpinBoxDelegate();
    
    virtual QString displayText( const KoProperty::Property* prop ) const;

    virtual QWidget * createEditor( int type, QWidget *parent, 
        const QStyleOptionViewItem & option, const QModelIndex & index ) const;
};

#endif
