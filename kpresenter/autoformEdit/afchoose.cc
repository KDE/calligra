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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "afchoose.h"
#include <klocale.h>
#include "afchoose.moc"
#include <qvbox.h>
#include <qtextstream.h>
#include <kstddirs.h>
#include <qdir.h>
#include "../kpresenter_factory.h"

/******************************************************************/
/* class AFChoose						  */
/******************************************************************/

/*==================== constructor ===============================*/
AFChoose::AFChoose(QWidget *parent,const char *name)
    :QTabDialog(parent,name,true)
{
    setCancelButton(i18n("Cancel"));
    setOkButton(i18n("OK"));
    groupList.setAutoDelete(true);
    getGroups();
    setupTabs();
    connect(this,SIGNAL(applyButtonPressed()),this,SLOT(chosen()));
}

/*===================== destrcutor ===============================*/
AFChoose::~AFChoose()
{
}

/*======================= get Groups =============================*/
void AFChoose::getGroups()
{
    // global autoforms
    QString afDir = locate( "autoforms", ".autoforms", KPresenterFactory::global() );

    QFile f( afDir );
    if ( f.open(IO_ReadOnly) ) {
	QTextStream t( &f );
	QString s;
	while ( !t.eof() ) {
	    s = t.readLine();
	    if ( !s.isEmpty() ) {
		grpPtr = new Group;
		grpPtr->dir.setFile( QFileInfo( afDir ).dirPath() + "/" + s.simplifyWhiteSpace() );
		grpPtr->name = s.simplifyWhiteSpace();
		groupList.append( grpPtr );
	    }
	}
	f.close();
    }
}

/*======================= setup Tabs =============================*/
void AFChoose::setupTabs()
{
    if (!groupList.isEmpty())
    {
	for (grpPtr=groupList.first();grpPtr != 0;grpPtr=groupList.next())
	{
	    grpPtr->tab = new QVBox(this);
	    grpPtr->loadWid = new KIconCanvas(grpPtr->tab);
	    qDebug( "%s", grpPtr->dir.absFilePath().latin1() );
	    // Changes for the new KIconCanvas (Werner)
	    QDir d( grpPtr->dir.absFilePath() );
	    d.setNameFilter( "*.png" );
	    if( d.exists() ) {
		QStringList files=d.entryList( QDir::Files | QDir::Readable, QDir::Name );
		for(unsigned int i=0; i<files.count(); ++i)
		    files[i]=grpPtr->dir.absFilePath() + QChar('/') + files[i];
		grpPtr->loadWid->loadFiles(files);
	    }
	    //grpPtr->loadWid->loadDir(grpPtr->dir.absFilePath(),"*.png");
	    grpPtr->loadWid->setBackgroundColor(colorGroup().base());
	    grpPtr->loadWid->show();
	    connect(grpPtr->loadWid,SIGNAL(nameChanged(QString)),
		    this,SLOT(nameChanged(QString)));
//	  connect(grpPtr->loadWid,SIGNAL(doubleClicked()),
//		  this,SLOT(chosen()));
//	  connect(grpPtr->loadWid,SIGNAL(doubleClicked()),
//		  this,SLOT(accept()));
	    grpPtr->label = new QLabel(grpPtr->tab);
	    grpPtr->label->setText(" ");
	    grpPtr->label->setMaximumHeight(grpPtr->label->sizeHint().height());
	    //grpPtr->tab->setMinimumSize(400,300);
	    addTab(grpPtr->tab,grpPtr->name);
	}
    }
}

/*====================== resize event ============================*/
void AFChoose::resizeEvent(QResizeEvent *e)
{
    QTabDialog::resizeEvent(e);
//   if (!groupList.isEmpty())
//     {
//	 for (grpPtr=groupList.first();grpPtr != 0;grpPtr=groupList.next())
//	{
//	  grpPtr->loadWid->resize(grpPtr->tab->width(),grpPtr->tab->height()-30);
//	  grpPtr->label->setGeometry(10,grpPtr->tab->height()-30,
//				     grpPtr->tab->width()-10,30);
//	}
//     }
}

/*====================== name changed ===========================*/
void AFChoose::nameChanged(QString name)
{
    QFileInfo fi(name);

    if (!groupList.isEmpty())
    {
	for (grpPtr=groupList.first();grpPtr != 0;grpPtr=groupList.next())
	{
	    grpPtr->label->setText(fi.baseName());
	    if (grpPtr->label->text().isEmpty())
		grpPtr->label->setText(" ");
	}
    }
}

/*======================= form chosen ==========================*/
void AFChoose::chosen()
{
    if (!groupList.isEmpty())
    {
	for (grpPtr=groupList.first();grpPtr != 0;grpPtr=groupList.next())
	{
	    if (grpPtr->tab->isVisible() && !grpPtr->loadWid->getCurrent().isEmpty()) 
		emit formChosen(grpPtr->loadWid->getCurrent());	
	}
    }
}
