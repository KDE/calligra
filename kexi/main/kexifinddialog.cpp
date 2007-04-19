/* This file is part of the KDE project
   Copyright (C) 2004-2007 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kexifinddialog.h"

#include <kstdguiitem.h>
#include <kstdaction.h>
#include <kpushbutton.h>
#include <kcombobox.h>
#include <klocale.h>
#include <kdebug.h>
#include <kdialog.h>
#include <kaction.h>
#include <kiconloader.h>

#include <qcheckbox.h>
#include <qlabel.h>
#include <qguardedptr.h>
#include <qlayout.h>
#include <qaccel.h>

//! @internal
class KexiFindDialog::Private
{
	public:
		Private()
		{
			accels.setAutoDelete(true);
		}
		~Private()
		{
		}
		//! Connects action \a action with appropriate signal \a member
		//! and optionally adds accel that will receive shortcut for \a action 
		//! at global scope of the dialog \a parent.
		void setActionAndAccel(KAction *action, QWidget* parent, const char* member)
		{
			if (!action)
				return;
			QObject::connect(parent, member, action, SLOT(activate()));
			if (action->shortcut().isNull())
				return;
			QAccel *accel = new QAccel(parent); // we want to handle dialog-wide shortcut as well
			accels.append( accel );
			accel->connectItem(
				accel->insertItem( action->shortcut() ), parent, member );
		}

		QStringList lookInColumnNames;
		QStringList lookInColumnCaptions;
		QString objectName; //!< for caption
		QGuardedPtr<KAction> findnextAction;
		QGuardedPtr<KAction> findprevAction;
		QGuardedPtr<KAction> replaceAction;
		QGuardedPtr<KAction> replaceallAction;
		QPtrList<QAccel> accels;
		bool replaceMode : 1;
};

//------------------------------------------

KexiFindDialog::KexiFindDialog( QWidget* parent )
 : KexiFindDialogBase(parent, "KexiFindDialog", false/*!modal*/, 
	Qt::WType_Dialog|Qt::WStyle_NormalBorder|Qt::WStyle_Title
	|Qt::WStyle_SysMenu|Qt::WStyle_Customize|Qt::WStyle_Tool)
 , d( new Private() )
{
	m_search->setCurrentItem((int)KexiSearchAndReplaceViewInterface::Options::SearchDown);
	layout()->setMargin( KDialog::marginHint() );
	layout()->setSpacing( KDialog::spacingHint() );
	KAction *a = KStdAction::findNext(0, 0, 0);
	m_btnFind->setText(a->text());
	m_btnFind->setIconSet(a->iconSet());
	delete a;
	m_btnClose->setText(KStdGuiItem::close().text());
	m_btnClose->setIconSet(KStdGuiItem::close().iconSet());
	connect(m_btnFind, SIGNAL(clicked()), this, SIGNAL(findNext()));
	connect(m_btnClose, SIGNAL(clicked()), this, SLOT(slotCloseClicked()));
	connect(m_btnReplace, SIGNAL(clicked()), this, SIGNAL(replaceNext()));
	connect(m_btnReplaceAll, SIGNAL(clicked()), this, SIGNAL(replaceAll()));
	// clear message after the text is changed
	connect(m_textToFind, SIGNAL(textChanged()), this, SIGNAL(updateMessage()));
	connect(m_textToReplace, SIGNAL(textChanged()), this, SIGNAL(updateMessage()));

	d->replaceMode = true; //to force updating by setReplaceMode()
	setReplaceMode(false);
	
	setLookInColumnList(QStringList(), QStringList());
}

KexiFindDialog::~KexiFindDialog()
{
	delete d;
}

void KexiFindDialog::setActions( KAction *findnext, KAction *findprev,
	KAction *replace, KAction *replaceall )
{
	d->findnextAction = findnext;
	d->findprevAction = findprev;
	d->replaceAction = replace;
	d->replaceallAction = replaceall;
	d->accels.clear();
	d->setActionAndAccel(d->findnextAction, this, SIGNAL(findNext()));
	d->setActionAndAccel(d->findprevAction, this, SIGNAL(findPrevious()));
	d->setActionAndAccel(d->replaceAction, this, SIGNAL(replaceNext()));
	d->setActionAndAccel(d->replaceallAction, this, SIGNAL(replaceAll()));
}

QStringList KexiFindDialog::lookInColumnNames() const
{
	return d->lookInColumnNames;
}

QStringList KexiFindDialog::lookInColumnCaptions() const
{
	return d->lookInColumnCaptions;
}

QString KexiFindDialog::currentLookInColumnName() const
{
	int index = m_lookIn->currentItem();
	if (index <= 0 || index >= (int)d->lookInColumnNames.count())
		return QString::null;
	else if (index == 1)
		return "(field)";
	return d->lookInColumnNames[index - 1/*"(All fields)"*/ - 1/*"(Current field)"*/];
}

QVariant KexiFindDialog::valueToFind() const
{
	return m_textToFind->currentText();
}

QVariant KexiFindDialog::valueToReplaceWith() const
{
	return m_textToReplace->currentText();
}

void KexiFindDialog::setLookInColumnList(const QStringList& columnNames, 
	const QStringList& columnCaptions)
{
	d->lookInColumnNames = columnNames;
	d->lookInColumnCaptions = columnCaptions;
	m_lookIn->clear();
	m_lookIn->insertItem(i18n("(All fields)"));
	m_lookIn->insertItem(i18n("(Current field)"));
	m_lookIn->insertStringList(d->lookInColumnCaptions);
}

void KexiFindDialog::setCurrentLookInColumnName(const QString& columnName)
{
	int index;
	if (columnName.isEmpty())
		index = 0;
	else if (columnName == "(field)")
		index = 1;
	else {
		index = d->lookInColumnNames.findIndex( columnName );
		if (index == -1) {
			kdWarning() << QString("KexiFindDialog::setCurrentLookInColumn(%1) column name not found on the list")
				.arg(columnName) << endl;
			return;
		}
		index = index + 1/*"(All fields)"*/ + 1/*"(Current field)"*/;
	}
	m_lookIn->setCurrentItem(index);
}

void KexiFindDialog::setReplaceMode(bool set)
{
	if (d->replaceMode == set)
		return;
	d->replaceMode = set;
	if (d->replaceMode) {
		m_promptOnReplace->show();
		m_replaceLbl->show();
		m_textToReplace->show();
		m_btnReplace->show();
		m_btnReplaceAll->show();
	}
	else {
		m_promptOnReplace->hide();
		m_replaceLbl->hide();
		m_textToReplace->hide();
		m_btnReplace->hide();
		m_btnReplaceAll->hide();
		resize(width(),height()-30);
	}
	setObjectNameForCaption(d->objectName);
	updateGeometry();
}

void KexiFindDialog::setObjectNameForCaption(const QString& name)
{
	d->objectName = name;
	if (d->replaceMode) {
		if (name.isEmpty())
			setCaption(i18n("Replace"));
		else
			setCaption(i18n("Replace in \"%1\"").arg(name));
	}
	else {
		if (name.isEmpty())
			setCaption(i18n("Find"));
		else
			setCaption(i18n("Find in \"%1\"").arg(name));
	}
}

void KexiFindDialog::setButtonsEnabled(bool enable)
{
	m_btnFind->setEnabled(enable);
	m_btnReplace->setEnabled(enable);
	m_btnReplaceAll->setEnabled(enable);
	if (!enable)
		setObjectNameForCaption(QString::null);
}

void KexiFindDialog::setMessage(const QString& message)
{
	m_messageLabel->setText(message);
}

void KexiFindDialog::updateMessage( bool found )
{
	if (found)
		setMessage(QString::null);
	else
		setMessage(i18n("The search item was not found"));
}

void KexiFindDialog::slotCloseClicked()
{
	reject();
}

void KexiFindDialog::show()
{
	m_textToFind->setFocus();
	QDialog::show();
}

KexiSearchAndReplaceViewInterface::Options KexiFindDialog::options() const
{
	KexiSearchAndReplaceViewInterface::Options options;
	if (m_lookIn->currentItem() <= 0) //"(All fields)"
		options.columnNumber = KexiSearchAndReplaceViewInterface::Options::AllColumns;
	else if (m_lookIn->currentItem() == 1) //"(Current field)"
		options.columnNumber = KexiSearchAndReplaceViewInterface::Options::CurrentColumn;
	else
		options.columnNumber = m_lookIn->currentItem()  - 1/*"(All fields)"*/ - 1/*"(Current field)"*/;
	options.textMatching 
		= (KexiSearchAndReplaceViewInterface::Options::TextMatching)m_match->currentItem();
	options.searchDirection 
		= (KexiSearchAndReplaceViewInterface::Options::SearchDirection)m_search->currentItem();
	options.caseSensitive = m_caseSensitive->isChecked();
	options.wholeWordsOnly = m_wholeWords->isChecked();
	options.promptOnReplace = m_promptOnReplace->isChecked();
	return options;
}

#include "kexifinddialog.moc"
