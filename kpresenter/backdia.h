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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef BACKDIA_H
#define BACKDIA_H

#include <qdialog.h>
#include <qframe.h>
#include <qdatetime.h>

#include <global.h>

class QLabel;
class QComboBox;
class QPushButton;
class KColorButton;
class QSlider;
class KPBackGround;
class KPresenterDoc;
class QCheckBox;
class QTabWidget;

/*******************************************************************
 *
 * Class: BackPreview
 *
 *******************************************************************/

class BackPreview : public QFrame
{
    Q_OBJECT

public:
    BackPreview( QWidget *parent, KPresenterDoc *doc );
    virtual ~BackPreview();

    KPBackGround *backGround() const {
	return back;
    }

protected:
    void resizeEvent( QResizeEvent *e );
    void drawContents( QPainter *p );

private:
    KPBackGround *back;

};

/******************************************************************/
/* class BackDia						  */
/******************************************************************/
class BackDia : public QDialog
{
    Q_OBJECT

public:
    BackDia( QWidget* parent, const char* name,
	     BackType backType, const QColor &backColor1,
	     const QColor &backColor2, BCType _bcType,
	     const QString &backPic, const QDateTime &picLM,
	     const QString &backClip, const QDateTime &clipLM,
	     BackView backPicView, bool _unbalanced,
	     int _xfactor, int _yfactor, KPresenterDoc *doc );

    QColor getBackColor1() const;
    QColor getBackColor2() const;
    BCType getBackColorType() const;
    BackType getBackType() const;
    QString getBackPixFilename() const { return chosenPic; }
    QDateTime getBackPixLastModified() const { return picLastModified; }
    QString getBackClipFilename() const { return chosenClip; }
    QDateTime getBackClipLastModified() const { return clipLastModified; }
    BackView getBackView() const;
    bool getBackUnbalanced() const;
    int getBackXFactor() const;
    int getBackYFactor() const;

protected:
    void showEvent( QShowEvent *e );

private:
    QLabel *lPicName, *picPreview, *lClipName;
    QCheckBox *unbalanced;
    QComboBox *cType, *backCombo, *picView;
    QPushButton *okBut, *applyBut, *applyGlobalBut, *cancelBut;
    QPushButton *picChoose, *clipChoose;
    KColorButton *color1Choose, *color2Choose;
    QSlider *xfactor, *yfactor;
    QString chosenPic;
    QString chosenClip;
    BackPreview *preview;
    bool picChanged, clipChanged, lockUpdate;
    QDateTime picLastModified, clipLastModified;
    QLabel *labXFactor, *labYFactor;
    QTabWidget *tabWidget;

private slots:
    void selectPic();
    void selectClip();
    void updateConfiguration();

    void Ok() { emit backOk( false ); }
    void Apply() { emit backOk( false ); }
    void ApplyGlobal() { emit backOk( true ); }

    void changeComboText(int _p);

signals:
    void backOk( bool );

};
#endif //BACKDIA_H
