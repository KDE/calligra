// (C) 1998 by F. Zigterman


/***************************************************************

     Requires the Qt and KDE widget libraries, available at no cost at
     http://www.troll.no and http://www.kde.org respectively

     Copyright (C) 1997, 1998 Fester Zigterman ( fzr@dds.nl )

     This program is free software; you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation; either version 2 of the License, or
     (at your option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with this program; if not, write to the Free Software
     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


***************************************************************/




#ifndef KPROPDLG_H
#define KPROPDLG_H

#include <qlist.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbt.h>
#include <qdialog.h>

#include <ktreelist.h>
#include <ktabctl.h>
#include <kbuttonbox.h>

/** This class provides a property dialog.
  * Tree list, Tabs, or wizard type dialogs are supported. 
  * Methods for adding and showing pages have been kept public, 
  * allthough sub-Classing it is strongly recommended.
  * to use the auto-config update connect your widgets need to 
  * have setConfig and getConfig() slots. Connect them in the 
  * KPropDlg subclass with connectConfig( YourWidget ).
  */
class KPropDlg : public QDialog
{
	Q_OBJECT
public:
	enum{ PREV = 1, NEXT = 2, OK = 4, APPLY = 8, CANCEL = 16 };

	/** Create a dialog with Tree or Tab support, and any combination of 
	  * Previous, Next, Apply, OK, and Cancel buttons. 
	  */
	KPropDlg( int dlgtype, int buttons, const char *title, QWidget *parent, const char *name=0, bool modal=TRUE );
	~KPropDlg();
	enum{ TREE = 1, TABS = 2 };
	
	/** Add a page to the dialog. If parentpage is omitted, the page will be 
	 *  added at the top level. If the position in the list is omitted,
	 *  the page will be appended at the end. Returns a QFrame which can hold 
	 *  the page contents.
	 */
	QFrame *addPage( int parentpage, const char *title, int position=0 );
	
	/** Remove a page from the dialog. Give either the name of the index of 
	 *  the page. Returns TRUE on success, FALSE on error.
	 */
	bool removePage( char *page=0, int index=0 );

	/** Display a specific page of the property dialog. (By widget as opposed
	 *  to by index so hose inheriting us don't have to worry about harding constants
	 *  for the first exposure.)
	 */
	void showPage( QWidget *w );

	/** adjust Page frame minimum size to fit the pages */
	void adjustPageHeight();

	/** connect Buttons to default handlers */
	void connectButtons(int buttons = OK|APPLY|CANCEL|PREV|NEXT );
	

signals:

	/** Raise signals when one of the button types are clicked. */
	void ApplyClicked();
	void OKClicked();
	void CancelClicked();
	void PrevClicked();
	void NextClicked();

	/** This signal is emitted when Apply or OK is clicked, to 
	  * indicate that the KConfig object has been modified.
	  */
 	void configChanged();
	
	/** setConfig() and getConfig() can be connected to any child widgets
	  * using connectConfig(), to enable automatic config updating.
	  */
	void setConfig();
	void getConfig();
	
public slots:

        /** Receive indication that button was select and emit appropriate
         *  signals. These methods may be reimplemented to change button behaviour.
         */
	  void receiveOK();
	  void receiveApply();
	  void receiveCancel();
	  void receivePrev();
	  void receiveNext();

	/** Set Button to use in the dialog. Button types can be a 
	 *  combination of: PREVIOUS, NEXT, OK, APPLY, CANCEL.
	 */
	void setButtonsEnabled( int types );
	int buttonsEnabled();
	
	/** Set the button text. */
	void setButton( int type, const char *text );

	/** Set the default button, which is activated by pressing 
	 *  the return key.
	 */
	void setDefaultButton( int type );

    /** For Tree layout, when highlighted, display page based on index.
      * The minimum size of the widget is set in this method ;
      * if you add items to Pages without calling this method afterward,
      * you might get qlayout warning messages.
      * NOTE: It seems that setting a specific page is not possible with 
      * a tab control.
      */
	bool showPage( int index );
	
	/** connect the Config object to the setConfig() and getConfig() slots */
	void connectConfig( QObject * propconf );

protected:
	
	/** ensures that the dialogs Page Frame will always fit the children.
	  * ( needed to eliminate annoying qlayout warnings )
	  */
	bool eventFilter( QObject *, QEvent *);

	/** Size the buttons and correct the size change for Motif default buttons. */
	void resizeButtons();

	/** Read the status of the PageList. Can be one of the following:
	  * NOLIST, NOPAGES, NOCURRENT, or a combination of LASTPAGE, FIRSTPAGE.
	  */
	int pageListStatus();
	
	enum { NOLIST, NOPAGES, NOCURRENT, FIRSTPAGE = 16, LASTPAGE = 32 };
	QPushButton *DefaultButton;

protected slots:
	/** Slot for simple Prev button action: previous page */
	void slotPrev();
	
	/** Slot for simple Next button action: next page */
	void slotNext();

	/** Accept the settings and close the dialog. calls setConfig() */
	virtual void acceptConfig();
	
	/** Cancel and close the dialog. */
	virtual void cancelConfig();

	/** Apply the changes, do NOT close the dialog */
	virtual void applyConfig();
	
	/** Virtual function, reimplement to write the changes, e.g. to KConfig */
//	virtual void setConfig();
	
	/** Virtual function, reimplement to read the config and put it in the
	  * the dialog pages
	  */
//	virtual void getConfig();
	
private:
	QList<QWidget> *PageList;
	QHBoxLayout *HLayout;
	QVBoxLayout *VLayout;
	QFrame *PageFrame;
	KTreeList *TreeList;
	QLabel *Title;
	QVBoxLayout *ButtonLayout;
	QLabel *ButtonWidget;
	KTabCtl *TabControl;
	QWidget *rpane;
	QWidget *ActivePage;
	
	QPushButton *ApplyButton, *OKButton, *CancelButton, *PrevButton, *NextButton;
	int DlgType, Buttons, TreeWidth, ActiveButtons;
	int minimumPageHeight;
};

#endif

