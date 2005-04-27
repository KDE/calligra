/*
   This file is part of the KDE project
   Copyright (C) 2000 Werner Trobin <trobin@kde.org>

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

#ifndef koTemplates_h
#define koTemplates_h

#include <qptrlist.h>
#include <qstringlist.h>
#include <qpixmap.h>

class KInstance;

/** @internal */
class KoTemplate {

public:
    KoTemplate(const QString &name,
               const QString &description=QString::null,
	       const QString &file=QString::null,
	       const QString &picture=QString::null,
	       bool hidden=false, bool touched=false);
    ~KoTemplate() {}

    QString name() const { return m_name; }
    QString description() const { return m_descr; };
    QString file() const { return m_file; }
    QString picture() const { return m_picture; }
    const QPixmap &loadPicture( KInstance* instance );

    bool isHidden() const { return m_hidden; }
    void setHidden(bool hidden=true) { m_hidden=hidden; m_touched=true; }

    bool touched() const { return m_touched; }

private:
    QString m_name, m_descr, m_file, m_picture;
    bool m_hidden;
    mutable bool m_touched;
    bool m_cached;
    QPixmap m_pixmap;
};


class KoTemplateGroup {

public:
    KoTemplateGroup(const QString &name,
		    const QString &dir=QString::null,
		    bool touched=false);
    ~KoTemplateGroup() {}

    QString name() const { return m_name; }
    QStringList dirs() const { return m_dirs; }
    void addDir(const QString &dir) { m_dirs.append(dir); m_touched=true; }

    /// If all children are hidden, we are hidden too
    bool isHidden() const;
    /// if we should hide, we hide all the children
    void setHidden(bool hidden=true) const;

    KoTemplate *first() { return m_templates.first(); }
    KoTemplate *next() { return m_templates.next(); }
    KoTemplate *last() { return m_templates.last(); }
    KoTemplate *prev() { return m_templates.prev(); }
    KoTemplate *current() { return m_templates.current(); }

    bool add(KoTemplate *t, bool force=false, bool touch=true);
    KoTemplate *find(const QString &name) const;

    bool touched() const { return m_touched; }

private:
    QString m_name;
    QStringList m_dirs;
    QPtrList<KoTemplate> m_templates;
    mutable bool m_touched;
};


class KoTemplateTree {

public:
    KoTemplateTree(const QCString &templateType, KInstance *instance,
		   bool readTree=false);
    ~KoTemplateTree() {}

    QCString templateType() const { return m_templateType; }
    KInstance *instance() const { return m_instance; }
    void readTemplateTree();
    void writeTemplateTree();

    KoTemplateGroup *first() { return m_groups.first(); }
    KoTemplateGroup *next() { return m_groups.next(); }
    KoTemplateGroup *last() { return m_groups.last(); }
    KoTemplateGroup *prev() { return m_groups.prev(); }
    KoTemplateGroup *current() { return m_groups.current(); }

    void add(KoTemplateGroup *g);
    KoTemplateGroup *find(const QString &name) const;

    KoTemplateGroup *defaultGroup() const { return m_defaultGroup; }
    KoTemplate *defaultTemplate() const { return m_defaultTemplate; }

private:
    void readGroups();
    void readTemplates();
    void writeTemplate(KoTemplate *t, KoTemplateGroup *group,
		       const QString &localDir);

    QCString m_templateType;
    KInstance *m_instance;
    QPtrList<KoTemplateGroup> m_groups;
    KoTemplateGroup *m_defaultGroup;
    KoTemplate *m_defaultTemplate;
};


namespace KoTemplates {
QString stripWhiteSpace(const QString &string);
}

#endif
