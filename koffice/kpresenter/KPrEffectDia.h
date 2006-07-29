// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#ifndef EFFECTDIA_H
#define EFFECTDIA_H

#include <kdialogbase.h>
#include <qptrlist.h>

class KPrView;
class QWidget;
class QComboBox;
class QLabel;
class QPushButton;
class QCheckBox;
class QBoxLayout;
class QResizeEvent;
class QLineEdit;
class KPrObject;
class KIntNumInput;
class KURLRequester;
class KPrSoundPlayer;
class QSpinBox;

class KPrEffectDia : public KDialogBase
{
    Q_OBJECT

public:
    KPrEffectDia( QWidget* parent, const char*, const QPtrList<KPrObject> &_objs,
               KPrView* );

    ~KPrEffectDia();

protected:
    void resizeEvent( QResizeEvent *e );

    QString getSoundFileFilter() const;

    QComboBox *cEffect, *cEffect2, *cDisappear, *cAppearSpeed, *cDisappearSpeed;
    QLabel *lEffect, *lEffect2, *lAppear, *lDisappear, *lDEffect, * lAppearSpeed, *lDisappearSpeed;
    QSpinBox *eAppearStep,*eDisappearStep;
    QCheckBox *disappear;
    QBoxLayout *topLayout;
    KIntNumInput *timerOfAppear, *timerOfDisappear;

    QCheckBox *appearSoundEffect, *disappearSoundEffect;
    QLabel *lSoundEffect1, *lSoundEffect2;
    KURLRequester *requester1, *requester2;
    QPushButton *buttonTestPlaySoundEffect1, *buttonTestStopSoundEffect1;
    QPushButton *buttonTestPlaySoundEffect2, *buttonTestStopSoundEffect2;

    KPrSoundPlayer *soundPlayer1, *soundPlayer2;

    KPrView *view;
    QPtrList<KPrObject> objs;

public slots:
    void slotEffectDiaOk();

protected slots:
    void disappearChanged();
    void appearEffectChanged( int num );
    void disappearEffectChanged( int num );
    void appearSoundEffectChanged();
    void disappearSoundEffectChanged();
    void slotRequesterClicked( KURLRequester * );
    void slotAppearFileChanged( const QString& );
    void slotDisappearFileChanged( const QString& );
    void playSound1();
    void playSound2();
    void stopSound1();
    void stopSound2();

};

#endif //EFFECTDIA_H
