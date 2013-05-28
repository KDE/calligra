#ifndef FAKE_KCURSOR_H
#define FAKE_KCURSOR_H

#include <QCursor>

class KCursor : public QCursor
{
public:
    KCursor( const QString & name, Qt::CursorShape fallback = Qt::ArrowCursor ) : QCursor(fallback) {}
    KCursor( const QCursor & cursor ) : QCursor(cursor) {}
    static void setAutoHideCursor( QWidget *w, bool enable, bool customEventFilter = false ) {}
    static void setHideCursorDelay( int ms ) {}
    static int hideCursorDelay() { return 1000; }
    static void autoHideEventFilter( QObject *, QEvent * ) {}
};

#endif
