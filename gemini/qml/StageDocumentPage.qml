/* This file is part of the KDE project
 * Copyright (C) 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
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

import QtQuick 1.1
import "components"
import org.calligra.CalligraComponents 0.1 as Calligra

Item {
    property alias source: stageCanvas.source;
    Calligra.PresentationCanvas {
        id: stageCanvas;
        anchors.fill: parent;
        onLoadingBegun: baseLoadingDialog.visible = true;
        onLoadingFinished: {
            console.debug("doc and part: " + doc() + " " + part());
            mainWindow.setDocAndPart(doc(), part());
            baseLoadingDialog.hideMe();
        }
    }
    Flickable {
        id: controllerFlickable;
        anchors.fill: parent;
        Calligra.CanvasControllerItem {
            canvas: stageCanvas;
            flickable: controllerFlickable;
        }
    }
    Button {
        id: nextButton;
        anchors {
            bottom: parent.bottom;
            right: parent.right;
            margins: Constants.DefaultMargin;
        }
        width: height;
        image: Settings.theme.icon("forward");
        color: Settings.theme.color("base/base");
        onClicked: {
            var currentSlide = stageCanvas.currentSlide;
            stageCanvas.currentSlide = stageCanvas.currentSlide + 1;
            if(currentSlide === stageCanvas.currentSlide) {
                stageCanvas.currentSlide = 0;
            }
        }
    }
    Button {
        anchors {
            bottom: parent.bottom;
            right: nextButton.left;
            margins: Constants.DefaultMargin;
        }
        image: Settings.theme.icon("back");
        width: height;
        color: Settings.theme.color("base/base");
        onClicked: {
            if(stageCanvas.currentSlide === 0) {
                stageCanvas.currentSlide = stageCanvas.slideCount() - 1;
            }
            else {
                stageCanvas.currentSlide = stageCanvas.currentSlide - 1;
            }
        }
    }
}
