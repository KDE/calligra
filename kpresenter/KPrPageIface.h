// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2002,2003,2004 Laurent MONTEL <montel@kde.org>

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

#ifndef KPRESENTER_PAGE_IFACE_H
#define KPRESENTER_PAGE_IFACE_H

#include <KoDocumentIface.h>
#include <dcopref.h>

#include <QString>
#include <QColor>
#include "global.h"
#include <QRect>
class KPrPage;

class KPrPageIface :public DCOPObject
{
    K_DCOP
public:
    KPrPageIface( KPrPage *_page, int pgnum );

k_dcop:
    virtual DCOPRef object( int num );
    virtual DCOPRef selectedObject();
    virtual DCOPRef textObject( int num );

    virtual DCOPRef groupObjects();

    virtual int numTextObject() const;


    virtual QString manualTitle()const;
    virtual void insertManualTitle(const QString & title);
    virtual QString pageTitle( const QString &_title = QString() ) const;

    virtual void setNoteText( const QString &_text );
    virtual QString noteText( )const;

    virtual unsigned int objNums() const;
    virtual int numSelected() const;
    virtual void ungroupObjects();
    virtual void raiseObjs();
    virtual void lowerObjs();
    virtual void copyObjs();

    virtual void slotRepaintVariable();
    virtual void recalcPageNum();

    virtual void setPageTimer(  int pageTimer );
    virtual void setPageSoundEffect(  bool soundEffect );
    virtual void setPageSoundFileName(  const QString &fileName );
    virtual QString pageSoundFileName()const;

    virtual bool pageSoundEffect() const;
    virtual int pageTimer() const;
    virtual int backYFactor() const;
    virtual int backXFactor() const;

    virtual int backType()const ;
    virtual int backView()const;
    virtual QColor backColor1()const;
    virtual QColor backColor2()const ;
    virtual int backColorType()const;
    virtual QString backPixFilename()const;
    virtual QString backClipFilename()const;
    virtual int pageEffect()const;
    virtual QString pageEffectString( )const;
    virtual void setPageEffect(const QString & );

    virtual bool backUnbalanced()const ;

    virtual QRect pageRect()const;

    virtual bool isSlideSelected();
    virtual void slideSelected(bool _b);
    virtual void changePicture( const QString & );

    virtual void setBackGroundColor1(const QColor &col);
    virtual void setBackGroundColor2(const QColor &col);
    virtual void setBackGroundColorType(const QString &type);

    DCOPRef insertRectangle(double x,double y, double h, double w);
    DCOPRef insertEllipse(double x,double y, double h, double w );
    DCOPRef insertPie( double x,double y, double h, double w );
    DCOPRef insertLine( double x1, double y1, double x2, double y2 );
    DCOPRef insertTextObject( double x, double y, double h, double w );
    DCOPRef insertPicture( const QString & file, double x, double y, double h, double w );

    void deSelectAllObj();

    bool oneObjectTextExist() const ;
    bool isOneObjectSelected() const;

    bool haveASelectedPartObj() const;
    bool haveASelectedGroupObj() const;
    void rotateAllObjSelected(float _newAngle);

    void moveAllObjectSelected( int diffx,int diffy);
    void deleteAllObjectSelected();

    void sendBackward();
    void bringForward();

    bool hasHeader()const;
    bool hasFooter()const;
    void setHeader( bool b );
    void setFooter( bool b );
    void setUseMasterBackground( bool useMasterBackground );
    bool useMasterBackground() const;

    void setDisplayObjectFromMasterPage( bool _b );
    bool displayObjectFromMasterPage() const;

    void setDisplayBackground( bool _b );
    bool displayBackground() const;

private:
    KPrPage *m_page;
};

#endif
