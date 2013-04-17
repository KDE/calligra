#ifndef FAKE_KCOMBOBOX_H
#define FAKE_KCOMBOBOX_H

#include <QComboBox>

#include "kofake_export.h"


class KComboBox : public QComboBox
{
public:
    KComboBox(QWidget *parent = 0) : QComboBox(parent) {}
};

#endif
