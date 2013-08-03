#ifndef FAKE_SONNET_CONFIGDIALOG_H
#define FAKE_SONNET_CONFIGDIALOG_H

#include "speller.h"
#include <QDialog>
#include <ksharedconfig.h>
#include <kofake_export.h>

namespace Sonnet
{

class KOFAKE_EXPORT ConfigDialog : public QDialog
{
    Q_OBJECT
public:
    ConfigDialog(QWidget *parent = 0) : QDialog(parent) {}
    ConfigDialog(KSharedConfig*, int = 0) : QDialog() {}
    virtual ~ConfigDialog() {}
    void setLanguage( const QString & ) {}
    QString language() const { return QString(); }
Q_SIGNALS:
    void languageChanged( const QString & );
    void configChanged();
};

}

#endif
