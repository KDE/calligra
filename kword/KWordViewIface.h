/* This file is part of the KDE project
   Copyright (C) 2001 Laurent Montel <lmontel@mandrakesoft.com>

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

#ifndef KWORD_VIEW_IFACE_H
#define KWORD_VIEW_IFACE_H

#include <KoViewIface.h>

#include <qstring.h>

class KWView;

class KWordViewIface : public KoViewIface
{
    K_DCOP
public:
    KWordViewIface( KWView *view_ );
    KWView * getView()const{return view;}
k_dcop:
    virtual void fileStatistics();
    virtual void editFind();
    virtual void editReplace();
    virtual void editCustomVars();
    virtual void editMailMergeDataBase();
    virtual void viewPageMode();
    virtual void viewPreviewMode();
    virtual void configure();
    virtual void spelling();
    virtual void autoFormat();
    virtual void stylist();
    virtual void createTemplate();

    virtual void insertTable();
    virtual void tableInsertRow();
    virtual void tableInsertCol();
    virtual void tableResizeCol();
    virtual void tableDeleteRow();
    virtual void tableDeleteCol();
    virtual void tableJoinCells();
    virtual void tableSplitCells();
    virtual void tableUngroupTable();

    virtual void insertPicture();
    virtual void toolsPart();

    virtual double zoom();
    virtual void setZoom( int zoom);
    virtual void editPersonalExpression();
    virtual void insertLink();
    virtual void insertFormula();

    virtual void formatFont();
    virtual void formatParagraph();
    virtual void formatPage();
    virtual void raiseFrame();
    virtual void lowerFrame();
    virtual void bringToFront();
    virtual void sendToBack();
    virtual void applyAutoFormat();
    virtual void createStyleFromSelection();

    virtual void insertFootNote();

    int configTableRows() const;
    int configTableCols() const;
    bool configTableIsFloating() const;
    int configSplitCellRows() const;
    int configSplitCellCols() const;
    void setConfigSplitCellRows( unsigned int _nb );
    void setConfigSplitCellCols( unsigned int _nb );

    //return false if there is no frame in document
    bool selectAllFrames();
    bool unSelectAllFrames();

    void editFootEndNote();
    void savePicture();
    void saveClipart();

private:
    KWView *view;

};

#endif
