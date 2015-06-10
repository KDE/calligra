/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003-2015 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXIPARTMANAGER_H
#define KEXIPARTMANAGER_H

//! @todo KEXI3 #include "kexistaticpart.h"
#include "kexiinternalpart.h"

#include <KDbResult>

namespace KexiPart
{
class Info;
class Part;

typedef QHash<QString, Info*> PartInfoDict;
typedef QHash<QString, Info*>::iterator PartInfoDictIterator;
typedef QList<Info*> PartInfoList;
typedef QList<Info*>::iterator PartInfoListIterator;
typedef QHash<QString, Part*> PartDict;

/**
 * @short KexiPart's manager: looks up and instantiates them
 *
 * It creates instances only when needed.
 */
class KEXICORE_EXPORT Manager : public QObject, /*public KDbObject,*/ public KDbResultable
{
    Q_OBJECT

public:
    /**
     * creates an empty instance
     */
    explicit Manager(QObject *parent = 0);
    ~Manager();

    /**
     * \return a part object for specified plugin ID @a pluginId, e.g. "org.kexi-project.table"
     * @note For compatibility with Kexi <= 2, if a string without any dot is provided, "org.kexi-project."
     *       will be prepended to the ID.
     * Dynamically loads a plugin using KexiPart::Info if needed. Returns 0 if loading failed.
     */
    Part *partForPluginId(const QString& pluginId);

    /**
     * \return a part object for specified info. Dlopens a part using KexiPart::Info
     * if needed. Return 0 if loading failed.
     */
    Part *part(Info *info);

    /**
     * \return the info for a corresponding plugin ID, e.g. "org.kexi-project.table"
     * @note For compatibility with Kexi <= 2, if a string without any dot is provided, "org.kexi-project."
     *       will be prepended to the ID.
     */
    Info *infoForPluginId(const QString& pluginId);

    /**
     * @return a list of the available KexiParts-based plugins in a well-defined order
     * Can return 0 if no plugins have been found, what means the installation is broken.
     */
    PartInfoList* infoList();

Q_SIGNALS:
    void partLoaded(KexiPart::Part*);
    void newObjectRequested(KexiPart::Info *info);

protected:
    //! Used by StaticPart
    //! @todo KEXI3 void insertStaticPart(KexiPart::StaticPart* part);

    //! Used by KexiInternalPart
    KexiInternalPart* internalPartForPluginId(const QString& pluginId);

private:
    /**
     * Queries the plugin system and creates a list of available parts.
     * @return false if required servicetype was not found (what means the installation is broken).
     */
    bool lookup();

    template <typename PartClass>
    PartClass* part(Info *info, QHash<QString, PartClass*> *partDict);

    Q_DISABLE_COPY(Manager)

    class Private;
    Private* const d;

    //! @todo KEXI3 friend KexiPart::StaticPart::StaticPart(const QString&, const QString&, const QString&);
    friend KexiInternalPart* KexiInternalPart::part(KDbMessageHandler*, const QString&);
};

}

#endif
