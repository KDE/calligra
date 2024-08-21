/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Pierre Stirnweiss <pstirnweiss@googlemail.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ValidParentStylesProxyModel.h"

#include <QModelIndex>

#include <KoParagraphStyle.h>
#include <KoStyleManager.h>

#include <QDebug>

ValidParentStylesProxyModel::ValidParentStylesProxyModel(QObject *parent)
    : StylesFilteredModelBase(parent)
    , m_styleManager(nullptr)
{
}

void ValidParentStylesProxyModel::setStyleManager(KoStyleManager *sm)
{
    Q_ASSERT(sm);

    m_styleManager = sm;
}

void ValidParentStylesProxyModel::createMapping()
{
    if (!m_styleManager || !m_sourceModel) {
        return;
    }
    m_sourceToProxy.clear();
    m_proxyToSource.clear();

    for (int i = 0; i < m_sourceModel->rowCount(QModelIndex()); ++i) {
        QModelIndex index = m_sourceModel->index(i, 0, QModelIndex());
        int id = (int)index.internalId();
        KoParagraphStyle *paragraphStyle = m_styleManager->paragraphStyle(id);
        if (paragraphStyle) {
            bool ok = true;
            KoParagraphStyle *testStyle = paragraphStyle;
            while (testStyle && ok) {
                ok = testStyle->styleId() != m_currentChildStyleId;
                testStyle = testStyle->parentStyle();
            }
            if (!ok) {
                continue; // we cannot inherit ourself even indirectly through the parent chain
            }
            m_proxyToSource.append(i); // the style is ok for parenting
        } else {
            KoCharacterStyle *characterStyle = m_styleManager->characterStyle(id);
            if (characterStyle) {
                bool ok = true;
                KoCharacterStyle *testStyle = characterStyle;
                while (testStyle && ok) {
                    ok = testStyle->styleId() != m_currentChildStyleId;
                    testStyle = testStyle->parentStyle();
                }
                if (!ok) {
                    continue; // we cannot inherit ourself even indirectly through the parent chain
                }
                m_proxyToSource.append(i); // the style is ok for parenting
            }
        }
    }
    m_sourceToProxy.fill(-1, m_sourceModel->rowCount(QModelIndex()));
    for (int i = 0; i < m_proxyToSource.count(); ++i) {
        m_sourceToProxy[m_proxyToSource.at(i)] = i;
    }
}

void ValidParentStylesProxyModel::setCurrentChildStyleId(int styleId)
{
    m_currentChildStyleId = styleId;
    Q_EMIT layoutAboutToBeChanged();
    createMapping();
    Q_EMIT layoutChanged();
}
