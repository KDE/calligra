/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef MUSIC_CORE_PARTGROUP_H
#define MUSIC_CORE_PARTGROUP_H

#include <QObject>
#include <QString>

namespace MusicCore {

class Sheet;

/**
 * A PartGroup can be used to group a consecutive number of parts together. This can for example be useful
 * to group all the string instruments. A group is defined by the indices of the first and last parts in the group.
 */
class PartGroup : public QObject
{
    Q_OBJECT
public:
    /// Used to identify the symbol shown for the group
    enum GroupSymbol {
        None,
        Brace,
        Line,
        Bracket,
        DefaultSymbol = None
    };

    /**
     * Create a new partgroup in the given sheet. The part group contains all parts from firstPart up to and
     * including lastPart. Call Sheet::addPartGroup to actually add the part group to the sheet.
     */
    PartGroup(Sheet* sheet, int firstPart, int lastPart);

    /**
     * Destructor.
     */
    ~PartGroup() override;

    /**
     * Returns the sheet this group is part of.
     */
    Sheet* sheet();

    /**
     * Changes the sheet this group is part of. Do not call this method after the partgroup has been added to a
     * sheet by calling the addPartGroup method.
     */
    void setSheet(Sheet* sheet);

    /**
     * Returns the index of the first part in this group.
     */
    int firstPart() const;

    /**
     * Returns the index of the last part in this group.
     */
    int lastPart() const;

    /**
     * Returns the name of this group.
     */
    QString name() const;

    /**
     * Returns the short name of this part group.
     */
    QString shortName(bool useFull = true) const;

    /**
     * Returns the grouping symbol of this group.
     */
    GroupSymbol symbol() const;

    /**
     * Returns true if the group should have common bar-lines.
     *
     * XXX MusicXML has Mensurstrich as third option... perhaps nice to add later, and do I want that? :)
     */
    bool commonBarLines() const;
public Q_SLOTS:
    /**
     * Sets the index of the first part in this group. The index should be >= 0 and < sheet().partCount().
     *
     * @param index the index at which this group should start.
     */
    void setFirstPart(int index);

    /**
     * Sets the index of the last part in this group. The index should be >= 0 and < sheet().partCount().
     *
     * @param index the index at which this group should end.
     */
    void setLastPart(int index);

    /**
     * Changes the name of this group.
     *
     * @param name the new name of this group.
     */
    void setName(const QString &name);

    /**
     * Changes the short name of this part group.
     */
    void setShortName(const QString& shortName);
    
    /**
     * Changes the grouping symbol of this group.
     *
     * @param symbol the new grouping symbol.
     */
    void setSymbol(MusicCore::PartGroup::GroupSymbol symbol);

    /**
     * Set whether the group should have common bar-lines.
     *
     * @param commonBarLines true if the group should have common bar-lines
     */
    void setCommonBarLines(bool commonBarLines);
Q_SIGNALS:
    void firstPartChanged(int index);
    void lastPartChanged(int index);
    void nameChanged(const QString& name);
    void shortNameChanged(const QString& shortName);
    void symbolChanged(MusicCore::PartGroup::GroupSymbol symbol);
    void commonBarLinesChanged(bool commonBarLines);
private:
    class Private;
    Private * const d;
};

} // namespace MusicCore

#endif // MUSIC_CORE_PART_H
