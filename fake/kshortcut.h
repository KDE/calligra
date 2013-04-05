#ifndef FAKE_KSHORTCUT_H
#define FAKE_KSHORTCUT_H

#include <QShortcut>
#include <QKeySequence>
#include <QSharedPointer>
#include <QDebug>

class KShortcut : public QKeySequence
{
public:
    //KShortcut() : QShortcut(0) {}
    KShortcut() : QKeySequence() {}
    //KShortcut() {}
private:
    //Q_DISABLE_COPY(KShortcut)
    //QSharedPointer<QShortcut> m_shortcut;
};

#endif
