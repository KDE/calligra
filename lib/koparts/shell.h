#ifndef OFFICE_SHELL_H
#define OFFICE_SHELL_H

#include <qlist.h>
#include <qaction.h>
#include <qdom.h>

#include <ktmainwindow.h>
#include <ktoolbar.h>
#include <kmenubar.h>

class QPopupMenu;
class QString;
class KStatusBar;

class Part;
class View;

class Shell : public KTMainWindow
{
    Q_OBJECT
public:
    enum SelectionPolicy { Direct, TriState };

    /**
     *  Constructor.
     *
     *  Initailizes the shell with menubar, toolbars, statusbar and view.
     *
     *  @param parent Parent widget of the shell.
     *  @param name   Name of the shell widget.
     *
     *  @see initShell
     */
    Shell( QWidget* parent = 0, const char* name = 0 );

    /**
     *  Destructor.
     */
    ~Shell();

    /**
     *  Initializes the shell.
     *
     *  Initailizes the shell with menubar, toolbars, statusbar and view. 
     */
    virtual void initShell();

    /**
     *  Retrieves the action collection of the shell.
     *
     *  @see action
     */
    QActionCollection* actionCollection();

    /**
     *  Sets the main part of this shell.
     */
    virtual void setRootPart( Part* );
    
    /**
     *  Retrieves the main part of this shell
     *
     *  @see setRootPart rootView
     */
    Part* rootPart();

    /**
     *  Retrieves the main view of this shell.
     *
     *  @see rootPart
     */
    View* rootView();

    /**
     * Setting the active view to 0 shows only the menus of
     * the shell, otherwise the menus and toolbars of the
     * view will be shown, too.
     *
     * @see createToolBars createMenuBar
     */
    void setActiveView( View* view, Part* part = 0 );
    
    View* activeView();
    
    Part* activePart();

    void setSelectedView( View* view, Part* part = 0 );
    
    View* selectedView();
    
    Part* selectedPart();

    void setSelectionPolicy( SelectionPolicy );
    
    SelectionPolicy selectionPolicy();

    /**
     *  Creates a new statusbar, activates it and returns the pointer to it.
     */
    KStatusBar *createStatusBar();

    KToolBar *viewToolBar( const char *name );
    
    KMenuBar *viewMenuBar( const char *name );

    virtual void setDoPartActivation( bool b )
    {
	m_bDoPartActivation = b;
    }
    
    bool doPartActivation() const
    {
	return m_bDoPartActivation;
    }

protected:

    void createToolBars( const QDomElement& element );

    void createMenuBar( const QDomElement& shell, const QDomElement& part );

    QPopupMenu* createMenu( const QDomElement& shell, const QDomElement& part );

    /**
     * Creates a toolbar for a view. The bar is appended to @ref #m_toolbars
     * and will be destructed if the currently active view becomes deactivated.
     *
     * @ref #createToolBars
     */
    KToolBar* createToolBar( const char* name = 0 );

    /**
     * This event filter detected focus changes. This is used
     * to detect when the user wants to activate another view.
     */
    bool eventFilter( QObject*, QEvent* );

    QString readConfigFile( const QString& filename ) const;

    /**
     *  Retrieves the resource file of this shell.
     *
     *  You have to overload it to retrieve your special resource file.
     *
     *  @return Name of the resource file.
     */
    virtual QString configFile() const = 0;

    /**
     *  Retrieves an actioen given by its name.
     *
     *  @param name Name of the action.
     *
     *  @return The action or NULL if no action with given name found.
     *
     *  @ref actionCollection
     */
    QAction* action( const char* name, bool shell = FALSE );

private:

    View* m_activeView;
    Part* m_activePart;

    View* m_selectedView;
    Part* m_selectedPart;

    Part* m_rootPart;

    QList<KToolBar> m_toolbars;

    QActionCollection m_collection;

    SelectionPolicy m_policy;

    KStatusBar *m_statusBar;

    bool m_bDoPartActivation;
};

#endif
