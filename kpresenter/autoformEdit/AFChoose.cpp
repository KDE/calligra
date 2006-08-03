// -*- Mode: c++-mode; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#include "AFChoose.h"

#include <QLabel>
#include <kvbox.h>
#include <qtextstream.h>
#include <QDir>

//Added by qt3to4:
#include <QCloseEvent>
#include <QPixmap>

#include <klocale.h>
#include <ksimpleconfig.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <kicondialog.h>

#include <KPrFactory.h>

AFChoose::AFChoose(QWidget *parent, const QString &caption, const char *name)
    : Q3TabDialog(parent,name,true)
{
    setCaption(caption);
    setCancelButton(i18n("&Cancel"));
    setOkButton(i18n("&OK"));
    groupList.setAutoDelete(true);
    getGroups();
    setupTabs();
    connect(this,SIGNAL(applyButtonPressed()),this,SLOT(chosen()));
    connect(this,SIGNAL(cancelButtonPressed()),this,SLOT(cancelClicked()));
}

AFChoose::~AFChoose()
{
}

void AFChoose::getGroups()
{
    // global autoforms (as we don't have an editor we don't have local ones)
    QString afDir = KStandardDirs::locate( "autoforms", ".autoforms", KPrFactory::global() );

    QFile f( afDir );
    if ( f.open(QIODevice::ReadOnly) ) {
        QTextStream t( &f );
        QString s;
        while ( !t.atEnd() ) {
            s = t.readLine();
            if ( !s.isEmpty() ) {
                grpPtr = new Group;
                QString directory=QFileInfo( afDir ).dirPath() + "/" + s.simplified();
                grpPtr->dir.setFile(directory);
                QDir d(directory);
                if(d.exists(".directory")) {
                    KSimpleConfig config(d.absolutePath()+"/.directory", true);
                    config.setDesktopGroup();
                    grpPtr->name=config.readEntry("Name");
                }
                groupList.append( grpPtr );
            }
        }
        f.close();
    }
}

void AFChoose::setupTabs()
{
    if (!groupList.isEmpty())
    {
        for (grpPtr=groupList.first();grpPtr != 0;grpPtr=groupList.next())
        {
            grpPtr->tab = new KVBox(this);
            grpPtr->tab->setWhatsThis( i18n( "Choose a predefined shape by clicking on it then clicking the OK button (or just double-click on the shape). You can then insert the shape onto your slide by drawing the area with the mouse pointer." ) );
            grpPtr->loadWid = new KIconCanvas(grpPtr->tab);
            // Changes for the new KIconCanvas (Werner)
            QDir d( grpPtr->dir.absoluteFilePath() );
            d.setNameFilter( "*.desktop" );
            if( d.exists() ) {
                QStringList files=d.entryList( QDir::Files | QDir::Readable, QDir::Name );
                for(unsigned int i=0; i<files.count(); ++i) {
                    QString path=grpPtr->dir.absoluteFilePath() + QChar('/');
                    files[i]=path + files[i];
                    KSimpleConfig config(files[i]);
                    config.setDesktopGroup();
                    if (config.readEntry("Type")=="Link") {
                        QString text=config.readEntry("Name");
                        QString icon=config.readEntry("Icon");
                        if(icon[0]!='/') // allow absolute paths for icons
                            icon=path + icon;
                        QString filename=config.readPathEntry("URL");
                        if(filename[0]!='/') {
                            if(filename.left(6)=="file:/") // I doubt this will happen
                                filename=filename.right(filename.length()-6);
                            filename=path + filename;
                        }
                        grpPtr->entries.insert(text, filename);
                        // now load the icon and create the item
                        // This code is shamelessly borrowed from KIconCanvas::slotLoadFiles
                        QImage img;
                        img.load(icon);
                        if (img.isNull()) {
                            kWarning() << "Couldn't find icon " << icon << endl;
                            continue;
                        }
                        if (img.width() > 60 || img.height() > 60) {
                            if (img.width() > img.height()) {
                                int height = (int) ((60.0 / img.width()) * img.height());
                                img = img.smoothScale(60, height);
                            } else {
                                int width = (int) ((60.0 / img.height()) * img.width());
                                img = img.smoothScale(width, 60);
                            }
                        }
                        QPixmap pic;
                        pic.convertFromImage(img);
                        Q3IconViewItem *item = new Q3IconViewItem(grpPtr->loadWid, text, pic);
                        item->setKey(text);
                        item->setDragEnabled(false);
                        item->setDropEnabled(false);
                    } else
                        continue; // Invalid .desktop file
                }
            }
            grpPtr->loadWid->setBackgroundColor(colorGroup().base());
            grpPtr->loadWid->setResizeMode(Q3IconView::Adjust);
            grpPtr->loadWid->sort();
            connect(grpPtr->loadWid,SIGNAL(nameChanged(QString)),
                    this,SLOT(nameChanged(QString)));
            connect(this, SIGNAL(currentChanged(QWidget *)), this,
                    SLOT(tabChanged(QWidget*)));
            connect(grpPtr->loadWid,SIGNAL( doubleClicked ( Q3IconViewItem *)),this,
                    SLOT(slotDoubleClick()));
            grpPtr->label = new QLabel(grpPtr->tab);
            grpPtr->label->setText(" ");
            grpPtr->label->setMaximumHeight(grpPtr->label->sizeHint().height());
            addTab(grpPtr->tab,grpPtr->name);
        }
    }
}

void AFChoose::slotDoubleClick()
{
    chosen();
    accept();
}

void AFChoose::nameChanged(QString name)
{
    for (grpPtr=groupList.first();grpPtr != 0;grpPtr=groupList.next())
        grpPtr->label->setText(name);
}

void AFChoose::tabChanged(QWidget *w) {

    for(grpPtr=groupList.first();grpPtr != 0;grpPtr=groupList.next()) {
        if(grpPtr->tab==w)
            grpPtr->label->setText(grpPtr->loadWid->getCurrent());
    }
}

void AFChoose::chosen()
{
    if (!groupList.isEmpty())
    {
        for (grpPtr=groupList.first();grpPtr != 0;grpPtr=groupList.next())
        {
            if (grpPtr->tab->isVisible() && !grpPtr->loadWid->getCurrent().isEmpty())
                emit formChosen(grpPtr->entries[grpPtr->loadWid->getCurrent()]);
            else
                emit afchooseCanceled();
        }
    }
}

void AFChoose::cancelClicked()
{
    emit afchooseCanceled();
}

void AFChoose::closeEvent ( QCloseEvent *e )
{
    emit afchooseCanceled();
    Q3TabDialog::closeEvent ( e );
}

#include "AFChoose.moc"
