/******************************************************************/
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer, Torben Weis                       */
/* E-Mail: reggie@kde.org, weis@kde.org                           */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Shell (header)                                         */
/******************************************************************/

/*********
 * KWordShell is copied from KSpread. Bug reports regarding the
 * shell should be sent to weis@kde.org
 *********/

#ifndef kword_shell_h
#define kword_shell_h

class KWordShell;

#include <koMainWindow.h>

class KWordDocument;
class KWordView;

#include <qlist.h>
#include <qstring.h>

/******************************************************************/
/* Class: KWordShell                                              */
/******************************************************************/

class KWordShell : public KoMainWindow
{
	Q_OBJECT
public:
	// C++
	KWordShell();
	~KWordShell();

	// C++
	virtual void cleanUp();
	void setDocument(KWordDocument *_doc);

	// C++
	virtual bool newDocument();
	virtual bool openDocument(const char *_filename,const char* _format);
	virtual bool saveDocument(const char *_file,const char *_format);
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

	KWordDocument* m_pDoc;
	KWordView* m_pView;

	static QList<KWordShell>* s_lstShells;
	static bool previewHandlerRegistered;
};

#endif

