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

#include "kptwbsdefinition.h"

#include <klocale.h>
#include <kdebug.h>

#include <QList>
#include <QString>
#include <QStringList>
#include <QPair>

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

WBSDefinition::WBSDefinition( const WBSDefinition &def ) {
    (void)this->operator=( def );
}

WBSDefinition::~WBSDefinition() {
}

WBSDefinition &WBSDefinition::operator=( const WBSDefinition &def ) {
    m_projectCode = def.m_projectCode;
    m_projectSeparator = def.m_projectSeparator;
    m_defaultDef.code = def.m_defaultDef.code;
    m_defaultDef.separator = def.m_defaultDef.separator;
    m_levelsEnabled = def.m_levelsEnabled;
    m_levelsDef = def.m_levelsDef;
    m_codeLists = def.m_codeLists;
    return *this;
}

void WBSDefinition::clear() {
    m_defaultDef.clear();
    m_levelsDef.clear();
}
    
QString WBSDefinition::wbs(uint index, int level) const {
    if (isLevelsDefEnabled()) {
        CodeDef def = levelsDef(level);
        if (!def.isEmpty()) {
            return code(def, index) + def.separator;
        }
    }
    return code(m_defaultDef, index) + m_defaultDef.separator;
}


QString WBSDefinition::code(uint index, int level) const {
    if (isLevelsDefEnabled()) {
        CodeDef def = levelsDef(level);
        if (!def.isEmpty()) {
            return code(def, index);
        }
    }
    return code(m_defaultDef, index);
}

QString WBSDefinition::separator(int level) const {
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

void WBSDefinition::setLevelsDef(int level, const QString& c, const QString& s) {
    m_levelsDef.insert(level, CodeDef(c, s));
}

bool WBSDefinition::level0Enabled() const {
    return m_levelsEnabled && !levelsDef(0).isEmpty();
}

const QChar Letters[] = { '?','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z' };

QString WBSDefinition::code(const CodeDef &def, uint index) const {
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

QString WBSDefinition::toRoman( int n, bool upper ) const
{
    if ( n >= 0 ) {
        QString s = QString::fromLatin1( RNThousands[ ( n / 1000 ) ] +
                                         RNHundreds[ ( n / 100 ) % 10 ] +
                                         RNTens[ ( n / 10 ) % 10 ] +
                                         RNUnits[ ( n ) % 10 ] );
        return upper ? s.toUpper() : s;
        
    } else { // should never happen, but better not crash if it does
        kWarning()<< " n=" << n;
        return QString::number( n );
    }
}

QStringList WBSDefinition::codeList() const {
    QStringList cl;
    QList<QPair<QString, QString> >::ConstIterator it;
    for (it = m_codeLists.constBegin(); it != m_codeLists.constEnd(); ++it) {
        cl.append((*it).second);
    }
    return cl;
}

int WBSDefinition::defaultCodeIndex() const {
    int index = -1;
    for(int i = 0; i < m_codeLists.count(); ++i) {
        if (m_defaultDef.code == m_codeLists.at(i).first) {
            index = i;
            break;
        }
    }
    return index;
}

bool WBSDefinition::setDefaultCode(uint index) {
    if ((int)index >= m_codeLists.size()) {
        return false;
    }
    m_defaultDef.code = m_codeLists[index].first;
    return true;
}

void WBSDefinition::setDefaultSeparator(const QString& s) {
    m_defaultDef.separator = s;
}

bool WBSDefinition::loadXML(KoXmlElement &element, XMLLoaderObject & ) {
    m_projectCode = element.attribute( "project-code" );
    m_projectSeparator = element.attribute( "project-separator" );
    m_levelsEnabled = (bool)element.attribute( "levels-enabled", "0" ).toInt();
    KoXmlNode n = element.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
        if (e.tagName() == "default") {
            m_defaultDef.code = e.attribute( "code", "Number" );
            m_defaultDef.separator = e.attribute( "separator", "." );
        } else if (e.tagName() == "levels") {
            KoXmlNode n = e.firstChild();
            for ( ; ! n.isNull(); n = n.nextSibling() ) {
                if ( ! n.isElement() ) {
                    continue;
                }
                KoXmlElement el = n.toElement();
                CodeDef d;
                d.code = el.attribute( "code" );
                d.separator = el.attribute( "separator" );
                int lvl = el.attribute( "level", "-1" ).toInt();
                if ( lvl >= 0 ) {
                    setLevelsDef( lvl, d );
                } else kError()<<"Invalid levels definition";
            }
        }
    }
    return true;
}

void WBSDefinition::saveXML(QDomElement &element)  const {
    QDomElement me = element.ownerDocument().createElement("wbs-definition");
    element.appendChild(me);

    me.setAttribute( "project-code", m_projectCode );
    me.setAttribute( "project-separator", m_projectSeparator );
    me.setAttribute( "levels-enabled", m_levelsEnabled );
    if ( ! m_levelsDef.isEmpty() ) {
        QDomElement ld = element.ownerDocument().createElement("levels");
        me.appendChild(ld);
        QMap<int, CodeDef>::ConstIterator it;
        for (it = m_levelsDef.constBegin(); it != m_levelsDef.constEnd(); ++it) {
            QDomElement l = element.ownerDocument().createElement("level");
            ld.appendChild(l);
            l.setAttribute( "level", it.key() );
            l.setAttribute( "code", it.value().code );
            l.setAttribute( "separator", it.value().separator );
        }
    }
    QDomElement cd = element.ownerDocument().createElement("default");
    me.appendChild(cd);
    cd.setAttribute("code", m_defaultDef.code);
    cd.setAttribute("separator", m_defaultDef.separator);
}

} //namespace KPlato
