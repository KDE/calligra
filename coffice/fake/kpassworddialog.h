#ifndef FAKE_KPASSWORDDIALOG_H
#define FAKE_KPASSWORDDIALOG_H

#include <kdialog.h>

class KPasswordDialog : public KDialog
{
public:
    KPasswordDialog(QWidget *parent = 0) : KDialog(parent) {}
    void setPrompt(const QString &prompt) {}
    QString prompt() const { return QString(); }
    void setPassword( const QString& password ) {}
    QString password() const { return QString(); }
};

#endif
