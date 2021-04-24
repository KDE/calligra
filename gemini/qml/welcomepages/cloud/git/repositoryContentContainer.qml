/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.0
import org.calligra 1.0
import "../../../components"

RepositoryContent {
    anchors.fill: parent;
    localrepo: (accountDetails !== null) ? accountDetails.readProperty("localrepo") : "";
    privateKeyFile: (accountDetails !== null) ? accountDetails.readProperty("privateKeyFile") : "";
    needsPrivateKeyPassphrase: (accountDetails !== null) ? accountDetails.readProperty("needsPrivateKeyPassphrase") : false;
    publicKeyFile: (accountDetails !== null) ? accountDetails.readProperty("publicKeyFile") : "";
    userForRemote: (accountDetails !== null) ? accountDetails.readProperty("userForRemote") : "";
}
