/*
   This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
                 2000 Werner Trobin <trobin@kde.org>

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

#include <koTemplateCreateDia.h>

#include <qlayout.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qheader.h>
#include <qpopupmenu.h>

#include <klineedit.h>
#include <klistview.h>
#include <klocale.h>
#include <koTemplates.h>
#include <kfiledialog.h>
#include <kurl.h>
#include <kmessagebox.h>
#include <kimageio.h>
#include <kstddirs.h>
#include <kdebug.h>
#include <kio/netaccess.h>

#include <stdlib.h>


class KoTemplateCreateDiaPrivate {
public:
    KoTemplateCreateDiaPrivate()
    {
        m_tree=0L;
        m_name=0L;
        m_default=0L;
        m_custom=0L;
        m_select=0L;
        m_preview=0L;
        m_groups=0L;
        m_add=0L;
        m_remove=0L;
    }
    ~KoTemplateCreateDiaPrivate() {
        delete m_tree;
    }

    KoTemplateTree *m_tree;
    KLineEdit *m_name;
    QRadioButton *m_default, *m_custom;
    QPushButton *m_select;
    QLabel *m_preview;
    QString m_customFile;
    QPixmap m_customPixmap;
    KListView *m_groups;
    QPushButton *m_add, *m_remove;
    bool m_changed;
};


/****************************************************************************
 *
 * Class: koTemplateCreateDia
 *
 ****************************************************************************/

KoTemplateCreateDia::KoTemplateCreateDia( const QCString &templateType, KInstance *instance,
                                          const QString &file, const QPixmap &pix, QWidget *parent ) :
    KDialogBase( parent, "template create dia", true, i18n( "Create a Template" ),
                 KDialogBase::Ok | KDialogBase::Cancel, KDialogBase::Ok ), m_file(file), m_pixmap(pix) {

    d=new KoTemplateCreateDiaPrivate();

    QFrame *mainwidget=makeMainWidget();
    QHBoxLayout *mbox=new QHBoxLayout(mainwidget, KDialogBase::marginHint(),
                                      KDialogBase::spacingHint());
    QVBoxLayout *leftbox=new QVBoxLayout(mbox);

    QLabel *label=new QLabel(i18n("Name:"), mainwidget);
    leftbox->addSpacing(label->fontMetrics().height()/2);
    QHBoxLayout *namefield=new QHBoxLayout(leftbox);
    namefield->addWidget(label);
    d->m_name=new KLineEdit(mainwidget);
    d->m_name->setFocus();
    connect(d->m_name, SIGNAL(textChanged(const QString &)),
            this, SLOT(slotNameChanged(const QString &)));
    namefield->addWidget(d->m_name);

    label=new QLabel(i18n("Group:"), mainwidget);
    leftbox->addWidget(label);
    d->m_groups=new KListView(mainwidget);
    leftbox->addWidget(d->m_groups);
    d->m_groups->addColumn("");
    d->m_groups->header()->hide();
    d->m_groups->setRootIsDecorated(true);
    d->m_groups->setSorting(0);

    d->m_tree=new KoTemplateTree(templateType, instance, true);
    fillGroupTree();
    d->m_groups->sort();

    connect( d->m_groups, SIGNAL( clicked( QListViewItem * ) ),
             SLOT( slotItemClicked( QListViewItem * ) ) );

    QHBoxLayout *bbox=new QHBoxLayout(leftbox);
    d->m_add=new QPushButton(i18n("Add Group..."), mainwidget);
    connect(d->m_add, SIGNAL(clicked()), this, SLOT(slotAddGroup()));
    bbox->addWidget(d->m_add);
    d->m_remove=new QPushButton(i18n("Remove"), mainwidget);
    connect(d->m_remove, SIGNAL(clicked()), this, SLOT(slotRemove()));
    bbox->addWidget(d->m_remove);

    QVBoxLayout *rightbox=new QVBoxLayout(mbox);
    QGroupBox *pixbox=new QGroupBox(i18n("Picture:"), mainwidget);
    rightbox->addWidget(pixbox);
    QVBoxLayout *pixlayout=new QVBoxLayout(pixbox, KDialogBase::marginHint(),
                                           KDialogBase::spacingHint());
    pixlayout->addSpacing(pixbox->fontMetrics().height()/2);
    pixlayout->addStretch(1);
    d->m_default=new QRadioButton(i18n("Default"), pixbox);
    d->m_default->setChecked(true);
    connect(d->m_default, SIGNAL(clicked()), this, SLOT(slotDefault()));
    pixlayout->addWidget(d->m_default);
    QHBoxLayout *custombox=new QHBoxLayout(pixlayout);
    d->m_custom=new QRadioButton(i18n("Custom"), pixbox);
    d->m_custom->setChecked(false);
    connect(d->m_custom, SIGNAL(clicked()), this, SLOT(slotCustom()));
    custombox->addWidget(d->m_custom);
    d->m_select=new QPushButton(i18n("Select..."), pixbox);
    connect(d->m_select, SIGNAL(clicked()), this, SLOT(slotSelect()));
    custombox->addWidget(d->m_select, 1);
    custombox->addStretch(1);
    pixlayout->addStretch(1);
    label=new QLabel(i18n("Preview:"), pixbox);
    pixlayout->addWidget(label);
    QHBoxLayout *previewbox=new QHBoxLayout(pixlayout);
    previewbox->addStretch(10);
    d->m_preview=new QLabel(pixbox); // setPixmap() -> auto resize?
    previewbox->addWidget(d->m_preview);
    previewbox->addStretch(10);
    pixlayout->addStretch(8);

    enableButtonOK(false);
    d->m_changed=false;
    updatePixmap();
}

KoTemplateCreateDia::~KoTemplateCreateDia() {
    delete d;
}

void KoTemplateCreateDia::createTemplate( const QCString &templateType, KInstance *instance,
                                          const QString &file, const QPixmap &pix, QWidget *parent ) {

    KoTemplateCreateDia *dia = new KoTemplateCreateDia( templateType, instance, file, pix, parent );
    dia->exec();
    delete dia;
}

void KoTemplateCreateDia::slotOk() {

    // get the current item, if there is one...
    QListViewItem *item=d->m_groups->currentItem();
    if(!item)
        item=d->m_groups->firstChild();
    if(!item) {    // safe :)
        d->m_tree->writeTemplateTree();
        KDialogBase::slotCancel();
        return;
    }
    // is it a group or a template? anyway - get the group :)
    if(item->depth()!=0)
        item=item->parent();
    if(!item) {    // *very* safe :P
        d->m_tree->writeTemplateTree();
        KDialogBase::slotCancel();
        return;
    }

    KoTemplateGroup *group=d->m_tree->find(item->text(0));
    if(!group) {    // even safer
        d->m_tree->writeTemplateTree();
        KDialogBase::slotCancel();
        return;
    }

    if(d->m_name->text().isEmpty()) {
        d->m_tree->writeTemplateTree();
        KDialogBase::slotCancel();
        return;
    }

    // copy the tmp file and the picture the app provides
    QString dir=d->m_tree->instance()->dirs()->saveLocation(d->m_tree->templateType());
    dir+=KoTemplates::stripWhiteSpace(group->name());
    QString templateDir=dir+"/.source/";
    QString iconDir=dir+"/.icon/";

    QString file=KoTemplates::stripWhiteSpace(d->m_name->text());
    QString icon=iconDir+file;
    icon+=".png";

    // try to find the extension for the template file :P
    unsigned int k=0;
    unsigned int foo=m_file.length();
    while(m_file[foo-k]!=QChar('.') && k<=foo) {
        ++k;
    }
    if(k<foo)
        file+=m_file.right(k);

    KoTemplate *t=new KoTemplate(d->m_name->text(), templateDir+file, icon, false, true);
    if(!group->add(t)) {
        KoTemplate *existingTemplate=group->find(t->name());
        // if the original template is hidden, we simply force the update >:->
        if(existingTemplate && existingTemplate->isHidden())
            group->add(t, true);
        // Otherwise ask the user
        else if(existingTemplate && !existingTemplate->isHidden()) {
            if(KMessageBox::warningYesNo(this, i18n("Do you really want to overwrite"
                                                    " the existing '%1' template?").
                                         arg(existingTemplate->name()))==KMessageBox::Yes)
                group->add(t, true);
            else
            {
                delete t;
                return;
            }
        }
    }


    if(!KStandardDirs::makeDir(templateDir) || !KStandardDirs::makeDir(iconDir)) {
        d->m_tree->writeTemplateTree();
        KDialogBase::slotCancel();
        return;
    }

    // copy the template file
    KURL dest;
    dest.setPath(templateDir+file);
    KIO::NetAccess::copy(m_file, dest);

    // if there's a .directory file, we copy this one, too
    bool ready=false;
    QStringList tmp=group->dirs();
    for(QStringList::ConstIterator it=tmp.begin(); it!=tmp.end() && !ready; ++it) {
        if((*it).contains(dir)==0) {
            QString file=(*it)+".directory";
            QFileInfo info(file);
            if(info.exists()) {
                KIO::NetAccess::copy(file, dir);
                ready=true;
            }
        }
    }
    // save the picture
    if(d->m_default->isChecked() && !m_pixmap.isNull())
        m_pixmap.save(icon, "PNG");
    else if(!d->m_customPixmap.isNull())
        d->m_customPixmap.save(icon, "PNG");
    else
        kdWarning(30004) << "Could not save the preview picture!" << endl;

    d->m_tree->writeTemplateTree();
    KDialogBase::slotOk();
}

void KoTemplateCreateDia::slotDefault() {

    d->m_default->setChecked(true);
    d->m_custom->setChecked(false);
    updatePixmap();
}

void KoTemplateCreateDia::slotCustom() {

    d->m_default->setChecked(false);
    d->m_custom->setChecked(true);
    if(d->m_customFile.isEmpty())
        slotSelect();
    else
        updatePixmap();
}

void KoTemplateCreateDia::slotSelect() {

    d->m_default->setChecked(false);
    d->m_custom->setChecked(true);

    KFileDialog fd(QString::null, KImageIO::pattern(KImageIO::Reading), 0, 0, true);
    fd.setCaption(i18n("Select a Picture"));
    KURL url;
    if (fd.exec()==QDialog::Accepted)
        url=fd.selectedURL();

    if(url.isEmpty()) {
        if(d->m_customFile.isEmpty()) {
            d->m_default->setChecked(true);
            d->m_custom->setChecked(false);
        }
        return;
    }

    if(!url.isLocalFile()) {
        KMessageBox::sorry(0L, i18n( "Only local files are currently supported."));
        return;
    }
    d->m_customFile=url.path();
    d->m_customPixmap=QPixmap();
    updatePixmap();
}

void KoTemplateCreateDia::slotItemClicked( QListViewItem * item ) {
    if ( item )
    {
        QString name = item->text( 0 );
        d->m_name->setText( name ); // calls slotNameChanged
    }
}

void KoTemplateCreateDia::slotNameChanged(const QString &name) {

    if((name.isEmpty() || !d->m_groups->firstChild()) && !d->m_changed)
        enableButtonOK(false);
    else
        enableButtonOK(true);
}

void KoTemplateCreateDia::slotAddGroup() {

    QString name=KoNewGroupDia::newGroupName(this);
    if(name.isEmpty())
        return;
    KoTemplateGroup *group=d->m_tree->find(name);
    if(group && !group->isHidden())
        return;

    QString dir=d->m_tree->instance()->dirs()->saveLocation(d->m_tree->templateType());
    dir+=name;
    KoTemplateGroup *newGroup=new KoTemplateGroup(name, dir, true);
    d->m_tree->add(newGroup);
    QListViewItem *item=new QListViewItem(d->m_groups, name);
    d->m_groups->setCurrentItem(item);
    d->m_groups->sort();
    d->m_name->setFocus();
    enableButtonOK(true);
    d->m_changed=true;
}

void KoTemplateCreateDia::slotRemove() {

    QListViewItem *item=d->m_groups->currentItem();
    if(!item)
        return;

    QString what;
        QString removed;
        if (item->depth()==0) {
                what =  i18n("Do you really want to remove that group?");
                removed = i18n("Remove group");
        } else {
                what =  i18n("Do you really want to remove that template?");
        removed = i18n("Remove template");
        }

    if(KMessageBox::warningYesNo(this, what,
                                 removed)==KMessageBox::No) {
        d->m_name->setFocus();
        return;
    }

    if(item->depth()==0) {
        KoTemplateGroup *group=d->m_tree->find(item->text(0));
        if(group)
            group->setHidden(true);
    }
    else {
        bool done=false;
        for(KoTemplateGroup *g=d->m_tree->first(); g!=0L && !done; g=d->m_tree->next()) {
            KoTemplate *t=g->find(item->text(0));
            if(t) {
                t->setHidden(true);
                done=true;
            }
        }
    }
    delete item;
    item=0L;
    enableButtonOK(true);
    d->m_name->setFocus();
    d->m_changed=true;
}

void KoTemplateCreateDia::updatePixmap() {

    if(d->m_default->isChecked() && !m_pixmap.isNull())
        d->m_preview->setPixmap(m_pixmap);
    else if(d->m_custom->isChecked() && !d->m_customFile.isEmpty()) {
        if(d->m_customPixmap.isNull()) {
            // use the code in KoTemplate to load the image... hacky, I know :)
            KoTemplate t("foo", "bar", d->m_customFile);
            d->m_customPixmap=t.loadPicture();
        }
        if(!d->m_customPixmap.isNull())
            d->m_preview->setPixmap(d->m_customPixmap);
        else
            d->m_preview->setText(i18n("Could not load picture."));
    }
    else
        d->m_preview->setText(i18n("No picture available."));
}

void KoTemplateCreateDia::fillGroupTree() {

    for(KoTemplateGroup *group=d->m_tree->first(); group!=0L; group=d->m_tree->next()) {
        if(group->isHidden())
            continue;
        QListViewItem *groupItem=new QListViewItem(d->m_groups, group->name());
        for(KoTemplate *t=group->first(); t!=0L; t=group->next()) {
            if(t->isHidden())
                continue;
            (void)new QListViewItem(groupItem, t->name());
        }
    }
}


KoNewGroupDia::KoNewGroupDia(QWidget *parent) :
    KDialogBase(parent, "KoNewGroupDia", true, i18n("Enter a name"),
                KDialogBase::Ok | KDialogBase::Cancel, KDialogBase::Ok) {

    QFrame *mainwidget=makeMainWidget();
    QGridLayout *grid=new QGridLayout(mainwidget, 4, 2, KDialogBase::marginHint(),
                                      KDialogBase::spacingHint());
    QLabel *label=new QLabel(i18n("Please enter the name of the new group."), mainwidget);
    grid->addMultiCellWidget(label, 1, 1, 0, 1);
    label=new QLabel(i18n("Name:"), mainwidget);
    grid->addWidget(label, 2, 0);
    m_name=new KLineEdit(mainwidget);
    m_name->setFocus();
    connect(m_name, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotTextChanged(const QString&)));
    grid->addWidget(m_name, 2, 1);
    grid->setRowStretch(0, 1);
    grid->setRowStretch(3, 1);
    enableButtonOK(false);
}

QString KoNewGroupDia::newGroupName(QWidget *parent) {

    KoNewGroupDia *dia=new KoNewGroupDia(parent);
    QString name;
    if(dia->exec()==QDialog::Accepted)
        name=dia->name();
    delete dia;
    return name;
}

QString KoNewGroupDia::name() const {
    return m_name->text();
}

void KoNewGroupDia::slotTextChanged(const QString &name) {

    if(name.isEmpty())
        enableButtonOK(false);
    else
        enableButtonOK(true);
}

#include <koTemplateCreateDia.moc>
