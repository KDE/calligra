#include "KSpreadDocIface.h"
#include <KoDocumentIface.h>

#include "kspread_doc.h"
#include "kspread_map.h"

#include <kapplication.h>
#include <dcopclient.h>
#include <qcolor.h>

KSpreadDocIface::KSpreadDocIface( KSpreadDoc* doc )
    : KoDocumentIface( doc )
{
}

DCOPRef KSpreadDocIface::map()
{
    return DCOPRef( kapp->dcopClient()->appId(),
                    static_cast<KSpreadDoc*>(m_pDoc)->map()->dcopObject()->objId() );
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
    return static_cast<KSpreadDoc*>(m_pDoc)->paperHeight();
}

float KSpreadDocIface::paperWidth()const
{
    return static_cast<KSpreadDoc*>(m_pDoc)->paperWidth();
}

float KSpreadDocIface::leftBorder()const
{
    return static_cast<KSpreadDoc*>(m_pDoc)->leftBorder();
}

float KSpreadDocIface::rightBorder()const
{
    return static_cast<KSpreadDoc*>(m_pDoc)->rightBorder();
}

float KSpreadDocIface::topBorder()const
{
    return static_cast<KSpreadDoc*>(m_pDoc)->topBorder();
}

float KSpreadDocIface::bottomBorder()const
{
    return static_cast<KSpreadDoc*>(m_pDoc)->bottomBorder();
}

QString KSpreadDocIface::paperFormatString() const
{
    return static_cast<KSpreadDoc*>(m_pDoc)->paperFormatString();
}

bool KSpreadDocIface::showColHeader()const
{
    return static_cast<KSpreadDoc*>(m_pDoc)->getShowColHeader();
}

bool KSpreadDocIface::showRowHeader()const
{
    return static_cast<KSpreadDoc*>(m_pDoc)->getShowRowHeader();
}

int KSpreadDocIface::indentValue()const
{
    return static_cast<KSpreadDoc*>(m_pDoc)->getIndentValue();
}

void KSpreadDocIface::setIndentValue(int _val)
{
    static_cast<KSpreadDoc*>(m_pDoc)->setIndentValue(_val);
}

void KSpreadDocIface::changeDefaultGridPenColor( const QColor &_col)
{
    static_cast<KSpreadDoc*>(m_pDoc)->changeDefaultGridPenColor(_col);
}

bool KSpreadDocIface::showCommentIndicator()const
{
    return static_cast<KSpreadDoc*>(m_pDoc)->getShowCommentIndicator();
}

bool KSpreadDocIface::showFormulaBar()const
{
    return static_cast<KSpreadDoc*>(m_pDoc)->getShowFormulaBar();
}

bool KSpreadDocIface::showStatusBar()const
{
    return static_cast<KSpreadDoc*>(m_pDoc)->getShowStatusBar();
}

bool KSpreadDocIface::showTabBar()const
{
    return static_cast<KSpreadDoc*>(m_pDoc)->getShowTabBar();
}
