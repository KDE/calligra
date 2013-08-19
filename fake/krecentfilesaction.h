#ifndef FAKE_KRECENTFILESACTION_H
#define FAKE_KRECENTFILESACTION_H

#include <QList>
#include <kurl.h>
#include <kselectaction.h>
#include <kconfiggroup.h>
#include <kicon.h>
#include "kofake_export.h"
class KOFAKE_EXPORT KRecentFilesAction : public KSelectAction
{
  Q_OBJECT
  Q_PROPERTY( int maxItems READ maxItems WRITE setMaxItems )
public:
    KRecentFilesAction(QObject *parent = 0) : KSelectAction(parent) {}
    KRecentFilesAction(const QString &text, QObject *parent) : KSelectAction(text, parent) {}
    KRecentFilesAction(const KIcon &icon, const QString &text, QObject *parent) : KSelectAction(icon, text, parent) {}
    //void addAction(QAction* action, const KUrl& url, const QString& name);
    //virtual QAction* removeAction(QAction* action);
    virtual void clear() {}
    int maxItems() const { return 00; }
    void setMaxItems( int maxItems ) {}
    void loadEntries( const KConfigGroup &config ) {}
    void saveEntries( const KConfigGroup &config ) {}
    void addUrl( const KUrl& url, const QString& name = QString() ) {}
    void removeUrl( const KUrl& url ) {}
    KUrl::List urls() const { return KUrl::List(); }
Q_SIGNALS:
    void urlSelected( const KUrl& url );
    void recentListCleared();
};

#endif
 
