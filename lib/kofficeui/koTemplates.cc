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
#include <qtextstream.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qimage.h>

#include <kdesktopfile.h>
#include <ksimpleconfig.h>
#include <kdebug.h>
#include <kinstance.h>
#include <kstddirs.h>
#include <kmessagebox.h>
#include <klocale.h>

#include <stdlib.h>


KoTemplate::KoTemplate(const QString &name, const QString &file,
		       const QString &picture, const bool &hidden,
		       const bool &touched) :
    m_name(name), m_file(file), m_picture(picture), m_hidden(hidden),
    m_touched(touched), m_cached(false) {
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


KoTemplateGroup::KoTemplateGroup(const QString &name, const QString &dir,
				 const bool &touched) : m_name(name),
							m_touched(touched) {
    m_dirs.append(dir);
    m_templates.setAutoDelete(true);
}

const bool KoTemplateGroup::isHidden() const {

    QListIterator<KoTemplate> it(m_templates);
    bool hidden=true;
    while(it.current()!=0L && hidden) {
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

const bool KoTemplateGroup::add(KoTemplate *t, bool force, bool touch) {

    KoTemplate *myTemplate=find(t->name());
    if(myTemplate==0L) {
	m_templates.append(t);
	m_touched=touch;
	return true;
    }
    else if(myTemplate && force) {
	m_templates.removeRef(myTemplate);
	m_templates.append(t);
	m_touched=touch;
	return true;
    }
    return false;
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

    QStringList localTemplates;
    // read from the local .templates file
    createLocalTemplateList(localTemplates);
    QString localDir=m_instance->dirs()->saveLocation(m_templateType);

    for(KoTemplateGroup *group=m_groups.first(); group!=0L; group=m_groups.next()) {
	//kdDebug() << "---------------------------------" << endl;
	//kdDebug() << "group: " << group->name() << endl;
	
	bool touched=false;
	for(KoTemplate *t=group->first(); t!=0L && !touched && !group->touched(); t=group->next())
	    touched=t->touched();
	
	if(group->touched() || touched) {
	    //kdDebug() << "touched" << endl;
	    if(!group->isHidden()) {
		//kdDebug() << "not hidden" << endl;
		createGroupDir(localDir, group, localTemplates);
	    }
	    else {
		//kdDebug() << "hidden" << endl;
		if(group->dirs().count()==1 && !group->dirs().grep(localDir).isEmpty()) {
		    //kdDebug() << "local only" << endl;
		    QString command="rm -rf ";
		    command+=group->dirs().first();
		    //kdDebug() << "command: " << command << endl;
		    localTemplates.remove(group->name());
		    system(command);
		}
		else {
		    //kdDebug() << "global" << endl;
		    createGroupDir(localDir, group, localTemplates);
		}
	    }
	}
	for(KoTemplate *t=group->first(); t!=0L; t=group->next()) {
	    //kdDebug() << "template: " << t->name() << endl;
	    if(t->touched())
		writeTemplate(t, group, localDir);
	    if(t->isHidden() && t->touched() && t->file().contains(localDir)) {
		//kdDebug() << "delete local template (rm -rf)" << endl;
		writeTemplate(t, group, localDir);
		QString command="rm -rf ";
		command+=t->file();
		command+=" ";
		command+=t->picture();
		//kdDebug() << "command: " << command << endl;
		system(command);
	    }
	}
    }
    // write back the local .templates file
    writeOutLocalTemplates(localTemplates);
}

void KoTemplateTree::add(KoTemplateGroup *g) {

    KoTemplateGroup *group=find(g->name());
    if(group==0L)
	m_groups.append(g);
    else
	group->addDir(g->dirs().first()); // "...there can be only one..." (Queen)
}

KoTemplateGroup *KoTemplateTree::find(const QString &name) const {

    QListIterator<KoTemplateGroup> it(m_groups);
    while(it.current() && it.current()->name()!=name)
	++it;
    return it.current();
}

void KoTemplateTree::readGroups() {

    QStringList dirs = m_instance->dirs()->resourceDirs(m_templateType);
    for(QStringList::ConstIterator it=dirs.begin(); it!=dirs.end(); ++it) {
	//kdDebug() << "dir: " << *it << endl;
	QFile templateInf(*it + ".templates");
	if(templateInf.open(IO_ReadOnly)) {
	    QTextStream stream(&templateInf);
	    QString tmp;
	    while(!stream.atEnd()) {
		stream >> tmp;
		tmp=KoTemplates::stripWhiteSpace(tmp);
		//kdDebug() << "string read: " << tmp << endl;
		if(!tmp.isEmpty()) {
		    KoTemplateGroup *g=new KoTemplateGroup(tmp, *it+tmp+QChar('/'));
		    add(g);
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
		KoTemplate *t=new KoTemplate(text, templatePath, icon, hidden);
		groupIt.current()->add(t, false, false); // false -> we aren't a "user", false -> don't
		                                         // "touch" the group to avoid useless
    		                                         // creation of dirs in .kde/blah/...
	    }
	}
    }
}

void KoTemplateTree::createLocalTemplateList(QStringList &list) {

    QFile localTemplateFile(m_instance->dirs()->saveLocation(m_templateType)+".templates");
    if(!localTemplateFile.open(IO_ReadOnly))
	return;

    QTextStream stream(&localTemplateFile);
    QString tmp;
    while(!stream.atEnd()) {
	stream >> tmp;
	tmp=KoTemplates::stripWhiteSpace(tmp);
	if(!tmp.isEmpty())
	    list.append(tmp);
    }
    localTemplateFile.close();
}

void KoTemplateTree::writeOutLocalTemplates(const QStringList &list) {

    if(list.isEmpty()) {
	QString command="rm -rf ";
	command+=m_instance->dirs()->saveLocation(m_templateType)+".templates";
	system(command);
	return;
    }

    QFile localTemplateFile(m_instance->dirs()->saveLocation(m_templateType)+".templates");
    if(!localTemplateFile.open(IO_WriteOnly))
	return;

    //kdDebug() << "count: " << list.count() << endl;
    QTextStream stream(&localTemplateFile);
    for(QStringList::ConstIterator it=list.begin(); it!=list.end(); ++it) {
	//kdDebug() << "writing out: " << (*it) << endl;
	stream << (*it) << endl;
    }
    localTemplateFile.close();
}

void KoTemplateTree::createGroupDir(const QString &localDir, KoTemplateGroup *group,
				    QStringList &localTemplates) {

    KStandardDirs::makeDir(localDir+group->name()); // create the local group dir
    if(localTemplates.grep(group->name()).isEmpty()) {
	//kdDebug() << "adding group ( " << group->name() << " ) locally" << endl;
	localTemplates.append(group->name());
    }
}

void KoTemplateTree::writeTemplate(KoTemplate *t, KoTemplateGroup *group,
				   const QString &localDir) {

    KSimpleConfig config(KoTemplates::stripWhiteSpace(localDir+group->name()+'/'+t->name()+".desktop"));
    config.setDesktopGroup();
    config.writeEntry("Type", "Link");
    config.writeEntry("URL", t->file());
    config.writeEntry("Name", t->name());
    config.writeEntry("Icon", t->picture());
    config.writeEntry("X-KDE-Hidden", t->isHidden());
}

namespace KoTemplates {
QString stripWhiteSpace(const QString &string) {

    QString ret;
    for(unsigned int i=0; i<string.length(); ++i) {
	QChar tmp(string[i]);
	if(!tmp.isSpace())
	    ret+=tmp;
    }
    return ret;
}
};
