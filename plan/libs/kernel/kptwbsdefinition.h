/* This file is part of the KDE project
   Copyright (C) 2005 Dag Andersen <danders@get2net.dk>

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

#ifndef KPTWBSDEFINITION_H
#define KPTWBSDEFINITION_H

#include "kplatokernel_export.h"

#include <KoXmlReader.h>

#include <QString>
#include <QMap>
#include <QPair>
#include <QList>

class QStringList;


namespace KPlato
{

class XMLLoaderObject;
    
class KPLATOKERNEL_EXPORT WBSDefinition {

public:
    /// Construct a default definition
    WBSDefinition();
    /// Copy constructor
    WBSDefinition( const WBSDefinition &def );
    /// Destructor
    ~WBSDefinition();

    class KPLATOKERNEL_EXPORT CodeDef {
        public:
            CodeDef() {}
            CodeDef(QString c, QString s) { code = c; separator = s; }
            ~CodeDef() {}
            void clear() { code.clear(); separator.clear(); }
            bool isEmpty() { return code.isEmpty(); }
            QString code;
            QString separator;
    };
    
    WBSDefinition &operator=( const WBSDefinition &def );
    
    void clear();
    
    /// Return wbs string.
    QString wbs(uint index, int level) const;
    /// Return wbs code.
    QString code(uint index, int level) const;
    /// Return wbs separator.
    QString separator(int level) const;
    
    CodeDef &defaultDef() { return m_defaultDef; }
    void setDefaultDef(CodeDef def) { m_defaultDef = def; }
    
    bool isLevelsDefEnabled() const { return m_levelsEnabled; }
    bool level0Enabled() const;
    void setLevelsDefEnabled(bool on) { m_levelsEnabled = on; }
    void clearLevelsDef() { m_levelsDef.clear(); }
    QMap<int, CodeDef> levelsDef() const { return m_levelsDef; }
    void setLevelsDef(QMap<int, CodeDef> def);
    CodeDef levelsDef(int level) const;
    void setLevelsDef(int level, CodeDef def);
    void setLevelsDef(int level, const QString& c, const QString& s);
    
    QStringList codeList() const;
    int defaultCodeIndex() const;
    bool setDefaultCode(uint index);
    QString defaultSeparator() const { return m_defaultDef.separator; }
    void setDefaultSeparator(const QString& s);

    QString projectCode() const { return m_projectCode; }
    void setProjectCode( const QString &str ) { m_projectCode = str; }
    QString projectSeparator() const { return m_projectSeparator; }
    void setProjectSeparator( const QString &str ) { m_projectSeparator = str; }
    
    /// Load from document
    bool loadXML(KoXmlElement &element, XMLLoaderObject &status );
    /// Save to document
    void saveXML(QDomElement &element) const;
    
protected:
    QString code(const CodeDef &def, uint index) const;
    QString toRoman(int n, bool upper = false) const;
    
private:
    QString m_projectCode;
    QString m_projectSeparator;
    CodeDef m_defaultDef;
    bool m_levelsEnabled;
    QMap<int, CodeDef> m_levelsDef;

    QList<QPair<QString, QString> > m_codeLists;
};

} //namespace KPlato

#endif //WBSDEFINITION_H
