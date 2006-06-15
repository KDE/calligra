// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2002, 2003 Ariya Hidayat <ariya@kde.org>

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

#ifndef PGCONFDIA_H
#define PGCONFDIA_H

#include "global.h"

#include <QMap>
#include <qpen.h>
#include <q3valuelist.h>
//Added by qt3to4:
#include <QLabel>

#include <kpagedialog.h>

class KPrDocument;

class QCheckBox;
class QComboBox;
class QColor;
class Q3ListView;
class QRadioButton;
class QSpinBox;

class KColorButton;
class QSlider;

class KPrPgConfDia : public KPageDialog
{
    Q_OBJECT

public:

    // constructor - destructor
    KPrPgConfDia( QWidget* parent, KPrDocument* doc );
    ~KPrPgConfDia();
    bool getInfiniteLoop() const;
    bool getManualSwitch() const;
    bool getPresentationDuration() const;
    QPen getPen() const;
    Q3ValueList<bool> getSelectedSlides() const;

    QString presentationName() const;

protected:

    KPrDocument* m_doc;

    QCheckBox *infiniteLoop, *presentationDuration;
    QRadioButton *m_manualButton, *m_autoButton;
    KColorButton* penColor;
    QSpinBox* penWidth;
    QComboBox *m_customSlideCombobox;
    Q3ListView *slides;
    QRadioButton *m_customSlide, *m_selectedSlide;
    QLabel *m_labelCustomSlide;
    void setupPageGeneral();
    void setupPageSlides();

public slots:
    void confDiaOk() { emit pgConfDiaOk(); }

signals:
    void pgConfDiaOk();

protected slots:
    void selectAllSlides();
    void deselectAllSlides();
    void radioButtonClicked();
};

#endif
