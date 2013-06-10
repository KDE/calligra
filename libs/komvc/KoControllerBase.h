#ifndef KOCONTROLLERBASE_H
#define KOCONTROLLERBASE_H

#include <QObject>
#include <QAction>

#include "komvc_export.h"

class KoMainWindowBase;

/**
 * @brief The KoControllerBase class is a per-application singleton
 * that keeps a database of all actions and their shortcuts.
 */
class KOMVC_EXPORT KoControllerBase : public QObject
{
    Q_OBJECT
public:
    explicit KoControllerBase(QObject *parent = 0);

    /// Add the default actions for the given window to the controller.
    void addWindow(KoMainWindowBase *window);

    /// @return the named action for the given window, or 0 if none exists
    QAction *actionByName(KoMainWindowBase *window, const QString &name) const;

private slots:

    void windowClosed();

private:

    class Private;
    Private *const d;
};

#endif // KOCONTROLLERBASE_H
