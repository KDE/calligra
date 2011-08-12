/* This file is part of the KDE project
   Copyright (C) 2004, 2007 Dag Andersen <danders@get2net.dk>

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

#include "kpttaskdefaultpanel.h"

#include "kptduration.h"
#include "kptmycombobox_p.h"
#include "kplatosettings.h"

#include <kabc/addressee.h>
#include <kabc/addresseedialog.h>

#include <QDateTime>
#include <QDateTimeEdit>
#include <QComboBox>

#include <KActionCollection>
#include <KTextEdit>
#include <kdebug.h>

namespace KPlato
{

TaskDefaultPanel::TaskDefaultPanel( QWidget *parent )
    : ConfigTaskPanelImpl( parent )
{
}

//-----------------------------
ConfigTaskPanelImpl::ConfigTaskPanelImpl(QWidget *p )
    : QWidget(p)
{

    setupUi(this);
    kcfg_ExpectedEstimate->setMinimumUnit( (Duration::Unit)KPlatoSettings::self()->minimumDurationUnit() );
    kcfg_ExpectedEstimate->setMaximumUnit( (Duration::Unit)KPlatoSettings::self()->maximumDurationUnit() );

    initDescription();

    connect(chooseLeader, SIGNAL(clicked()), SLOT(changeLeader()));
    
    connect( kcfg_ConstraintStartTime, SIGNAL( dateTimeChanged ( const QDateTime& ) ), SLOT( startDateTimeChanged( const QDateTime& ) ) );
    
    connect( kcfg_ConstraintEndTime, SIGNAL( dateTimeChanged ( const QDateTime& ) ), SLOT( endDateTimeChanged( const QDateTime& ) ) );

    // Hack to have an interface to kcfg wo adding a custom class for this
    kcfg_Unit->addItems( Duration::unitList( true ) );
    connect( kcfg_ExpectedEstimate, SIGNAL( unitChanged( int ) ), SLOT( unitChanged( int ) ) );
    kcfg_Unit->hide();
    connect( kcfg_Unit, SIGNAL( currentIndexChanged( int ) ), SLOT( currentUnitChanged( int ) ) );
}

void ConfigTaskPanelImpl::initDescription()
{
    toolbar->setToolButtonStyle( Qt::ToolButtonIconOnly );

    KActionCollection *collection = new KActionCollection( this ); //krazy:exclude=tipsandthis
    kcfg_Description->setRichTextSupport( KRichTextWidget::SupportBold |
                                            KRichTextWidget::SupportItalic |
                                            KRichTextWidget::SupportUnderline |
                                            KRichTextWidget::SupportStrikeOut |
                                            KRichTextWidget::SupportChangeListStyle |
                                            KRichTextWidget::SupportAlignment |
                                            KRichTextWidget::SupportFormatPainting );

    kcfg_Description->createActions( collection );

    toolbar->addAction( collection->action( "format_text_bold" ) );
    toolbar->addAction( collection->action( "format_text_italic" ) );
    toolbar->addAction( collection->action( "format_text_underline" ) );
    toolbar->addAction( collection->action( "format_text_strikeout" ) );
    toolbar->addSeparator();

    toolbar->addAction( collection->action( "format_list_style" ) );
    toolbar->addSeparator();

    toolbar->addAction( collection->action( "format_align_left" ) );
    toolbar->addAction( collection->action( "format_align_center" ) );
    toolbar->addAction( collection->action( "format_align_right" ) );
    toolbar->addAction( collection->action( "format_align_justify" ) );
    toolbar->addSeparator();

//    toolbar->addAction( collection->action( "format_painter" ) );

    kcfg_Description->append( "" );
    kcfg_Description->setReadOnly( false );
    kcfg_Description->setOverwriteMode( false );
    kcfg_Description->setLineWrapMode( KTextEdit::WidgetWidth );
    kcfg_Description->setTabChangesFocus( true );

}


void ConfigTaskPanelImpl::changeLeader()
{
    KABC::Addressee a = KABC::AddresseeDialog::getAddressee(this);
    if (!a.isEmpty())
    {
        kcfg_Leader->setText(a.fullEmail());
    }
}

void ConfigTaskPanelImpl::startDateTimeChanged( const QDateTime &dt )
{
    if ( dt > kcfg_ConstraintEndTime->dateTime() ) {
        kcfg_ConstraintEndTime->setDateTime( dt );
    }
}

void ConfigTaskPanelImpl::endDateTimeChanged( const QDateTime &dt )
{
    if ( kcfg_ConstraintStartTime->dateTime() > dt ) {
        kcfg_ConstraintStartTime->setDateTime( dt );
    }
}

void ConfigTaskPanelImpl::unitChanged( int unit )
{
    if ( kcfg_Unit->currentIndex() != unit ) {
        kcfg_Unit->setCurrentIndex( unit );
        // kcfg uses the activated() signal to track changes
        emit kcfg_Unit->emitActivated( unit );
    }
}

void ConfigTaskPanelImpl::currentUnitChanged( int unit )
{
    // to get values set at startup
    if ( unit != kcfg_ExpectedEstimate->unit() ) {
        kcfg_ExpectedEstimate->setUnit( (Duration::Unit)unit );
    }
}

}  //KPlato namespace

#include "kpttaskdefaultpanel.moc"
