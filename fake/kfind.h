#ifndef FAKE_KFIND_H
#define FAKE_KFIND_H

#include <QObject>
#include <QWidget>
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

#if 0
    /**
     * @return true if the application must supply a new text fragment
     * It also means the last call returned "NoMatch". But by storing this here
     * the application doesn't have to store it in a member variable (between
     * calls to slotFindNext()).
     */
    bool needData() const;

    /**
     * Call this when needData returns true, before calling find().
     * @param data the text fragment (line)
     * @param startPos if set, the index at which the search should start.
     * This is only necessary for the very first call to setData usually,
     * for the 'find in selection' feature. A value of -1 (the default value)
     * means "process all the data", i.e. either 0 or data.length()-1 depending
     * on FindBackwards.
     */
    void setData( const QString& data, int startPos = -1 );

    /**
     * Call this when needData returns true, before calling find(). The use of
     * ID's is especially useful if you're using the FindIncremental option.
     * @param id the id of the text fragment
     * @param data the text fragment (line)
     * @param startPos if set, the index at which the search should start.
     * This is only necessary for the very first call to setData usually,
     * for the 'find in selection' feature. A value of -1 (the default value)
     * means "process all the data", i.e. either 0 or data.length()-1 depending
     * on FindBackwards.
     */
    void setData( int id, const QString& data, int startPos = -1 );

    /**
     * Walk the text fragment (e.g. text-processor line, kspread cell) looking for matches.
     * For each match, emits the highlight() signal and displays the find-again dialog
     * proceeding.
     */
    Result find();

    /**
     * Return the current options.
     *
     * Warning: this is usually the same value as the one passed to the constructor,
     * but options might change _during_ the replace operation:
     * e.g. the "All" button resets the PromptOnReplace flag.
     *
     * @see KFind::Options
     */
    long options() const;

    /**
     * Set new options. Usually this is used for setting or clearing the
     * FindBackwards options.
     *
     * @see KFind::Options
     */
    virtual void setOptions( long options );

    /**
     * @return the pattern we're currently looking for
     */
    QString pattern() const;

    /**
     * Change the pattern we're looking for
     */
    void setPattern( const QString& pattern );

    /**
     * Return the number of matches found (i.e. the number of times
     * the highlight signal was emitted).
     * If 0, can be used in a dialog box to tell the user "no match was found".
     * The final dialog does so already, unless you used setDisplayFinalDialog(false).
     */
    int numMatches() const;

    /**
     * Call this to reset the numMatches count
     * (and the numReplacements count for a KReplace).
     * Can be useful if reusing the same KReplace for different operations,
     * or when restarting from the beginning of the document.
     */
    virtual void resetCounts();

    /**
     * Virtual method, which allows applications to add extra checks for
     * validating a candidate match. It's only necessary to reimplement this
     * if the find dialog extension has been used to provide additional
     * criterias.
     *
     * @param text  The current text fragment
     * @param index The starting index where the candidate match was found
     * @param matchedlength The length of the candidate match
     */
    virtual bool validateMatch( const QString & text,
                                int index,
                                int matchedlength );

    /**
     * Returns true if we should restart the search from scratch.
     * Can ask the user, or return false (if we already searched the whole document).
     *
     * @param forceAsking set to true if the user modified the document during the
     * search. In that case it makes sense to restart the search again.
     *
     * @param showNumMatches set to true if the dialog should show the number of
     * matches. Set to false if the application provides a "find previous" action,
     * in which case the match count will be erroneous when hitting the end,
     * and we could even be hitting the beginning of the document (so not all
     * matches have even been seen).
     */
    virtual bool shouldRestart( bool forceAsking = false, bool showNumMatches = true ) const;

    /**
     * Search the given string, and returns whether a match was found. If one is,
     * the length of the string matched is also returned.
     *
     * A performance optimised version of the function is provided for use
     * with regular expressions.
     *
     * @param text The string to search.
     * @param pattern The pattern to look for.
     * @param index The starting index into the string.
     * @param options The options to use.
     * @param matchedlength The length of the string that was matched
     * @return The index at which a match was found, or -1 if no match was found.
     */
    static int find( const QString &text, const QString &pattern, int index, long options, int *matchedlength );

    static int find( const QString &text, const QRegExp &pattern, int index, long options, int *matchedlength );

    /**
     * Displays the final dialog saying "no match was found", if that was the case.
     * Call either this or shouldRestart().
     */
    virtual void displayFinalDialog() const;

    /**
     * Return (or create) the dialog that shows the "find next?" prompt.
     * Usually you don't need to call this.
     * One case where it can be useful, is when the user selects the "Find"
     * menu item while a find operation is under way. In that case, the
     * program may want to call setActiveWindow() on that dialog.
     */
    KDialog* findNextDialog( bool create = false );

    /**
     * Close the "find next?" dialog. The application should do this when
     * the last match was hit. If the application deletes the KFind, then
     * "find previous" won't be possible anymore.
     *
     * IMPORTANT: you should also call this if you are using a non-modal
     * find dialog, to tell KFind not to pop up its own dialog.
     */
    void closeFindNextDialog();

    /**
     * @return the current matching index ( or -1 ).
     * Same as the matchingIndex parameter passed to highlight.
     * You usually don't need to use this, except maybe when updating the current data,
     * so you need to call setData( newData, index() ).
     */
    int index() const;

Q_SIGNALS:

    /**
     * Connect to this signal to implement highlighting of found text during the find
     * operation.
     *
     * If you've set data with setData(id, text), use the signal highlight(id,
     * matchingIndex, matchedLength)
     *
     * WARNING: If you're using the FindIncremental option, the text argument
     * passed by this signal is not necessarily the data last set through
     * setData(), but can also be an earlier set data block.
     *
     * @see setData()
     */
    void highlight(const QString &text, int matchingIndex, int matchedLength);

    /**
     * Connect to this signal to implement highlighting of found text during the find
     * operation.
     *
     * Use this signal if you've set your data with setData(id, text), otherwise
     * use the signal with highlight(text, matchingIndex, matchedLength).
     *
     * WARNING: If you're using the FindIncremental option, the id argument
     * passed by this signal is not necessarily the id of the data last set
     * through setData(), but can also be of an earlier set data block.
     *
     * @see setData()
     */
    void highlight(int id, int matchingIndex, int matchedLength);

    // ## TODO docu
    // findprevious will also emit findNext, after temporarily switching the value
    // of FindBackwards
    void findNext();

    /**
     * Emitted when the options have changed.
     * This can happen e.g. with "Replace All", or if our 'find next' dialog
     * gets a "find previous" one day.
     */
    void optionsChanged();

    /**
     * Emitted when the 'find next' dialog is being closed.
     * Some apps might want to remove the highlighted text when this happens.
     * Apps without support for "Find Next" can also do m_find->deleteLater()
     * to terminate the find operation.
     */
    void dialogClosed();

protected:
    QWidget* parentWidget() const;
    QWidget* dialogsParent() const;

private:
    friend class KReplace;
    friend class KReplacePrivate;
    struct Private;
    Private* const d;
    Q_PRIVATE_SLOT( d, void _k_slotFindNext() )
    Q_PRIVATE_SLOT( d, void _k_slotDialogClosed() )
#endif
};

#if 0
Q_DECLARE_OPERATORS_FOR_FLAGS(KFind::SearchOptions)
#endif

#endif
