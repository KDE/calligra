#ifndef KSPREAD_EVENTS
#define KSPREAD_EVENTS

#include <qevent.h>
#include <qrect.h>
#include <qstring.h>

#include <string.h>

class Event : public QEvent
{
public:
    Event();
    ~Event();
    
    virtual const char* eventName() const = 0;
};

class KSpreadSelectionChanged : public Event
{
public:
    KSpreadSelectionChanged( const QRect&, const QString& table );
    ~KSpreadSelectionChanged();

    QRect rect() const { return m_rect; }
    QString table() const { return m_table; }
    
    const char* eventName() const { return "KSpread/View/SelectionChanged"; }
    
    static bool test( QEvent* e );
    
private:
    QRect m_rect;
    QString m_table;
};

#endif
