#ifndef FAKE_KSHORTCUT_H
#define FAKE_KSHORTCUT_H

#include <QList>
#include <QShortcut>
#include <QKeySequence>
#include <QSharedPointer>
#include <QDebug>

class KShortcut : public QKeySequence
{
public:
    KShortcut() : QKeySequence() {}
    KShortcut(const QKeySequence &primary) : QKeySequence(primary) {}
    KShortcut(const QKeySequence &primary, const QKeySequence &alternate) : QKeySequence(primary) {}
    KShortcut(int keyQtPri, int keyQtAlt = 0) : QKeySequence() {}
    KShortcut(const QString &description) : QKeySequence() {}
    KShortcut(const QList<QKeySequence> &seqs) : QKeySequence() {}

    QKeySequence primary() const { return *this; }
    QKeySequence alternate() const { return QKeySequence(); }

#if 0
    bool contains(const QKeySequence &needle) const;
    bool conflictsWith(const QKeySequence &needle) const;

    bool operator==(const KShortcut &other) const;
    bool operator!=(const KShortcut &other) const;
#endif

    operator QList<QKeySequence>() const { return toList(); }

    enum EmptyHandling { KeepEmpty = 0, RemoveEmpty };
    QList<QKeySequence> toList(enum EmptyHandling handleEmpty = RemoveEmpty) const { return QList<QKeySequence>() << *this; }

    operator QVariant() const { return QVariant(); }

    void setPrimary(const QKeySequence &keySeq) {}
    void setAlternate(const QKeySequence &keySeq) {}
    void remove(const QKeySequence &keySeq, enum EmptyHandling handleEmpty = RemoveEmpty) {}
};

#endif
