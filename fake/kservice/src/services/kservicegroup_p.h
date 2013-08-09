/* This file is part of the KDE project
   Copyright (C) 2000 Waldo Bastian <bastian@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KSERVICEGROUPPRIVATE_H
#define KSERVICEGROUPPRIVATE_H

#include "kservicegroup.h"
#include <ksycocaentry_p.h>

#include <QtCore/QStringList>

class KServiceGroupPrivate : public KSycocaEntryPrivate
{
public:
    K_SYCOCATYPE( KST_KServiceGroup, KSycocaEntryPrivate )

    KServiceGroupPrivate(const QString &path) 
        : KSycocaEntryPrivate(path),
          m_bNoDisplay(false), m_bShowEmptyMenu(false), m_bShowInlineHeader(false), m_bInlineAlias(false),
          m_bAllowInline(false), m_inlineValue(4), m_bDeep(false), m_childCount(-1)
    {
    }

    KServiceGroupPrivate(QDataStream &str, int offset) 
        : KSycocaEntryPrivate(str, offset),
          m_bNoDisplay(false), m_bShowEmptyMenu(false), m_bShowInlineHeader(false), m_bInlineAlias(false),
          m_bAllowInline(false), m_inlineValue(4), m_bDeep(false), m_childCount(-1)

    {
    }

    virtual void save(QDataStream &s);

    virtual QString name() const
    {
        return path;
    }

    void load(const QString &cfg);
    void load(QDataStream &s);

    int childCount() const;

    KServiceGroup::List
    entries(KServiceGroup *group, bool sort, bool excludeNoDisplay, bool allowSeparators, bool sortByGenericName);
  /**
   * This function parse attributes into menu
   */
    void parseAttribute( const QString &item ,  bool &showEmptyMenu, bool &showInline, bool &showInlineHeader, bool & showInlineAlias ,int &inlineValue );


    bool m_bNoDisplay : 1;
    bool m_bShowEmptyMenu : 1;
    bool m_bShowInlineHeader : 1;
    bool m_bInlineAlias : 1;
    bool m_bAllowInline : 1;
    int m_inlineValue;
    QStringList suppressGenericNames;
    QString directoryEntryPath;
    QStringList sortOrder;
    QString m_strCaption;
    QString m_strIcon;
    QString m_strComment;

    KServiceGroup::List m_serviceList;
    bool m_bDeep;
    QString m_strBaseGroupName;
    mutable int m_childCount;
};

class KServiceSeparator : public KSycocaEntry //krazy:exclude=dpointer (dummy class)
{
public:
  typedef KSharedPtr<KServiceSeparator> Ptr;
public:
  /**
   * Construct a service separator
   */
  KServiceSeparator();

};


#endif
