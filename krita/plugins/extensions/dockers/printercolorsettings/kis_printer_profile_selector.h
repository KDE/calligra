 
#ifndef KISPRINTERPROFILECHOOSER_H
#define KISPRINTERPROFILECHOOSER_H

#include <QWidget>
#include <QPrinterInfo>
#include <QFutureWatcher>
#include <QPrinter>

#include <kis_color_space_selector.h>

#include "ui_wdgprintcolorsettings.h"
#include "ui_wdgprintcolorspaceselector.h"

class KoColorProfile;

class KisPrinterProfileChooser : public KisColorSpaceSelector
{
    Q_OBJECT
public:
    KisPrinterProfileChooser(QWidget *parent = 0);
    ~KisPrinterProfileChooser();
private:
    QPrinter *m_printer;
};

#endif