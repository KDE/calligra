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

QColor KSpreadDocIface::pageBorderColor() const
{
    return doc->pageBorderColor();
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

void KSpreadDocIface::changePageBorderColor( const QColor & _color)
{
    doc->changePageBorderColor( _color);
    doc->refreshInterface();
}

void KSpreadDocIface::addIgnoreWordAll( const QString &word)
{
    doc->addIgnoreWordAll( word );
}

void KSpreadDocIface::clearIgnoreWordAll( )
{
    doc->clearIgnoreWordAll();
}

QStringList KSpreadDocIface::spellListIgnoreAll() const
{
    return doc->spellListIgnoreAll();
}

void KSpreadDocIface::addStringCompletion(const QString & stringCompletion)
{
    doc->addStringCompletion( stringCompletion );
}

int KSpreadDocIface::zoom() const
{
    return doc->zoom();
}
