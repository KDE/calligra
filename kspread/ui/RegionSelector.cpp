/* This file is part of the KDE project
   Copyright 1999-2006 The KSpread Team <koffice-devel@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

#include "RegionSelector.h"

// KSpread
#include "FormulaEditorHighlighter.h"
#include "Selection.h"

// KOffice

// KDE
#include <kdebug.h>
#include <kdialog.h>
#include <klocale.h>
#include <ktextedit.h>

// Qt
#include <QEvent>
#include <QHBoxLayout>
#include <QToolButton>

using namespace KSpread;

class RegionSelector::Private
{
  public:
    Selection* selection;
    QDialog* parentDialog;
    KDialog* dialog;
    KTextEdit* textEdit;
    QToolButton* button;
    FormulaEditorHighlighter* highlighter;
    DisplayMode displayMode;
    SelectionMode selectionMode;
    static RegionSelector* s_focussedSelector;
};

RegionSelector* RegionSelector::Private::s_focussedSelector = 0;

RegionSelector::RegionSelector(QWidget* parent)
  : QWidget( parent ),
    d( new Private )
{
  setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );

  d->displayMode = Widget;
  d->parentDialog = 0;
  d->selection = 0;
  d->dialog = 0;
  d->button = new QToolButton( this );
  d->button->setCheckable( true );
  d->button->setIcon( KIcon( "selection" ) );
  d->highlighter = 0;
  d->textEdit = new KTextEdit( this );
  d->textEdit->setLineWrapMode( QTextEdit::NoWrap );
  d->textEdit->setWordWrapMode( QTextOption::NoWrap );
  d->textEdit->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );
  d->textEdit->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  d->textEdit->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  d->textEdit->setFixedHeight( d->button->height() - 2*d->textEdit->frameWidth() ); // FIXME
  d->textEdit->setTabChangesFocus( true );

  QHBoxLayout* layout = new QHBoxLayout( this );
  layout->setMargin( 0 );
  layout->setSpacing( 2 );
  layout->addWidget( d->textEdit );
  layout->addWidget( d->button );

  d->button->installEventFilter( this );
  d->textEdit->installEventFilter( this );
  connect( d->button, SIGNAL( toggled(bool) ),
           this, SLOT( switchDisplayMode(bool) ) );
}

RegionSelector::~RegionSelector()
{
  d->selection->endReferenceSelection();
  d->selection->setSelectionMode(Selection::MultipleCells);
  delete d;
}

void RegionSelector::setSelectionMode( SelectionMode mode )
{
  d->selectionMode = mode;
  // TODO adjust selection
}

void RegionSelector::setSelection(Selection* selection)
{
    d->selection = selection;
    d->highlighter = new FormulaEditorHighlighter(d->textEdit, d->selection);
    connect(d->selection, SIGNAL(changed(const Region&)), this, SLOT(choiceChanged()));
}

void RegionSelector::setDialog( QDialog* dialog )
{
  d->parentDialog = dialog;
}

KTextEdit* RegionSelector::textEdit() const
{
  return d->textEdit;
}

bool RegionSelector::eventFilter( QObject* object, QEvent* event )
{
  if ( event->type() == QEvent::Close )
  {
    if ( object == d->dialog  && d->button->isChecked() )
    {
      // TODO Stefan: handle as button click
//       d->button->toggle();
      event->ignore();
      return true; // eat it
    }
  }
  else if ( event->type() == QEvent::FocusIn )
  {
    Private::s_focussedSelector = this;
    d->selection->startReferenceSelection();
    if (d->selectionMode == SingleCell)
    {
      d->selection->setSelectionMode( Selection::SingleCell );
    }
    else
    {
      d->selection->setSelectionMode( Selection::MultipleCells );
    }
    // TODO Stefan: initialize choice
  }
  return QObject::eventFilter( object, event );
}

void RegionSelector::switchDisplayMode( bool state )
{
  Q_UNUSED(state)
      kDebug() ;

  if ( d->displayMode == Widget )
  {
    d->displayMode = Dialog;

    d->dialog = new KDialog( d->parentDialog->parentWidget(), Qt::Tool );
    d->dialog->resize( d->parentDialog->width(), 20 );
    d->dialog->move( d->parentDialog->pos() );
    d->dialog->setButtons( 0 );
    d->dialog->setModal( false );

    if ( d->selectionMode == SingleCell )
    {
      d->dialog->setCaption( i18n("Select Single Cell") );
    }
    else // if ( d->selectionMode == MultipleCells )
    {
      d->dialog->setCaption( i18n("Select Multiple Cells") );
    }

    QWidget* widget = new QWidget( d->dialog );
    QHBoxLayout* layout = new QHBoxLayout( widget );
    layout->setMargin( 0 );
    layout->setSpacing( 0 );
    layout->addWidget( d->textEdit );
    layout->addWidget( d->button );

    d->dialog->setMainWidget( widget );
    d->dialog->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    d->dialog->installEventFilter( this );
    d->dialog->show();
    d->parentDialog->hide();
  }
  else
  {
    d->displayMode = Widget;

    layout()->addWidget( d->textEdit );
    layout()->addWidget( d->button );

    d->parentDialog->move( d->dialog->pos() );
    d->parentDialog->show();
    delete d->dialog;
    d->dialog = 0;
  }
}

void RegionSelector::choiceChanged()
{
  if ( Private::s_focussedSelector != this )
    return;

  if ( d->selection->isValid() )
  {
    QString area = d->selection->name();
    d->textEdit->setPlainText( area );
  }
}

#include "RegionSelector.moc"
