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

#include <kstandardguiitem.h>
#include <kstdaction.h>
#include <kpushbutton.h>
#include <kcombobox.h>
#include <klocale.h>
#include <kdebug.h>
#include <kdialog.h>
#include <kiconloader.h>
#include <KAction>
#include <KShortcut>

#include <qcheckbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <QList>
#include <QShortcut>
#include <QPointer>

#include <kexi_global.h>

//! @internal
class KexiFindDialog::Private
{
  public:
    Private()
    {
    }
    ~Private()
    {
      qDeleteAll(shortcuts);
      shortcuts.clear();
    }
    //! Connects action \a action with appropriate signal \a member
    //! and optionally adds shortcut that will receive shortcut for \a action 
    //! at global scope of the dialog \a parent.
    void setActionAndShortcut(KAction *action, QWidget* parent, const char* member)
    {
#ifdef __GNUC__
#warning not tested: setActionAndShortcut::setActionAndShortcut()
#else
#pragma WARNING( not tested: setActionAndShortcut::setActionAndShortcut() )
#endif
      if (!action)
        return;
      QObject::connect(parent, member, action, SLOT(trigger()));
      if (action->shortcut().isEmpty())
        return;
      // we want to handle dialog-wide shortcut as well
      if (!action->shortcut().primary().isEmpty()) {
        QShortcut *shortcut = new QShortcut(action->shortcut().primary(), parent, member);
        shortcuts.append( shortcut );
      }
      if (!action->shortcut().alternate().isEmpty()) {
        QShortcut *shortcut = new QShortcut(action->shortcut().alternate(), parent, member);
        shortcuts.append( shortcut );
      }
    }

    QStringList lookInColumnNames;
    QStringList lookInColumnCaptions;
    QString objectName; //!< for caption
    QPointer<KAction> findnextAction;
    QPointer<KAction> findprevAction;
    QPointer<KAction> replaceAction;
    QPointer<KAction> replaceallAction;
    QList<QShortcut*> shortcuts;
    bool replaceMode : 1;
};

//------------------------------------------

KexiFindDialog::KexiFindDialog( QWidget* parent )
 : QDialog(parent, 
  Qt::Dialog|Qt::WindowTitleHint|Qt::WindowSystemMenuHint|Qt::Tool)
 , d( new Private() )
{
  setObjectName("KexiFindDialog");
  setupUi(this);
  m_search->setCurrentIndex(
    (int)KexiSearchAndReplaceViewInterface::Options::SearchDown);
  layout()->setMargin( KDialog::marginHint() );
  layout()->setSpacing( KDialog::spacingHint() );
  KAction *a = KStandardAction::findNext(0, 0, 0);
  m_btnFind->setText(a->text());
  m_btnFind->setIcon(KIcon(a->icon()));
  delete a;
  m_btnClose->setText(KStandardGuiItem::close().text());
  m_btnClose->setIcon(KStandardGuiItem::close().icon());
  connect(m_btnFind, SIGNAL(clicked()), this, SIGNAL(findNext()));
  connect(m_btnClose, SIGNAL(clicked()), this, SLOT(slotCloseClicked()));
  connect(m_btnReplace, SIGNAL(clicked()), this, SIGNAL(replaceNext()));
  connect(m_btnReplaceAll, SIGNAL(clicked()), this, SIGNAL(replaceAll()));
  // clear message after the text is changed
  connect(m_textToFind, SIGNAL(editTextChanged(const QString&)), this, SLOT(updateMessage(const QString&)));
  connect(m_textToReplace, SIGNAL(editTextChanged(const QString&)), this, SLOT(updateMessage(const QString&)));

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
  qDeleteAll(d->shortcuts);
  d->setActionAndShortcut(d->findnextAction, this, SIGNAL(findNext()));
  d->setActionAndShortcut(d->findprevAction, this, SIGNAL(findPrevious()));
  d->setActionAndShortcut(d->replaceAction, this, SIGNAL(replaceNext()));
  d->setActionAndShortcut(d->replaceallAction, this, SIGNAL(replaceAll()));
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
  int index = m_lookIn->currentIndex();
  if (index <= 0 || index >= (int)d->lookInColumnNames.count())
    return QString();
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
  m_lookIn->addItem(i18n("(All fields)"));
  m_lookIn->addItem(i18n("(Current field)"));
  m_lookIn->addItems(d->lookInColumnCaptions);
}

void KexiFindDialog::setCurrentLookInColumnName(const QString& columnName)
{
  int index;
  if (columnName.isEmpty())
    index = 0;
  else if (columnName == "(field)")
    index = 1;
  else {
    index = d->lookInColumnNames.indexOf( columnName );
    if (index == -1) {
      kWarning() << QString(
        "KexiFindDialog::setCurrentLookInColumn(%1) column name not found on the list")
        .arg(columnName) << endl;
      return;
    }
    index = index + 1/*"(All fields)"*/ + 1/*"(Current field)"*/;
  }
  m_lookIn->setCurrentIndex(index);
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
      setWindowTitle(i18n("Replace"));
    else
      setWindowTitle(i18n("Replace in \"%1\"", name));
  }
  else {
    if (name.isEmpty())
      setWindowTitle(i18n("Find"));
    else
      setWindowTitle(i18n("Find in \"%1\"", name));
  }
}

void KexiFindDialog::setButtonsEnabled(bool enable)
{
  m_btnFind->setEnabled(enable);
  m_btnReplace->setEnabled(enable);
  m_btnReplaceAll->setEnabled(enable);
  if (!enable)
    setObjectNameForCaption(QString());
}

void KexiFindDialog::setMessage(const QString& message)
{
  m_messageLabel->setText(message);
}

void KexiFindDialog::updateMessage( bool found )
{
  if (found)
    setMessage(QString());
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
  if (m_lookIn->currentIndex() <= 0) //"(All fields)"
    options.columnNumber = KexiSearchAndReplaceViewInterface::Options::AllColumns;
  else if (m_lookIn->currentIndex() == 1) //"(Current field)"
    options.columnNumber = KexiSearchAndReplaceViewInterface::Options::CurrentColumn;
  else
    options.columnNumber = m_lookIn->currentIndex()  - 1/*"(All fields)"*/ - 1/*"(Current field)"*/;
  options.textMatching 
    = (KexiSearchAndReplaceViewInterface::Options::TextMatching)m_match->currentIndex();
  options.searchDirection 
    = (KexiSearchAndReplaceViewInterface::Options::SearchDirection)m_search->currentIndex();
  options.caseSensitive = m_caseSensitive->isChecked();
  options.wholeWordsOnly = m_wholeWords->isChecked();
  options.promptOnReplace = m_promptOnReplace->isChecked();
  return options;
}

#include "kexifinddialog.moc"
