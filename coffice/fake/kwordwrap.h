#ifndef FAKE_KWORDWRAP_H
#define FAKE_KWORDWRAP_H

#include <QRect>
#include <QString>
#include <QPainter>

class KWordWrap
{
public:
    enum { FadeOut = 0x10000000, Truncate = 0x20000000 };
    static KWordWrap* formatText( QFontMetrics &fm, const QRect & r, int flags, const QString & str, int len = -1 ) { return 0; }
    QRect boundingRect() const { return QRect(); }
    QString wrappedString() const { return QString(); }
    QString truncatedString( bool dots = true ) const { return QString(); }
    void drawText( QPainter *painter, int x, int y, int flags = Qt::AlignLeft ) const {}
    static void drawFadeoutText( QPainter *p, int x, int y, int maxW, const QString &t ) {}
    static void drawTruncateText( QPainter *p, int x, int y, int maxW, const QString &t ) {}
};

#endif
