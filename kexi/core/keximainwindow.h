#ifndef KEXIMAINWINDOW_H
#define KEXIMAINWINDOW_H

#include <kmdimainfrm.h>
#include <kmdidefines.h>

class KexiProject;
class KexiBrowser;
class KMdiChildView;
class KexiDialogBase;

/**
 * @short Kexi's main window
 */
class KexiMainWindow : public KMdiMainFrm
{
	Q_OBJECT

	public:
		/**
		 * creates a emtpy mainwindow
		 */
		KexiMainWindow();
		~KexiMainWindow();

		KexiProject	*project() { return m_project; }

		/**
		 * registers a dialog for watching and adds it to the view
		 */
		void		registerChild(KexiDialogBase *dlg);


	protected:
		//reimplementation of events
		virtual void	closeEvent(QCloseEvent *);

		/**
		 * creates standard actions like new, open, save ...
		 */
		void		initActions();


		/**
		 * sets up the window from user settings (e.g. mdi mode)
		 */
		void		restoreSettings();

		/**
		 * writes user settings back
		 */
		void		storeSettings();

	protected slots:

		/**
		 * parsers command line options and checks if we should open the startupdlg
		 * or a file direclty
		 */
		void		parseCmdLineOptions();

		/**
		 * creates browser and fills it with empty items
		 */
		void		initBrowser();

		/**
		 * this slot is called if a window changes
		 */
		void		activeWindowChanged(KMdiChildView *dlg);

	private:
		KexiProject	*m_project;
		KexiBrowser	*m_browser;
};

#endif

