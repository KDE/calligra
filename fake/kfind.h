#ifndef FAKE_KFIND_H
#define FAKE_KFIND_H

#include <QObject>
#include <QWidget>
#include <kdialog.h>

#include "kofake_export.h"

class KOFAKE_EXPORT KFind : public QObject
{
    Q_OBJECT

public:

    /// the options
    enum Options
    {
        WholeWordsOnly = 1,     ///< Match whole words only.
        FromCursor = 2,         ///< Start from current cursor position.
        SelectedText = 4,       ///< Only search selected area.
        CaseSensitive = 8,      ///< Consider case when matching.
        FindBackwards = 16,     ///< Go backwards.
        RegularExpression = 32, ///< Interpret the pattern as a regular expression.
        FindIncremental = 64,   ///< Find incremental.
        // Note that KReplaceDialog uses 256 and 512
        // User extensions can use boolean options above this value.
        MinimumUserOption = 65536 ///< user options start with this bit
    };    
#if 0
    Q_DECLARE_FLAGS(SearchOptions, Options)
#endif

    KFind(const QString &pattern, long options, QWidget *parent) {}
    KFind(const QString &pattern, long options, QWidget *parent, QWidget* findDialog) {}
    virtual ~KFind() {}

    enum Result { NoMatch, Match };

    bool needData() const { return false; }
    void setData( const QString& data, int startPos = -1 ) {}
    void setData( int id, const QString& data, int startPos = -1 ) {}
    Result find() { return NoMatch; }
    long options() const{ return 0; }
    virtual void setOptions( long options ) {}
    QString pattern() const{ return QString(); }
    void setPattern( const QString& pattern ) {}
    int numMatches() const{ return 0; }
    virtual void resetCounts() {}
    virtual bool validateMatch( const QString & text, int index, int matchedlength ) { return false; }
    virtual bool shouldRestart( bool forceAsking = false, bool showNumMatches = true ) const { return false; }
    static int find( const QString &text, const QString &pattern, int index, long options, int *matchedlength ) { return 0; }
    static int find( const QString &text, const QRegExp &pattern, int index, long options, int *matchedlength ) { return 0; }
    virtual void displayFinalDialog() const {}
    KDialog* findNextDialog( bool create = false ) { return 0; }
    void closeFindNextDialog() {}
    int index() const { return 0; }

#if 0
Q_SIGNALS:
    void highlight(const QString &text, int matchingIndex, int matchedLength);
    void highlight(int id, int matchingIndex, int matchedLength);
    void findNext();
    void optionsChanged();
    void dialogClosed();
protected:
    QWidget* parentWidget() const;
    QWidget* dialogsParent() const;
#endif
};

#if 0
Q_DECLARE_OPERATORS_FOR_FLAGS(KFind::SearchOptions)
#endif

#endif
