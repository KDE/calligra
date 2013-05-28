#ifndef FAKE_KNEWPASSWORDDIALOG_H
#define FAKE_KNEWPASSWORDDIALOG_H

#include <kpassworddialog.h>

class KNewPasswordDialog : public KPasswordDialog
{
public:
    KNewPasswordDialog(QWidget *parent = 0) : KPasswordDialog(parent) {}
};

#endif
