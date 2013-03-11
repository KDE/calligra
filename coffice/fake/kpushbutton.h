#ifndef FAKE_KPUSHBUTTON_H
#define FAKE_KPUSHBUTTON_H

#include <QPushButton>
#include <QDebug>

#include <kguiitem.h>

class KPushButton : public QPushButton
{
public:
    KPushButton(QWidget *parent = 0) : QPushButton(parent) {}
    KPushButton(const QString &text, QWidget *parent = 0) : QPushButton(text, parent) {}
    void setGuiItem(const KGuiItem &item) { setText(item.text()); }
};

#endif
