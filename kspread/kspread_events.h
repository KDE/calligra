#ifndef KSPREAD_EVENTS
#define KSPREAD_EVENTS

#include <qevent.h>
#include <qrect.h>
#include <qstring.h>

#include <string.h>

#include <kparts/event.h>
#include <koffice_export.h>

class KSPREAD_EXPORT KSpreadSelectionChanged : public KParts::Event
{
public:
    KSpreadSelectionChanged( const QRect&, const QString& sheet );
    ~KSpreadSelectionChanged();

    QRect rect() const { return m_rect; }
    QString sheet() const { return m_sheet; }

    static bool test( const QEvent* e ) { return KParts::Event::test( e, s_strSelectionChanged ); }

private:
    static const char *s_strSelectionChanged;
    QRect m_rect;
    QString m_sheet;
};

#endif
