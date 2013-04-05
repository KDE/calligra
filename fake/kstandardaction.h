#ifndef FAKE_KSTANDARDACTION_H
#define FAKE_KSTANDARDACTION_H

#include <kaction.h>
#include <kstandardshortcut.h>
#include <krecentfilesaction.h>

#include <QObject>
#include <QMetaObject>
#include <QMetaEnum>

class KStandardAction : public QObject
{
    Q_OBJECT
    Q_ENUMS(StandardAction)
public:
    enum StandardAction {
        ActionNone,

        // File Menu
        New, Open, OpenRecent, Save, SaveAs, Revert, Close,
        Print, PrintPreview, Mail, Quit,

        // Edit Menu
        Undo, Redo, Cut, Copy, Paste, SelectAll, Deselect, Find, FindNext, FindPrev,
        Replace,

        // View Menu
        ActualSize, FitToPage, FitToWidth, FitToHeight, ZoomIn, ZoomOut,
        Zoom, Redisplay,

        // Go Menu
        Up, Back, Forward, Home /*Home page*/, Prior, Next, Goto, GotoPage, GotoLine,
        FirstPage, LastPage, DocumentBack, DocumentForward,

        // Bookmarks Menu
        AddBookmark, EditBookmarks,

        // Tools Menu
        Spelling,

        // Settings Menu
        ShowMenubar, ShowToolbar, ShowStatusbar,
        SaveOptions, KeyBindings,
        Preferences, ConfigureToolbars,

        // Help Menu
        Help, HelpContents, WhatsThis, ReportBug, AboutApp, AboutKDE,
        TipofDay,

        // Other standard actions
        ConfigureNotifications,
        FullScreen,
        Clear,
        PasteText,
        SwitchApplicationLanguage
    };

    static QString name(StandardAction a)
    {
        return QString();
//        KStandardAction sa;
//        int idx = sa.metaObject()->indexOfEnumerator("StandardAction");
//        Q_ASSERT(idx >= 0);
//        QMetaEnum e = sa.metaObject()->enumerator(idx);
//        return QString::fromLatin1(e.valueToKey(int(a)));
    }

#if 0
/**
   * Creates an action corresponding to one of the
   * KStandardAction::StandardAction actions, which is connected to the given
   * object and @p slot, and is owned by @p parent.
   *
   * The signal that is connected to @p slot is triggered(bool), except for the case of
   * OpenRecent standard action, which uses the urlSelected(const KUrl &) signal of
   * KRecentFilesAction.
   *
   * @param id The StandardAction identifier to create a KAction for.
   * @param recvr The QObject to receive the signal, or 0 if no notification
   *              is needed.
   * @param slot  The slot to connect the signal to (remember to use the SLOT() macro).
   * @param parent The QObject that should own the created KAction, or 0 if no parent will
   *               own the KAction returned (ensure you delete it manually in this case).
   */
  KDEUI_EXPORT KAction* create(StandardAction id, const QObject *recvr, const char *slot, QObject *parent);

  /**
   * This will return the internal name of a given standard action.
   */
  KDEUI_EXPORT const char* name( StandardAction id );

  /**
   * Returns a list of all standard names. Used by KAccelManager
   * to give those heigher weight.
   */
  KDEUI_EXPORT QStringList stdNames();

  /**
   * Returns a list of all actionIds.
   *
   * @since 4.2
   */
  KDEUI_EXPORT QList<StandardAction> actionIds();

  /**
   * Returns the standardshortcut associated with @a actionId.
   *
   * @param actionId    The actionId whose associated shortcut is wanted.
   *
   * @since 4.2
   */
  KDEUI_EXPORT KStandardShortcut::StandardShortcut shortcutForActionId(StandardAction id);
#endif

  static KAction *openNew(const QObject *recvr, const char *slot, QObject *parent)
  {
      return 0;
  }

  static KAction *open(const QObject *recvr, const char *slot, QObject *parent)
  {
      return 0;
  }

  static KRecentFilesAction *openRecent(const QObject *recvr, const char *slot, QObject *parent)
  {
      KRecentFilesAction *a = new KRecentFilesAction(i18n("Open recent"), parent);
      connect(a, SIGNAL(triggered(bool)), recvr, slot);
      return a;
  }

#if 0
  /**
   * Save the current document.
   */
  KDEUI_EXPORT KAction *save(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Save the current document under a different name.
   */
  KDEUI_EXPORT KAction *saveAs(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Revert the current document to the last saved version
   * (essentially will undo all changes).
   */
  KDEUI_EXPORT KAction *revert(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Close the current document.
   */
  KDEUI_EXPORT KAction *close(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Print the current document.
   */
  KDEUI_EXPORT KAction *print(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Show a print preview of the current document.
   */
  KDEUI_EXPORT KAction *printPreview(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Mail this document.
   */
  KDEUI_EXPORT KAction *mail(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Quit the program.
   *
   * Note that you probably want to connect this action to either QWidget::close()
   * or QApplication::closeAllWindows(), but not QApplication::quit(), so that
   * KMainWindow::queryClose() is called on any open window (to warn the user
   * about unsaved changes for example).
   */
  KDEUI_EXPORT KAction *quit(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Undo the last operation.
   */
  KDEUI_EXPORT KAction *undo(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Redo the last operation.
   */
  KDEUI_EXPORT KAction *redo(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Cut selected area and store it in the clipboard.
   * Calls cut() on the widget with the current focus.
   */
  KDEUI_EXPORT KAction *cut(QObject *parent);

  /**
   * Copy selected area and store it in the clipboard.
   * Calls copy() on the widget with the current focus.
   */
  KDEUI_EXPORT KAction *copy(QObject *parent);

  /**
   * Paste the contents of clipboard at the current mouse or cursor
   * Calls paste() on the widget with the current focus.
   */
  KDEUI_EXPORT KAction *paste(QObject *parent);

  /**
   * Clear selected area.  Calls clear() on the widget with the current focus.
   * Note that for some widgets, this may not provide the intended behavior.  For
   * example if you make use of the code above and a K3ListView has the focus, clear()
   * will clear all of the items in the list.  If this is not the intened behavior
   * and you want to make use of this slot, you can subclass K3ListView and reimplement
   * this slot.  For example the following code would implement a K3ListView without this
   * behavior:
   *
   * \code
   * class MyListView : public K3ListView {
   *   Q_OBJECT
   * public:
   *   MyListView( QWidget * parent = 0, const char * name = 0, WFlags f = 0 ) : K3ListView( parent, name, f ) {}
   *   virtual ~MyListView() {}
   * public Q_SLOTS:
   *   virtual void clear() {}
   * };
   * \endcode
   */
   KDEUI_EXPORT KAction *clear(QObject *parent);

  /**
   * Calls selectAll() on the widget with the current focus.
   */
  KDEUI_EXPORT KAction *selectAll(QObject *parent);

  /**
   * Cut selected area and store it in the clipboard.
   */
  KDEUI_EXPORT KAction *cut(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Copy the selected area into the clipboard.
   */
  KDEUI_EXPORT KAction *copy(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Paste the contents of clipboard at the current mouse or cursor
   * position.
   */
  KDEUI_EXPORT KAction *paste(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Paste the contents of clipboard at the current mouse or cursor
   * position. Provide a button on the toolbar with the clipboard history
   * menu if Klipper is running.
   */
  KDEUI_EXPORT KAction *pasteText(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Clear the content of the focus widget
   */
  KDEUI_EXPORT KAction *clear(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Select all elements in the current document.
   */
  KDEUI_EXPORT KAction *selectAll(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Deselect any selected elements in the current document.
   */
  KDEUI_EXPORT KAction *deselect(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Initiate a 'find' request in the current document.
   */
  KDEUI_EXPORT KAction *find(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Find the next instance of a stored 'find'.
   */
  KDEUI_EXPORT KAction *findNext(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Find a previous instance of a stored 'find'.
   */
  KDEUI_EXPORT KAction *findPrev(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Find and replace matches.
   */
  KDEUI_EXPORT KAction *replace(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * View the document at its actual size.
   */
  KDEUI_EXPORT KAction *actualSize(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Fit the document view to the size of the current window.
   */
  KDEUI_EXPORT KAction *fitToPage(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Fit the document view to the width of the current window.
   */
  KDEUI_EXPORT KAction *fitToWidth(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Fit the document view to the height of the current window.
   */
  KDEUI_EXPORT KAction *fitToHeight(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Zoom in.
   */
  KDEUI_EXPORT KAction *zoomIn(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Zoom out.
   */
  KDEUI_EXPORT KAction *zoomOut(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Popup a zoom dialog.
   */
  KDEUI_EXPORT KAction *zoom(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Redisplay or redraw the document.
   */
  KDEUI_EXPORT KAction *redisplay(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Move up (web style menu).
   */
  KDEUI_EXPORT KAction *up(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Move back (web style menu).
   */
  KDEUI_EXPORT KAction *back(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Move forward (web style menu).
   */
  KDEUI_EXPORT KAction *forward(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Go to the "Home" position or document.
   */
  KDEUI_EXPORT KAction *home(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Scroll up one page.
   */
  KDEUI_EXPORT KAction *prior(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Scroll down one page.
   */
  KDEUI_EXPORT KAction *next(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Go to somewhere in general.
   */
  KDEUI_EXPORT KAction *goTo(const QObject *recvr, const char *slot, QObject *parent);


  /**
   * Go to a specific page (dialog).
   */
  KDEUI_EXPORT KAction *gotoPage(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Go to a specific line (dialog).
   */
  KDEUI_EXPORT KAction *gotoLine(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Jump to the first page.
   */
  KDEUI_EXPORT KAction *firstPage(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Jump to the last page.
   */
  KDEUI_EXPORT KAction *lastPage(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Move back (document style menu).
   */
  KDEUI_EXPORT KAction *documentBack(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Move forward (document style menu).
   */
  KDEUI_EXPORT KAction *documentForward(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Add the current page to the bookmarks tree.
   */
  KDEUI_EXPORT KAction *addBookmark(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Edit the application bookmarks.
   */
  KDEUI_EXPORT KAction *editBookmarks(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Pop up the spell checker.
   */
  KDEUI_EXPORT KAction *spelling(const QObject *recvr, const char *slot, QObject *parent);


  /**
   * Show/Hide the menubar.
   */
  KDEUI_EXPORT KToggleAction *showMenubar(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Show/Hide the statusbar.
   */
  KDEUI_EXPORT KToggleAction *showStatusbar(const QObject *recvr, const char *slot, QObject *parent);

  /**
   * Switch to/from full screen mode
   */
  KDEUI_EXPORT KToggleFullScreenAction *fullScreen(const QObject *recvr, const char *slot, QWidget *window, QObject *parent);

#endif

  static KAction *saveOptions(const QObject *recvr, const char *slot, QObject *parent) { return 0; }
  static KAction *keyBindings(const QObject *recvr, const char *slot, QObject *parent) { return 0; }
  static KAction *preferences(const QObject *recvr, const char *slot, QObject *parent) { return 0; }
  static KAction *configureToolbars(const QObject *recvr, const char *slot, QObject *parent) { return 0; }
  static KAction *configureNotifications(const QObject *recvr, const char *slot, QObject *parent) { return 0; }
  static KAction *help(const QObject *recvr, const char *slot, QObject *parent) { return 0; }
  static KAction *helpContents(const QObject *recvr, const char *slot, QObject *parent) { return 0; }
  static KAction *whatsThis(const QObject *recvr, const char *slot, QObject *parent) { return 0; }
  static KAction *tipOfDay(const QObject *recvr, const char *slot, QObject *parent) { return 0; }
  static KAction *reportBug(const QObject *recvr, const char *slot, QObject *parent) { return 0; }
  static KAction *aboutApp(const QObject *recvr, const char *slot, QObject *parent) { return 0; }
  static KAction *aboutKDE(const QObject *recvr, const char *slot, QObject *parent) { return 0; }

};

#endif
