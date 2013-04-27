import QtQuick 1.1
import DocumentView 1.0

Page {
    id: documentPage

    function openFile(file) {
        infoLabel.text = ""
        zoomButton.enabled = false

        documentViewItem.openFile(file)

        //documentViewItem.zoom = 1.0
        documentViewItem.x = 0.0
        documentViewItem.y = 0.0
        flickable.contentX = 0.0
        flickable.contentY = 0.0
    }

    signal openClicked()

    ButtonBar {
        id: buttonBar
        PageStack {
            id: barPageStack
            height: buttonRow.height
            Page {
                id: buttonPage
                Row {
                    id: buttonRow
                    width: parent.width
                    spacing: 5
                    Button {
                        id: fileButton
                        //iconSource: "qrc:///images/open.png"
                        text: "Open"
                        onClicked: {
                            documentPage.openClicked()
                        }
                    }
                    Button {
                        id: zoomButton
                        //iconSource: "qrc:///images/zoom.png"
                        text: "Zoom"
                        checkable: true
                        checked: false
                        enabled: false
                        onEnabledChanged: {
                            if (checked && !enabled)
                                checked = false
                        }
                        Behavior on checked {
                            NumberAnimation {
                                duration: 100
                                target: zoomSlider
                                properties: "height"
                                from: zoomButton.checked ? zoomSlider.wantedHeight : 0
                                to: zoomButton.checked ? 0 : zoomSlider.wantedHeight
                            }
                        }
                    }
                    Label {
                        id: fileLabel
                        elide: Text.ElideLeft
                        verticalAlignment: Text.AlignVCenter
                        height: fileButton.height
                        width: buttonRow.width - buttonRow.spacing - fileButton.width
                    }
                }
            }
            Page {
                id: progressPage
                ProgressBar {
                    id: progressBar
                    height: parent.height
                    value: 0
                }
            }
        }
    }

    Slider {
        id: zoomSlider
        anchors {
            top: buttonBar.bottom
            left: parent.left
            right: parent.right
        }
        property real wantedHeight: barPageStack.height
        height: 0
        //visible: height > 0
        clip: true
        minimum: 0
        maximum: 400
        value: 100
        property bool valueIsChanging: false
        onValueChanged: {
            if (!valueIsChanging) {
                valueIsChanging = true;
                documentViewItem.setZoomBegin()
            }
            var factor = value / 100.0
            documentViewItem.setZoom(factor, flickable.contentX, flickable.contentY)
        }
        onValueChangedDone: {
            valueIsChanging = false
            documentViewItem.setZoomEnd()
        }
        Component.onCompleted: {
            documentViewItem.zoomChanged.connect( function() {
                var v = documentViewItem.zoom * 100.0
                zoomSlider.value = v
            })
        }
    }

    Rectangle {
        id: infoBox
        visible: infoLabel.text.length > 0
        anchors {
            top: zoomSlider.bottom
            left: parent.left
            right: parent.right
            margins: 5
        }
        height: infoLabel.height
        color: "transparent"
        Label {
            id: infoLabel
            color: "#ff3333"
            font.bold: true
            wrapMode: Text.Wrap
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }
        }
    }

    Flickable {
        id: flickable
        anchors {
            top: infoBox.visible ? infoBox.bottom : zoomSlider.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            //margins: 5
        }
        contentHeight: documentViewItem.implicitHeight
        contentWidth: Math.max(documentViewItem.implicitWidth, parent.width)
        clip: true

        PinchArea {
            id: pinchArea
            width: Math.max(flickable.contentWidth, flickable.width)
            height: Math.max(flickable.contentHeight, flickable.height)

            function distance(p1, p2) {
                var dx = p2.x - p1.x
                var dy = p2.y - p1.y
                return Math.sqrt(dx*dx + dy*dy)
            }

            property real initialDistance
            property real initialZoom
            property real zoomCenterX
            property real zoomCenterY
            property real zoomValue
            onZoomValueChanged: {
                if (zoomValue != documentViewItem.zoom)
                    documentViewItem.setZoom(zoomValue, zoomCenterX, zoomCenterY)
            }
            SmoothedAnimation {
                id: zoomAnimation
                duration: 300
                target: pinchArea
                properties: "zoomValue"
                velocity: -1
            }

            onPinchStarted: {
                initialDistance = distance(pinch.point1, pinch.point2)
                initialZoom = documentViewItem.zoom
                zoomValue = documentViewItem.zoom
                zoomAnimation.from = documentViewItem.zoom
                documentViewItem.setZoomBegin()
            }

            onPinchUpdated: {
                //if (pinch.pointCount < 2)
                //    return

                var currentDistance = distance(pinch.point1, pinch.point2)
                var scale = (currentDistance / initialDistance) * initialZoom

                zoomCenterX = flickable.contentX // + pinch.center.x
                zoomCenterY = flickable.contentY // + pinch.center.y
                zoomAnimation.to = scale
                zoomAnimation.start()
            }

            onPinchFinished: {
                zoomAnimation.complete()
                documentViewItem.setZoomEnd()
            }
        }

        DocumentViewItem {
            id: documentViewItem
            pageColor: "#ffffff"

            function setZoom(factor, x, y) {
                var prevImplicitWidth = documentViewItem.implicitWidth
                var prevImplicitHeight = documentViewItem.implicitHeight
                documentViewItem.zoom = factor
                flickable.contentX += (documentViewItem.implicitWidth - prevImplicitWidth) * (x / prevImplicitWidth)
                flickable.contentY += (documentViewItem.implicitHeight - prevImplicitHeight) * (y / prevImplicitHeight)
            }

            onOpenFileFailed: {
                infoLabel.text = qsTr("Error: %1").arg(error)
                zoomButton.enabled = false
            }

            onProgressUpdated: {
                if (percent < 0) {
                    fileLabel.text = file()
                    if (barPageStack.currentPage == progressPage)
                        barPageStack.push(buttonPage, true)
                    progressBar.value = 0
                    zoomButton.enabled = true
                } else {
                    if (barPageStack.currentPage != progressPage)
                        barPageStack.push(progressPage, true)
                    progressBar.value = percent
                }
            }
        }
    }

    Component.onCompleted: {
        var file = Settings.openFileRequested()
//file='/home/android/Call-Accounting-System_Quota-Howto.docx'
        if (file.length > 0)
            documentPage.openFile(file)
        else
            barPageStack.push(buttonPage, true)

        Settings.openFileRequestedChanged.connect( function(file) {
            if (file.length > 0)
                documentPage.openFile(file)
        } )
//zoomButton.checked = true
    }
}
