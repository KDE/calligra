/*  This file is part of the KDE project

    Copyright (c) 2011 Sven Langkamp <sven.langkamp@gmail.com>
    Copyright (C) 2011 Srikanth Tiyyagura <srikanth.tulasiram@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "KoResourceTagStore.h"

#include <QDebug>
#include <QStringList>
#include <kstandarddirs.h>
#include <QFile>
#include <QDir>
#include <KoResourceServer.h>

KoResourceTagStore::KoResourceTagStore(KoResourceServerBase *resourceServer, const QString& resourceType, const QString& extensions)
    : m_serverExtensions(extensions)
    , m_resourceServer(resourceServer)
{
    m_tagsXMLFile =  KStandardDirs::locateLocal("data", "krita/tags/" + resourceType + "_tags.xml");
    m_config = KConfigGroup(KGlobal::config(), "resource tagging");
    readXMLFile();
}

KoResourceTagStore::~KoResourceTagStore()
{
    serializeTags();
}

QStringList KoResourceTagStore::assignedTagsList(KoResource* resource) const
{
    return m_resourceToTag.values(resource);
}

QStringList KoResourceTagStore::tagNamesList() const
{
    return m_tagList.uniqueKeys();
}

void KoResourceTagStore::addTag(KoResource* resource, const QString& tag)
{
    if (!resource) {
        m_tagList.insert(tag, 0);
        return;
    }
    if (m_resourceToTag.contains(resource, tag)) {
        return;
    }
    m_resourceToTag.insert(resource, tag);
    if (m_tagList.contains(tag)) {
        m_tagList[tag]++;
    } else {
        m_tagList.insert(tag, 1);
    }
}

void KoResourceTagStore::delTag(KoResource* resource, const QString& tag)
{
    int res = m_resourceToTag.remove(resource, tag);

    if (res > 0) { // decrease the usecount for this tag
        if (m_tagList.contains(tag)) {
            if (m_tagList[tag] > 0) {
                m_tagList[tag]--;
            }
        }
    }
}

void KoResourceTagStore::delTag(const QString& tag)
{
    Q_ASSERT(!m_resourceToTag.values().contains(tag));
    m_tagList.remove(tag);
}

QStringList KoResourceTagStore::searchTag(const QString& lineEditText)
{
    QStringList tagsList = lineEditText.split(QRegExp("[,]\\s*"), QString::SkipEmptyParts);
    if (tagsList.isEmpty()) {
        return QStringList();
    }

    QSet<KoResource*> resources;

    foreach (QString tag, tagList) {
        foreach (KoResource *res, m_resourceToTag.keys(tag)) {
            resources << res;
        }
    }
    QStringList filenames;
    foreach (KoResource *res, resources) {
        filesnames << adjustedFileName(res->filename());
    }

    return removeAdjustedFileNames(filenames);
}

void KoResourceTagStore::writeXMLFile()
{
    QFile f(m_tagsXMLFile);
    //bool fileExists = f.exists();
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        kWarning() << "Cannot write meta information to '" << m_tagsXMLFile << "'.";
        return;
    }
    QDomDocument doc;
    QDomElement root;

    QDomDocument docTemp("tags");
    doc = docTemp;
    doc.appendChild(doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""));
    root = doc.createElement("tags");
    doc.appendChild(root);


    foreach (KoResource *res, m_resourceToTag.uniqueKeys()) {
        QDomElement resourceEl = doc.createElement("resource");
        resourceEl.setAttribute("identifier", res->filename().replace(QDir::homePath(), QString("~")));
        resourceEl.setAttribute("md5", res->md5());

        foreach (const QString &tag, m_resourceToTag.values(res)) {
            QDomElement tagEl = doc.createElement("tag");
            tagEl.appendChild(doc.createTextNode(tag));
            resourceEl.appendChild(tagEl);
        }
        root.appendChild(resourceEl);

    }

    // Now write empty tags
    foreach (const QString &tag, m_tagList.uniqueKeys())  {
        if (m_tagList[tag] == 0) {
            QDomElement resourceEl = doc.createElement("resource");
            resourceEl.setAttribute("identifier", "dummy");
            QDomElement tagEl = doc.createElement("tag");
            tagEl.appendChild(doc.createTextNode(tag));
            resourceEl.appendChild(tagEl);
             root.appendChild(resourceEl);
        }
    }

    QTextStream metastream(&f);
    metastream << doc.toString();

    f.close();

}

void KoResourceTagStore::readXMLFile()
{

    QString inputFile;

    if (QFile::exists(m_tagsXMLFile)) {
        inputFile = m_tagsXMLFile;
    } else {
        inputFile = KStandardDirs::locateLocal("data", "krita/tags.xml");
    }

    QFile f(inputFile);
    if (!f.open(QIODevice::ReadOnly)) {
        return;
    }

    QDomDocument doc;
    if (!doc.setContent(&f)) {
        kWarning() << "The file could not be parsed.";
        return;
    }

    QDomElement root = doc.documentElement();
    if (root.tagName() != "tags") {
        kWarning() << "The file doesn't seem to be of interest.";
        return;
    }

    QDomNodeList resourceNodesList = root.childNodes();

    QString resourceMD5;
    QString resourceName;

    for (int i = 0; i < resourceNodesList.count(); i++) {
        QDomElement element = resourceNodesList.at(i).toElement();
        if (element.tagName() == "resource") {

            KoResource *res = 0;

            if (element.hasAttribute("md5")) {
                resourceMD5 = element.attribute("md5");
            }
            if (element.hasAttribute("identifier")) {
                resourceName = element.attribute("identifier");
            }

            // Old-style, filename based tag stores
            if (resourceMD5.isEmpty() && !resourceName.isEmpty()) {
                res = m_resourceServer->res(resourceName);
            }
            else {
                continue; // No md5, no identifier, hopeless
            }

            if (resourceName.isEmpty() && !resourceMD5.isEmpty()) {
                resourceName = m_resourceServer->fileNameForMd5(resourceMD5);
            }
            if (resourceName.isEmpty() || resourceMD5.isEmpty()) {
                continue; // neither -- hopeless.
            }


            if (resourceMD5 == "dummy" || isServerResource(resourceMD5)) {

                QDomNodeList tagNodesList = resourceNodesList.at(i).childNodes();

                for (int j = 0; j < tagNodesList.count() ; j++) {

                    QDomElement tagEl = tagNodesList.at(j).toElement();

                    if (resourceMD5  != "dummy") {
                        addTagInternal(resourceMD5.toAscii(), tagEl.text());
                    } else {
                        addTag(0, tagEl.text());
                    }
                }
            }
        }
    }
}

bool KoResourceTagStore::isServerResource(const QString &resourceName) const
{
    bool removeChild = false;
    QStringList extensionsList = m_serverExtensions.split(':');
    foreach (QString extension, extensionsList) {
        if (resourceName.contains(extension.remove('*'))) {
            removeChild = true;
            break;
        }
    }
    return removeChild;
}

QString KoResourceTagStore::adjustedFileName(const QString &fileName) const
{
    if (!isServerResource(fileName)) {
        return fileName + "-krita" + m_serverExtensions.split(':').takeFirst().remove('*');
    }
    return fileName;
}

QStringList KoResourceTagStore::removeAdjustedFileNames(QStringList fileNamesList)
{
    foreach (const QString & fileName, fileNamesList) {
        if (fileName.contains("-krita")) {
            fileNamesList.append(fileName.split("-krita").takeFirst());
            fileNamesList.removeAll(fileName);
        }
    }
    return fileNamesList;
}

void KoResourceTagStore::serializeTags()
{
    writeXMLFile();
}
