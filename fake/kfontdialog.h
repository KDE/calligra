#ifndef FAKE_KFONTDIALOG_H
#define FAKE_KFONTDIALOG_H

#include <kdialog.h>

class KFontDialog : public KDialog
{
public:
    KFontDialog(QWidget *p = 0, Qt::WFlags flags = 0) : KDialog(p, flags) {}
};

#endif
