/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1998                   */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* KPresenter is under GNU GPL                                    */
/******************************************************************/
/* Module: KPresenter Shell (header)                              */
/******************************************************************/

#ifndef __kpresenter_shell_h__
#define __kpresenter_shell_h__

#include <koMainWindow.h>

class KPresenterDoc;
class KPresenterView;

#include <qlist.h>
#include <qstring.h>

/*****************************************************************/
/* class KPresenterShell                                         */
/*****************************************************************/

class KPresenterShell : public KoMainWindow
{
    Q_OBJECT
public:
    // C++
    KPresenterShell();
    ~KPresenterShell();

    // C++
    virtual void cleanUp();
    void setDocument( KPresenterDoc *_doc );

    // C++
    virtual bool newDocument();
    virtual bool openDocument( const char *_filename, const char* _format );
    virtual bool saveDocument();
    virtual bool closeDocument();
    virtual bool closeAllDocuments();

protected slots:
    void slotFileNew();
    void slotFileOpen();
    void slotFileSave();
    void slotFileSaveAs();
    void slotFilePrint();
    void slotFileClose();
    void slotFileQuit();

protected:
    // C++
    virtual KOffice::Document_ptr document();
    virtual KOffice::View_ptr view();

    virtual bool printDlg();
    virtual void helpAbout();
    virtual int documentCount();

    bool isModified();
    bool requestClose();

    void releaseDocument();

    KPresenterDoc* m_pDoc;
    KPresenterView* m_pView;

    static QList<KPresenterShell>* s_lstShells;
    static bool previewHandlerRegistered;

};

#endif
