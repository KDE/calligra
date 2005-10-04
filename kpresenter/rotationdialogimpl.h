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

#include <qlabel.h>
#include <kdialogbase.h>

class TextPreview;
class RotationPropertyUI;
class QObject;
class QEvent;
class CircleGroup;

class RotationDialogImpl : public KDialogBase
{
    Q_OBJECT

public:
    RotationDialogImpl( QWidget *parent, const char* name = 0 );
    ~RotationDialogImpl() {}

    void setAngle( double angle );
    double angle();

protected slots:
    void angleChanged( double );
    void angleMode( int );
    void slotOk();

protected:
    TextPreview *m_preview;
    RotationPropertyUI *m_dialog;
    CircleGroup *m_angleGroup;

private:
    bool noSignals;
};

class CircleToggle : public QLabel
{
    Q_OBJECT
public:
    CircleToggle(QWidget *parent, const QString &image, int id);
    int id() { return m_id; }

signals:
    void clicked(int id);

public slots:
    void setChecked(bool on);

protected:
    void mousePressEvent ( QMouseEvent * e );

private:
    QPixmap m_on, m_off;
    bool m_selected;
    int m_id;
};

class CircleGroup : public QFrame
{
    Q_OBJECT;
public:
    CircleGroup(QWidget *parent);
    void setAngle(int angle);
    void add(CircleToggle *button);

signals:
    void clicked(int id);

private slots:
    void selectionChanged(int buttonId);

private:
    QPtrList<CircleToggle> m_buttons;
    bool noSignals;
};

#endif
