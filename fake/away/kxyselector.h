#ifndef FAKE_KXYSELECTOR_H
#define FAKE_KXYSELECTOR_H

#include <QWidget>

class KXYSelector : public QWidget
{
public:
    KXYSelector( QWidget *parent=0 ) : QWidget(parent) {}
    void setValues( int xPos, int yPos ) {}
    void setXValue( int xPos ) {}
    void setYValue( int yPos ) {}
    void setRange( int minX, int minY, int maxX, int maxY ) {}
    void setMarkerColor( const QColor &col ) {}
    int xValue() const { return 0; }
    int yValue() const { return 0; }
    QRect contentsRect() const { return QRect(); }
    virtual QSize minimumSizeHint() const { return QSize(); }
};

#endif
