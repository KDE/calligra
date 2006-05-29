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

#include "KWordViewIface.h"

#include <kurl.h>

#include "KWView.h"
#include "KWGUI.h"
#include "KWCanvas.h"
#include "KWDocument.h"
#include "KWFrameSet.h"

#include <dcopclient.h>
//Added by qt3to4:
#include <QList>

KWordViewIface::KWordViewIface( KWView *view_ )
    : KoViewIface( view_ )
{
    view = view_;
}

void KWordViewIface::fileStatistics()
{
    view->fileStatistics();
}

void KWordViewIface::editFind()
{
    view->editFind();
}

void KWordViewIface::editReplace()
{
    view->editReplace();
}

void KWordViewIface::editCustomVars()
{
    view->editCustomVars();
}

void KWordViewIface::editMailMergeDataBase()
{
    view->editMailMergeDataBase();
}

void KWordViewIface::viewPageMode()
{
    view->viewPageMode();
}

void KWordViewIface::viewPreviewMode()
{
    view->viewPreviewMode();
}

void KWordViewIface::configure()
{
    view->configure();
}

void KWordViewIface::spelling()
{
    view->slotSpellCheck();
}

void KWordViewIface::autoFormat()
{
    view->extraAutoFormat();
}

void KWordViewIface::stylist()
{
    view->extraStylist();
}

void KWordViewIface::createTemplate()
{
    view->extraCreateTemplate();
}

void KWordViewIface::insertTable()
{
    view->insertTable();
}

void KWordViewIface::insertPicture()
{
    view->insertPicture();
}

static KoPicture picture( const QString& fileName )
{
    KoPicture picture;
    KoPictureKey key;
    key.setKeyFromFile( fileName );
    picture.setKey( key );
    picture.loadFromFile( fileName );
    return picture;
}

void KWordViewIface::insertPictureFromFile( QString fileName, double x, double y, int w, int h )
{
    view->getGUI()->canvasWidget()->insertPictureDirect( picture( fileName ), KoPoint( x, y ), QSize(w, h) );
}

void KWordViewIface::insertInlinePicture( QString fileName, int w, int h )
{
    view->insertPicture( picture( fileName ), true /*makeInline*/, true /*keepRatio*/, w, h );
    view->getGUI()->canvasWidget()->insertInlinePicture();
}

void KWordViewIface::formatPage()
{
    view->formatPage();
}

void KWordViewIface::toolsPart()
{
    view->toolsPart();
}

double KWordViewIface::zoom()
{
    return view->kWordDocument()->zoom();
}


void KWordViewIface::setZoom( int zoom)
{
    view->showZoom( zoom ); // share the same zoom
    view->setZoom( zoom,true);
    view->kWordDocument()->updateZoomRuler();
}

void KWordViewIface::editPersonalExpression()
{
    view->editPersonalExpr();
}

void KWordViewIface::insertLink()
{
    view->insertLink();
}

void KWordViewIface::insertFormula()
{
    view->insertFormula();
}

void KWordViewIface::formatFont()
{
    view->formatFont();
}

void KWordViewIface::formatParagraph()
{
    view->formatParagraph();
}

void KWordViewIface::raiseFrame()
{
  view->raiseFrame();
}

void KWordViewIface::lowerFrame()
{
  view->lowerFrame();
}

void KWordViewIface::bringToFront()
{
    view->bringToFront();
}

void KWordViewIface::sendToBack()
{
    view->sendToBack();
}


void KWordViewIface::applyAutoFormat()
{
    view->applyAutoFormat();
}


void KWordViewIface::createStyleFromSelection()
{
    view->createStyleFromSelection();
}

int KWordViewIface::tableSelectCell(const QString &name, uint row, uint col)
{
    return view->tableSelectCell(name, row, col);
}

void KWordViewIface::tableInsertRow()
{
    view->tableInsertRow();
}

void KWordViewIface::tableInsertRow(uint row)
{
    view->tableInsertRow(row);
}

void KWordViewIface::tableInsertCol()
{
    view->tableInsertCol();
}

void KWordViewIface::tableInsertCol(uint col)
{
    view->tableInsertCol(col);
}

void KWordViewIface::tableResizeCol()
{
    view->tableResizeCol();
}

void KWordViewIface::tableDeleteRow()
{
    view->tableDeleteRow();
}

int KWordViewIface::tableDeleteRow(uint row)
{
    QList<uint> list;
    list.append(row);
    return view->tableDeleteRow(list);
}

void KWordViewIface::tableDeleteCol()
{
    view->tableDeleteCol();
}

int KWordViewIface::tableDeleteCol(uint col)
{
    QList<uint> list;
    list.append(col);
    return view->tableDeleteCol(list);
}

void KWordViewIface::tableJoinCells()
{
    view->tableJoinCells();
}

void KWordViewIface::tableSplitCells()
{
    view->tableSplitCells();
}

void KWordViewIface::tableSplitCells(uint rows, uint cols)
{
    view->tableSplitCells(cols, rows);
}

void KWordViewIface::tableUngroupTable()
{
    view->tableUngroupTable();
}

void KWordViewIface::insertFootNote()
{
    view->insertFootNote();
}

int KWordViewIface::configTableRows() const
{
    return (int)view->getGUI()->canvasWidget()->tableRows();
}

int KWordViewIface::configTableCols() const
{
    return (int)view->getGUI()->canvasWidget()->tableCols();
}

bool KWordViewIface::configTableIsFloating() const
{
    return view->getGUI()->canvasWidget()->tableIsFloating();
}

QString KWordViewIface::configTableTemplateName() const
{
    return view->getGUI()->canvasWidget()->tableTemplateName();
}

void KWordViewIface::setConfigTableTemplateName(const QString &_name)
{
    view->getGUI()->canvasWidget()->setTableTemplateName( _name );
}

void KWordViewIface::selectAllFrames()
{
    view->getGUI()->canvasWidget()->selectAllFrames(true);
}

void KWordViewIface::unSelectAllFrames()
{
    view->getGUI()->canvasWidget()->selectAllFrames(false);
}


void KWordViewIface::editFootEndNote()
{
    view->editFootEndNote();
}

void KWordViewIface::savePicture()
{
    view->savePicture();
}

void KWordViewIface::insertFile()
{
    view->insertFile();
}

void KWordViewIface::tableStylist()
{
    view->tableStylist();
}

void KWordViewIface::addBookmark()
{
    view->addBookmark();
}

void KWordViewIface::selectBookmark()
{
    view->selectBookmark();
}

void KWordViewIface::tableProperties()
{
    view->tableProperties();
}

void KWordViewIface::importStyle()
{
    view->importStyle();
}

// ### TODO: network transparency
void KWordViewIface::insertFile(const QString & path)
{

    KUrl url;
    url.setPath( path );
    view->insertFile( url );
}

void KWordViewIface::createFrameStyle()
{
    view->createFrameStyle();
}

void KWordViewIface::setPictureInline( bool _inline)
{
    view->getGUI()->canvasWidget()->setPictureInline( _inline);
}

bool KWordViewIface::pictureInline() const
{
    return view->getGUI()->canvasWidget()->pictureInline();
}

void KWordViewIface::setPictureKeepRatio( bool _keep)
{
    view->getGUI()->canvasWidget()->setPictureKeepRatio( _keep);
}

bool KWordViewIface::pictureKeepRatio() const
{
    return view->getGUI()->canvasWidget()->pictureKeepRatio();
}

void KWordViewIface::changeAutoSpellCheckStatus(bool b)
{
    view->autoSpellCheck(b);
}

void KWordViewIface::deselectAllFrames()
{
    view->deselectAllFrames();
}

int KWordViewIface::currentPage()
{
    return view->currentPage();
}

void KWordViewIface::convertTableToText()
{
    view->convertTableToText();
}

void KWordViewIface::convertToTextBox()
{
    view->convertToTextBox();
}

void KWordViewIface::addIgnoreAllWord()
{
    view->slotAddIgnoreAllWord();
}

void KWordViewIface::addPersonalExpression()
{
    view->addPersonalExpression();
}

void KWordViewIface::addWordToDictionary()
{
    view->addWordToDictionary();
}

void KWordViewIface::sortText()
{
    view->sortText();
}

void KWordViewIface::insertPage()
{
    view->insertPage();
}

void KWordViewIface::createLinkedFrame()
{
    view->createLinkedFrame();
}


void KWordViewIface::lineBreak()
{
    view->slotLineBreak();
}

void KWordViewIface::increaseNumberingLevel()
{
    view->slotIncreaseNumberingLevel();
}

void KWordViewIface::decreaseNumberingLevel()
{
    view->slotDecreaseNumberingLevel();
}
