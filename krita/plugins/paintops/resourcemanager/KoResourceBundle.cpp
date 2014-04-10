/*
 *  Copyright (c) 2014 Victor Lafon metabolic.ewilan@hotmail.fr
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KoResourceBundle.h"
#include "KoResourceBundleManager.h"
#include "KoXmlResourceBundleManifest.h"
#include "KoXmlResourceBundleMeta.h"

#include <kglobal.h>
#include <kcomponentdata.h>
#include <kstandarddirs.h>

#include <QtCore/QProcessEnvironment>
#include <QtCore/QDate>
#include <QtCore/QDir>

#include <iostream>
using namespace std;

KoResourceBundle::KoResourceBundle(QString const& bundlePath):KoResource(bundlePath)
{
    m_installed=false; //TODO Vérifier l'utilité
    m_manager=new KoResourceBundleManager(QProcessEnvironment::systemEnvironment().value("KDEDIRS").section(':',0,0).append("/share/apps/krita/"));
}

KoResourceBundle::~KoResourceBundle()
{
    delete m_manager;
    delete m_meta;
    delete m_manifest;
}

QString KoResourceBundle::defaultFileExtension() const
{
    return QString(".zip");
}
bool KoResourceBundle::load()
{
    m_manager->setReadPack(filename());
    if (m_manager->bad()) {
        m_manifest=new KoXmlResourceBundleManifest();
        m_meta=new KoXmlResourceBundleMeta();
        m_meta->addTag("name",filename(),true);
        m_installed=false;
    }
    else {
        //TODO Vérifier si on peut éviter de recréer manifest et meta à chaque load
        //A optimiser si possible
        m_manifest=new KoXmlResourceBundleManifest(m_manager->getFile("manifest.xml"));
        m_meta=new KoXmlResourceBundleMeta(m_manager->getFile("meta.xml"));
        QImage thumbnail;
        thumbnail.load(m_manager->getFile("thumbnail.jpg"),"JPG");
        m_manager->close();
        m_installed=m_manifest->isInstalled();
        setImage(thumbnail);
        setValid(true);
    }
    return true;
}

//TODO Vérifier que l'updated est bien placé
bool KoResourceBundle::save()
{
    if (m_manager->bad()) {
        m_meta->addTags(m_manifest->getTagList());
    }
    addMeta("updated",QDate::currentDate().toString("dd/MM/yyyy"));

    m_manager->createPack(m_manifest,m_meta);

    setValid(true);

    return load();
}

void KoResourceBundle::install()
{
    load(); //TODO Vérifier si ce load est nécessaire
    if (!m_manager->bad()) {
        m_manager->extractKFiles(m_manifest->getFilesToExtract());
        m_manifest->exportTags();
        //TODO Vérifier que l'export est validé et copié dans les fichiers
        //TODO Sinon, déterminer pourquoi et comment faire
        m_installed=true;
        m_manifest->install();
        save();
        //TODO Modifier les chemins des fichiers si c'est la première installation
    }
}

void KoResourceBundle::uninstall()
{
    if (!m_installed)
        return;

    QList<QString> directoryList = m_manifest->getDirList();
    QString shortPackName = m_meta->getShortPackName();
    QString dirPath;

    for (int i = 0; i < directoryList.size(); i++) {
        dirPath = this->m_manager->getKritaPath();
        dirPath.append(directoryList.at(i)).append("/").append(shortPackName);

        if (!removeDir(dirPath)) {
            cerr<<"Error : Couldn't delete folder : "<<qPrintable(dirPath)<<endl;
        }
    }

    m_installed=false;
    m_manifest->uninstall();
    save();
}

void KoResourceBundle::addMeta(QString type,QString value)
{
    if (type=="created") {
        setValid(true);
    }
    m_meta->addTag(type,value);
    m_meta->show();
}

void KoResourceBundle::addFile(QString fileType,QString filePath)
{
    m_manifest->addTag(fileType,filePath);
}

void KoResourceBundle::removeFile(QString fileName)
{
    QList<QString> list=m_manifest->removeFile(fileName);

    for (int i=0;i<list.size();i++) {
        m_meta->removeFirstTag("tag",list.at(i));
    }
}

bool KoResourceBundle::removeDir(const QString & dirName)
{
    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System
                    | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = removeDir(info.absoluteFilePath());
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(dirName);
    }
    return result;
}

void KoResourceBundle::addResourceDirs()
{
    QList<QString> listeType = m_manifest->getDirList();
    for(int i = 0; i < listeType.size();i++) {
        KGlobal::mainComponent().dirs()->addResourceDir(listeType.at(i).toLatin1().data(), this->m_manager->getKritaPath()+listeType.at(i)+"/"+this->name());
    }
}

bool KoResourceBundle::isInstalled()
{
    return m_installed;
}

void KoResourceBundle::rename(QString filename)
{
    addMeta("name",filename);
    if (isInstalled()) {
        QList<QString> directoryList = m_manifest->getDirList();
        QString dirPath;
        QDir dir;
        for (int i = 0; i < directoryList.size(); i++) {
            dirPath = this->m_manager->getKritaPath();
            dirPath.append(directoryList.at(i)).append("/").append(filename);
            dir.rename(dirPath,dirPath.section('/',0,dirPath.count('/')-1).append("/").append(filename));
        }
    }
    save();
}

QString KoResourceBundle::getAuthor()
{
    return m_meta->getValue("author");
}

QString KoResourceBundle::getLicense()
{
    return m_meta->getValue("license");
}

QString KoResourceBundle::getWebSite()
{
    return m_meta->getValue("website");
}

QString KoResourceBundle::getCreated()
{
    return m_meta->getValue("created");
}

QString KoResourceBundle::getUpdated()
{
    return m_meta->getValue("updated");
}
