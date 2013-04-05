#ifndef FAKE_KFINDDIALOG_H
#define FAKE_KFINDDIALOG_H

#include <kdialog.h>
#include <QStringList>

class KFindDialog : public KDialog
{
public:
    explicit KFindDialog( QWidget *parent = 0, long options = 0, const QStringList &findStrings = QStringList(), bool hasSelection = false, bool replaceDialog = false ) : KDialog(parent), m_options(options) {}
    void setFindHistory( const QStringList &history ) { m_history = history; }
    QStringList findHistory() const { return m_history; }
    void setHasSelection( bool hasSelection ) {}
    void setHasCursor( bool hasCursor ) {}
    void setSupportsBackwardsFind( bool supports ) {}
    void setSupportsCaseSensitiveFind( bool supports ) {}
    void setSupportsWholeWordsFind( bool supports ) {}
    void setSupportsRegularExpressionFind( bool supports ) {}
    void setOptions( long options ) { m_options = options; }
    long options() const { return m_options; }
    QString pattern() const { return m_pattern; }
    void setPattern ( const QString &pattern ) { m_pattern = pattern; }
    QWidget *findExtension() const { return 0; }
private:
    QStringList m_history;
    long m_options;
    QString m_pattern;
};

#endif
