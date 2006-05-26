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

#ifndef BACKDIA_H
#define BACKDIA_H

#include <kdialogbase.h>
#include <q3frame.h>
#include <qdatetime.h>
//Added by qt3to4:
#include <QShowEvent>
#include <QLabel>

#include "global.h"
#include "KPrBackground.h"

#include <KoBrush.h>
#include <KoPicture.h>

class QLabel;
class QComboBox;
class KColorButton;
class QSlider;
class KPrBackGround;
class QCheckBox;
class QTabWidget;
class KPrPage;
class KUrlRequester;


class KPrBackPreview : public Q3Frame
{
    Q_OBJECT

public:
    KPrBackPreview( QWidget *parent, KPrPage *page );
    virtual ~KPrBackPreview();

    KPrBackGround *backGround() const {
        return back;
    }

protected:
    virtual void drawContents( QPainter *p );

private:
    KPrBackGround *back;

};

class KPrBackDialog : public KDialogBase
{
    Q_OBJECT

public:
    KPrBackDialog( QWidget* parent, const char* name,
             BackType backType, const QColor &backColor1,
             const QColor &backColor2, BCType _bcType,
             const KoPicture &backPic,
             BackView backPicView, bool _unbalanced,
             int _xfactor, int _yfactor, KPrPage *_page  );

    KPrBackGround::Settings getBackGround() const;
    bool useMasterBackground() const;

protected:
    void showEvent( QShowEvent *e );
    QColor getBackColor1() const;
    QColor getBackColor2() const;
    BCType getBackColorType() const;
    BackType getBackType() const;
    KoPicture getBackPicture() const { return m_picture; }
    BackView getBackView() const;
    bool getBackUnbalanced() const;
    int getBackXFactor() const;
    int getBackYFactor() const;
private:
    QLabel *picPreview;
    QCheckBox *unbalanced;
    QComboBox *cType, *backCombo, *picView;
    QLabel *picChooseLabel;
    KUrlRequester *picChoose;
    KColorButton *color1Choose, *color2Choose;
    QSlider *xfactor, *yfactor;
    KPrBackPreview *preview;
    bool picChanged, lockUpdate;
    QLabel *labXFactor, *labYFactor;
    QTabWidget *tabWidget;
    QCheckBox *m_useMasterBackground;

    bool oldUseMasterBackground;
    BackType oldBackType;
    QColor oldBackColor1;
    QColor oldBackColor2;
    BCType oldBcType;
    BackView oldBackPicView;
    bool oldUnbalanced;
    int oldXFactor;
    int oldYFactor;

    KoPicture m_picture, m_oldpicture;
    KPrPage *m_page;

private slots:
    void aboutToSelectPic();
    void afterSelectPic( const QString & );
    void updateConfiguration();

    void Ok() { emit backOk( this, false ); }
    void Apply() { emit backOk( this, false ); }
    void ApplyGlobal() { emit backOk( this, true ); }

    void changeComboText(int _p);
    void slotReset();

signals:
    void backOk( KPrBackDialog*, bool );

};
#endif //BACKDIA_H
