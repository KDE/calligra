#include "kspread_events.h"

// ----------------------------------------------------

using namespace KSpread;

const char *SelectionChanged::s_strSelectionChanged = "KSpread/View/SelectionChanged";

SelectionChanged::SelectionChanged( const Region& region, const QString& sheet )
    : KParts::Event( s_strSelectionChanged )
{
    m_region = region;
    m_sheet = sheet;
}

SelectionChanged::~SelectionChanged()
{
}

