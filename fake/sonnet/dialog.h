#ifndef FAKE_SONNET_DIALOG_H
#define FAKE_SONNET_DIALOG_H

#include <kdialog.h>

namespace Sonnet
{

class BackgroundChecker;

class Dialog : public KDialog
{
public:
    Dialog(BackgroundChecker *checker, QWidget *parent) : KDialog(parent) {}
    QString originalBuffer() const { return QString(); }
    QString buffer() const { return QString(); }
    void show() {}
    void activeAutoCorrect(bool _active) {}

    // Hide warning about done(), which is a slot in QDialog and a signal here.
    //using KDialog::done;

    void showProgressDialog(int timeout = 500) {}
    void showSpellCheckCompletionMessage( bool b = true ) {}
    void setSpellCheckContinuedAfterReplacement( bool b ) {}
    void setBuffer(const QString &) {}
};

}

#endif


