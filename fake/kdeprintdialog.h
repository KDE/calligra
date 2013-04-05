#ifndef FAKE_KDIALOG_H
#define FAKE_KDIALOG_H

#include <QPrinter>
#include <QPrintDialog>

class KdePrint
{
public:

    static QPrintDialog* createPrintDialog(QPrinter *printJob, QList<QWidget*> options, QWidget *parent)
    {
        QPrintDialog *p = new QPrintDialog(printJob, parent);
        return p;
    }

};

#endif
 
