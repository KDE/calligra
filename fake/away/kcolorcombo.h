#ifndef FAKE_KCOLORCOMBO_H
#define FAKE_KCOLORCOMBO_H

#include <QColor>
#include <QComboBox>

class KColorCombo : public QComboBox
{
public:
    KColorCombo( QWidget *parent = 0 ) : QComboBox(parent) {}
    void setColor( const QColor &col ) {}
    QColor color() const { return QColor(); }
    bool isCustomColor() const { return false; }
    void setColors(const QList<QColor> &colors ) {}
    QList<QColor> colors() const { return QList<QColor>(); }
    void showEmptyList() {}
};

#endif


