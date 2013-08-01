#ifndef FAKE_KPUSHBUTTON_H
#define FAKE_KPUSHBUTTON_H

#include <QPushButton>
#include <QDebug>

#include <kguiitem.h>

#include "kofake_export.h"

class KOFAKE_EXPORT KPushButton : public QPushButton
{
    Q_OBJECT
public:
    KPushButton(QWidget *parent = 0) : QPushButton(parent) {}
    KPushButton(const QString &text, QWidget *parent = 0) : QPushButton(text, parent) {}
    void setGuiItem(const KGuiItem &item) { setText(item.text()); }
};

#endif
