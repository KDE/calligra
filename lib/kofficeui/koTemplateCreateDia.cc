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

#include <klineedit.h>
#include <klistview.h>
#include <klocale.h>
#include <koTemplates.h>


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
	m_customPixmapCached=false;
	m_groups=0L;
    }
    ~KoTemplateCreateDiaPrivate() {}

    KoTemplateTree *m_tree;
    KLineEdit *m_name;
    QRadioButton *m_default, *m_custom;
    QPushButton *m_select;
    QLabel *m_preview;
    QString m_customFile;
    QPixmap m_customPixmap;
    bool m_customPixmapCached;
    KListView *m_groups;
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
    namefield->addWidget(d->m_name);

    label=new QLabel(i18n("Group:"), mainwidget);
    leftbox->addWidget(label);
    d->m_groups=new KListView(mainwidget);
    leftbox->addWidget(d->m_groups);
    d->m_groups->addColumn("");
    d->m_groups->header()->hide();
    d->m_groups->setRootIsDecorated(true);

    d->m_tree=new KoTemplateTree(templateType, instance, true);
    // fill group listview

    QVBoxLayout *rightbox=new QVBoxLayout(mbox);
    QGroupBox *pixbox=new QGroupBox(i18n("Picture:"), mainwidget);
    rightbox->addWidget(pixbox);
    QVBoxLayout *pixlayout=new QVBoxLayout(pixbox, KDialogBase::marginHint(),
					   KDialogBase::spacingHint());
    pixlayout->addSpacing(pixbox->fontMetrics().height()/2);
    d->m_default=new QRadioButton(i18n("Default"), pixbox);
    d->m_default->setChecked(true);
    pixlayout->addWidget(d->m_default);
    QHBoxLayout *custombox=new QHBoxLayout(pixlayout);
    d->m_custom=new QRadioButton(i18n("Custom"), pixbox);
    d->m_custom->setChecked(false);
    custombox->addWidget(d->m_custom);
    d->m_select=new QPushButton(i18n("Select..."), pixbox);
    custombox->addWidget(d->m_select);
    label=new QLabel(i18n("Preview:"), pixbox);
    pixlayout->addWidget(label);
    d->m_preview=new QLabel(pixbox); // setPixmap() -> auto resize?
    pixlayout->addWidget(d->m_preview);

    updatePixmap();
}

void KoTemplateCreateDia::createTemplate( const QString &templateType, KInstance *instance,
					  const QString &file, const QPixmap &pix, QWidget *parent ) {

    KoTemplateCreateDia *dia = new KoTemplateCreateDia( templateType, instance, file, pix, parent );
    dia->exec();
    delete dia;
}

void KoTemplateCreateDia::updatePixmap() {

    if(d->m_default->isChecked())
	d->m_preview->setPixmap(m_pixmap);
    else {
    }
}

#include <koTemplateCreateDia.moc>
