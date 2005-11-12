#include "kspread_events.h"

// ----------------------------------------------------

using namespace KSpread;

const char *SelectionChanged::s_strSelectionChanged = "KSpread/View/SelectionChanged";

SelectionChanged::SelectionChanged( const QRect& rect, const QString& sheet )
    : KParts::Event( s_strSelectionChanged )
{
    m_rect = rect;
    m_sheet = sheet;
}

SelectionChanged::~SelectionChanged()
{
}

