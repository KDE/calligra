// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2002 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2004 Thorsten Zachmann <zachmann@kde.org>

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

#ifndef SLIDETRANSITIONDIA_H
#define SLIDETRANSITIONDIA_H

#include <qtimer.h>
#include <qwidget.h>

#include <kdialogbase.h>

#include "global.h"

class KPrPageEffects;
class KPrView;
class KPrSoundPlayer;
class KURLRequester;
class SlideTransitionWidget;

class KPrSlideTransitionDia : public KDialogBase
{
    Q_OBJECT
public:
    KPrSlideTransitionDia( QWidget *parent, const char *name, KPrView *view );
    ~KPrSlideTransitionDia();

protected:
    void apply( bool global );
    // TODO reuse
    QString getSoundFileFilter();
    
    SlideTransitionWidget *m_dialog;      
    KPrView *m_view;
    KPrSoundPlayer *m_soundPlayer;
    
    PageEffect m_effect;
    EffectSpeed m_effectSpeed;
    bool m_soundEffect;
    QString m_soundFileName;
    int m_slideTime;

    KPrPageEffects *m_pageEffect;
    QTimer m_pageEffectTimer;
    QPixmap m_pixmap;
    QPixmap m_target;
    
protected slots:    
    void effectChanged( int );
    void playSound();
    void stopSound();
    void preview();
    void slotDoPageEffect();
    void slotOk();
    void slotRequesterClicked( KURLRequester * );
    void slotSoundFileChanged( const QString& text );
    void slotUser1();
    void soundEffectChanged();
};


#endif /* SLIDETRANSITIONDIA_H */
