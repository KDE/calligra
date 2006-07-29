#ifndef KSPREAD_EVENTS
#define KSPREAD_EVENTS

#include <qevent.h>
#include <qrect.h>
#include <qstring.h>

#include <string.h>

#include <kparts/event.h>
#include <koffice_export.h>

#include "region.h"
namespace KSpread
{

class KSPREAD_EXPORT SelectionChanged : public KParts::Event
{
public:
    SelectionChanged( const Region&, const QString& sheet );
    ~SelectionChanged();

    Region region() const { return m_region; }
    QString sheet() const { return m_sheet; }

    static bool test( const QEvent* e ) { return KParts::Event::test( e, s_strSelectionChanged ); }

private:
    static const char *s_strSelectionChanged;
    Region m_region;
    QString m_sheet;
};

} // namespace KSpread

#endif
