/*
   This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
                 2000 Werner Trobin <wtrobin@mandrakesoft.com>

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
#include <kdebug.h>


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
	m_popup=0L;
    }
    ~KoTemplateCreateDiaPrivate() {
	delete m_tree;
	delete m_popup;
    }

    KoTemplateTree *m_tree;
    KLineEdit *m_name;
    QRadioButton *m_default, *m_custom;
    QPushButton *m_select;
    QLabel *m_preview;
    QString m_customFile;
    QPixmap m_customPixmap;
    KListView *m_groups;

    QPopupMenu *m_popup;
    QListViewItem *m_currentItem;
};


/****************************************************************************
 *
 * Class: koTemplateCreateDia
 *
 ****************************************************************************/

KoTemplateCreateDia::KoTemplateCreateDia( const QString &templateType, KInstance *instance,
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
    connect(d->m_groups, SIGNAL(rightButtonPressed(QListViewItem *, const QPoint &, int)),
	    this, SLOT(slotPopup(QListViewItem *, const QPoint &, int)));

    d->m_tree=new KoTemplateTree(templateType, instance, true);
    fillGroupTree();

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
    updatePixmap();
}

void KoTemplateCreateDia::createTemplate( const QString &templateType, KInstance *instance,
					  const QString &file, const QPixmap &pix, QWidget *parent ) {

    KoTemplateCreateDia *dia = new KoTemplateCreateDia( templateType, instance, file, pix, parent );
    dia->exec();
    delete dia;
}

void KoTemplateCreateDia::slotOk() {

    // add the template to the tree and write the tree to the disk
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
    if(d->m_customFile.isNull() || d->m_customFile.isEmpty())
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
	KMessageBox::sorry(0L, i18n( "Only local files supported, yet."));
	return;
    }
    d->m_customFile=url.path();
    d->m_customPixmap=QPixmap();
    updatePixmap();
}

void KoTemplateCreateDia::slotNameChanged(const QString &name) {

    if(name.isEmpty() || name.isNull())
	enableButtonOK(false);
    else
	enableButtonOK(true);
}

void KoTemplateCreateDia::slotPopup(QListViewItem *item, const QPoint &p, int) {

    d->m_currentItem=item;

    if(!d->m_popup) {
	d->m_popup=new QPopupMenu(0, "create-template popup");
	d->m_popup->insertItem(i18n("&New Group..."), this, SLOT(slotNewGroup()));
	d->m_popup->insertItem(i18n("&Remove"), this, SLOT(slotRemove()));
    }
    d->m_popup->popup(p);
}

void KoTemplateCreateDia::slotNewGroup() {

    QString name=KoNewGroupDia::newGroupName(this);
    if(name.isNull() || name.isEmpty())
	return;

    d->m_groups->clear();
    fillGroupTree();
}

void KoTemplateCreateDia::slotRemove() {
}

void KoTemplateCreateDia::updatePixmap() {

    if(d->m_default->isChecked() && !m_pixmap.isNull())
	d->m_preview->setPixmap(m_pixmap);
    else if(d->m_custom->isChecked() && !d->m_customFile.isNull()) {
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
	QListViewItem *groupItem=new QListViewItem(d->m_groups, group->name());
	for(KoTemplate *t=group->first(); t!=0L; t=group->next())
	    (void)new QListViewItem(groupItem, t->name());
    }
}


KoNewGroupDia::KoNewGroupDia(QWidget *parent) :
    KDialogBase(parent, "KoNewGroupDia", true, i18n("Enter a name"),
		KDialogBase::Ok | KDialogBase::Cancel) {

    QFrame *mainwidget=makeMainWidget();
    QGridLayout *grid=new QGridLayout(mainwidget, 4, 2, KDialogBase::marginHint(),
				      KDialogBase::spacingHint());
    QLabel *label=new QLabel(i18n("Please enter the name of the new group."), mainwidget);
    grid->addMultiCellWidget(label, 1, 1, 0, 1);
    label=new QLabel(i18n("Name:"), mainwidget);
    grid->addWidget(label, 2, 0);
    m_name=new KLineEdit(mainwidget);
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

    if(name.isNull() || name.isEmpty())
	enableButtonOK(false);
    else
	enableButtonOK(true);
}

#include <koTemplateCreateDia.moc>
