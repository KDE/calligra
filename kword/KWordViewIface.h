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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
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

    virtual int tableSelectCell(const QString &name, uint row, uint col);
    virtual void insertTable();
    virtual void tableInsertRow();
    virtual void tableInsertRow(uint row);
    virtual void tableInsertCol();
    virtual void tableInsertCol(uint col);
    virtual void tableResizeCol();
    virtual void tableDeleteRow();
    virtual int  tableDeleteRow(uint row);
    virtual void tableDeleteCol();
    virtual int  tableDeleteCol(uint col);
    virtual void tableJoinCells();
    virtual void tableSplitCells();
    virtual void tableSplitCells(uint rows, uint cols);
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

    //it can return QString::null if there is any template selected
    QString configTableTemplateName() const;
    void setConfigTableTemplateName(const QString &_name);

    void selectAllFrames();
    void unSelectAllFrames();

    void editFootEndNote();
    void savePicture();

    void insertFile();
    void tableStylist();

    void addBookmark();
    void selectBookmark();
    void tableProperties();
    void importStyle();

    void insertFile(const QString & path);
    void createFrameStyle();

    void setPictureInline( bool _inline);
    bool pictureInline() const;

    void setPictureKeepRatio( bool _keep);
    bool pictureKeepRatio() const;
    void changeAutoSpellCheckStatus(bool b);
    void deselectAllFrames();
    int currentPage();
    void convertTableToText();
    void convertToTextBox();
    void addIgnoreAllWord();
    void addPersonalExpression();
#if 0 // KWORD_HORIZONTAL_LINE
    // MOC_SKIP_BEGIN
    // Comment out, as dcopidl has no stop/start words
    /*
    void insertHorizontalLine();
    void changeHorizontalLine();
    */
    // MOC_SKIP_END
#endif
    void addWordToDictionary();
    void sortText();
    void insertPage();
    void createLinkedFrame();

    void lineBreak();

    void increaseNumberingLevel();
    void decreaseNumberingLevel();

private:
    KWView *view;

};

#endif
