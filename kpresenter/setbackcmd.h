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

#ifndef setbackcmd_h
#define setbackcmd_h

#include <qcolor.h>
#include <command.h>
#include <global.h>
#include <kpimage.h>
class KPresenterDoc;

/******************************************************************/
/* Class: SetBackCmd						  */
/******************************************************************/

class SetBackCmd : public Command
{
public:
    SetBackCmd( QString _name, QColor _backColor1, QColor _backColor2, BCType _bcType,
		bool _backUnbalanced, int _backXFactor, int _backYFactor,
		const KPImageKey & _backPix, QString _backClip, BackView _backView, BackType _backType,
		QColor _oldBackColor1, QColor _oldBackColor2, BCType _oldBcType,
		bool _oldBackUnbalanced, int _oldBackXFactor, int _oldBackYFactor,
		const KPImageKey & _oldBackPix, QString _oldBackClip, BackView _oldBackView, BackType _oldBackType,
		bool _takeGlobal, int _currPgNum, KPresenterDoc *_doc );

    virtual void execute();
    virtual void unexecute();

protected:
    SetBackCmd()
    {; }

    QColor backColor1, backColor2;
    bool unbalanced;
    int xfactor, yfactor;
    KPImageKey backPix;
    QString backClip;
    BCType bcType;
    BackView backView;
    BackType backType;
    QColor oldBackColor1, oldBackColor2;
    bool oldUnbalanced;
    int oldXFactor, oldYFactor;
    KPImageKey oldBackPix;
    QString oldBackClip;
    BCType oldBcType;
    BackView oldBackView;
    BackType oldBackType;
    bool takeGlobal;
    int currPgNum;
    KPresenterDoc *doc;

};

#endif
