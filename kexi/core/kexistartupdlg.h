#ifndef KEXISTARTUPDLG_H
#define KEXISTARTUPDLG_H

#include "kexistartupdlgui.h"

class KexiStartupDlg : public KexiStartupDlgUI
{
	Q_OBJECT

	public:
		enum Result
		{
			CreateNew = 0,
			CreateNewAlternative,
			OpenExisting,
			OpenRecent,
			Cancel
		};

		KexiStartupDlg(QWidget *parent);
		~KexiStartupDlg();

		const QString	fileName() { return m_file; }

	protected slots:
		void		checkRecent();
		void		checkNew();
		void		openRecentClicked();
		void		otherClicked();

		virtual	void	accept();
		virtual void	reject();

	private:
		QString		m_file;
};

#endif

