#ifndef FAKE_KPARTS_EVENT_H
#define FAKE_KPARTS_EVENT_H

//#include <QComboBox>

//#include <kurl.h>
//#include <kio/netaccess.h>
//#include <kconfiggroup.h>
//#include <kcomponentdata.h>

//class QWidget;

#define KPARTS_EVENT_TYPE QEvent::User+50

namespace KParts
{
class Part;

class Event : public QEvent
{
public:
    //Event( const char *eventName ) : QEvent() {}
    Event(int t) : QEvent(QEvent::Type(t)) {}
    Event(QEvent::Type t) : QEvent(t) {}
#if 0
    virtual const char *eventName() const;
    static bool test( const QEvent *event, const char *name );
#endif
    static bool test( const QEvent *event ) { return dynamic_cast<const Event*>(event); }
};

class GUIActivateEvent : public Event
{
public:
    GUIActivateEvent(bool activated) : Event(KPARTS_EVENT_TYPE), m_activated(activated) {}
    GUIActivateEvent(QEvent::Type t, bool activated) : Event(t), m_activated(activated) {}
    bool activated() const {}
    static bool test( const QEvent *event ) { return dynamic_cast<const GUIActivateEvent*>(event); }
private:
    bool m_activated;
};

class PartActivateEvent : public Event
{
public:
    PartActivateEvent( bool activated, Part *part, QWidget *widget ) : Event(KPARTS_EVENT_TYPE+1), m_activated(activated), m_part(part), m_widget(widget) {}
    Part *part() const { return m_part; }
    QWidget *widget() const { return m_widget; }
    bool activated() const { return m_activated; }
    static bool test( const QEvent *event ) { return dynamic_cast<const PartActivateEvent*>(event); }
private:
    bool m_activated;
    Part *m_part;
    QWidget *m_widget;
};

class PartSelectEvent : public Event
{
public:
    PartSelectEvent( bool selected, Part *part, QWidget *widget ) : Event(KPARTS_EVENT_TYPE+2), m_selected(selected), m_part(part), m_widget(widget) {}
    bool selected() const { return m_selected; }
    Part *part() const { return m_part; }
    QWidget *widget() const { return m_widget; }
    static bool test( const QEvent *event ) { return dynamic_cast<const PartSelectEvent*>(event); }
private:
    bool m_selected;
    Part *m_part;
    QWidget *m_widget;
};

} // namespace

#endif 
