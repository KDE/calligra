/* This file is part of the KDE project
   Copyright (C) 2000 Werner Trobin <wtrobin@mandrakesoft.com>

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

#include <koTemplates.h>

#include <qstringlist.h>
#include <qfile.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qimage.h>

#include <kdesktopfile.h>
#include <ksimpleconfig.h>
#include <kdebug.h>
#include <kinstance.h>
#include <kstddirs.h>


KoTemplate::KoTemplate(const QString &name, const QString &file,
		       const QString &picture, const bool &hidden) :
    m_name(name), m_file(file), m_picture(picture), m_hidden(hidden),
    m_touched(false), m_cached(false) {
}

const QPixmap &KoTemplate::loadPicture() {

    if(m_cached)
	return m_pixmap;
    // This code is shamelessly borrowed from KIconCanvas::slotLoadFiles
    QImage img;
    img.load(m_picture);
    if (img.isNull()) {
	kdWarning() << "Couldn't find icon " << m_picture << endl;
	m_pixmap=QPixmap();
	return m_pixmap;
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
    m_pixmap.convertFromImage(img);
    m_cached=true;
    return m_pixmap;
}


KoTemplateGroup::KoTemplateGroup(const QString &name, const QString &dir) :
    m_name(name), m_touched(false) {
    m_dirs.append(dir);
    m_templates.setAutoDelete(true);
}

const bool KoTemplateGroup::isHidden() const {

    QListIterator<KoTemplate> it(m_templates);
    bool hidden=false;
    while(it.current()!=0L && !hidden) {
	hidden=it.current()->isHidden();
	++it;
    }
    return hidden;
}

void KoTemplateGroup::setHidden(const bool &hidden) const {

    QListIterator<KoTemplate> it(m_templates);
    for( ; it.current()!=0L; ++it)
	it.current()->setHidden(hidden);
    m_touched=true;
}

void KoTemplateGroup::add(KoTemplate *t) {

    if(find(t->name())==0L)
	m_templates.append(t);
}

KoTemplate *KoTemplateGroup::find(const QString &name) const {

    QListIterator<KoTemplate> it(m_templates);
    while(it.current() && it.current()->name()!=name)
	++it;
    return it.current();
}


KoTemplateTree::KoTemplateTree(const QString &templateType,
			       KInstance *instance, const bool &readTree) :
    m_templateType(templateType), m_instance(instance) {

    m_groups.setAutoDelete(true);
    if(readTree)
	readTemplateTree();
}

void KoTemplateTree::readTemplateTree() {

    readGroups();
    readTemplates();
}

void KoTemplateTree::writeTemplateTree() {
    // TODO
}

void KoTemplateTree::add(KoTemplateGroup *g) {

    KoTemplateGroup *group=find(g->name());
    if(group==0L)
	m_groups.append(g);
    else
	group->addDir(g->dirs().first()); // "...there can be only one..." (queen)
}

KoTemplateGroup *KoTemplateTree::find(const QString &name) const {

    QListIterator<KoTemplateGroup> it(m_groups);
    while(it.current() && it.current()->name()!=name)
	++it;
    return it.current();
}

void KoTemplateTree::readGroups() {

    QString dir;
    char c[256];

    QStringList dirs = m_instance->dirs()->resourceDirs(m_templateType);
    for(QStringList::ConstIterator it=dirs.begin(); it!=dirs.end(); ++it) {
	//kdDebug() << "dir: " << *it << endl;
	QFile templateInf(*it + ".templates");
	if(templateInf.open(IO_ReadOnly)) {
	    while(!templateInf.atEnd()) {
		templateInf.readLine(c, 255);
		c[255]='\0';     // if everything else fails :P
		dir=c;
		dir=dir.stripWhiteSpace();
		//kdDebug() << "string: " << dir << endl;
		if(!dir.isEmpty()) {
		    KoTemplateGroup *g=new KoTemplateGroup(dir, *it+dir+"/");
		    m_groups.append(g);
		}
	    }
	    templateInf.close();
	}
    }
}

void KoTemplateTree::readTemplates() {

    QListIterator<KoTemplateGroup> groupIt(m_groups);
    for( ; groupIt.current()!=0L; ++groupIt) {
	QStringList dirs=groupIt.current()->dirs();
	for(QStringList::ConstIterator it=dirs.begin(); it!=dirs.end(); ++it) {
	    QDir d(*it);
	    if( !d.exists() )
		continue;
	    QStringList files=d.entryList( QDir::Files | QDir::Readable, QDir::Name );
	    for(unsigned int i=0; i<files.count(); ++i) {
		QString filePath = *it + files[i];
		//kdDebug() << "filePath: " << filePath << endl;
		QString icon;
		QString text;
		QString hidden_str;
		bool hidden=false;
		QString templatePath;
		// If a desktop file, then read the name from it.
		// Otherwise (or if no name in it?) use file name
		if (KDesktopFile::isDesktopFile(filePath)) {
		    KSimpleConfig config(filePath, true);
		    config.setDesktopGroup();
		    if (config.readEntry("Type")=="Link") {
			text=config.readEntry("Name");
			//kdDebug() << "name: " << text << endl;
			icon=config.readEntry("Icon");
			//kdDebug() << "icon1: " << icon << endl;
			if(icon[0]!='/') // allow absolute paths for icons
			    icon=*it + icon;
			//kdDebug() << "icon2: " << icon << endl;
			hidden_str=config.readEntry("X-KDE-Hidden");
			if(hidden_str.lower()=="true")
			    hidden=true;
			//kdDebug() << "hidden: " << hidden_str << endl;
			templatePath=config.readEntry("URL");
			//kdDebug() << "Link to : " << templatePath << endl;
			if(templatePath[0]!='/') {
			    if(templatePath.left(6)=="file:/") // I doubt this will happen
				templatePath=templatePath.right(templatePath.length()-6);
			    //else
			    //	kdDebug() << "dirname=" << *it << endl;
                            templatePath=*it+templatePath;
			    //kdDebug() << "templatePath: " << templatePath << endl;
			}
		    } else
			continue; // Invalid
		}
		// The else if and the else branch are here for compat. with the old system
		else if ( files[i].right(4) != ".png" )
		    // Ignore everything that is not a PNG file
		    continue;
		else {
		    // Found a PNG file - the template must be here in the same dir.
		    icon = filePath;
		    QFileInfo fi(filePath);
		    text = fi.baseName();
		    templatePath = filePath; // Note that we store the .png file as the template !
		    // That's the way it's always been done. Then the app replaces the extension...
		}

		//kdDebug() << "hiho!" << endl;
		KoTemplate *t=new KoTemplate(text, templatePath, icon, hidden);
		groupIt.current()->add(t);
	    }
	}
    }
}
