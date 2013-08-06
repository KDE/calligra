#ifndef FAKE_KCOLORBUTTON_H
#define FAKE_KCOLORBUTTON_H

#include <QColor>
#include <QPushButton>
#include <klocale.h>

#include "kofake_export.h"


class KColorButton : public QPushButton
{
public:
    KColorButton(QWidget *parent = 0) : QPushButton(parent) {}
    KColorButton(QColor, QWidget *parent = 0) : QPushButton(parent) {}
    void setAlphaChannelEnabled(bool) {}
    QColor color() const { return QColor(); }
    void setColor(QColor) const {}
    QColor defaultColor() const { return QColor(); }
    void setDefaultColor(QColor) {}
};

#endif
