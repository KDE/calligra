#include "kspread_events.h"

// ----------------------------------------------------

const char *KSpreadSelectionChanged::s_strSelectionChanged = "KSpread/View/SelectionChanged";

KSpreadSelectionChanged::KSpreadSelectionChanged( const QRect& rect, const QString& sheet )
    : KParts::Event( s_strSelectionChanged )
{
    m_rect = rect;
    m_sheet = sheet;
}

KSpreadSelectionChanged::~KSpreadSelectionChanged()
{
}

