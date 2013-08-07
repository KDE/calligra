#ifndef FAKE_KDATEWIDGET_H
#define FAKE_KDATEWIDGET_H

#include <QWidget>

#include "kofake_export.h"


class KOFAKE_EXPORT KDateWidget : public QWidget
{
    Q_OBJECT
public:
    KDateWidget(QWidget *parent)
        : QWidget(parent)
        , m_date(QDate())
    {
    }

    const QDate& date() const {return m_date;}

    bool setDate(const QDate &date) {m_date=date; return true;}

private:
    QDate m_date;

};

#endif
 
