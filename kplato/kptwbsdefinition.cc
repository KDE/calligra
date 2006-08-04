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

#include "kptwbsdefinition.h"


#include <klocale.h>
#include <kdebug.h>

#include <QString>
#include <qstringlist.h>
#include <qpair.h>
//Added by qt3to4:
#include <Q3ValueList>

namespace KPlato
{


WBSDefinition::WBSDefinition() {
    m_levelsEnabled = false;
    
    m_defaultDef.code = "Number";
    m_defaultDef.separator = ".";
    
    m_codeLists.append(qMakePair(QString("Number"), i18n("Number")));
    m_codeLists.append(qMakePair(QString("Roman, upper case"), i18n("Roman, upper case")));
    m_codeLists.append(qMakePair(QString("Roman, lower case"), i18n("Roman, lower case")));
    m_codeLists.append(qMakePair(QString("Letter, upper case"), i18n("Letter, upper case")));
    m_codeLists.append(qMakePair(QString("Letter, lower case"), i18n("Letter, lower case")));
}

WBSDefinition::~WBSDefinition() {
}

void WBSDefinition::clear() {
    m_defaultDef.clear();
    m_levelsDef.clear();
}
    
QString WBSDefinition::wbs(uint index, int level) {
    if (isLevelsDefEnabled()) {
        CodeDef def = levelsDef(level);
        if (!def.isEmpty()) {
            return code(def, index) + def.separator;
        }
    }
    return code(m_defaultDef, index) + m_defaultDef.separator;
}


QString WBSDefinition::code(uint index, int level) {
    if (isLevelsDefEnabled()) {
        CodeDef def = levelsDef(level);
        if (!def.isEmpty()) {
            return code(def, index);
        }
    }
    return code(m_defaultDef, index);
}

QString WBSDefinition::separator(int level) {
    if (isLevelsDefEnabled()) {
        CodeDef def = levelsDef(level);
        if (!def.isEmpty()) {
            return def.separator;
        }
    }
    return m_defaultDef.separator;
}

void WBSDefinition::setLevelsDef(QMap<int, CodeDef> def) { 
    m_levelsDef.clear();
    m_levelsDef = def; 
}

WBSDefinition::CodeDef WBSDefinition::levelsDef(int level) const { 
    return m_levelsDef.contains(level) ? m_levelsDef[level] : CodeDef(); 
}
    
void WBSDefinition::setLevelsDef(int level, CodeDef def) {
    m_levelsDef.insert(level, def);
}

void WBSDefinition::setLevelsDef(int level, QString c, QString s) {
    m_levelsDef.insert(level, CodeDef(c, s));
}

bool WBSDefinition::level0Enabled() {
    return m_levelsEnabled && !levelsDef(0).isEmpty();
}

const QChar Letters[] = { '?','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z' };

QString WBSDefinition::code(CodeDef &def, uint index) {
    if (def.code == "Number") {
        return QString("%1").arg(index);
    }
    if (def.code == "Roman, lower case") {
        return QString("%1").arg(toRoman(index));
    }
    if (def.code == "Roman, upper case") {
        return QString("%1").arg(toRoman(index, true));
    }
    if (def.code == "Letter, lower case") {
        if (index > 26) {
            index = 0;
        }
        return QString("%1").arg(Letters[index]);
    }
    if (def.code == "Letter, upper case") {
        if (index > 26) {
            index = 0;
        }
        return QString("%1").arg(Letters[index].toUpper());
    }
    return QString();
}

// Nicked from koparagcounter.cc
const QByteArray RNUnits[] = {"", "i", "ii", "iii", "iv", "v", "vi", "vii", "viii", "ix"};
const QByteArray RNTens[] = {"", "x", "xx", "xxx", "xl", "l", "lx", "lxx", "lxxx", "xc"};
const QByteArray RNHundreds[] = {"", "c", "cc", "ccc", "cd", "d", "dc", "dcc", "dccc", "cm"};
const QByteArray RNThousands[] = {"", "m", "mm", "mmm"};

QString WBSDefinition::toRoman( int n, bool upper )
{
    if ( n >= 0 ) {
        QString s = QString::fromLatin1( RNThousands[ ( n / 1000 ) ] +
                                         RNHundreds[ ( n / 100 ) % 10 ] +
                                         RNTens[ ( n / 10 ) % 10 ] +
                                         RNUnits[ ( n ) % 10 ] );
        return upper ? s.toUpper() : s;
        
    } else { // should never happen, but better not crash if it does
        kWarning()<< k_funcinfo << " n=" << n << endl;
        return QString::number( n );
    }
}

QStringList WBSDefinition::codeList() {
    QStringList cl;
    Q3ValueList<QPair<QString, QString> >::Iterator it;
    for (it = m_codeLists.begin(); it != m_codeLists.end(); ++it) {
        cl.append((*it).second);
    }
    return cl;
}

int WBSDefinition::defaultCodeIndex() const {
    Q3ValueList<QPair<QString, QString> >::const_iterator it;
    int i = -1;
    for(it = m_codeLists.begin(); it != m_codeLists.end(); ++it) {
        ++i;
        if (m_defaultDef.code == (*it).first)
            break;
    }
    return i;
}

bool WBSDefinition::setDefaultCode(uint index) {
    Q3ValueList<QPair<QString, QString> >::const_iterator it = m_codeLists.at(index);
    if (it == m_codeLists.end()) {
        return false;
    }
    m_defaultDef.code = (*it).first;
    return true;
}

void WBSDefinition::setDefaultSeparator(QString s) {
    m_defaultDef.separator = s;
}

} //namespace KPlato
