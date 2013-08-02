#ifndef FAKE_KDATEPICKER_H
#define FAKE_KDATEPICKER_H

#include <QWidget>

#include <kdatetime.h>

#include "kofake_export.h"


class KDatePicker : public QWidget
{
public:
    KDatePicker(QWidget *parent) : QWidget(parent) {}
};

#endif
