#ifndef FAKE_KREPLACEDIALOG_H
#define FAKE_KREPLACEDIALOG_H

#include <kfinddialog.h>

class KReplaceDialog : public KFindDialog
{
public:
    enum Options { PromptOnReplace = 256, BackReference = 512 };
    explicit KReplaceDialog( QWidget *parent = 0, long options = 0, const QStringList &findStrings = QStringList(), const QStringList &replaceStrings = QStringList(), bool hasSelection = true ) : KFindDialog(parent, options, findStrings, hasSelection) {}
    void setReplacementHistory( const QStringList &history ) { m_replacementHistory = history; }
    QStringList replacementHistory() const { return m_replacementHistory; }
    //void setOptions( long options ) {}
    //long options() const {}
    QString replacement() const { return m_replacement; }
    QWidget *replaceExtension() const { return 0; }
private:
    QStringList m_replacementHistory;
    QString m_replacement;
};

#endif
