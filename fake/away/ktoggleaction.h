#ifndef FAKE_KTOGGLEACTION_H
#define FAKE_KTOGGLEACTION_H

#include <kaction.h>
#include <kguiitem.h>

class KToggleAction : public KAction
{
public:
    KToggleAction(QObject *parent) : KAction(parent) { setCheckable(true); }
    KToggleAction(const QString &text, QObject *parent) : KAction(text, parent) { setCheckable(true); }
    KToggleAction(const KIcon &icon, const QString &text, QObject *parent) : KAction(icon, text, parent) { setCheckable(true); }
    void setCheckedState( const KGuiItem& checkedItem ) {}
};

#endif
