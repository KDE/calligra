#ifndef FAKE_KSEPARATOR_H
#define FAKE_KSEPARATOR_H

#include <QFrame>
#include <kofake_export.h>

class KOFAKE_EXPORT KSeparator : public QFrame
{
    Q_OBJECT
    Q_PROPERTY( Qt::Orientation orientation READ orientation WRITE setOrientation )
public:
    KSeparator(QWidget* parent=0) : QFrame(parent), m_orientation(Qt::Horizontal) { init(); }
    KSeparator(Qt::Orientation orientation, QWidget* parent=0) : QFrame(parent), m_orientation(orientation) { init(); }
    Qt::Orientation orientation() const { return m_orientation; }
    void setOrientation(Qt::Orientation orientation) { m_orientation = orientation; }
private:
    Qt::Orientation m_orientation;
    void init() { setFixedSize(10,10); }
};

#endif
