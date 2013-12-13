#ifndef FAKE_KACTIONMENU_H
#define FAKE_KACTIONMENU_H

#include <kaction.h>
#include <QMenu>

#include "kofake_export.h"


class KActionMenu : public KAction
{
public:
    KActionMenu(QObject *parent = 0) : KAction(parent) { init(); }
    KActionMenu(const QString &text, QObject *parent) : KAction(text, parent) { init(); }
    KActionMenu(const KIcon &icon, const QString &text, QObject *parent) : KAction(icon, text, parent) { init(); }
    virtual ~KActionMenu() { delete m_menu; }
    void addAction(QAction *a) { menu()->addAction(a); }
    void addSeparator() { menu()->addSeparator(); }
    void setDelayed(bool) {}
private:
    QMenu *m_menu;
    void init() { m_menu = new QMenu(); setMenu(m_menu); }
};

#endif
