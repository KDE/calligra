#ifndef FAKE_KCOMPLETION_H
#define FAKE_KCOMPLETION_H

#include <QObject>
#include <kglobalsettings.h>

class KCompletion : public QObject
{
    //Q_ENUMS( CompOrder )
    //Q_PROPERTY( CompOrder order READ order WRITE setOrder )
    //Q_PROPERTY( bool ignoreCase READ ignoreCase WRITE setIgnoreCase )
    //Q_PROPERTY( QStringList items READ items WRITE setItems )
    //Q_OBJECT
public:
    enum CompOrder { Sorted, Insertion, Weighted };
    KCompletion() : QObject() {}
    virtual ~KCompletion() {}

#if 0
    /**
     * Attempts to find an item in the list of available completions,
     * that begins with @p string. Will either return the first matching item
     * (if there is more than one match) or QString(), if no match was
     * found.
     *
     * In the latter case, a sound will be issued, depending on
     * soundsEnabled().
     * If a match was found, it will also be emitted via the signal
     * match().
     *
     * If this is called twice or more often with the same string while no
     * items were added or removed in the meantime, all available completions
     * will be emitted via the signal #matches().
     * This happens only in shell-completion-mode.
     *
     * @param string the string to complete
     * @return the matching item, or QString() if there is no matching
     * item.
     * @see slotMakeCompletion
     * @see substringCompletion
     */
    virtual QString makeCompletion( const QString& string );

    /**
     * Returns a list of all completion items that contain the given @p string.
     * @param string the string to complete
     * @return a list of items which all contain @p text as a substring,
     * i.e. not necessarily at the beginning.
     *
     * @see makeCompletion
     */
    QStringList substringCompletion( const QString& string ) const;

    /**
     * Returns the next item from the matching-items-list.
     * When reaching the beginning, the list is rotated so it will return the
     * last match and a sound is issued (depending on soundsEnabled()).
     * @return the next item from the matching-items-list.
     * When there is no match, QString() is returned and
     * a sound is be issued.
     * @see slotPreviousMatch
     */
    QString previousMatch();

    /**
     * Returns the next item from the matching-items-list.
     * When reaching the last item, the list is rotated, so it will return
     * the first match and a sound is issued (depending on
     * soundsEnabled()).
     * @return the next item from the matching-items-list.  When there is no
     * match, QString() is returned and a sound is issued
     * @see slotNextMatch
     */
    QString nextMatch();

    /**
     * Returns the last match. Might be useful if you need to check whether
     * a completion is different from the last one.
     * @return the last match. QString() is returned when there is no
     *         last match.
     */
    virtual const QString& lastMatch() const;
#endif

    QStringList items() const { return QStringList(); }
    bool isEmpty() const { return true; }

#if 0
    /**
     * Sets the completion mode to Auto/Manual, Shell or None.
     * If you don't set the mode explicitly, the global default value
     * KGlobalSettings::completionMode() is used.
     * KGlobalSettings::CompletionNone disables completion.
     * @param mode the completion mode
     * @see completionMode
     * @see KGlobalSettings::completionMode
     */
    virtual void setCompletionMode( KGlobalSettings::Completion mode );

    /**
     * Return the current completion mode.
     * May be different from KGlobalSettings::completionMode(), if you
     * explicitly called setCompletionMode().
     * @return the current completion mode
     * @see setCompletionMode
     */
    KGlobalSettings::Completion completionMode() const;

    /**
     * KCompletion offers three different ways in which it offers its items:
     * @li in the order of insertion
     * @li sorted alphabetically
     * @li weighted
     *
     * Choosing weighted makes KCompletion perform an implicit weighting based
     * on how often an item is inserted. Imagine a web browser with a location
     * bar, where the user enters URLs. The more often a URL is entered, the
     * higher priority it gets.
     *
     * Note: Setting the order to sorted only affects new inserted items,
     * already existing items will stay in the current order. So you probably
     * want to call setOrder( Sorted ) before inserting items, when you want
     * everything sorted.
     *
     * Default is insertion order.
     * @param order the new order
     * @see order
     */
    virtual void setOrder( CompOrder order );

    /**
     * Returns the completion order.
     * @return the current completion order.
     * @see setOrder
     */
    CompOrder order() const;

    /**
     * Setting this to true makes KCompletion behave case insensitively.
     * E.g. makeCompletion( "CA" ); might return "carp\@cs.tu-berlin.de".
     * Default is false (case sensitive).
     * @param ignoreCase true to ignore the case
     * @see ignoreCase
     */
    virtual void setIgnoreCase( bool ignoreCase );

    /**
     * Return whether KCompletion acts case insensitively or not.
     * Default is false (case sensitive).
     * @return true if the case will be ignored
     * @see setIgnoreCase
     */
    bool ignoreCase() const;

    /**
     * Returns a list of all items matching the last completed string.
     * Might take some time, when you have LOTS of items.
     * @return a list of all matches for the last completed string.
     * @see substringCompletion
     */
    QStringList allMatches();

    /**
     * Returns a list of all items matching @p string.
     * @param string the string to match
     * @return the list of all matches
     */
    QStringList allMatches( const QString& string );

    /**
     * Returns a list of all items matching the last completed string.
     * Might take some time, when you have LOTS of items.
     * The matches are returned as KCompletionMatches, which also
     * keeps the weight of the matches, allowing
     * you to modify some matches or merge them with matches
     * from another call to allWeightedMatches(), and sort the matches
     * after that in order to have the matches ordered correctly.
     *
     * @return a list of all completion matches
     * @see substringCompletion
     */
    KCompletionMatches allWeightedMatches();

    /**
     * Returns a list of all items matching @p string.
     * @param string the string to match
     * @return a list of all matches
     */
    KCompletionMatches allWeightedMatches( const QString& string );

    /**
     * Enables/disables playing a sound when
     * @li makeCompletion() can't find a match
     * @li there is a partial completion (= multiple matches in
     *     Shell-completion mode)
     * @li nextMatch() or previousMatch() hit the last possible
     *     match -> rotation
     *
     * For playing the sounds, KNotifyClient() is used.
     *
     * @param enable true to enable sounds
     * @see soundsEnabled
     */
    virtual void setSoundsEnabled( bool enable );

    /**
     * Tells you whether KCompletion will play sounds on certain occasions.
     * Default is enabled.
     * @return true if sounds are enabled
     * @see setSoundsEnabled
     */
    bool soundsEnabled() const;

    /**
     * Returns true when more than one match is found.
     * @return true if there are more than one match
     * @see multipleMatches
     */
    bool hasMultipleMatches() const;

public Q_SLOTS:
    /**
     * Attempts to complete "string" and emits the completion via match().
     * Same as makeCompletion() (just as a slot).
     * @param string the string to complete
     * @see makeCompletion
     */
    void slotMakeCompletion( const QString& string ) { //inline (redirect)
        (void) makeCompletion( string );
    }

    /**
     * Searches the previous matching item and emits it via match().
     * Same as previousMatch() (just as a slot).
     * @see previousMatch
     */
    void slotPreviousMatch() { //inline (redirect)
        (void) previousMatch();
    }

    /**
     * Searches the next matching item and emits it via match().
     * Same as nextMatch() (just as a slot).
     * @see nextMatch
     */
    void slotNextMatch() { //inline (redirect)
        (void) nextMatch();
    }
#endif

    void insertItems( const QStringList& items ) {}
    virtual void setItems( const QStringList& list) {}
    void addItem( const QString& item) {}
    void addItem( const QString& item, uint weight ) {}
    void removeItem( const QString& item) {}
    virtual void clear() {}

#if 0
Q_SIGNALS:
    /**
     * The matching item. Will be emitted by makeCompletion(),
     * previousMatch() or nextMatch(). May be QString() if there
     * is no matching item.
     * @param item the match, or QString() if there is none
     */
    void match( const QString& item);

    /**
     * All matching items. Will be emitted by makeCompletion() in shell-
     * completion-mode, when the same string is passed to makeCompletion twice
     * or more often.
     * @param matchlist the list of matches
     */
    void matches( const QStringList& matchlist);

    /**
     * This signal is emitted, when calling makeCompletion() and more than
     * one matching item is found.
     * @see hasMultipleMatches
     */
    void multipleMatches();
#endif
};

#if 0
// some more helper stuff
typedef KSortableList<QString> KCompletionMatchesList;
class KCompletionMatchesPrivate;

/**
 * This structure is returned by KCompletion::allWeightedMatches .
 * It also keeps the weight of the matches, allowing
 * you to modify some matches or merge them with matches
 * from another call to allWeightedMatches(), and sort the matches
 * after that in order to have the matches ordered correctly
 *
 * Example (a simplified example of what Konqueror's completion does):
 * \code
 * KCompletionMatches matches = completion->allWeightedMatches( location );
 * if( !location.startsWith( "www." ))
 matches += completion->allWeightedmatches( "www." + location" );
 * matches.removeDuplicates();
 * QStringList list = matches.list();
 * \endcode
 *
 * @short List for keeping matches returned from KCompletion
 */
class KDEUI_EXPORT KCompletionMatches : public KCompletionMatchesList
{
public:
    KCompletionMatches( bool sort );
    KCompletionMatches( const KCompletionMatches& );
    KCompletionMatches &operator=( const KCompletionMatches& );
    ~KCompletionMatches();
    /**
     * Removes duplicate matches. Needed only when you merged several matches
     * results and there's a possibility of duplicates.
     */
    void removeDuplicates();
    /**
     * Returns the matches as a QStringList.
     * @param sort if false, the matches won't be sorted before the conversion,
     *             use only if you're sure the sorting is not needed
     * @return the list of matches
     */
    QStringList list( bool sort = true ) const;
    /**
     * If sorting() returns false, the matches aren't sorted by their weight,
     * even if true is passed to list().
     * @return true if the matches won't be sorted
     */
    bool sorting() const;
};

/**
 * An abstract base class for adding a completion feature
 * into widgets.
 *
 * This is a convenience class that provides the basic functions
 * needed to add text completion support into widgets.  All that
 * is required is an implementation for the pure virtual function
 * setCompletedText.  Refer to KLineEdit or KComboBox
 * to see how easily such support can be added using this as a base
 * class.
 *
 * @short An abstract class for adding text completion support to widgets.
 * @author Dawit Alemayehu <adawit@kde.org>
 */
class KDEUI_EXPORT KCompletionBase
{
public:
    /**
     * Constants that represent the items whose short-cut
     * key-binding is programmable.  The default key-bindings
     * for these items are defined in KStandardShortcut.
     */
    enum KeyBindingType {
        /**
         * Text completion (by default Ctrl-E).
         */
        TextCompletion,
        /**
         * Switch to previous completion (by default Ctrl-Up).
         */
        PrevCompletionMatch,
        /**
         * Switch to next completion (by default Ctrl-Down).
         */
        NextCompletionMatch,
        /**
         * Substring completion (by default Ctrl-T).
         */
        SubstringCompletion
    };


    // Map for the key binding types mentioned above.
    typedef QMap<KeyBindingType, KShortcut> KeyBindingMap;

    /**
     * Default constructor.
     */
    KCompletionBase();

    /**
     * Destructor.
     */
    virtual ~KCompletionBase();

    /**
     * Returns a pointer to the current completion object.
     *
     * If the completion object does not exist, it is automatically created and
     * by default handles all the completion signals internally unless @p hsig
     * is set to false. It is also automatically destroyed when the destructor
     * is called. You can change this default behavior using the
     * @ref setAutoDeleteCompletionObject and @ref setHandleSignals member
     * functions.
     *
     * See also @ref compObj.
     *
     * @param hsig if true, handles completion signals internally.
     * @return a pointer the completion object.
     */
    KCompletion* completionObject( bool hsig = true );

    /**
     * Sets up the completion object to be used.
     *
     * This method assigns the completion object and sets it up to automatically
     * handle the completion and rotation signals internally.  You should use
     * this function if you want to share one completion object among your
     * widgets or need to use a customized completion object.
     *
     * The object assigned through this method is not deleted when this object's
     * destructor is invoked unless you explicitly call @ref setAutoDeleteCompletionObject
     * after calling this method. Be sure to set the bool argument to false, if
     * you want to handle the completion signals yourself.
     *
     * @param compObj a KCompletion() or a derived child object.
     * @param hsig if true, handles completion signals internally.
     */
    virtual void setCompletionObject( KCompletion* compObj, bool hsig = true );

    /**
     * Enables this object to handle completion and rotation
     * events internally.
     *
     * This function simply assigns a boolean value that
     * indicates whether it should handle rotation and
     * completion events or not.  Note that this does not
     * stop the object from emitting signals when these
     * events occur.
     *
     * @param handle if true, handle completion & rotation internally.
     */
    virtual void setHandleSignals( bool handle );

    /**
     * Returns true if the completion object is deleted
     * upon this widget's destruction.
     *
     * See setCompletionObject() and enableCompletion()
     * for details.
     *
     * @return true if the completion object will be deleted
     *              automatically
     */
    bool isCompletionObjectAutoDeleted() const;

    /**
     * Sets the completion object when this widget's destructor
     * is called.
     *
     * If the argument is set to true, the completion object
     * is deleted when this widget's destructor is called.
     *
     * @param autoDelete if true, delete completion object on destruction.
     */
    void setAutoDeleteCompletionObject( bool autoDelete );

    /**
     * Sets the widget's ability to emit text completion and
     * rotation signals.
     *
     * Invoking this function with @p enable set to @p false will
     * cause the completion & rotation signals not to be emitted.
     * However, unlike setting the completion object to @p NULL
     * using setCompletionObject, disabling the emition of
     * the signals through this method does not affect the current
     * completion object.
     *
     * There is no need to invoke this function by default.  When a
     * completion object is created through completionObject or
     * setCompletionObject, these signals are set to emit
     * automatically.  Also note that disabling this signals will not
     * necessarily interfere with the objects ability to handle these
     * events internally.  See setHandleSignals.
     *
     * @param enable if false, disables the emition of completion & rotation signals.
     */
    void setEnableSignals( bool enable );

    /**
     * Returns true if the object handles the signals.
     *
     * @return true if this signals are handled internally.
     */
    bool handleSignals() const;

    /**
     * Returns true if the object emits the signals.
     *
     * @return true if signals are emitted
     */
    bool emitSignals() const;

    /**
     * Sets the type of completion to be used.
     *
     * The completion modes supported are those defined in
     * KGlobalSettings().  See below.
     *
     * @param mode Completion type:
     *   @li CompletionNone:  Disables completion feature.
     *   @li CompletionAuto:  Attempts to find a match &
     *                        fills-in the remaining text.
     *   @li CompletionMan:   Acts the same as the above
     *                        except the action has to be
     *                        manually triggered through
     *                        pre-defined completion key.
     *   @li CompletionShell: Mimics the completion feature
     *                        found in typical *nix shell
     *                        environments.
     *   @li CompletionPopup: Shows all available completions at once,
     *                        in a listbox popping up.
     */
    virtual void setCompletionMode( KGlobalSettings::Completion mode );

    /**
     * Returns the current completion mode.
     *
     * The return values are of type KGlobalSettings::Completion.
     * See setCompletionMode() for details.
     *
     * @return the completion mode.
     */
    KGlobalSettings::Completion completionMode() const;

    /**
     * Sets the key-binding to be used for manual text
     * completion, text rotation in a history list as
     * well as a completion list.
     *
     *
     * When the keys set by this function are pressed, a
     * signal defined by the inheriting widget will be activated.
     * If the default value or 0 is specified by the second
     * parameter, then the key-binding as defined in the global
     * setting should be used.  This method returns false value
     * for @p key is negative or the supplied key-binding conflicts
     * with the ones set for one of the other features.
     *
     * NOTE: To use a modifier key (Shift, Ctrl, Alt) as part of
     * the key-binding simply simply @p sum up the values of the
     * modifier and the actual key.  For example, to use CTRL+E as
     * a key binding for one of the items, you would simply supply
     * @p "Qt::CtrlButton + Qt::Key_E" as the second argument to this
     * function.
     *
     * @param item the feature whose key-binding needs to be set:
     *   @li TextCompletion the manual completion key-binding.
     *   @li PrevCompletionMatch    the previous match key for multiple completion.
     *   @li NextCompletionMatch    the next match key for for multiple completion.
     *   @li SubstringCompletion  the key for substring completion
     * @param key key-binding used to rotate down in a list.
     * @return true if key-binding can successfully be set.
     * @see getKeyBinding
     */
    bool setKeyBinding( KeyBindingType item , const KShortcut& key );

    /**
     * Returns the key-binding used for the specified item.
     *
     * This methods returns the key-binding used to activate
     * the feature feature given by @p item.  If the binding
     * contains modifier key(s), the SUM of the modifier key
     * and the actual key code are returned.
     *
     * @param item the item to check
     * @return the key-binding used for the feature given by @p item.
     * @see setKeyBinding
     */
    KShortcut getKeyBinding( KeyBindingType item ) const;

    /**
     * Sets this object to use global values for key-bindings.
     *
     * This method changes the values of the key bindings for
     * rotation and completion features to the default values
     * provided in KGlobalSettings.
     *
     * NOTE: By default inheriting widgets should uses the
     * global key-bindings so that there will be no need to
     * call this method.
     */
    void useGlobalKeyBindings();

    /**
     * A pure virtual function that must be implemented by
     * all inheriting classes.
     *
     * This function is intended to allow external completion
     * implementations to set completed text appropriately.  It
     * is mostly relevant when the completion mode is set to
     * CompletionAuto and CompletionManual modes. See
     * KCompletionBase::setCompletedText.
     * Does nothing in CompletionPopup mode, as all available
     * matches will be shown in the popup.
     *
     * @param text the completed text to be set in the widget.
     */
    virtual void setCompletedText( const QString& text ) = 0;

    /**
     * A pure virtual function that must be implemented by
     * all inheriting classes.
     * @param items the list of completed items
     * @param autoSuggest if @c true, the first element of @p items
     *        is auto-completed (i.e. pre-selected).
     */
    virtual void setCompletedItems( const QStringList& items, bool autoSuggest =true ) = 0;

    /**
     * Returns a pointer to the completion object.
     *
     * This method is only different from completionObject()
     * in that it does not create a new KCompletion object even if
     * the internal pointer is @c NULL. Use this method to get the
     * pointer to a completion object when inheriting so that you
     * won't inadvertently create it!!
     *
     * @return the completion object or @c NULL if one does not exist.
     */
    KCompletion* compObj() const;
};
#endif

#endif
