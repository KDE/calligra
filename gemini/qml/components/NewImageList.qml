/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.0

ListView {
    id: base;
    clip: true;

    signal clicked(variant options);

    delegate: ListItem {
        width: ListView.view.width;

        title: model.name;
        image.source: Settings.theme.icon(model.image);
        image.asynchronous: false;
        imageShadow: false;

        gradient: Gradient {
            GradientStop { position: 0; color: Settings.theme.color("components/newImageList/start") }
            GradientStop { position: 0.4; color: Settings.theme.color("components/newImageList/stop"); }
        }

        onClicked: {
            switch(model.bnrole) {
                case "a5p": {
                    base.clicked({
                        width: 600,
                        height: 875,
                        resolution: 150,
                    });
                }
                case "a5l": {
                    base.clicked({
                        width: 875,
                        height: 600,
                        resolution: 150,
                    });
                }
                case "a4p": {
                    base.clicked({
                        width: 1200,
                        height: 1750,
                        resolution: 150,
                    });
                }
                case "a4l": {
                    base.clicked({
                        width: 1750,
                        height: 1200,
                        resolution: 150,
                    });
                }
                case "screen": {
                    base.clicked({
                        width: Krita.Window.width,
                        height: Krita.Window.height,
                        resolution: 72.0
                    });
                }
                case "custom": {
                    base.clicked(null);
                }
                case "clip": {
                    base.clicked({ source: "clipboard" });
                }
                case "webcam": {
                    base.clicked({ source: "webcam" });
                }
            }
        }
    }

    model: ListModel {
        ListElement { bnrole: "a4p";    name: "Blank Image (A4 Portrait)"; image: "A4portrait-black" }
        ListElement { bnrole: "a4l";    name: "Blank Image (A4 Landscape)"; image: "A4landscape-black" }
//                 ListElement { bnrole: "a5p";    name: "Blank Image (A5 Portrait)"; image: "../images/svg/icon-A4portrait-black.svg" }
//                 ListElement { bnrole: "a5l";    name: "Blank Image (A5 Landscape)"; image: "../images/svg/icon-A4landscape-black.svg" }
        ListElement { bnrole: "screen"; name: "Blank Image (Screen Size)"; image: "filenew-black" }
        ListElement { bnrole: "custom"; name: "Custom Image"; image: "filenew-black" }
        ListElement { bnrole: "clip";   name: "From Clipboard"; image: "fileclip-black" }
//                 ListElement { bnrole: "webcam"; name: "From Camera"; image: "../images/svg/icon-camera-black.svg" }
    }

    ScrollDecorator { }
}
