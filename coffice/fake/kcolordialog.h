#ifndef FAKE_KCOLORDIALOG_H
#define FAKE_KCOLORDIALOG_H

#include <QColorDialog>

class KColorDialog : public QColorDialog
{
public:
    KColorDialog(QWidget *parent = 0) : QColorDialog(parent) {}

    static int getColor(QColor &color) {
        QColor c = QColorDialog::getColor();
        if (!c.isValid())
            return QDialog::Rejected;
        color = c;
        return QDialog::Accepted;
    }
};

#endif
 
 
