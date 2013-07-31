#ifndef FAKE_SONNET_CONFIGWIDGET_H
#define FAKE_SONNET_CONFIGWIDGET_H

#include <QWidget>
#include <kconfig.h>

namespace Sonnet
{

class ConfigWidget : public QWidget
{
public:
    ConfigWidget(KConfig *config, QWidget *parent) : QWidget(parent) {}
    bool backgroundCheckingButtonShown() const { return false; }
    void setLanguage( const QString &language ) {}
    QString language() const { return QString(); }
    void save() {}
    void setBackgroundCheckingButtonShown( bool ) {}
    void slotDefault() {}
};

}

#endif

