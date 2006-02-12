/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999 Simon Hausmann <hausmann@kde.org>
   Copyright (C) 2000-2005 David Faure <faure@kde.org>
   Copyright (C) 2005 Sven Lüppken <sven@kde.org>

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
#ifndef __koshell_window_h__
#define __koshell_window_h__

#include <KoMainWindow.h>
#include <KoQueryTrader.h>
#include <ktabwidget.h>
#include <qptrlist.h>
#include <qmap.h>
#include <qtoolbutton.h>
#include <qvaluelist.h>

#include "iconsidepane.h"

class QVBox;
class QIconView;
class QIconViewItem;
class QSplitter;
class KoDocumentEntry;
class KoView;
class KoShellGUIClient;

/////// class KoShellWindow ////////

class KoShellWindow : public KoMainWindow
{
  Q_OBJECT

public:

  KoShellWindow();
  virtual ~KoShellWindow();

  virtual void setRootDocument( KoDocument *doc );
  /**
   * Update caption from document info - call when document info
   * (title in the about page) changes.
   */
  virtual void updateCaption();

  virtual QString configFile() const;

  KAction *mnuSaveAll;
  KAction* partSpecificHelpAction;

protected slots:

  virtual void slotFileNew();
  virtual void slotFileClose();
  virtual void slotFileOpen();
  void saveAll();

  void showPartSpecificHelp();

  void slotSidebar_Part(int);
  void slotSidebar_Document(int);
  void tab_contextMenu(QWidget * ,const QPoint &);

  /**
    Used for showing or hiding the sidebar and the components-label.
  */
  void slotShowSidebar();
  /**
    This slot is called whenever the user clicks on a "component" in the sidebar. It loads a new
    document which can be edited with the chosen component.
    @param item The component the user clicked on
  */
  void slotSidebarItemClicked( QIconViewItem *item );
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

private:

  struct Page
  {
    KoDocument *m_pDoc;
    KoView *m_pView;
    int m_id;
  };

  virtual bool queryClose();
  virtual bool openDocumentInternal( const KURL & url, KoDocument * newdoc = 0L );
  virtual void slotConfigureKeys();
  void closeDocument();
  void saveSettings();
  void switchToPage( QValueList<Page>::Iterator it );


  QValueList<Page> m_lstPages;
  QValueList<Page>::Iterator m_activePage;

  IconSidePane *m_pSidebar;
  QLabel *m_pComponentsLabel;
  QSplitter *m_pLayout;
  KTabWidget *m_pFrame;
  QToolButton *m_tabCloseButton;

  // Map of available parts (the int is the koolbar item id)
  QMap<int,KoDocumentEntry> m_mapComponents;

  // Saved between openDocument and setRootDocument
  KoDocumentEntry m_documentEntry;

  KoShellGUIClient *m_client;
  void createShellGUI( bool create = true );

  int m_grpFile; //the ID number for the component group in the side bar
  int m_grpDocuments; //the ID number for the docuemnt group in the side bar
};

//////// class KoShellGUIClient //////////

class KoShellGUIClient : public KXMLGUIClient
{
public:
  KoShellGUIClient( KoShellWindow *window );
};

#endif // __koshell_window_h__

