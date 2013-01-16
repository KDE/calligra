/* This file is part of the KDE project
   Copyright (C) 2011 Dag Andersen <danders@get2net.dk>

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

#include "kptworkpackagemergedialog.h"
#include "kptpackage.h"
#include "kptproject.h"
#include "kpttask.h"

#include <KMessageBox>
#include <KExtendableItemDelegate>

#include <QTreeView>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QList>
#include <QStyle>
#include <QStyleOption>
#include <QPixmap>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QCheckBox>

#include "kmessagebox_copy.cpp" // themedMessageBoxIcon()

namespace KPlato
{

PackageInfoWidget::PackageInfoWidget( Package *package, QWidget* parent)
    : QFrame( parent ),
    m_package( package )
{
    setFrameStyle( QFrame::Sunken | QFrame::StyledPanel );
    QHBoxLayout *l = new QHBoxLayout( this );
    l->addSpacing( 20 );
    QCheckBox *w = new QCheckBox( this );
    w->setText( i18n( "Used Effort" ) );
    w->setCheckState( package->settings.usedEffort ? Qt::Checked : Qt::Unchecked );
    l->addWidget( w );
    connect(w, SIGNAL(stateChanged(int)), SLOT(slotUsedEffortChanged(int)));
    w = new QCheckBox( this );
    w->setText( i18n( "Task Progress" ) );
    w->setCheckState( package->settings.progress ? Qt::Checked : Qt::Unchecked );
    l->addWidget( w );
    connect(w, SIGNAL(stateChanged(int)), SLOT(slotProgressChanged(int)));
    w = new QCheckBox( this );
    w->setText( i18n("&Documents") );
    w->setCheckState( package->settings.documents ? Qt::Checked : Qt::Unchecked );
    l->addWidget( w );
    connect(w, SIGNAL(stateChanged(int)), SLOT(slotDocumentsChanged(int)));
}

void PackageInfoWidget::slotUsedEffortChanged( int s )
{
    m_package->settings.usedEffort = (bool)s;
}

void PackageInfoWidget::slotProgressChanged( int s )
{
    m_package->settings.progress = (bool)s;
}

void PackageInfoWidget::slotDocumentsChanged( int s )
{
    m_package->settings.documents = (bool)s;
}

WorkPackageMergePanel::WorkPackageMergePanel( QWidget *parent )
    : QWidget( parent )
{
    setupUi( this );
}

WorkPackageMergeDialog::WorkPackageMergeDialog( const QString &text, const QMap<KDateTime, Package*> &list, QWidget *parent )
    : KDialog( parent ),
    m_packages( list.values() )
{
    panel.ui_text->setText( text );

    QIcon icon = themedMessageBoxIcon( QMessageBox::Information );
    if ( ! icon.isNull() ) {
        QStyleOption option;
        option.initFrom( this );
        panel.ui_icon->setPixmap( icon.pixmap( style()->pixelMetric( QStyle::PM_MessageBoxIconSize, &option, this ) ) );
    }

    setButtons( KDialog::Yes | KDialog::No );

    panel.ui_view->setHeaderHidden( true );
    panel.ui_view->setRootIsDecorated( false );
    m_delegate = new KExtendableItemDelegate( panel.ui_view );
    panel.ui_view->setItemDelegate( m_delegate );

    m_model = new QStandardItemModel( panel.ui_view );
    foreach( Package *p, m_packages ) {
        QList<QStandardItem*> items;
        items << new QStandardItem();
        items << new QStandardItem( p->project->childNode( 0 )->name() );
        items << new QStandardItem( static_cast<Task*>( p->project->childNode( 0 ) )->workPackage().ownerName() );
        items << new QStandardItem( KGlobal::locale()->formatDateTime( p->timeTag ) );

        if ( p->toTask ) {
            items[ CheckColumn ]->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable );
            items[ CheckColumn ]->setCheckState( Qt::Checked );

            items[ TaskNameColumn ]->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable );

            items[ OwnerNameColumn ]->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable );

            items[DateTimeColumn]->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable );
            items[ DateTimeColumn ]->setTextAlignment( Qt::AlignCenter );
        }
        m_model->appendRow( items );
    }

    // sort on datetime first
    QSortFilterProxyModel *dtsort = new QSortFilterProxyModel( panel.ui_view );
    dtsort->setSourceModel( m_model );
    dtsort->sort( DateTimeColumn, Qt::DescendingOrder );

    // then on task name
    QSortFilterProxyModel *tasksort = new QSortFilterProxyModel( panel.ui_view );
    tasksort->setSourceModel( dtsort );
    tasksort->sort( TaskNameColumn, Qt::AscendingOrder );

    panel.ui_view->setModel( tasksort );
    for ( int c = 0; c < m_model->columnCount(); ++c ) {
        panel.ui_view->resizeColumnToContents( c );
    }
    setMainWidget( &panel );

    slotChanged();

    connect(panel.ui_view->model(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), SLOT(slotChanged()));
    connect(panel.ui_view, SIGNAL(activated(const QModelIndex&)), SLOT(slotActivated(const QModelIndex&)));
}

WorkPackageMergeDialog::~WorkPackageMergeDialog()
{
    m_delegate->contractAll();
}

void WorkPackageMergeDialog::slotActivated( const QModelIndex &idx )
{
    QModelIndex i = idx;
    if ( i.column() >= CheckColumn ) {
        i = i.model()->index( i.row(), TaskNameColumn, i.parent() );
    }
    if ( i.column() != TaskNameColumn ) {
        return;
    }
    if ( m_delegate->isExtended( i ) ) {
        m_delegate->contractItem( i );
    } else {
        m_delegate->extendItem( new PackageInfoWidget( m_packages.at( idx.row() ) ), i );
    }
}

QList<int> WorkPackageMergeDialog::checkedList() const
{
    QList<int> lst;
    int count = m_model->rowCount();
    for ( int i = 0; i < count; ++i ) {
        if ( m_model->index( i, 0 ).data( Qt::CheckStateRole ).toInt() == Qt::Checked ) {
            lst << i;
        }
    }
    return lst;
}
void WorkPackageMergeDialog::slotChanged()
{
    enableButton( KDialog::Yes, m_model->rowCount() > 0 );
}

} // namespace KPlato

#include "kptworkpackagemergedialog.moc"
