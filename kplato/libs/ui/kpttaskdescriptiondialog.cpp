/* This file is part of the KDE project
   Copyright (C) 2009 Dag Andersen <koffice-devel@kde.org

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

#include "kpttaskdescriptiondialog.h"
#include "kptnode.h"
#include "kpttask.h"
#include "kptcommand.h"

#include <KTextEdit>
#include <KLocale>
#include <KTextEdit>
#include <KActionCollection>
#include <kdebug.h>

namespace KPlato
{

TaskDescriptionPanel::TaskDescriptionPanel(Node &node, QWidget *p, bool readOnly )
    : TaskDescriptionPanelImpl( node, p )
{
    initDescription( readOnly );
    setStartValues( node );

    descriptionfield->setFocus();
}

void TaskDescriptionPanel::setStartValues( Node &node )
{
    namefield->setText(node.name());
    descriptionfield->setTextOrHtml( node.description() );
}

MacroCommand *TaskDescriptionPanel::buildCommand()
{
    QString s = i18n("Modify task description");
    if ( m_node.type() == Node::Type_Milestone ) {
        s = i18n("Modify milestone description");
    } else if ( m_node.type() == Node::Type_Summarytask ) {
        s = i18n("Modify summary task description");
    } else if ( m_node.type() == Node::Type_Project ) {
        s = i18n("Modify project description");
    }
    MacroCommand *cmd = new MacroCommand(s);
    bool modified = false;

    if ( m_node.description() != descriptionfield->toPlainText() ) {
        cmd->addCommand(new NodeModifyDescriptionCmd(m_node, descriptionfield->toPlainText()));
        modified = true;
    }
    if (!modified) {
        delete cmd;
        return 0;
    }
    return cmd;
}

bool TaskDescriptionPanel::ok() {
    return true;
}

void TaskDescriptionPanel::initDescription( bool readOnly )
{
    toolbar->setVisible( ! readOnly );
    toolbar->setToolButtonStyle( Qt::ToolButtonIconOnly );

    KActionCollection *collection = new KActionCollection( this ); //krazy:exclude=tipsandthis
    descriptionfield->setRichTextSupport( KRichTextWidget::SupportBold |
                                            KRichTextWidget::SupportItalic |
                                            KRichTextWidget::SupportUnderline |
                                            KRichTextWidget::SupportStrikeOut |
                                            KRichTextWidget::SupportChangeListStyle |
                                            KRichTextWidget::SupportAlignment |
                                            KRichTextWidget::SupportFormatPainting );

    descriptionfield->createActions( collection );

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

    descriptionfield->append( "" );
    descriptionfield->setReadOnly( readOnly );
    descriptionfield->setOverwriteMode( false );
    descriptionfield->setLineWrapMode( KTextEdit::WidgetWidth );
    descriptionfield->setTabChangesFocus( true );

}

//-----------------------------
TaskDescriptionPanelImpl::TaskDescriptionPanelImpl( Node &node, QWidget *p )
    : QWidget(p),
      m_node(node)
{

    setupUi(this);

    connect( descriptionfield, SIGNAL( textChanged() ), SLOT( slotChanged() ) );
}

void TaskDescriptionPanelImpl::slotChanged()
{
    emit textChanged( descriptionfield->textOrHtml() != m_node.description() );
}

//-----------------------------
TaskDescriptionDialog::TaskDescriptionDialog( Task &task, QWidget *p, bool readOnly )
    : KDialog(p)
{
    setCaption( i18n( "Task Description" ) );
    if ( readOnly ) {
        setButtons( Close );
    } else {
        setButtons( Ok|Cancel );
        setDefaultButton( Ok );
    }
    showButtonSeparator( true );

    m_descriptionTab = new TaskDescriptionPanel( task, this, readOnly );
    setMainWidget(m_descriptionTab);

    enableButtonOk(false);

    connect( m_descriptionTab, SIGNAL( textChanged( bool ) ), this, SLOT( enableButtonOk(bool) ) );
}

MacroCommand *TaskDescriptionDialog::buildCommand()
{
    return m_descriptionTab->buildCommand();
}

void TaskDescriptionDialog::slotButtonClicked( int button )
{
    if (button == KDialog::Ok) {
        if ( ! m_descriptionTab->ok() ) {
            return;
        }
        accept();
    } else {
        KDialog::slotButtonClicked( button );
    }
}


}  //KPlato namespace

#include "kpttaskdescriptiondialog.moc"
