#ifndef FAKE_KRECENTDOCUMENT_H
#define FAKE_KRECENTDOCUMENT_H

#include <QString>
#include <QStringList>

class KRecentDocument
{
public:
    static QStringList recentDocuments() { return QStringList(); }
    static void add(const KUrl& url) {}
    static void add(const KUrl& url, const QString& desktopEntryName) {}
    static void add(const QString &documentStr, bool isURL = false) {}
    static void clear() {}
    static int maximumItems() { return 100; }
    static QString recentDocumentDirectory() { return QString(); }
};


#endif
 
