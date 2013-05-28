#ifndef FAKE_KREPLACE_H
#define FAKE_KREPLACE_H

#include <kfind.h>

class KReplace : public KFind
{
public:
    KReplace(const QString &pattern, const QString &replacement, long options, QWidget *parent = 0) : KFind(pattern, options, parent) {}
    KReplace(const QString &pattern, const QString &replacement, long options, QWidget *parent, QWidget* replaceDialog) : KFind(pattern, options, parent, replaceDialog) {}
    virtual ~KReplace() {}

    int numReplacements() const { return 0; }
    virtual void resetCounts() {}
    Result replace() { return NoMatch; }
    KDialog* replaceNextDialog( bool create = false) { return 0; }
    void closeReplaceNextDialog() {}
    static int replace( QString &text, const QString &pattern, const QString &replacement, int index, long options, int *replacedLength ) { return 0; }
    static int replace( QString &text, const QRegExp &pattern, const QString &replacement, int index, long options, int *replacedLength ) { return 0; }
    virtual bool shouldRestart( bool forceAsking = false, bool showNumMatches = true ) const { return false; }
    virtual void displayFinalDialog() const {}
#if 0
Q_SIGNALS:
    void replace(const QString &text, int replacementIndex, int replacedLength, int matchedLength);
#endif
};

#endif
