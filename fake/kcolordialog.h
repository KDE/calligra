#ifndef FAKE_KCOLORDIALOG_H
#define FAKE_KCOLORDIALOG_H

#include <QColorDialog>

#include "kofake_export.h"


class KColorDialog : public QColorDialog
{
public:
    KColorDialog(QWidget *parent = 0) : QColorDialog(parent) {}

    static int getColor(QColor &color, QWidget *parent = 0) {
        QColor c = QColorDialog::getColor(color, parent);
        if (!c.isValid())
            return QDialog::Rejected;
        color = c;
        return QDialog::Accepted;
    }
};

#endif
 
 
