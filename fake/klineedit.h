#ifndef FAKE_KLINEEDIT_H
#define FAKE_KLINEEDIT_H

#include <QLineEdit>

#include <QGridLayout>
#include <klocale.h>

class KLineEdit : public QLineEdit
{
public:
    KLineEdit(QWidget *parent = 0) : QLineEdit(parent) {}
    void setClearButtonShown(bool) {}
    void setClickMessage(QString) {}
};

#endif
