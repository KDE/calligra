#include "KSpreadDocIface.h"
#include <KoDocumentIface.h>

#include "kspread_doc.h"
#include "kspread_map.h"

#include <kapplication.h>
#include <dcopclient.h>
#include <qcolor.h>

KSpreadDocIface::KSpreadDocIface( KSpreadDoc* _doc )
    : KoDocumentIface( _doc )
{
    doc=_doc;
}

DCOPRef KSpreadDocIface::map()
{
    return DCOPRef( kapp->dcopClient()->appId(),
                    doc->map()->dcopObject()->objId() );
}

bool KSpreadDocIface::save()
{
    // TODO
  return false;
}

bool KSpreadDocIface::saveAs( const QString& )
{
    // TODO
  return false;
}


float KSpreadDocIface::paperHeight()const
{
    return doc->paperHeight();
}

float KSpreadDocIface::paperWidth()const
{
    return doc->paperWidth();
}

float KSpreadDocIface::leftBorder()const
{
    return doc->leftBorder();
}

float KSpreadDocIface::rightBorder()const
{
    return doc->rightBorder();
}

float KSpreadDocIface::topBorder()const
{
    return doc->topBorder();
}

float KSpreadDocIface::bottomBorder()const
{
    return doc->bottomBorder();
}

QString KSpreadDocIface::paperFormatString() const
{
    return doc->paperFormatString();
}

bool KSpreadDocIface::showColHeader()const
{
    return doc->getShowColHeader();
}

bool KSpreadDocIface::showRowHeader()const
{
    return doc->getShowRowHeader();
}

int KSpreadDocIface::indentValue()const
{
    return doc->getIndentValue();
}

void KSpreadDocIface::setIndentValue(int _val)
{
    doc->setIndentValue(_val);
}

void KSpreadDocIface::changeDefaultGridPenColor( const QColor &_col)
{
    doc->changeDefaultGridPenColor(_col);
}

bool KSpreadDocIface::showCommentIndicator()const
{
    return doc->getShowCommentIndicator();
}

bool KSpreadDocIface::showFormulaBar()const
{
    return doc->getShowFormulaBar();
}

bool KSpreadDocIface::showStatusBar()const
{
    return doc->getShowStatusBar();
}

bool KSpreadDocIface::showTabBar()const
{
    return doc->getShowTabBar();
}

QString KSpreadDocIface::headLeft()const
{
    return doc->headLeft();
}

QString KSpreadDocIface::headMid()const
{
    return doc->headMid();
}

QString KSpreadDocIface::headRight()const
{
    return doc->headRight();
}

QString KSpreadDocIface::footLeft()const
{
    return doc->footLeft();
}

QString KSpreadDocIface::footMid()const
{
    return doc->footMid();
}

QString KSpreadDocIface::footRight()const
{
    return doc->footRight();
}

void KSpreadDocIface::setShowVerticalScrollBar(bool _show)
{
    doc->setShowVerticalScrollBar(_show);
    doc->refreshInterface();
}

void KSpreadDocIface::setShowHorizontalScrollBar(bool _show)
{
    doc->setShowHorizontalScrollBar(_show);
    doc->refreshInterface();
}

void KSpreadDocIface::setShowColHeader(bool _show)
{
    doc->setShowColHeader(_show);
    doc->refreshInterface();
}

void KSpreadDocIface::setShowRowHeader(bool _show)
{
    doc->setShowRowHeader(_show);
    doc->refreshInterface();
}

void KSpreadDocIface::setShowTabBar(bool _show)
{
    doc->setShowTabBar(_show);
    doc->refreshInterface();
}

void KSpreadDocIface::setShowCommentIndicator(bool _show)
{
    doc->setShowCommentIndicator(_show);
    doc->refreshInterface();
}

void KSpreadDocIface::setShowMessageError(bool _show)
{
    doc->setShowMessageError(_show);
}

bool KSpreadDocIface::showMessageError()const
{
    return doc->getShowMessageError();
}

void KSpreadDocIface::setHeaderLeft(const QString & text)
{
    doc->setHeadFootLine( text,headMid(), headRight(),
                          footLeft(), footMid(),footRight() );
}

void KSpreadDocIface::setHeaderMiddle(const QString & text)
{
    doc->setHeadFootLine( headLeft(),text, headRight(),
                          footLeft(), footMid(),footRight() );

}

void KSpreadDocIface::setHeaderRight(const QString & text)
{
    doc->setHeadFootLine( headLeft(),headMid(), text,
                          footLeft(), footMid(),footRight() );
}

void KSpreadDocIface::setFooterLeft(const QString & text)
{
    doc->setHeadFootLine( headLeft(),headMid(), headRight(),
                          text, footMid(),footRight() );
}

void KSpreadDocIface::setFooterMiddle(const QString & text)
{
    doc->setHeadFootLine( headLeft(),headMid(), headRight(),
                          footLeft(), text,footRight() );
}

void KSpreadDocIface::setFooterRight(const QString & text)
{
    doc->setHeadFootLine( headLeft(),headMid(), headRight(),
                          footLeft(), footMid(),text );
}

