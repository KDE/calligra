/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __ko_main_window_h__
#define __ko_main_window_h__

#include <opMainWindow.h>
#include <opMainWindowIf.h>
#include <openparts.h>
#include <koffice.h>

class OPMenu;
class OPMenuBar;
class KoFrame;
class KoMainWindowIf;
class KoDocument;
class KoViewIf;

/**
 * This class is used to represent a main window
 * of a KOffice component. Each main window contains
 * a menubar and some toolbars. In addition it has
 * a @ref KoFrame which embeds the @ref KoView.
 */
class KoMainWindow : public OPMainWindow
{
  Q_OBJECT
public:
  KoMainWindow( const char *_name = 0L );
  ~KoMainWindow();

  /**
   * @return a pointer to the CORBA interface of the base class.
   */
  virtual OPMainWindowIf* interface();
  /**
   * @return a pointer to the CORBA interface. By default the window
   *         does not offer a CORBA interface, but calling this function
   *         creates one.
   */
  virtual KoMainWindowIf* koInterface();

  /**
   * Creates the file menu. Overload if you need your own one.
   * This function is called whenever some other view gets focus
   * since this means an update of the complete menubar.
   */
  virtual void createFileMenu( OPMenuBar* );
  /**
   * Creates the help menu. Overload if you need your own one.
   * This function is called whenever some other view gets focus
   * since this means an update of the complete menubar.
   */
  virtual void createHelpMenu( OPMenuBar* );

  virtual void setRootPart( unsigned long _part_id );
  virtual void setRootPart( KoViewIf* _view );
  
  virtual void cleanUp();

  virtual KOffice::Document_ptr document() = 0L;
  virtual KOffice::View_ptr view() = 0L;

  /**
   * @return the frame used in this window.
   */
  KoFrame *frame() { return m_pFrame; }

  /**
   * Create a new empty document and show it.
   *
   * @return TRUE on success.
   */
  virtual bool newDocument() { return false; };
  /**
   * Load the desired document and show it.
   *
   * @return TRUE on success.
   */
  virtual bool openDocument( const char* /* _filename */, const char* /* _format */ ) { return false; };
  
  /**
   * Saves the document, asking for a filename if necessary.
   * Reset the URL of the document to "" in slotFileSaveAs
   * @param _native_format the standard mimetype for your document
   * Will allow to use filters if saving to another format
   * @param _native_pattern *.kwd for KWord
   * @return TRUE on success or on cancel, false on error
   * (don't display anything in this case, the error dialog box is also implemented here
   *  but restore the original URL in slotFileSaveAs)
   */
  virtual bool saveDocument( const char* _native_format, const char* _native_pattern );

protected slots:
  /**
   * Called if the activated part changes.
   */
  virtual void slotActivePartChanged( unsigned long _new_part_id, unsigned long _old_opart_id );

  virtual void slotFileNew();
  virtual void slotFileOpen();
  virtual void slotFileSave();
  /**
   * Called by File / Save as...
   * Usual implementation :
   *
   *  QString _url = m_pDoc->url();
   *  m_pDoc->setURL( "" );
   *  if ( !saveDocument() )
   *      m_pDoc->setURL( _url );
   *
   */
  virtual void slotFileSaveAs();
  virtual void slotFilePrint();
  virtual void slotFileClose();
  virtual void slotFileQuit();
  virtual void slotHelpAbout();

protected:
  OPMenu* m_pFileMenu;
  OPMenu* m_pHelpMenu;

  int m_idMenuFile_New;
  int m_idMenuFile_Open;
  int m_idMenuFile_Save;
  int m_idMenuFile_SaveAs;
  int m_idMenuFile_Print;
  int m_idMenuFile_Close;
  int m_idMenuFile_Quit;
  int m_idMenuHelp_About;

  /**
   * Ids for the toolbar buttons.
   */
  enum { TOOLBAR_NEW, TOOLBAR_OPEN, TOOLBAR_SAVE, TOOLBAR_PRINT };

  KoFrame* m_pFrame;
  KoMainWindowIf* m_pKoInterface;
};

/**
 * The CORBA interface for @ref KoMainWindow.
 */
class KoMainWindowIf : virtual public OPMainWindowIf,
		       virtual public KOffice::MainWindow_skel
{
public:
  KoMainWindowIf( KoMainWindow* _main );
  ~KoMainWindowIf();

  // IDL
  virtual void setMarkedPart( OpenParts::Id id );
  /**
   * The document attached to the window. This is NOT always the document
   * which has the focus.
   */
  virtual KOffice::Document_ptr document();
  /**
   * The view of @ref #document.
   */
  virtual KOffice::View_ptr view();
  virtual CORBA::Boolean partClicked( OpenParts::Id _part_id, CORBA::Long _button );
  
protected:
  void unmarkPart();

  KoMainWindow* m_pKoMainWindow;

  OpenParts::Id m_iMarkedPart;
};

#endif
