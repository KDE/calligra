#include <qcheckbox.h>
#include <qlistbox.h>
#include <qradiobutton.h>
#include <qpushbutton.h>

#include <kfiledialog.h>

#include "kexiproject.h"
#include "kexistartupdlg.h"

KexiStartupDlg::KexiStartupDlg(QWidget *parent)
 : KexiStartupDlgUI(parent)
{
	connect(alternativeDS, SIGNAL(clicked()), this, SLOT(checkNew()));
	connect(recentFiles, SIGNAL(clicked(QListBoxItem *)), this, SLOT(checkRecent()));

	connect(openRecent, SIGNAL(clicked()), this, SLOT(openRecentClicked()));
	connect(createProject, SIGNAL(clicked()), this, SLOT(otherClicked()));
	connect(openProject, SIGNAL(clicked()), this, SLOT(otherClicked()));
}

void
KexiStartupDlg::checkRecent()
{
	openRecent->setChecked(true);
	btnOk->setEnabled(true);
}

void
KexiStartupDlg::checkNew()
{
	if(alternativeDS->isChecked())
		createProject->setChecked(true);

	btnOk->setEnabled(true);
}

void
KexiStartupDlg::openRecentClicked()
{
	if(recentFiles->currentItem() == -1)
		btnOk->setEnabled(false);
}

void
KexiStartupDlg::otherClicked()
{
	if(!createProject->isChecked())
		alternativeDS->setChecked(false);

	btnOk->setEnabled(true);
}

void
KexiStartupDlg::accept()
{
	if(createProject->isChecked() && alternativeDS->isChecked())
	{
		done(CreateNewAlternative);
	}
	else if(createProject->isChecked())
	{
		done(CreateNew);
	}
	else if(openProject->isChecked())
	{
		m_file = KFileDialog::getOpenFileName(QString::null, "*.kexi");
		done(OpenExisting);
	}
	else
	{
		done(OpenRecent);
	}
}

void
KexiStartupDlg::reject()
{
	done(Cancel);
}

KexiStartupDlg::~KexiStartupDlg()
{
}

#include "kexistartupdlg.moc"

