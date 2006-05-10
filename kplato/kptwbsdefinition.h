/* This file is part of the KDE project
   Copyright (C) 2005 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation;
   version 2 of the License.

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

#include <QString>
#include <QMap>
#include <qpair.h>
#include <q3valuelist.h>

class QStringList;

namespace KPlato
{

class Part;

class WBSDefinition {

public:
    WBSDefinition();
    ~WBSDefinition();

    class CodeDef {
        public:
            CodeDef() {}
            CodeDef(QString c, QString s) { code = c; separator = s; }
            ~CodeDef() {}
            void clear() { code = separator = QString(); }
            bool isEmpty() { return code.isEmpty(); }
            QString code;
            QString separator;
    };
    
    void clear();
    
    /// Return wbs string.
    QString wbs(uint index, int level);
    /// Return wbs code.
    QString code(uint index, int level);
    /// Return wbs separator.
    QString separator(int level);
    
    CodeDef &defaultDef() { return m_defaultDef; }
    void setDefaultDef(CodeDef def) { m_defaultDef = def; }
    
    bool isLevelsDefEnabled() const { return m_levelsEnabled; }
    bool level0Enabled();
    void setLevelsDefEnabled(bool on) { m_levelsEnabled = on; }
    void clearLevelsDef() { m_levelsDef.clear(); }
    const QMap<int, CodeDef> &levelsDef() const { return m_levelsDef; }
    void setLevelsDef(QMap<int, CodeDef> def);
    CodeDef levelsDef(int level) const;
    void setLevelsDef(int level, CodeDef def);
    void setLevelsDef(int level, QString c, QString s);
    
    QStringList codeList();
    int defaultCodeIndex() const;
    bool setDefaultCode(uint index);
    QString defaultSeparator() const { return m_defaultDef.separator; }
    void setDefaultSeparator(QString s);

protected:
    QString code(CodeDef &def, uint index);
    QString toRoman(int n, bool upper = false);
    
private:
    CodeDef m_defaultDef;
    
    bool m_levelsEnabled;
    QMap<int, CodeDef> m_levelsDef;

    Q3ValueList<QPair<QString, QString> > m_codeLists;
};

} //namespace KPlato

#endif //WBSDEFINITION_H
