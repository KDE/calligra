/*
 * CoreAttributes.h - TaskJuggler
 *
 * Copyright (c) 2001, 2002, 2003, 2004, 2005 by Chris Schlaeger <cs@kde.org>
 * Copyright (c) 2011 by Dag Andersen <danders@get2net.dk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * $Id$
 */

#ifndef _CoreAttributes_h_
#define _CoreAttributes_h_

#include "kplatotj_export.h"

#include <QString>
#include <QMap>

#include "FlagList.h"
#include "CustomAttribute.h"

namespace TJ
{

class Project;
class CoreAttributes;
class CoreAttributesList;
class CoreAttributesListIterator;
class CustomAttributeDefinition;

/**
 * @short This class is the base class for all attribute classes.
 * @author Chris Schlaeger <cs@kde.org>
 */
class KPLATOTJ_EXPORT CoreAttributes
{
public:
    CoreAttributes(Project* p, const QString& i, const QString& n,
                   CoreAttributes* parent_, const QString& df = QString(),
                   uint dl = 0);
    virtual ~CoreAttributes();

    virtual CAType getType() const { return CA_Undefined; }

    const QString& getId() const { return id; }
    QString getFullId() const;

    const QString& getDefinitionFile() const { return definitionFile; }
    uint getDefinitionLine() const { return definitionLine; }

    void setIndex(int idx) { index = idx; }
    int getIndex() const { return index; }

    void setSequenceNo(uint no) { sequenceNo = no; }
    uint getSequenceNo() const { return sequenceNo; }

    void setHierarchNo(uint no);
    QString getHierarchNo() const;

    void setHierarchIndex(uint no);
    QString getHierarchIndex() const;
    QString getHierarchLevel() const;

    Project* getProject() const { return project; }

    void setName(const QString& n) { name = n; }
    const QString& getName() const { return name; }
    void getFullName(QString& fullName) const;

    CoreAttributes* getParent() const { return parent; }

    uint treeLevel() const;

    CoreAttributesList getSubList() const;
    CoreAttributesListIterator getSubListIterator() const;

    bool hasSubs() const;
    void addFlag(QString flag) { flags.addFlag(flag); }
    void purgeFlags() { flags.clear(); }
    void clearFlag(const QString& flag) { flags.clearFlag(flag); }
    bool hasFlag(const QString& flag) { return flags.hasFlag(flag); }
    FlagList getFlagList() const { return flags; }

    bool hasSameAncestor(const CoreAttributes* c) const;
    bool isDescendantOf(const CoreAttributes* c) const;
    bool isParentOf(const CoreAttributes* c) const;

    bool isRoot() const { return parent == 0; }
    bool isLeaf() const;

    void addCustomAttribute(const QString& id, CustomAttribute* ca);
    const CustomAttribute* getCustomAttribute(const QString& id) const;
    const QMap<QString, CustomAttribute*>& getCustomAttributeDict() const
    {
        return customAttributes;
    }
    void inheritCustomAttributes
        (const QMap<QString, CustomAttributeDefinition*>& dict);

protected:
    /// A pointer to access information that are global to the project.
    Project* project;

    /// An ID that must be unique within the attribute class.
    QString id;

    /// A short description of the attribute.
    QString name;

    /// Pointer to parent. If there is no parent the pointer is 0.
    CoreAttributes* parent;

    /* Name of the tjp file that caused the creation of this CoreAttribute. It
     * may be empty if it was not created from a .tjp file. */
    const QString definitionFile;

    /* Line in the .tjp file that caused the createtion of  this Core
     * Attribute. It may be 0 if it was not created from a .tjp file. */
    uint definitionLine;

    /**
     * The index of the attribute declaration within the project files. Each
     * attribute lists has it's own indices.
     */
    uint sequenceNo;

    /**
     * The index of the attribute declaration within it's parents children.
     */
    uint hierarchNo;
    /**
     * The index of the attributes in a logical order that takes the tree
     * structure and the start and end date into account. Each attribute list
     * has it's own indices.
     */
    int index;

    /**
     * The index of the attributes of the same parent in a logical order that
     * takes the tree structure and the start and end date into account. Each
     * attribute list has it's own indices.
     */
    uint hierarchIndex;

    /// List of child attributes.
    CoreAttributesList* sub;

    /// List of flags set for this attribute.
    FlagList flags;

    /// User defined, optional attributes.
    QMap<QString, CustomAttribute*> customAttributes;
} ;

} // namespace TJ

KPLATOTJ_EXPORT QDebug operator<<( QDebug dbg, const TJ::CoreAttributes* t );
KPLATOTJ_EXPORT QDebug operator<<( QDebug dbg, const TJ::CoreAttributes& t );

#endif
