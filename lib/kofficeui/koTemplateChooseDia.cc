/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
                 2000, 2001 Werner Trobin <trobin@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

// Description: Template Choose Dialog

/******************************************************************/

#include <qfile.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qvbox.h>
#include <qtabwidget.h>
#include <qradiobutton.h>
#include <qcombobox.h>

#include <kdesktopfile.h>
#include <klocale.h>
#include <kbuttonbox.h>
#include <kurl.h>
#include <kfiledialog.h>
#include <kinstance.h>
#include <kstddirs.h>
#include <koFilterManager.h>
#include <koTemplates.h>
#include <kseparator.h>
#include <krecentdocument.h>
#include <kio/netaccess.h>
#include <kstringhandler.h>

#include <koTemplateChooseDia.h>


class KoTemplateChooseDiaPrivate {
public:
    KoTemplateChooseDiaPrivate(const QCString& templateType, KInstance* global,
                               const QCString &format, const QString &nativePattern,
                               const QString &nativeName,
                               const KoTemplateChooseDia::DialogType &dialogType) :
        m_templateType(templateType), m_global(global), m_format(format),
        m_nativePattern(nativePattern), m_nativeName(nativeName),
        m_dialogType(dialogType), m_firstTime(true), tree(0L), m_mainwidget(0L),
        m_rbTemplates(0L), m_rbFile(0L), m_rbEmpty(0L), m_bFile(0L), m_tabs(0L) {
    }
    ~KoTemplateChooseDiaPrivate() {}

    QCString m_templateType;
    KInstance* m_global;
    QCString m_format;
    QString m_nativePattern;
    QString m_nativeName;
    KoTemplateChooseDia::DialogType m_dialogType;
    bool m_firstTime;
    KoTemplateTree *tree;

    QString m_templateName;
    QString m_fullTemplateName;
    KoTemplateChooseDia::ReturnType m_returnType;
    QString m_file;

    QWidget *m_mainwidget;

    QRadioButton *m_rbTemplates;
    QRadioButton *m_rbFile;
    QRadioButton *m_rbRecent;
    QRadioButton *m_rbEmpty;
    QPushButton *m_bFile;
    QComboBox *m_recent;

    QTabWidget *m_tabs;
    QDict<MyIconCanvas> canvasDict;
    QMap<QString, QString> recentFilesMap;
};

/******************************************************************/
/* Class: KoTemplateChooseDia                                     */
/******************************************************************/

/*================================================================*/
KoTemplateChooseDia::KoTemplateChooseDia(QWidget *parent, const char *name, KInstance* global,
                                         const QCString &format, const QString &nativePattern,
                                         const QString &nativeName, const DialogType &dialogType,
                                         const QCString& templateType) :
    KDialogBase(parent, name, true, i18n("Choose"), KDialogBase::Ok | KDialogBase::Cancel,
                KDialogBase::Ok) {

    d=new KoTemplateChooseDiaPrivate(templateType, global, format, nativePattern,
                                     nativeName, dialogType);
    enableButtonOK(false);

    if(!templateType.isNull() && !templateType.isEmpty() && dialogType!=NoTemplates)
        d->tree=new KoTemplateTree(templateType, global, true);

    d->m_mainwidget=makeMainWidget();
    setupDialog();

    d->m_templateName = "";
    d->m_fullTemplateName = "";
    d->m_returnType = Cancel;

    connect(this, SIGNAL(okClicked()), SLOT(ok()));
}

KoTemplateChooseDia::~KoTemplateChooseDia() {
    delete d->tree;
    delete d;
    d=0L;
}

/*================================================================*/
KoTemplateChooseDia::ReturnType KoTemplateChooseDia::choose(KInstance* global, QString &file,
                                                            const QCString &format, const QString &nativePattern,
                                                            const QString &nativeName,
                                                            const KoTemplateChooseDia::DialogType &dialogType,
                                                            const QCString& templateType) {
    bool res = false;
    KoTemplateChooseDia *dlg = new KoTemplateChooseDia( 0, "Choose", global, format, nativePattern,
                                                        nativeName, dialogType, templateType);
    if(dialogType!=NoTemplates)
        dlg->resize( 500, 400 );
    else
        dlg->resize( 10, 10 );  // geometry managed!

    if ( dlg->exec() == QDialog::Accepted ) {
        res = true;
        file = dlg->getFullTemplate();
    }

    KoTemplateChooseDia::ReturnType rt = dlg->getReturnType();
    delete dlg;

    return res ? rt : KoTemplateChooseDia::Cancel;
}

QString KoTemplateChooseDia::getTemplate() {
    return d->m_templateName;
}

QString KoTemplateChooseDia::getFullTemplate() {
    return d->m_fullTemplateName;
}

KoTemplateChooseDia::ReturnType KoTemplateChooseDia::getReturnType() {
    return d->m_returnType;
}

KoTemplateChooseDia::DialogType KoTemplateChooseDia::getDialogType() {
    return d->m_dialogType;
}

void KoTemplateChooseDia::setupDialog()
{
    QGridLayout *grid=new QGridLayout( d->m_mainwidget, 10, 1, KDialogBase::marginHint(),
                                       KDialogBase::spacingHint() );
    KSeparator *line;

    if ( d->m_dialogType==Everything ) {
        line = new KSeparator( QFrame::HLine, d->m_mainwidget );
        grid->addWidget( line, 0, 0 );

        d->m_rbTemplates = new QRadioButton( i18n( "Create new document from a &Template" ), d->m_mainwidget );
        connect( d->m_rbTemplates, SIGNAL( clicked() ), this, SLOT( openTemplate() ) );
        grid->addWidget( d->m_rbTemplates, 1, 0 );
    }

    if ( d->tree && d->m_dialogType!=NoTemplates ) {
        d->m_tabs = new QTabWidget( d->m_mainwidget );

        // for sorting stuff (ugly, will be replaced)
        QDict<QVBox> tabDict;
        QStringList tabNames;

        for ( KoTemplateGroup *group = d->tree->first(); group!=0L; group=d->tree->next() ) {
            if(group->isHidden())
                continue;
            QVBox *tab=new QVBox( d->m_tabs );
            MyIconCanvas *canvas = new MyIconCanvas( tab );
            canvas->setBackgroundColor( colorGroup().base() );
            canvas->setResizeMode(QIconView::Adjust);
            canvas->setWordWrapIconText( true ); // DF
            canvas->show();
            canvas->load(group);
            canvas->sort();
            connect( canvas, SIGNAL( doubleClicked( QIconViewItem * ) ),
                     this, SLOT( chosen(QIconViewItem *) ) );
            connect( canvas, SIGNAL( currentChanged( QIconViewItem * ) ),
                     this, SLOT( currentChanged( QIconViewItem * ) ) );
            d->canvasDict.insert(group->name(), canvas);
            // *uuuugggggllllyyyyyy* (Werner)
            tabDict.insert(group->name(), tab);
            tabNames.append(group->name());
        }
        tabNames.sort();
        for(QStringList::ConstIterator it=tabNames.begin(); it!=tabNames.end(); ++it)
            d->m_tabs->addTab( tabDict[(*it)], (*it) );

        KoTemplateGroup *defaultGroup=d->tree->defaultGroup();
        if(defaultGroup)
            d->m_tabs->showPage(tabDict[defaultGroup->name()]);

        connect( d->m_tabs, SIGNAL( selected( const QString & ) ), this, SLOT( tabsChanged( const QString & ) ) );
        grid->addWidget( d->m_tabs, 2, 0 );
    }

    line = new KSeparator( QFrame::HLine, d->m_mainwidget );
    grid->addWidget( line, 3, 0 );

    if ( d->m_dialogType!=OnlyTemplates ) {
        QHBoxLayout *row = new QHBoxLayout( grid );
        d->m_rbFile = new QRadioButton( i18n( "&Open an existing document" ), d->m_mainwidget );
        connect( d->m_rbFile, SIGNAL( clicked() ), this, SLOT( openFile() ) );
        row->addWidget(d->m_rbFile);
        row->addSpacing(30);
        d->m_bFile = new QPushButton( i18n( "Choose..." ), d->m_mainwidget );
        d->m_bFile->setMaximumSize( d->m_bFile->sizeHint() );
        row->addWidget(d->m_bFile);
        connect( d->m_bFile, SIGNAL( clicked() ), this, SLOT( chooseFile() ) );

        line = new KSeparator( QFrame::HLine, d->m_mainwidget );
        grid->addWidget( line, 5, 0 );

        row = new QHBoxLayout( grid );
        d->m_rbRecent = new QRadioButton( i18n( "&Open a recent document" ), d->m_mainwidget );
        connect( d->m_rbRecent, SIGNAL( clicked() ), this, SLOT( openRecent() ) );
        row->addWidget(d->m_rbRecent);
        row->addSpacing(30);
        d->m_recent = new QComboBox( d->m_mainwidget, "recent files" );

        QString oldGroup;
        oldGroup=d->m_global->config()->group();
        d->m_global->config()->setGroup( "RecentFiles" );

        // read file list
        QStringList lst;
        int i=1;
        QString key=QString( "File%1" ).arg( i );
        QString value=d->m_global->config()->readEntry( key, QString::null );
        if(value.startsWith("file:"))
            value=value.mid(5);
        QString squeezed=KStringHandler::csqueeze(value);
        while( !squeezed.isNull() ) {
            lst.append( squeezed );
            d->recentFilesMap.insert(squeezed, value);
            ++i;
            key=QString( "File%1" ).arg( i );
            value=d->m_global->config()->readEntry( key, QString::null );
            if(value.startsWith("file:"))
                value=value.mid(5);
            squeezed=KStringHandler::csqueeze(value);
        }

        // set file
        if( lst.isEmpty() )
            lst.append( i18n("No recent files available") );
        d->m_recent->insertStringList( lst );
        d->m_global->config()->setGroup( oldGroup );

        row->addWidget(d->m_recent);
        connect( d->m_recent, SIGNAL( activated(int) ), this, SLOT( openRecent() ) );

        line = new KSeparator( QFrame::HLine, d->m_mainwidget );
        grid->addWidget( line, 7, 0 );

        d->m_rbEmpty = new QRadioButton( i18n( "Start with an &empty document" ), d->m_mainwidget );
        connect( d->m_rbEmpty, SIGNAL( clicked() ), this, SLOT( openEmpty() ) );
        grid->addWidget( d->m_rbEmpty, 8, 0 );

        line = new KSeparator( QFrame::HLine, d->m_mainwidget );
        grid->addWidget( line, 9, 0 );
        openEmpty();
    }
}

/*================================================================*/
void KoTemplateChooseDia::currentChanged( QIconViewItem * )
{
    openTemplate();
}

/*================================================================*/
void KoTemplateChooseDia::chosen(QIconViewItem *)
{
    slotOk();
}

/*================================================================*/
void KoTemplateChooseDia::openTemplate()
{
    if ( d->m_dialogType==Everything ) {
        d->m_rbTemplates->setChecked( true );
        d->m_rbFile->setChecked( false );
        d->m_rbRecent->setChecked( false );
        d->m_rbEmpty->setChecked( false );
    }
    enableButtonOK( true );
}

/*================================================================*/
void KoTemplateChooseDia::openFile()
{
    if(d->m_dialogType!=NoTemplates)
        d->m_rbTemplates->setChecked( false );
    d->m_rbFile->setChecked( true );
    d->m_rbRecent->setChecked( false );
    d->m_rbEmpty->setChecked( false );
}


/*================================================================*/
void KoTemplateChooseDia::openRecent()
{
    if(d->m_dialogType!=NoTemplates)
        d->m_rbTemplates->setChecked( false );
    d->m_rbFile->setChecked( false );
    d->m_rbRecent->setChecked( true );
    d->m_rbEmpty->setChecked( false );
    d->m_file=d->recentFilesMap[d->m_recent->currentText()];
    enableButtonOK( false ); // because of that async stuff
    enableButtonOK( KIO::NetAccess::exists( KURL( d->m_file ) ) );
}

/*================================================================*/
void KoTemplateChooseDia::openEmpty()
{
    if(d->m_dialogType!=NoTemplates)
        d->m_rbTemplates->setChecked( false );
    d->m_rbFile->setChecked( false );
    d->m_rbRecent->setChecked( false );
    d->m_rbEmpty->setChecked( true );

    enableButtonOK( true );
}

/*================================================================*/
void KoTemplateChooseDia::chooseFile()
{
    // Save current state - in case of Cancel
    bool bEmpty = d->m_rbEmpty->isChecked();
    bool bTemplates = (d->m_dialogType!=NoTemplates) && d->m_rbTemplates->isChecked();
    openFile();

    // Use dir from currently selected file
    QString dir = QString::null;
    if ( QFile::exists( d->m_file ) )
        dir = QFileInfo( d->m_file ).absFilePath();

    KFileDialog *dialog=new KFileDialog(dir, QString::null, 0L, "file dialog", true);
    dialog->setCaption( i18n("Open document") );
    KoFilterManager::self()->prepareDialog(dialog, KoFilterManager::Import, d->m_format,
                                           d->m_nativePattern, d->m_nativeName, true);
    KURL u;

    if(dialog->exec()==QDialog::Accepted)
    {
        u=dialog->selectedURL();
        KRecentDocument::add(dialog->selectedURL().url(), !dialog->selectedURL().isLocalFile());
    } else //revert state
    {
        if (bEmpty) openEmpty();
        if (bTemplates) openTemplate();
    }

    KoFilterManager::self()->cleanUp();
    delete dialog;

    QString filename = u.path();
    QString url = u.url();
    bool local = u.isLocalFile();

    bool ok = !url.isEmpty();
    if (local) // additionnal checks for local files
        ok = ok && (QFileInfo( filename ).isFile() ||
                    (QFileInfo( filename ).isSymLink() &&
                     !QFileInfo( filename ).readLink().isEmpty() &&
                     QFileInfo( QFileInfo( filename ).readLink() ).isFile() ) );

    if ( ok )
    {
        if (local)
            d->m_file=filename;
        else
            d->m_file=url;
        slotOk();
    }
}

void KoTemplateChooseDia::tabsChanged( const QString & ) {
    if ( !d->m_firstTime ) openTemplate();
    d->m_firstTime = false;
}

void KoTemplateChooseDia::ok() {

    if ( d->m_dialogType==OnlyTemplates || d->m_dialogType==Everything && d->m_rbTemplates->isChecked() ) {
        d->m_returnType = Template;
        QString groupName=d->m_tabs->tabLabel(d->m_tabs->currentPage());
        MyIconCanvas *canvas=d->canvasDict.find(groupName);
        if(!canvas || !canvas->currentItem()) {
            d->m_returnType=Empty;
            return;
        }
        QString templateName=canvas->currentItem()->text();
        KoTemplateGroup *group=d->tree->find(groupName);
        if(!group) {
            d->m_returnType=Empty;
            return;
        }
        KoTemplate *t=group->find(templateName);
        if(!t) {
            d->m_returnType=Empty;
            return;
        }
        d->m_templateName=t->name();
        d->m_fullTemplateName=t->file();
    }
    else if ( d->m_dialogType!=OnlyTemplates && (d->m_rbFile->isChecked() || d->m_rbRecent->isChecked()) ) {
        d->m_returnType = File;
        d->m_fullTemplateName = d->m_templateName = d->m_file;
    }
    else if ( d->m_dialogType!=OnlyTemplates && d->m_rbEmpty->isChecked() )
        d->m_returnType = Empty;
    else
        d->m_returnType = Cancel;
}


void MyIconCanvas::load( KoTemplateGroup *group )
{
    for(KoTemplate *t=group->first(); t!=0L; t=group->next()) {
        if(t->isHidden())
            continue;
        QIconViewItem *item = new QIconViewItem(this, t->name(), t->loadPicture());
        item->setKey(t->name());
        item->setDragEnabled(false);
        item->setDropEnabled(false);
    }
}

#include <koTemplateChooseDia.moc>
