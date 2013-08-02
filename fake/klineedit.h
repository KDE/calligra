#ifndef FAKE_KLINEEDIT_H
#define FAKE_KLINEEDIT_H

#include <QLineEdit>

#include <QGridLayout>
#include <klocale.h>
#include <kglobalsettings.h>
#include <kcompletion.h>

class KLineEdit : public QLineEdit
{
public:
    KLineEdit(QWidget *parent = 0) : QLineEdit(parent) {}
    void setClearButtonShown(bool) {}
    void setClickMessage(QString) {}

    virtual void setCompletionMode( KGlobalSettings::Completion mode ) {}
    void setCompletionModeDisabled( KGlobalSettings::Completion mode, bool disable = true ) {}
    virtual void setCompletionObject( KCompletion *, bool hsig = true ) {}

};

#endif
