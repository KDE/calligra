#ifndef FAKE_KACTION_H
#define FAKE_KACTION_H

#include <QObject>
#include <QWidgetAction>

#include <kurl.h>
#include <kicon.h>
#include <klocale.h>
#include "kofake_export.h"

class KOFAKE_EXPORT KAction : public QWidgetAction
{
    Q_OBJECT
public:
    KAction(QObject *parent = 0)
        : QWidgetAction(parent) {}
    virtual ~KAction() {}
    KAction(const QString &text, QObject *parent)
        : QWidgetAction(parent)

    { setText(text); }
    KAction(const KIcon &icon, const QString &text, QObject *parent)
        : QWidgetAction(parent) { setIcon(icon); setText(text); }

    void setHelpText(const QString &help) {
        if (whatsThis().isEmpty())
                setWhatsThis(help);
    }
};

#endif
