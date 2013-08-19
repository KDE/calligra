/* This file is part of the KDE libraries
   Copyright (C) 2000 David Faure <faure@kde.org>, Alexander Neundorf <neundorf@kde.org>
             (C) 2000, 2002 Carsten Pfeiffer <pfeiffer@kde.org>
             (C) 2010 Sebastian Trueg <trueg@kde.org>

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

#include "keditlistwidget.h"

#include <QtCore/QStringList>
#include <QKeyEvent>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QListView>
#include <QPushButton>
#include <QApplication>
#include <QComboBox>
#include <QStringListModel>

class KEditListWidgetPrivate
{
public:
    KEditListWidgetPrivate( KEditListWidget* parent )
        : lineEdit(0),
          editingWidget(0),
          q(parent) {
    }
    QListView *listView;
    QPushButton *servUpButton, *servDownButton;
    QPushButton *servNewButton, *servRemoveButton;
    QLineEdit *lineEdit;
    QWidget* editingWidget;
    QVBoxLayout* mainLayout;
    QVBoxLayout* btnsLayout;
    QStringListModel *model;

    bool checkAtEntering;
    KEditListWidget::Buttons buttons;

    void init( bool check = false, KEditListWidget::Buttons buttons = KEditListWidget::All,
               QWidget *representationWidget = 0 );
    void setEditor( QLineEdit* lineEdit, QWidget* representationWidget = 0 );
    void updateButtonState();
    QModelIndex selectedIndex();

private:
    KEditListWidget* q;
};


void KEditListWidgetPrivate::init( bool check, KEditListWidget::Buttons newButtons,
                                   QWidget *representationWidget )
{
    checkAtEntering = check;

    servNewButton = servRemoveButton = servUpButton = servDownButton = 0L;
    q->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,
                                 QSizePolicy::Preferred));

    mainLayout = new QVBoxLayout(q);

    QHBoxLayout* subLayout = new QHBoxLayout;
    btnsLayout = new QVBoxLayout;
    btnsLayout->addStretch();

    model = new QStringListModel();
    listView = new QListView(q);
    listView->setModel(model);

    subLayout->addWidget(listView);
    subLayout->addLayout(btnsLayout);

    mainLayout->insertLayout(1, subLayout);

    setEditor( lineEdit, representationWidget );

    buttons = 0;
    q->setButtons( newButtons );

    q->connect(listView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
               SLOT(slotSelectionChanged(QItemSelection,QItemSelection)));
}


void KEditListWidgetPrivate::setEditor( QLineEdit* newLineEdit, QWidget* representationWidget )
{
    if (editingWidget != lineEdit &&
        editingWidget != representationWidget) {
        delete editingWidget;
    }
    if (lineEdit != newLineEdit) {
        delete lineEdit;
    }
    lineEdit = newLineEdit ? newLineEdit : new QLineEdit(q);
    editingWidget = representationWidget ?
                    representationWidget : lineEdit;

    if ( representationWidget )
        representationWidget->setParent(q);

    mainLayout->insertWidget(0,editingWidget);

    lineEdit->installEventFilter(q);

    q->connect(lineEdit,SIGNAL(textChanged(QString)),SLOT(typedSomething(QString)));
    q->connect(lineEdit,SIGNAL(returnPressed()),SLOT(addItem()));

    // maybe supplied lineedit has some text already
    q->typedSomething( lineEdit->text() );


    // fix tab ordering
    q->setTabOrder(editingWidget, listView);
    QWidget* w = listView;
    if (servNewButton) {
        q->setTabOrder(w,servNewButton);
        w = servNewButton;
    }
    if (servRemoveButton) {
        q->setTabOrder(w,servRemoveButton);
        w = servRemoveButton;
    }
    if (servUpButton) {
        q->setTabOrder(w,servUpButton);
        w = servUpButton;
    }
    if (servDownButton) {
        q->setTabOrder(w,servDownButton);
        w = servDownButton;
    }
}


void KEditListWidgetPrivate::updateButtonState()
{
    QModelIndex index = selectedIndex();
    if (servUpButton) {
        servUpButton->setEnabled(index.isValid());
    }
    if (servDownButton) {
        servDownButton->setEnabled(index.isValid());
    }
    if (servRemoveButton) {
        servRemoveButton->setEnabled(index.isValid());
    }
}

QModelIndex KEditListWidgetPrivate::selectedIndex()
{
    QItemSelectionModel *selection = listView->selectionModel();
    const QModelIndexList selectedIndexes = selection->selectedIndexes();
    if ( !selectedIndexes.isEmpty() && selectedIndexes[0].isValid() )
        return selectedIndexes[0];
    else
        return QModelIndex();
}


class KEditListWidget::CustomEditorPrivate
{
public:
    CustomEditorPrivate(KEditListWidget::CustomEditor *q):
        q(q),
        representationWidget(0),
        lineEdit(0) {}

    KEditListWidget::CustomEditor *q;
    QWidget *representationWidget;
    QLineEdit *lineEdit;
};

KEditListWidget::CustomEditor::CustomEditor()
    : d(new CustomEditorPrivate(this))
{
}

KEditListWidget::CustomEditor::CustomEditor( QWidget *repWidget, QLineEdit *edit )
    : d(new CustomEditorPrivate(this))
{
    d->representationWidget = repWidget;
    d->lineEdit = edit;
}

KEditListWidget::CustomEditor::CustomEditor( QComboBox *combo )
    : d(new CustomEditorPrivate(this))
{
    d->representationWidget = combo;
    d->lineEdit = qobject_cast<QLineEdit*>( combo->lineEdit() );
    Q_ASSERT( d->lineEdit );
}

KEditListWidget::CustomEditor::~CustomEditor()
{
    delete d;
}

void KEditListWidget::CustomEditor::setRepresentationWidget( QWidget *repWidget )
{
    d->representationWidget = repWidget;
}

void KEditListWidget::CustomEditor::setLineEdit( QLineEdit *edit )
{
    d->lineEdit = edit;
}

QWidget *KEditListWidget::CustomEditor::representationWidget() const
{
    return d->representationWidget;
}

QLineEdit *KEditListWidget::CustomEditor::lineEdit() const
{
    return d->lineEdit;
}

KEditListWidget::KEditListWidget(QWidget *parent)
    : QWidget(parent), d(new KEditListWidgetPrivate(this))
{
    d->init();
}

KEditListWidget::KEditListWidget(const CustomEditor& custom,
                                 QWidget *parent,
                                 bool checkAtEntering,
                                 Buttons buttons)
    :QWidget(parent), d(new KEditListWidgetPrivate(this))
{
    d->lineEdit = custom.lineEdit();
    d->init( checkAtEntering, buttons, custom.representationWidget() );
}

KEditListWidget::~KEditListWidget()
{
    delete d;
}

void KEditListWidget::setCustomEditor( const CustomEditor& editor )
{
    d->setEditor( editor.lineEdit(), editor.representationWidget() );
}

QListView *KEditListWidget::listView() const
{
    return d->listView;
}

QLineEdit *KEditListWidget::lineEdit() const
{
    return d->lineEdit;
}

QPushButton *KEditListWidget::addButton() const
{
    return d->servNewButton;
}

QPushButton *KEditListWidget::removeButton() const
{
    return d->servRemoveButton;
}

QPushButton *KEditListWidget::upButton() const
{
    return d->servUpButton;
}

QPushButton *KEditListWidget::downButton() const
{
    return d->servDownButton;
}

int KEditListWidget::count() const
{
    return int(d->model->rowCount());
}

void KEditListWidget::setButtons( Buttons buttons )
{
    if ( d->buttons == buttons )
        return;

    if ( ( buttons & Add ) && !d->servNewButton ) {
        d->servNewButton = new QPushButton(QIcon::fromTheme(QStringLiteral("list-add")), tr("&Add"), this);
        d->servNewButton->setEnabled(false);
        d->servNewButton->show();
        connect(d->servNewButton, SIGNAL(clicked()), SLOT(addItem()));

        d->btnsLayout->insertWidget(0, d->servNewButton);
    } else if ( ( buttons & Add ) == 0 && d->servNewButton ) {
        delete d->servNewButton;
        d->servNewButton = 0;
    }

    if ( ( buttons & Remove ) && !d->servRemoveButton ) {
        d->servRemoveButton = new QPushButton(QIcon::fromTheme(QStringLiteral("list-remove")), tr("&Remove"), this);
        d->servRemoveButton->setEnabled(false);
        d->servRemoveButton->show();
        connect(d->servRemoveButton, SIGNAL(clicked()), SLOT(removeItem()));

        d->btnsLayout->insertWidget(1, d->servRemoveButton);
    } else if ( ( buttons & Remove ) == 0 && d->servRemoveButton ) {
        delete d->servRemoveButton;
        d->servRemoveButton = 0;
    }

    if ( ( buttons & UpDown ) && !d->servUpButton ) {
        d->servUpButton = new QPushButton(QIcon::fromTheme(QStringLiteral("arrow-up")), tr("Move &Up"), this);
        d->servUpButton->setEnabled(false);
        d->servUpButton->show();
        connect(d->servUpButton, SIGNAL(clicked()), SLOT(moveItemUp()));

        d->servDownButton = new QPushButton(QIcon::fromTheme(QStringLiteral("arrow-down")), tr("Move &Down"), this);
        d->servDownButton->setEnabled(false);
        d->servDownButton->show();
        connect(d->servDownButton, SIGNAL(clicked()), SLOT(moveItemDown()));

        d->btnsLayout->insertWidget(2, d->servUpButton);
        d->btnsLayout->insertWidget(3, d->servDownButton);
    } else if ( ( buttons & UpDown ) == 0 && d->servUpButton ) {
        delete d->servUpButton; d->servUpButton = 0;
        delete d->servDownButton; d->servDownButton = 0;
    }

    d->buttons = buttons;
}

void KEditListWidget::setCheckAtEntering(bool check)
{
    d->checkAtEntering = check;
}

bool KEditListWidget::checkAtEntering()
{
    return d->checkAtEntering;
}

void KEditListWidget::typedSomething(const QString& text)
{
    if(currentItem() >= 0) {
        if(currentText() != d->lineEdit->text())
        {
            // IMHO changeItem() shouldn't do anything with the value
            // of currentItem() ... like changing it or emitting signals ...
            // but TT disagree with me on this one (it's been that way since ages ... grrr)
            bool block = d->listView->signalsBlocked();
            d->listView->blockSignals( true );
            QModelIndex currentIndex = d->selectedIndex();
            if ( currentIndex.isValid() )
              d->model->setData(currentIndex,text);
            d->listView->blockSignals( block );
            emit changed();
        }
    }

    if ( !d->servNewButton )
        return;

    if ( !d->lineEdit->hasAcceptableInput() ) {
        d->servNewButton->setEnabled(false);
        return;
    }

    if (!d->checkAtEntering)
        d->servNewButton->setEnabled(!text.isEmpty());
    else
    {
        if (text.isEmpty())
        {
            d->servNewButton->setEnabled(false);
        }
        else
        {
            QStringList list = d->model->stringList();
            bool enable = !list.contains( text, Qt::CaseSensitive );
            d->servNewButton->setEnabled( enable );
        }
    }
}

void KEditListWidget::moveItemUp()
{
    if (!d->listView->isEnabled())
    {
        QApplication::beep();
        return;
    }

    QModelIndex index = d->selectedIndex();
    if ( index.isValid() ) {
      if (index.row() == 0) {
          QApplication::beep();
          return;
      }

      QModelIndex aboveIndex = d->model->index( index.row() - 1, index.column() );

      QString tmp = d->model->data( aboveIndex, Qt::DisplayRole ).toString();
      d->model->setData( aboveIndex, d->model->data( index, Qt::DisplayRole ) );
      d->model->setData( index, tmp );

      d->listView->selectionModel()->select(index, QItemSelectionModel::Deselect);
      d->listView->selectionModel()->select(aboveIndex, QItemSelectionModel::Select);
    }

    emit changed();
}

void KEditListWidget::moveItemDown()
{
    if (!d->listView->isEnabled())
    {
        QApplication::beep();
        return;
    }

    QModelIndex index = d->selectedIndex();
    if ( index.isValid() ) {
      if (index.row() == d->model->rowCount() - 1) {
          QApplication::beep();
          return;
      }

      QModelIndex belowIndex = d->model->index( index.row() + 1, index.column() );

      QString tmp = d->model->data( belowIndex, Qt::DisplayRole ).toString();
      d->model->setData( belowIndex, d->model->data( index, Qt::DisplayRole ) );
      d->model->setData( index, tmp );

      d->listView->selectionModel()->select(index, QItemSelectionModel::Deselect);
      d->listView->selectionModel()->select(belowIndex, QItemSelectionModel::Select);
    }

    emit changed();
}

void KEditListWidget::addItem()
{
    // when checkAtEntering is true, the add-button is disabled, but this
    // slot can still be called through Key_Return/Key_Enter. So we guard
    // against this.
    if ( !d->servNewButton || !d->servNewButton->isEnabled() )
        return;

    QModelIndex currentIndex = d->selectedIndex();

    const QString& currentTextLE=d->lineEdit->text();
    bool alreadyInList(false);
    //if we didn't check for dupes at the inserting we have to do it now
    if (!d->checkAtEntering)
    {
        // first check current item instead of dumb iterating the entire list
        if ( currentIndex.isValid() ) {
          if ( d->model->data( currentIndex, Qt::DisplayRole ).toString() == currentTextLE )
            alreadyInList = true;
        }
        else
        {
            alreadyInList = d->model->stringList().contains( currentTextLE, Qt::CaseSensitive );
        }
    }
    if ( d->servNewButton )
        d->servNewButton->setEnabled(false);

    bool block = d->lineEdit->signalsBlocked();
    d->lineEdit->blockSignals(true);
    d->lineEdit->clear();
    d->lineEdit->blockSignals(block);

    d->listView->selectionModel()->setCurrentIndex(currentIndex, QItemSelectionModel::Deselect);

    if (!alreadyInList)
    {
        block = d->listView->signalsBlocked();

        if ( currentIndex.isValid() ) {
          d->model->setData(currentIndex, currentTextLE );
        } else {
            QStringList lst;
            lst<<currentTextLE;
            lst<<d->model->stringList();
            d->model->setStringList(lst);
        }
        emit changed();
        emit added( currentTextLE ); // TODO: pass the index too
    }

    d->updateButtonState();
}

int KEditListWidget::currentItem() const
{
    QModelIndex selectedIndex = d->selectedIndex();
    if ( selectedIndex.isValid() )
        return selectedIndex.row();
    else
        return -1;
}

void KEditListWidget::removeItem()
{
    QModelIndex currentIndex = d->selectedIndex();
    if ( !currentIndex.isValid() )
      return;

    if ( currentIndex.row() >= 0 )
    {
        QString removedText = d->model->data( currentIndex, Qt::DisplayRole ).toString();

        d->model->removeRows( currentIndex.row(), 1 );

        d->listView->selectionModel()->clear();

        emit changed();

        emit removed( removedText );
    }

    d->updateButtonState();
}

void KEditListWidget::enableMoveButtons(const QModelIndex &newIndex, const QModelIndex&)
{
    int index = newIndex.row();

    // Update the lineEdit when we select a different line.
    if(currentText() != d->lineEdit->text())
        d->lineEdit->setText(currentText());

    bool moveEnabled = d->servUpButton && d->servDownButton;

    if (moveEnabled )
    {
        if (d->model->rowCount() <= 1)
        {
            d->servUpButton->setEnabled(false);
            d->servDownButton->setEnabled(false);
        }
        else if (index == (d->model->rowCount() - 1))
        {
            d->servUpButton->setEnabled(true);
            d->servDownButton->setEnabled(false);
        }
        else if (index == 0)
        {
            d->servUpButton->setEnabled(false);
            d->servDownButton->setEnabled(true);
        }
        else
        {
            d->servUpButton->setEnabled(true);
            d->servDownButton->setEnabled(true);
        }
    }

    if ( d->servRemoveButton )
        d->servRemoveButton->setEnabled(true);
}

void KEditListWidget::clear()
{
    d->lineEdit->clear();
    d->model->setStringList( QStringList() );
    emit changed();
}

void KEditListWidget::insertStringList(const QStringList& list, int index)
{
    QStringList content = d->model->stringList();
    if ( index < 0 )
      content += list;
    else
      for ( int i = 0, j = index; i < list.count(); ++i, ++j )
        content.insert( j, list[ i ] );

    d->model->setStringList( content );
}

void KEditListWidget::insertItem(const QString& text, int index)
{
  QStringList list = d->model->stringList();

  if ( index < 0 )
    list.append( text );
  else
    list.insert( index, text );

  d->model->setStringList(list);
}

QString KEditListWidget::text(int index) const
{
  const QStringList list = d->model->stringList();

  return list[ index ];
}

QString KEditListWidget::currentText() const
{
  QModelIndex index = d->selectedIndex();
  if ( !index.isValid() )
    return QString();
  else
    return text( index.row() );
}

QStringList KEditListWidget::items() const
{
    return d->model->stringList();
}

void KEditListWidget::setItems(const QStringList& items)
{
  d->model->setStringList(items);
}

KEditListWidget::Buttons KEditListWidget::buttons() const
{
  return d->buttons;
}

void KEditListWidget::slotSelectionChanged( const QItemSelection&, const QItemSelection& )
{
    d->updateButtonState();
    QModelIndex index = d->selectedIndex();
    enableMoveButtons(index,  QModelIndex());
    if (index.isValid()) {
        d->lineEdit->setFocus( Qt::OtherFocusReason );
    }
}

bool KEditListWidget::eventFilter( QObject* o, QEvent* e )
{
    if (o == d->lineEdit && e->type() == QEvent::KeyPress ) {
        QKeyEvent* keyEvent = (QKeyEvent*)e;
        if (keyEvent->key() == Qt::Key_Down ||
            keyEvent->key() == Qt::Key_Up) {
            return ((QObject*)d->listView)->event(e);
        } else if(keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            return true;
        }
    }

    return false;
}

