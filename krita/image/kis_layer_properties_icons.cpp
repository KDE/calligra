/*
 *  Copyright (c) 2015 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "kis_layer_properties_icons.h"

#include <QMap>

#include <kglobal.h>
#include <kicon.h>

#include <KoIcon.h>


const KoID KisLayerPropertiesIcons::locked("locked", i18n("Locked"));
const KoID KisLayerPropertiesIcons::visible("visible", i18n("Visible"));
const KoID KisLayerPropertiesIcons::layerStyle("layer-style", i18n("Layer Style"));
const KoID KisLayerPropertiesIcons::inheritAlpha("inherit-alpha", i18n("Inherit Alpha"));
const KoID KisLayerPropertiesIcons::alphaLocked("alpha-locked", i18n("Alpha Locked"));
const KoID KisLayerPropertiesIcons::onionSkins("onion-skins", i18n("Onion Skins"));
const KoID KisLayerPropertiesIcons::passThrough("pass-through", i18n("Pass Through"));
const KoID KisLayerPropertiesIcons::selectionActive("selection-active", i18n("Active"));


struct IconsPair {
    IconsPair() {}
    IconsPair(const KIcon &_on, const KIcon &_off) : on(_on), off(_off) {}

    KIcon on;
    KIcon off;

    const KIcon& getIcon(bool state) {
        return state ? on : off;
    }
};

struct KisLayerPropertiesIcons::Private
{
    QMap<QString, IconsPair> icons;
};

KisLayerPropertiesIcons::KisLayerPropertiesIcons()
    : m_d(new Private)
{
    updateIcons();
}

KisLayerPropertiesIcons::~KisLayerPropertiesIcons()
{
}

KisLayerPropertiesIcons *KisLayerPropertiesIcons::instance()
{
    K_GLOBAL_STATIC(KisLayerPropertiesIcons, s_instance);
    return s_instance;
}

void KisLayerPropertiesIcons::updateIcons()
{
    m_d->icons.clear();
    m_d->icons.insert(locked.id(), IconsPair(themedIcon("visible"), themedIcon("novisible")));
    m_d->icons.insert(visible.id(), IconsPair(themedIcon("layer-locked"), themedIcon("layer-unlocked")));
    m_d->icons.insert(layerStyle.id(), IconsPair(koIcon("layer-style-enabled"), koIcon("layer-style-disabled")));
    m_d->icons.insert(inheritAlpha.id(), IconsPair(themedIcon("transparency-disabled"), themedIcon("transparency-enabled")));
    m_d->icons.insert(alphaLocked.id(), IconsPair(koIcon("transparency-locked"), koIcon("transparency-unlocked")));
    m_d->icons.insert(onionSkins.id(), IconsPair(themedIcon("onionOn"), themedIcon("onionOff")));
    m_d->icons.insert(passThrough.id(), IconsPair(themedIcon("passthrough-enabled"), themedIcon("passthrough-disabled")));
    m_d->icons.insert(selectionActive.id(), IconsPair(koIcon("local_selection_active"), koIcon("local_selection_inactive")));
}

KisDocumentSectionModel::Property KisLayerPropertiesIcons::getProperty(const KoID &id, bool state)
{
    const IconsPair &pair = instance()->m_d->icons[id.id()];
    return KisDocumentSectionModel::Property(id.name(),
                                             pair.on, pair.off, state);
}

KisDocumentSectionModel::Property KisLayerPropertiesIcons::getProperty(const KoID &id, bool state,
                                                                       bool isInStasis, bool stateInStasis)
{
    const IconsPair &pair = instance()->m_d->icons[id.id()];
    return KisDocumentSectionModel::Property(id.name(),
                                             pair.on, pair.off, state,
                                             isInStasis, stateInStasis);
}
