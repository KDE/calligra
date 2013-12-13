#ifndef FAKE_KHELPMENU_H
#define FAKE_KHELPMENU_H

#include <QObject>
#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QString>

#include <kaboutdata.h>

class KHelpMenu : public QObject
{
    Q_OBJECT
  public:
    explicit KHelpMenu( QWidget *parent=0, const QString &aboutAppText=QString(), bool showWhatsThis=true ) : QObject(parent) {}
    KHelpMenu( QWidget *parent, const KAboutData &aboutData, bool showWhatsThis=true ) : QObject(parent) {}
    QMenu *menu() { return 0; }
    enum MenuId { menuHelpContents = 0, menuWhatsThis = 1, menuAboutApp = 2, menuAboutKDE = 3, menuReportBug = 4, menuSwitchLanguage = 5 };
    QAction *action( MenuId id ) const { return 0; }
  public Q_SLOTS:
    void appHelpActivated() {}
    void contextHelpActivated() {}
    void aboutApplication() {}
    void aboutKDE() {}
    void reportBug() {}
    void switchApplicationLanguage() {}
};

#endif
