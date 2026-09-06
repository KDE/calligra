/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Martin Pfeiffer <hubipete@gmx.net>
                 2009 Jeremias Epperlein <jeeree@web.de>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "MultiscriptElement.h"

#include "AttributeManager.h"
#include "FormulaCursor.h"
#include "FormulaDebug.h"

#include <KoXmlReader.h>
#include <KoXmlWriter.h>

#include <QPainter>

MultiscriptElement::MultiscriptElement(BasicElement *parent)
    : FixedElement(parent)
{
    m_baseElement = std::make_unique<RowElement>(this);
}

MultiscriptElement::~MultiscriptElement()
{
    // Delete all of the scripts
}

void MultiscriptElement::paint(QPainter &painter, AttributeManager *am)
{
    Q_UNUSED(painter)
    Q_UNUSED(am)
    /*do nothing as this element has no visual representation*/
}

void MultiscriptElement::ensureEvenNumberElements()
{
    if (m_postScripts.size() % 2 == 1) {
        // Odd number - add a None element to the end
        m_postScripts.push_back(nullptr);
    }
    if (m_preScripts.size() % 2 == 1) {
        // Odd number - add a None element to the end
        m_preScripts.push_back(nullptr);
    }
}

void MultiscriptElement::layout(const AttributeManager *am)
{
    // Get the minimum amount of shifting
    qreal subscriptshift = am->doubleOf("subscriptshift", this);
    qreal superscriptshift = am->doubleOf("superscriptshift", this);
    // Add half a thin space between both sup and superscript, so there is a minimum
    // of a whole thin space between them.
    qreal halfthinspace = am->layoutSpacing(this) / 2.0;

    // First make sure that there are an even number of elements in both subscript and superscript
    ensureEvenNumberElements();

    // Go through all the superscripts (pre and post) and find the maximum heights;
    // BaseLine is the distance from the top to the baseline.
    // Depth is the distance from the baseline to the bottom
    qreal maxSuperScriptDepth = 0.0;
    qreal maxSuperScriptBaseLine = 0.0;
    qreal maxSubScriptDepth = 0.0;
    qreal maxSubScriptBaseLine = 0.0;
    bool isSuperscript = true; // Toggle after each element time
    for (const auto &scriptOwner : m_postScripts) {
        BasicElement *script = scriptOwner.get();
        isSuperscript = !isSuperscript; // Toggle each time
        if (!script)
            continue; // Null means no element - just a blank
        if (isSuperscript) {
            maxSuperScriptDepth = qMax(script->height() - script->baseLine(), maxSuperScriptDepth);
            maxSuperScriptBaseLine = qMax(script->baseLine(), maxSuperScriptBaseLine);
        } else {
            // Find out how much the subscript sticks below the baseline
            maxSubScriptDepth = qMax(script->height() - script->baseLine(), maxSubScriptDepth);
            maxSubScriptBaseLine = qMax(script->baseLine(), maxSubScriptBaseLine);
        }
    }
    for (const auto &scriptOwner : m_preScripts) {
        BasicElement *script = scriptOwner.get();
        isSuperscript = !isSuperscript; // Toggle each time
        if (!script)
            continue; // Null means no element - just a blank
        if (isSuperscript) {
            maxSuperScriptDepth = qMax(script->height() - script->baseLine(), maxSuperScriptDepth);
            maxSuperScriptBaseLine = qMax(script->baseLine(), maxSuperScriptBaseLine);
        } else {
            // Find out how much the subscript sticks below the baseline
            maxSubScriptDepth = qMax(script->height() - script->baseLine(), maxSubScriptDepth);
            maxSubScriptBaseLine = qMax(script->baseLine(), maxSubScriptBaseLine);
        }
    }
    // The yOffsetBase is the amount the base element is moved down to make
    // room for the superscript
    qreal yOffsetBase = 0;
    if (maxSuperScriptDepth + maxSuperScriptBaseLine > 0) {
        yOffsetBase = maxSuperScriptDepth + maxSuperScriptBaseLine - m_baseElement->height() / 2.0 + halfthinspace;
        yOffsetBase = qMax(yOffsetBase, superscriptshift);
    }
    // The yOffsetSub is the amount the subscript elements /baseline/ are moved down.
    qreal yOffsetSub =
        yOffsetBase + maxSubScriptBaseLine + qMax(m_baseElement->height() / 2 + halfthinspace, m_baseElement->height() - maxSubScriptBaseLine + subscriptshift);

    qreal xOffset = 0.0; // We increment this as we go along, to keep track of where to place elements
    qreal lastSuperScriptWidth = 0.0;
    // Now we have all the information needed to start putting elements in place.
    // We start from the far left, and work to the far right.
    for (int i = m_preScripts.size() - 1; i >= 0; i--) {
        // We start from the end, and work in.
        // m_preScripts[0].get() is subscript etc.  So even i is subscript, odd i is superscript
        if (i % 2 == 0) {
            // i is even, so subscript
            if (!m_preScripts[i].get()) {
                xOffset += lastSuperScriptWidth;
            } else {
                // For a given vertical line, this is processed after the superscript
                qreal offset = qMax(qreal(0.0), (lastSuperScriptWidth - m_preScripts[i].get()->width()) / qreal(2.0));
                m_preScripts[i].get()->setOrigin(QPointF(offset + xOffset, yOffsetSub - m_preScripts[i].get()->baseLine()));
                xOffset += qMax(lastSuperScriptWidth, m_preScripts[i].get()->width());
            }
            if (i != 0) // No halfthinspace between the first element and the base element
                xOffset += halfthinspace;
        } else {
            // i is odd, so superscript
            // For a given vertical line, we process the superscript first, then
            // the subscript.  We need to look at the subscript (i-1) as well
            // to find out how to align them
            if (!m_preScripts[i].get())
                lastSuperScriptWidth = 0.0;
            else {
                lastSuperScriptWidth = m_preScripts[i].get()->width();
                qreal offset = 0.0;
                if (m_preScripts[i - 1].get()) // the subscript directly below us.
                    offset = qMax(qreal(0.0), (m_preScripts[i - 1].get()->width() - lastSuperScriptWidth) / qreal(2.0));
                m_preScripts[i].get()->setOrigin(QPointF(offset + xOffset, maxSuperScriptBaseLine - m_preScripts[i].get()->baseLine()));
            }
        }
    }

    // We have placed all the prescripts now.  So now place the base element
    m_baseElement->setOrigin(QPointF(xOffset, yOffsetBase));
    xOffset += m_baseElement->width();
    qreal lastSubScriptWidth = 0.0;
    // Now we can draw the post scripts.  This code is very similar, but this time we will parse
    // the subscript before the superscript
    for (int i = 0; i < m_postScripts.size(); i++) {
        // We start from the start, and work out.
        // m_preScripts[0].get() is subscript etc.  So even i is subscript, odd i is superscript
        if (i % 2 == 0) {
            // i is even, so subscript
            // For a given vertical line, we process the subscript first, then
            // the superscript.  We need to look at the superscript (i+1) as well
            // to find out how to align them

            if (!m_postScripts[i].get()) {
                lastSubScriptWidth = 0.0;
            } else {
                lastSubScriptWidth = m_postScripts[i].get()->width();
                // For a given vertical line, this is processed after the superscript
                qreal offset = 0.0;
                if (m_postScripts.size() > i + 1 && m_postScripts[i + 1].get() != nullptr) // the subscript directly below us.
                    offset = qMax(qreal(0.0), (m_postScripts[i + 1].get()->width() - lastSubScriptWidth) / qreal(2.0));
                m_postScripts[i].get()->setOrigin(QPointF(offset + xOffset, yOffsetSub - m_postScripts[i].get()->baseLine()));
            }
        } else {
            // i is odd, so superscript
            if (!m_postScripts[i].get())
                xOffset += lastSubScriptWidth;
            else {
                qreal offset = qMax(qreal(0.0), (lastSubScriptWidth - m_postScripts[i].get()->width()) / qreal(2.0));
                m_postScripts[i].get()->setOrigin(QPointF(offset + xOffset, maxSuperScriptBaseLine - m_postScripts[i].get()->baseLine()));
                xOffset += qMax(lastSubScriptWidth, m_postScripts[i].get()->width());
            }
            if (i != m_postScripts.size() - 1)
                xOffset += halfthinspace; // Don't add an unneeded space at the very end
        }
    }

    // Finally, set our boundingbox
    setWidth(xOffset);
    setHeight(yOffsetSub + maxSubScriptDepth);
    setBaseLine(yOffsetBase + m_baseElement->baseLine());
}

bool MultiscriptElement::acceptCursor(const FormulaCursor &cursor)
{
    Q_UNUSED(cursor)
    return false;
}

const QList<BasicElement *> MultiscriptElement::childElements() const
{
    QList<BasicElement *> list;

    list << m_baseElement.get();

    // postscript elements
    for (const auto &tmpOwner : m_postScripts) {
        BasicElement *tmp = tmpOwner.get();
        if (tmp)
            list << tmp;
    }

    // prescript elements
#if 1
    for (const auto &tmpOwner : m_preScripts) {
        BasicElement *tmp = tmpOwner.get();
        if (tmp)
            list << tmp;
    }
#else
    // What is this strange construction?
    for (int i = m_preScripts.size() - 2; i >= 0; i -= 2) {
        if (m_preScripts[i].get())
            list << m_preScripts[i].get();
        if (m_preScripts[i + 1].get())
            list << m_preScripts[i + 1].get();
    }
#endif

    return list;
}

QString MultiscriptElement::attributesDefaultValue(const QString &attribute) const
{
    Q_UNUSED(attribute)
    return QString();
}

ElementType MultiscriptElement::elementType() const
{
    return MultiScript;
}

bool MultiscriptElement::readMathMLContent(const KoXmlElement &parent)
{
    BasicElement *tmpElement = nullptr;
    KoXmlElement tmp;
    bool prescript = false; // When we see a mprescripts tag, we enable this
    bool baseElement = false; // True when the base element is read.
    forEachElement(tmp, parent)
    {
        if (tmp.tagName() == "none") {
            // In mathml, we read subscript, then superscript, etc.  To skip one,
            // you use "none"
            // To represent "none" we use a nullptr
            if (prescript)
                m_preScripts.push_back(nullptr);
            else
                m_postScripts.push_back(nullptr);
            continue;
        } else if (tmp.tagName() == "mprescripts") {
            prescript = true;
            // In mathml, when we see this tag, all the elements after it are
            //  for prescripts
            continue;
        }

        auto element = ElementFactory::createElement(tmp.tagName(), this);
        tmpElement = element.get();
        if (!tmpElement->readMathML(tmp)) {
            return false;
        }

        // The very first element is the base
        if (!baseElement) {
            m_baseElement.reset(element.release());
            baseElement = true;
        } else if (prescript)
            m_preScripts.push_back(std::unique_ptr<BasicElement>(element.release()));
        else
            m_postScripts.push_back(std::unique_ptr<BasicElement>(element.release()));
    }

    ensureEvenNumberElements();
    Q_ASSERT(m_baseElement); // We should have at least a BasicElement for the base
    return true;
}

void MultiscriptElement::writeMathMLContent(KoXmlWriter *writer, const QString &ns) const
{
    m_baseElement->writeMathML(writer, ns); // Just save the children in
                                            // the right order
    for (const auto &tmpOwner : m_postScripts) {
        BasicElement *tmp = tmpOwner.get();
        if (tmp)
            tmp->writeMathML(writer, ns);
        else {
            // We need to use a none element for missing elements in the super/sub scripts
            QString s = ns.isEmpty() ? QString::fromLatin1("none") : ns.toLatin1() + QString::fromLatin1(":none");
            writer->startElement(s.toLatin1());
            writer->endElement();
        }
    }
    if (m_preScripts.empty())
        return;
    QString s = ns.isEmpty() ? QString::fromLatin1("mprescripts") : ns.toLatin1() + QString::fromLatin1(":mprescripts");
    writer->startElement(s.toLatin1());
    writer->endElement();
    for (const auto &tmpOwner : m_preScripts) {
        BasicElement *tmp = tmpOwner.get();
        if (tmp)
            tmp->writeMathML(writer, ns);
        else {
            // We need to use a none element for missing elements in the super/sub scripts
            QString n = ns.isEmpty() ? QString::fromLatin1("none") : ns.toLatin1() + QString::fromLatin1(":none");
            writer->startElement(n.toLatin1());
            writer->endElement();
        }
    }
}

// int MultiscriptElement::length() const
// {
//     if (!m_postScripts.empty() && m_postScripts.last()==0) {
//         //the last element is empty, so there are no cursor positions around it
//         return 2*(m_preScripts.size()+m_postScripts.size())-1;
//     } else {
//         return 2*(m_preScripts.size()+m_postScripts.size()+1)-1;
//     }
// }

bool MultiscriptElement::moveCursor(FormulaCursor &newcursor, FormulaCursor &oldcursor)
{
    // grouppositions:  1 3   1 3
    //                  0 2 2 0 2
    //                  |     | |
    // pairs:           0 1   0 1
    // TODO: Fill this out

    int childposition = newcursor.position() / 2;
    // this should be cached
    int prescriptCount = 0;
    for (const auto &tmpOwner : m_preScripts) {
        BasicElement *tmp = tmpOwner.get();
        if (tmp) {
            prescriptCount++;
        }
    }
    if (childposition == prescriptCount) {
        // we are in BasePosition
        if (newcursor.direction() == MoveUp || newcursor.direction() == MoveDown) {
            return false;
        }
        if (m_postScripts.empty() && m_preScripts.empty()) {
            // this should not happen
            return moveSingleSituation(newcursor, oldcursor, childElements().indexOf(m_baseElement.get()));
        }
        if (newcursor.direction() == MoveLeft) {
            if (!m_preScripts.empty()) {
                // we search for the first non nullptr element to the left
                int i;
                for (i = 0; i < m_preScripts.size(); i++) {
                    if (m_preScripts[i].get()) {
                        break;
                    }
                }
                if ((i < m_preScripts.size()) && m_preScripts[i].get()) {
                    return moveHorSituation(newcursor, oldcursor, childElements().indexOf(m_preScripts[i].get()), childElements().indexOf(m_baseElement.get()));
                }
            }
            return moveSingleSituation(newcursor, oldcursor, 0);
        } else if (newcursor.direction() == MoveRight) {
            if (!m_postScripts.empty()) {
                // we search for the first non nullptr element to the left
                int i;
                for (i = 0; i < m_postScripts.size(); i++) {
                    if (m_postScripts[i].get()) {
                        break;
                    }
                }
                if (m_postScripts[i].get()) {
                    return moveHorSituation(newcursor,
                                            oldcursor,
                                            childElements().indexOf(m_baseElement.get()),
                                            childElements().indexOf(m_postScripts[i].get()));
                }
            }
            return moveSingleSituation(newcursor, oldcursor, childElements().indexOf(m_baseElement.get()));
        }
    } else {
        int groupposition;
        auto scriptIndex = [](const auto &scripts, BasicElement *element) {
            for (size_t i = 0; i < scripts.size(); ++i) {
                if (scripts[i].get() == element)
                    return static_cast<int>(i);
            }
            return -1;
        };
        bool prescript = true;
        if (childposition < prescriptCount) {
            // determine the position in the pre-/postscripts we are in
            groupposition = scriptIndex(m_preScripts, childElements()[childposition]);
        } else {
            groupposition = scriptIndex(m_postScripts, childElements()[childposition]);
            prescript = false;
        }
        int pair = groupposition / 2;
        if (newcursor.direction() == MoveUp || newcursor.direction() == MoveDown) {
            //             debugFormula << groupposition<<" - "<<prescriptCount<< "-" <<pair;
            if (prescript) {
                if (m_preScripts[pair * 2].get() && m_preScripts[pair * 2 + 1].get()) {
                    return moveVertSituation(newcursor,
                                             oldcursor,
                                             childElements().indexOf(m_preScripts[pair * 2 + 1].get()),
                                             childElements().indexOf(m_preScripts[pair * 2].get()));
                } else {
                    return false;
                }
            } else {
                if (m_postScripts[pair * 2].get() && m_postScripts[pair * 2 + 1].get()) {
                    return moveVertSituation(newcursor,
                                             oldcursor,
                                             childElements().indexOf(m_postScripts[pair * 2 + 1].get()),
                                             childElements().indexOf(m_postScripts[pair * 2].get()));
                } else {
                    return false;
                }
            }
        } else if (newcursor.direction() == MoveLeft) {
            if (prescript) {
                // we are in the prescripts
                int i = groupposition + 2;
                if (!((i < m_preScripts.size()) && m_preScripts[i].get())) {
                    for (i = groupposition + 1; i < m_preScripts.size(); i++) {
                        if (m_preScripts[i].get()) {
                            break;
                        }
                    }
                }
                if ((i < m_preScripts.size()) && m_preScripts[i].get()) {
                    return moveHorSituation(newcursor,
                                            oldcursor,
                                            childElements().indexOf(m_preScripts[i].get()),
                                            childElements().indexOf(m_preScripts[groupposition].get()));
                } else {
                    return moveSingleSituation(newcursor, oldcursor, childElements().indexOf(m_preScripts[groupposition].get()));
                }
            } else {
                // we are in the postscripts
                int i = groupposition - 1;
                if (!(i >= 0) && m_postScripts[i].get()) {
                    for (i = groupposition - 2; i >= 0; i--) {
                        if (m_postScripts[i].get()) {
                            break;
                        }
                    }
                }
                if ((i >= 0) && m_postScripts[i].get()) {
                    return moveHorSituation(newcursor,
                                            oldcursor,
                                            childElements().indexOf(m_postScripts[i].get()),
                                            childElements().indexOf(m_postScripts[groupposition].get()));
                } else {
                    return moveHorSituation(newcursor,
                                            oldcursor,
                                            childElements().indexOf(m_baseElement.get()),
                                            childElements().indexOf(elementNext(newcursor.position())));
                }
            }
        } else if (newcursor.direction() == MoveRight) {
            if (prescript) {
                // we are in the prescripts
                int i = groupposition - 2;
                if (!((i >= 0) && m_preScripts[i].get())) {
                    for (i = groupposition - 1; i >= 0; i--) {
                        if (m_preScripts[i].get()) {
                            break;
                        }
                    }
                }
                if ((i >= 0) && m_preScripts[i].get()) {
                    //                    debugFormula << "Going from "<< groupposition <<" to " <<i;
                    return moveHorSituation(newcursor,
                                            oldcursor,
                                            childElements().indexOf(m_preScripts[groupposition].get()),
                                            childElements().indexOf(m_preScripts[i].get()));
                } else {
                    return moveHorSituation(newcursor,
                                            oldcursor,
                                            childElements().indexOf(elementNext(newcursor.position())),
                                            childElements().indexOf(m_baseElement.get()));
                }
            } else {
                // we are in the postscripts
                int i = groupposition + 2;
                if (!((i < m_postScripts.size()) && m_postScripts[i].get())) {
                    for (i = groupposition + 1; i < m_postScripts.size(); i++) {
                        if (m_postScripts[i].get()) {
                            break;
                        }
                    }
                }
                if ((i < m_postScripts.size()) && m_postScripts[i].get()) {
                    return moveHorSituation(newcursor,
                                            oldcursor,
                                            childElements().indexOf(m_postScripts[groupposition].get()),
                                            childElements().indexOf(m_postScripts[i].get()));
                } else {
                    return moveSingleSituation(newcursor, oldcursor, childElements().indexOf(m_preScripts[groupposition].get()));
                }
            }
        }
    }
    return false;
}

bool MultiscriptElement::setCursorTo(FormulaCursor &cursor, QPointF point)
{
    if (cursor.isSelecting()) {
        return false;
    }
    foreach (BasicElement *tmp, childElements()) {
        if (tmp->boundingRect().contains(point)) {
            return tmp->setCursorTo(cursor, point - tmp->origin());
        }
    }
    return m_baseElement->setCursorTo(cursor, point - m_baseElement->origin());
}
