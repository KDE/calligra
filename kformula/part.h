/* KFormula - Formula Editor
 *
 * This code is free. Do with it what you want.
 *
 * (c) KDE Project
 * Andrea Rizzi
 * Bilibao@ouverture.it
 */

#ifndef PART_H
#define PART_H

class FormulaEditor;

#include <kpart/kpart.h>
#include <korb/korb.h>
//#include <kapp.h>
#include <qprinter.h>
#include <qlist.h>
#include <qpopmenu.h>

#include "widget.h"

/**
 */

 
class FormulaEditor : public KPart
{
    Q_OBJECT
public:
    /**
     * IDs for the help menu
     *
     * @see #slotHelp
     */
    enum{ ID_HELP_ABOUT=900, ID_HELP_USING, ID_HELP_KDE };

    FormulaEditor( KPartShell *_shell, QWidget *_parent = 0L );
    ~FormulaEditor();

    /**
     * @see KPart::activate
     */
    virtual void activate( bool _status );

    /**
     * @see KPart::showGUI
     */
    virtual void showGUI();
    /**
     * @see KPart::hideGUI
     */
    virtual void hideGUI();
    /**
     * @see KPart::showFileMenu
     */
    virtual void showFileMenu();
    
    /**
     * @param _url is a valid URL.
     */
    virtual bool save( const char *_url );

    /**
     * @param _korb is the KorbSession which has to be used for writing.
     */
    virtual OBJECT save( KorbSession *_korb );

    /**
     * Saves all of your children. This is just a convenience function.
     */
    virtual bool saveChildren( KorbSession *_korb, OBJECT _o_obj );
    /**
     * You can do easy saving here. This is just a convenience function.
     */
    virtual bool save( QIODevice *_device );
    
    /**
     * @param _url is a valid URL.
     */
    virtual bool load( const char *_url );

    /**
     * Load the data using the @ref KorbSession.
     *
     * @param _korb is the KorbSession which has to be used for reading.
     * @param _obj is the object id of the object we want to read.
     */
    virtual bool load( KorbSession *_korb, OBJECT _obj );

    /**
     * Loads all of your children.
     *
     * @see #saveChilden
     */
    virtual bool loadChildren( KorbSession *_korb, OBJECT _o_obj );
    /**
     * You can implement easy loading here.
     *
     * @see #save
     */
    virtual bool load( QIODevice *_device );

    /**
     * @return the URL of this part if it has been created from a file ( @ref #load )
     *         or if it has been saved to a file ( @ref #save ). Otherwise the
     *         return value is 0L.
     */
    const char* getURL() { return fileURL.data(); }

    virtual bool hasDocumentChanged() { return documentChanged; }
    virtual void setDocumentChanged( bool _c ) { documentChanged = _c; }
    /**
     * This function is called if the user wants to close this part
     * or its parent. You have the right for a "veto".
     *
     */
    virtual bool closePart();

    /**
     * Insert the name of your lib here without the "lib" prefix.
     * Usually this name is in lower case letters.
     */
    virtual const char* getLibName() { return "mypart"; };

    /**
     * Check wether you are the parent of the KPart.
     */
    virtual bool isParentOf( KPart *_part );

    /**
     * Tell that we need space for the scrollbars if we are
     * active.
     */
    virtual int getRightGUISize() { return 20; }
    /**
     * Tell that we need space for the scrollbars if we are
     * active.
     */
    virtual int getBottomGUISize() { return 20; }

    void print( QPainter &_painter, bool _asChild = TRUE );
    

   // void about();
   // void HtmlHelp();
  //  void chtext(const char * text);
  //  void chtype(int type);
protected:
    /**
     * This slot is called in order of inserting a new part.
     *
     * @see #insertNewPart
     */
public slots:
         void slotInsertPart( int _id );

    void slotSaveAs();
    void slotSave();
    void slotOpen();
    void slotClose();
    void slotQuit();
    void slotPrint();
    void slotHelp( int _id );
    
protected:
    /**
     * This function really inserts a new part. The part is just appened
     * to @ref #childList.
     */
    void insertNewPart( KPartEntry* _entry );

    /**
     * Pointer to your apps widget.
     */
    Kformulaw *kformulaw;
    
    /**
     * The URL of the this part. This variable is only set if the @ref #load function
     * had been called with an URL as argument.
     *
     * @see #load
     */
    QString fileURL;

    /**
     * Indicates wether the user should save the document before deleting it.
     *
     * @see #hasDocumentChanged
     */
    bool documentChanged;

    /**
     * List of all children. Children are all embedded KParts.
     * This is just an idea of how to store your children. You may do it
     * different an delete this variable, but mention that you have to change
     * the load/save functions and @ref #insertNewPart then.
     */
    QList<KPart> childList;

    /**
     * This menu holds an entry for every KPart being available on your system.
     */
    QPopupMenu *insertMenu;

private:
//    FormulaDisplay *display;
//    KConfig *config;        
//    KMenuBar *menubar;
//    KStatusBar *statusbar;	
//    KToolBar *toolbar;
  /*  KToolBar *fontbar;
    KToolBar *thistypebar;
    KToolBar *symbolsbar;*/
//    KApplication *app;

};

#endif

