#ifndef FAKE_KDATEWIDGET_H
#define FAKE_KDATEWIDGET_H

#include <QWidget>

#include "kofake_export.h"


class KOFAKE_EXPORT KDateWidget : public QWidget
{
    Q_OBJECT
public:
    enum OperationMode { Other = 0, Opening, Saving };

    KDateWidget(QWidget *parent)
        : QWidget(parent)
    {
    }

    const QDate& date() const {return m_date;}

    bool setDate(const QDate &date) {return true;}

private:
    QDate m_date;

};

#endif
 
