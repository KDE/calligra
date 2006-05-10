// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2005 Thomas Zander <zander@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; version 2.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/
#ifndef __rotationdialogimpl_h__
#define __rotationdialogimpl_h__

#include <QLabel>
#include <kdialogbase.h>

class KPrTextPreview;
class RotationPropertyUI;
class QObject;
class QEvent;
class KPrCircleGroup;

/**
  * A dialog that lets the user interactively choose an angle for rotation.
  */
class KPrRotationDialogImpl : public KDialogBase
{
    Q_OBJECT

public:
    KPrRotationDialogImpl( QWidget *parent, const char* name = 0 );
    ~KPrRotationDialogImpl() {}

    void setAngle( double angle );
    double angle();

protected slots:
    void angleChanged( double );
    void angleMode( int );
    void slotOk();

protected:
    KPrTextPreview *m_preview;
    RotationPropertyUI *m_dialog;
    KPrCircleGroup *m_angleGroup;

private:
    bool noSignals;
};

/**
 * A toggle-button like widget that shows one pixmap when it is checked,
 *  and another when it is unselected.
 */
class KPrCircleToggle : public QLabel
{
    Q_OBJECT
public:
    /**
     * Constructor.
     * @param parent the parent widget, as required by Qt.
     * @param image the named image that we will use. "rotate/" is
     *      prepended and "dn" is appended for the checked state.
     * @param id  the id that will be used in the clicked signal
     */
    KPrCircleToggle(QWidget *parent, const QString &image, int id);
    /// return the id which is passed in the constructor
    int id() { return m_id; }

signals:
    /// this signal will be emitted whenever the button becomes checked
    void clicked(int id);

public slots:
    /**
     * Check or uncheck the button.  On change the toggle will emit the clicked signal.
     * @param on the new state of the button.
     */
    void setChecked(bool on);

protected:
    /// overwritten method from QWidget.
    void mousePressEvent ( QMouseEvent * e );

private:
    QPixmap m_on, m_off;
    bool m_selected;
    int m_id;
};

/**
 * A button-group equivalent for a set of KPrCircleToggle classes.
 */
class KPrCircleGroup : public QFrame
{
    Q_OBJECT
public:
    /**
     * Constructor.
     * @param parent the parent widget, as required by Qt.
     */
    KPrCircleGroup(QWidget *parent);
    /**
     * Set the angle the group is currently representing. If there is a child button
     * that registred itself (using add()) with an ID that matches the argument angle
     * that button will be checked.  All other buttons will be disabled.
     * @param angle the new angle to be represented by this circle
     */
    void setAngle(int angle);
    /**
     * Add a KPrCircleToggle button as one of the representers of this circle.
     * @param button the button
     */
    void add(KPrCircleToggle *button);

signals:
    /// clicked will be emitted when one of the child buttons is clicked.
    void clicked(int id);

private slots:
    void selectionChanged(int buttonId);

private:
    QPtrList<KPrCircleToggle> m_buttons;
    bool noSignals;
};

#endif
