#ifndef FAKE_KACTIONCOLLECTION_H
#define FAKE_KACTIONCOLLECTION_H

#include <QString>
#include <QStringList>
#include <QList>
#include <QObject>
#include <QWidget>

#include <kaction.h>
#include <kstandardaction.h>

#include "kofake_export.h"


class KOFAKE_EXPORT KActionCollection : public QObject
{
    Q_OBJECT
public:
    KActionCollection(QObject *parent = 0) : QObject(parent) {}
    QAction* action(const QString &name) const {
        int index = m_actionNames.lastIndexOf(name);
        return index < 0 ? 0 : m_actions[index];
    }
    QList<QAction*> actions() const { return m_actions; }
    void addAction(const QString &name, QAction *action) {
        m_actionNames.append(name);
        m_actions.append(action);
    }
    void addAction(KStandardAction::StandardAction, const QString &name, QAction *action) {
        m_actionNames.append(name);
        m_actions.append(action);
    }
    KAction* addAction(KStandardAction::StandardAction, const QString &name, QObject *receiver, const char *slot) {
        KAction *a = new KAction(name, this);
        connect(a, SIGNAL(triggered(bool)), receiver, slot);
        m_actionNames.append(name);
        m_actions.append(a);
        return a;
    }
    void takeAction(QAction *action) {
        do {
            int index = m_actions.indexOf(action);
            if (index < 0)
                break;
            m_actionNames.removeAt(index);
            m_actions.removeAt(index);
        } while(true);
    }
    void addAssociatedWidget(QWidget*) {}
private:
    QList<QAction*> m_actions;
    QStringList m_actionNames;
};

#endif
