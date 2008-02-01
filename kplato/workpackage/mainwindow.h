/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999 Simon Hausmann <hausmann@kde.org>
   Copyright (C) 2000-2005 David Faure <faure@kde.org>
   Copyright (C) 2005 Sven L�ppken <sven@kde.org>
   Copyright (C) 2008 Dag Andersen <kplato@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/
#ifndef KPLATOWORK_MAINWINDOW_H
#define KPLATOWORK_MAINWINDOW_H

#include "kplatowork_export.h"

#include <KoMainWindow.h>
#include <KoQueryTrader.h>

#include <KoApplication.h>

#include <ktabwidget.h>

#include <q3ptrlist.h>
#include <QMap>
#include <qtoolbutton.h>
#include <q3valuelist.h>
#include <QLabel>

#include <kmimetype.h>
#include <kvbox.h>

namespace std { }
using namespace std;

namespace KPlatoWork {
    class DocumentChild;
    class Part;
}
namespace KPlato {
    class Document;
}

class KComponentData;
class KVBox;
class Q3IconView;
class Q3IconViewItem;
class QSplitter;
class KoDocumentEntry;
class KoView;

class KPlatoWork_MainGUIClient;

/////// class KPlatoWork_MainWindow ////////

class KPLATOWORK_EXPORT KPlatoWork_MainWindow : public KoMainWindow
{
  Q_OBJECT

public:
    explicit KPlatoWork_MainWindow( const KComponentData &instance );
    virtual ~KPlatoWork_MainWindow();
    
    bool openDocument( const KUrl &url );
    using KoMainWindow::openDocument;
    
    virtual void setRootDocument( KoDocument *doc );
    /**
    * Update caption from document info - call when document info
    * (title in the about page) changes.
    */
    virtual void updateCaption();
    virtual void updateCaption( const QString &caption, bool modified );
    using KoMainWindow::updateCaption;
    
    virtual QString configFile() const;
    
    KAction* partSpecificHelpAction;

    void editDocument( KPlatoWork::Part *part, const KPlato::Document *doc );
    
    bool isEditing() const { return m_editing; }
    bool isModified() const;
    
protected slots:
    virtual void slotActivePartChanged( KParts::Part *newPart );

    virtual void slotFileNew();
    virtual void slotFileClose();
    virtual void slotFileOpen();
    /**
     *  Saves the current document with the current name.
     */
    virtual void slotFileSave();

    /**
     *  Saves the current document with a new name.
     */
    virtual void slotFileSaveAs();
    
    void sendMail();
    
    void saveAll();
    
    void showPartSpecificHelp();
    
    void tab_contextMenu(QWidget * ,const QPoint &);
    
    void slotKSLoadCompleted();
    void slotKSLoadCanceled (const QString &);
    void slotNewDocumentName();
    /**
    This slot is called whenever the user clicks on a tab to change the document. It looks for the
    changed widget in the list of all pages and calls switchToPage with the iterator which points
    to the page corresponding to the widget.
    @param widget The current widget
    */
    void slotUpdatePart( QWidget* widget );

protected:
    /// Open document from workpackage store
    bool editWorkpackageDocument( const KPlato::Document *doc, KPlatoWork::Part *part );
    /// Open KParts document from workpackage store
    bool editKPartsDocument(  KPlatoWork::Part *part, KService::Ptr service, const KPlato::Document *doc );
    /// Open KOfficePart document from workpackage store
    bool editKOfficePartDocument( KPlatoWork::Part *part, KMimeType::Ptr mimetype, const KPlato::Document *doc );
    void enableHelp( bool enable );
    
    virtual bool saveDocument( bool saveas = false, bool silent = false );

    void setMainDocument( const KoDocument *doc, bool main = true );
    KoDocument *mainDocument() const;
    bool isMainDocument( const KoDocument *doc ) const;
    
private:
    class Page
    {
    public:
        Page() : m_pDoc( 0 ), m_pView( 0 ), part( 0 ), isKParts( false ),  m_id( -1 ), isMainDocument( false ) {}
        bool operator==( const Page &p ) const { return m_pDoc == p.m_pDoc && m_pView == p.m_pView && isKParts == p.isKParts && m_id == p.m_id && isMainDocument == p.isMainDocument; }
        bool isEmpty() const { return (*this) == Page(); }
        KoDocument *m_pDoc;
        KoView *m_pView;
        KParts::ReadWritePart *part;
        bool isKParts;
        int m_id;
        bool isMainDocument;
    };

private: // methods
    Page findPage( const KParts::ReadWritePart *doc ) const;
    void setupTabWidget();
    void chooseNewDocument( InitDocFlags initDocFlags );
    virtual bool openDocumentInternal( const KUrl & url, KoDocument * newdoc = 0L );
    virtual bool queryClose();
    virtual void slotConfigureKeys();
    void closeDocument();
    void saveSettings();
    void switchToPage( int index );
    
    void removePage( KParts::PartBase *doc );
    
private: // variables
    bool m_editing; // a workpackage doc is open for editing
    int m_activePage;
    QList<Page> m_lstPages;

    QLabel *m_pComponentsLabel;
    QSplitter *m_pLayout;
    KTabWidget *m_pFrame;
    QToolButton *m_tabCloseButton;

    // Saved between openDocument and setRootDocument
    KoDocumentEntry m_documentEntry;

    KPlatoWork_MainGUIClient *m_client;
    void createShellGUI( bool create = true );

};

//////// class MainGUIClient //////////

class KPLATOWORK_EXPORT KPlatoWork_MainGUIClient : public KXMLGUIClient
{
public:
    KPlatoWork_MainGUIClient( KPlatoWork_MainWindow *window );
};

#endif // KPLATOWORK_MAINWINDOW_H

