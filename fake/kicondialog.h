#ifndef FAKE_KICONDIALOG_H
#define FAKE_KICONDIALOG_H

#include <kdialog.h>
#include <kicon.h>

class KIconDialog : public KDialog
{
public:
    KIconDialog(QWidget *widget = 0) : KDialog(widget) {}
    static QString getIcon() { return QString(); }
};

#endif
