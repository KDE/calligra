#ifndef FAKE_KDATEPICKER_H
#define FAKE_KDATEPICKER_H

#include <QWidget>

#include <kdatetime.h>

#include "kofake_export.h"


class KDatePicker : public QWidget
{
public:
    KDatePicker(QWidget *parent=0) : QWidget(parent) {}

    const QDate& date() const {return m_date;}

    bool setDate(const QDate &date) {return true;}

private:
    QDate m_date;

};

#endif
