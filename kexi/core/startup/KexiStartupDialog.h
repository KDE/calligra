/* This file is part of the KDE project
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

   Based on: KoTemplateChooseDia.h
   Original autors:
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   2000, 2001 Werner Trobin <trobin@kde.org>
   2002, 2003 Thomas Nagy <tnagy@eleve.emn.fr>

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

#ifndef KexiStartupDialog_h
#define KexiStartupDialog_h

#include <kdialogbase.h>
#include <kicondialog.h>
#include <kiconview.h>
#include <kfileiconview.h>
#include <kfiledialog.h>

#include <qlabel.h>
#include <qsplitter.h>

#include <kexidb/connectiondata.h>

/**
 * Our reimplementation of KIconCanvas used within the template-chooser dialog.
 * @internal
 */
/*class KoTCDIconCanvas : public KIconCanvas
{
    Q_OBJECT
    public:
	KoTCDIconCanvas( QWidget *parent = 0, const char *name = 0L )
	    : KIconCanvas( parent, name ) {}

	bool isCurrentValid() { return currentItem(); }
	QIconViewItem * load(KoTemplateGroup *group, const QString& name);

    protected:
	virtual void keyPressEvent( QKeyEvent *e ) {
	    if ( e->key() == Key_Return || e->key() == Key_Enter )
		e->ignore();
	    else
		KIconCanvas::keyPressEvent( e );
	}
};

/// @internal
class KoTCDIconViewItem : public KIconViewItem
{
    public:
	KoTCDIconViewItem(QIconView *parent=0)
	    : KIconViewItem ( parent )
	    {}

	KoTCDIconViewItem(QIconView *parent=0, const QString &text=0, const QPixmap &icon=0,
                      const QString &descr=0, const QString &fullname=0)
	    : KIconViewItem(parent, text, icon)
	    {
            m_descr = descr;
            m_full = fullname;
	    }

	QString getDescr() const { return m_descr; }
	QString getFName() const { return m_full; }

    private :
	QString m_descr;
	QString m_full;

};
*/

/**
 * Our reimplementation of KFileIconView used as the "recent files" view
 * within the template-chooser dialog.
 * @internal
 */
/*class KoTCDRecentFilesIconView : public KFileIconView {
    Q_OBJECT
    public:
	KoTCDRecentFilesIconView( QWidget* parent, const char* name ) :
		KFileIconView( parent, name ), toolTip(0)
	{
	    connect( this, SIGNAL( onItem( QIconViewItem * ) ),
                     SLOT( showToolTip( QIconViewItem * ) ) );
	    connect( this, SIGNAL( onViewport() ),
                     SLOT( removeToolTip() ) );
	}
        virtual ~KoTCDRecentFilesIconView();
    protected:
        virtual void hideEvent( QHideEvent * );

    private slots:
        void showToolTip( QIconViewItem* );
        void removeToolTip();
    private:
        QLabel* toolTip;
};
*/

class KTextBrowser;

/*! Helper class for displaying templates set with description. */
class KEXICORE_EXPORT TemplatesPage : public QSplitter
{
	Q_OBJECT
	
	public:
		TemplatesPage( Orientation o, QWidget * parent = 0, const char * name = 0 );
		~TemplatesPage(); 
		void addItem(const QString& key, const QString& name, 
			const QString& description, const QPixmap& icon);
	private slots:
		void itemClicked(QIconViewItem *item);
	
	public:
		KIconView *templates;
		KTextBrowser *info;
};


class KexiStartupDialogPrivate;
class KexiProjectData;
class KexiProjectSet;
class KexiDBConnectionSet;

/**
 *  This class is used to show the template dialog
 *  on startup. Unless you need something special, you should use the static
 *  method choose().
 *
 *  @short The template choose dialog
 *  @author Reginald Stadlbauer <reggie@kde.org>
 *  @author Werner Trobin <trobin@kde.org>
 */
class KEXICORE_EXPORT KexiStartupDialog : public KDialogBase
{
    Q_OBJECT

public:
	/**
	 * The Dialog returns one of these values depending
	 * on the input of the user.
	 * Cancel = The user pressed 'Cancel'
	 * Template = The user selected a template
	 * File = The user has chosen a file
	 * Empty = The user selected "Empty document"
	 */
	enum Result { CancelResult=0, TemplateResult=1, OpenExistingResult=2, OpenRecentResult=3 };
	/**
	 * To configure the dialog you have to use this enum 
	 * (any !=0 or'ed value is ok)
	 * - Templates = Show "Templates" tab
	 * - OpenExisting Show "Open existing" tab
	 * - OpenRecent = Show "Recent" tab
	 ( - Everything = Show everything above
	 */
	enum DialogType { Templates = 1, OpenExisting = 2, OpenRecent = 4, Everything = (1+2+4) };

	/*! Creates a dialog.
	 @param dialogType
	 @param recentProjects a set of recent projects' info, used for "Open recent" tab
	 @param connSet conenction set used to present available conenctions
	  in "Open Existing" tab. Pass an empty object is this tab is not used.
	 */
	KexiStartupDialog(
		int dialogType, 
		const KexiDBConnectionSet& connSet,
		const KexiProjectSet& recentProjects,
		QWidget *parent = 0, const char *name = 0 );
//		KInstance* global,
//		const QCString &format="",
//		const QString &nativePattern=QString::null,
//		const QString &nativeName=QString::null,
//		const QCString& templateType="");
	~KexiStartupDialog();

	/**
	 * This is the static method you'll normally use to show the
	 * dialog.
	 * @param global the KInstance of your app
	 * @param file this is the filename which is returned to your app
	 * More precisely, it's a url (to give to KURL) if ReturnType is File
	 * and it's a path (to open directly) if ReturnType is Template
	 *
	 * @param format is the mimetype of the app (e.g. application/x-kspread)
	 * @param nativeName is the name of your app (e.g KSpread)
	 * @param nativePattern is the native pattern (e.g. *.ksp)
	 * @param dialogType the type of the dialog
	 * @param templateType the template type of your application (see kword or
	 *        kpresenter for details)
	 * @return The return type (see above)
	 */
/*js	static ReturnType choose(KInstance* global, QString &file,
		const QCString &format="",
		const QString &nativePattern=QString::null,
		const QString &nativeName=QString::null,
		const DialogType &dialogType=Everything,
		const QCString& templateType="");
*/
	/**
	 * Method to get the current template
	 */
//	QString getTemplate() const;
	/**
	 * Method to get the "full" template (path+template)
	 */
//	QString getFullTemplate() const;
	

	/**
	 * The dialogType - normally you won't need this one
	 */
//	DialogType getDialogType() const;

	/*! Executes dialog. 
	 \return one of Result values. Use this after dialog is closed. */
	int result() const;

	/*! \return key string of selected database template if result() is TemplateResult,
		otherwise null string. The key is of form: "<group>/<name>",
		for example: "business/address_book".
		For blank databases "blank" key is used - then additional information 
		about selected connection should be used with appropriate methods. */
	QString selectedTemplateKey() const;

	/*! \return data of selected Kexi project (if "Open Recent" tab was selected).
		Returns NULL if no selection has been made or other tab was selected.
	*/
	const KexiProjectData* selectedProjectData() const;
	
	/*! \return name of selected Kexi project file 
		(if "Open Existing" tab was selected and this file was clicked).
		Returns empty string if no such selection has been made or other tab was selected.
	*/
	QString selectedExistingFile() const;

	/*! \return a pointer to selected Kexi connection data.
		(if "Open Existing" tab was selected and this connection data was clicked).
		Returns NULL if no such selection has been made or other tab was selected.
	*/
	const KexiDB::ConnectionData* selectedExistingConnection();

	/*! Reimplemented for internal reasons */	
	virtual void show();

public slots:

protected slots:
    /**
     * Activated when the Ok button has been clicked.
     */
//    virtual void slotOk();
	virtual void done(int r);
	virtual void reject();
	
	//! slot activated when one of page in templates window is shown
	void templatesPageShown(QWidget *page);

	//! Any icon view item has been executed (dblclicked)
	void templateItemExecuted(QIconViewItem *item);

	//! Any icon view item has been selected
	void templateItemSelected(QIconViewItem *item);

	//! Any tab has been selected
	void tabShown(QWidget *w);

	//! helper
	void recentProjectDoubleClicked(const KexiProjectData *data);
	void existingFileSelected(const QString &f);
	void showSimpleConnForOpenExisting();
	void showAdvancedConnForOpenExisting();
	void connectionItemForOpenExistingDBLClicked(QListViewItem *item);

protected:
	virtual bool eventFilter( QObject *o, QEvent *e );
	
	//! helper: updates a state of dialog's OK button
	void updateDialogOKButton(QWidget *w);

private:
	void setupPageTemplates();
	void setupPageOpenExisting();
	void setupPageOpenRecent();
	
	//! used internally on accepting templates selection
	void updateSelectedTemplateKeyInfo();

	KexiStartupDialogPrivate *d;

/*	QString descriptionText(const QString &name, const QString &description);
	void setupDialog();
	void setupTemplateDialog(QWidget * widgetbase, QGridLayout * layout);
	void setupFileDialog(QWidget * widgetbase, QGridLayout * layout);
	void setupRecentDialog(QWidget * widgetbase, QGridLayout * layout);
	bool collectInfo();
	bool noStartupDlg() const;
*/
//    private slots:

//	void chosen(QIconViewItem *);
//	void currentChanged( QIconViewItem * );
//	void recentSelected( QIconViewItem * );
};

//! internal
class KexiStartupFileDialog : public KFileDialog
{
	Q_OBJECT
	
public :
	KexiStartupFileDialog(
		const QString& startDir, QString& filter,
		QWidget *parent=0, const char *name=0);
	KURL currentURL();

	// Return true if the current URL exists, show msg box if not
	bool checkURL();

signals:
//	void cancelClicked();

protected:
	// Typing a file that doesn't exist closes the file dialog, we have to
	// handle this case better here.
	virtual void accept();
	virtual void reject();
};

#endif

