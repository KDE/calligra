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
    void fileStatistics();
    void editFind();
    void editReplace();
    void editCustomVars();
    void editMailMergeDataBase();
    void viewPageMode();
    void viewPreviewMode();
    void configure();
    void spelling();
    void autoFormat();
    void stylist();
    void createTemplate();

    int tableSelectCell(const QString &name, uint row, uint col);
    void insertTable();
    void tableInsertRow();
    void tableInsertRow(uint row);
    void tableInsertCol();
    void tableInsertCol(uint col);
    void tableResizeCol();
    void tableDeleteRow();
    int  tableDeleteRow(uint row);
    void tableDeleteCol();
    int  tableDeleteCol(uint col);
    void tableJoinCells();
    void tableSplitCells();
    void tableSplitCells(uint rows, uint cols);
    void tableUngroupTable();

    void insertPicture(); // not good, triggers the dialog

    /**
     * Insert an inline picture at the current character position
     * TODO
     */
    void insertInlinePicture( QString fileName, int w = 0, int h = 0 );
    /**
     * Insert a (non-inline) picture at the given position
     * If w and h are 0, the default size from the image is used
     */
    void insertPictureFromFile( QString fileName, double x, double y, int w = 0, int h = 0 );


    void toolsPart();

    double zoom();
    void setZoom( int zoom);
    void editPersonalExpression();
    void insertLink();
    void insertFormula();

    void formatFont();
    void formatParagraph();
    void formatPage();
    void raiseFrame();
    void lowerFrame();
    void bringToFront();
    void sendToBack();
    void applyAutoFormat();
    void createStyleFromSelection();

    void insertFootNote();

    int configTableRows() const;
    int configTableCols() const;
    bool configTableIsFloating() const;

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
