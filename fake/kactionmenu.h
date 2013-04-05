#ifndef FAKE_KACTIONMENU_H
#define FAKE_KACTIONMENU_H

#include <kaction.h>

class KActionMenu : public KAction
{
public:
    KActionMenu(QObject *parent = 0) : KAction(parent) {}
    KActionMenu(const QString &text, QObject *parent) : KAction(text, parent) {}
    KActionMenu(const KIcon &icon, const QString &text, QObject *parent) : KAction(icon, text, parent) {}
    void addAction(QAction*) {}
    void addSeparator() {}
    void setDelayed(bool) {}
};

#endif
