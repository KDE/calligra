/*
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef _SECTIONSIO_H_
#define _SECTIONSIO_H_

#include <QObject>
#include <QMap>

class QDomDocument;
class QDomElement;
class QTimer;
class RootSection;
class Section;
class SectionGroup;

class SectionsIO : public QObject
{
    Q_OBJECT
public:
    explicit SectionsIO(RootSection* rootSection);
    ~SectionsIO();
public:
    enum PushMode {
        SinglePush,
        RecursivePush
    };
    /**
     * push a section to save
     */
    void push(Section* _section, PushMode _pushMode = SinglePush);
public Q_SLOTS:
    void save();
private:
    void load();
private:
    RootSection* m_rootSection;
    QTimer* m_timer;
    struct SaveContext;
    QMap<Section*, SaveContext*> m_contextes;
    QString m_directory; ///< directory where the sections are saved
private:
    /**
     * Save the structure and update the m_contextes map
     * @param contextToRemove contains a list of used context, that need to be removed
     *                        from that list, otherwise doSave will remove the
     *                        associated files
     */
    void saveTheStructure(QDomDocument& doc, QDomElement& elt, SectionGroup* root, QList<SaveContext*>& contextToRemove);
    void loadTheStructure(QDomElement& elt, SectionGroup* root, RootSection* _rootSection);
    QString generateFileName();
    bool usedFileName(const QString&);
    QString structureFileName();
    int m_nextNumber;
    QList<Section* > m_sectionsToSave;
};

#endif
