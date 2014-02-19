/* This file is part of the KDE project
 * Copyright (C) 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
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
import org.krita.sketch 1.0

PageStack {
    id: base;
    clip: true;

    initialPage: Page {
        ListView {
            id: listView;
			anchors.left: parent.left;
			anchors.right: parent.right;
			anchors.top: parent.top;
			
			height: parent.height - Constants.GridHeight * 2;
			clip: true;
		
            delegate: ListItem {
                title: model.name;
                image: model.image;
                imageShadow: false;
                imageSmooth: false;
                imageFillMode: Image.PreserveAspectCrop;
                imageCache: false;

                onClicked: {
                    base.push( detailsPage,
                              { title: model.title,
                                description: model.description});
                }
                Image {
                    id: cloudIcon;
					visible: model.cloudSynced;
                    source: "../images/icon_steam_cloud-black.png"

                    anchors.right: parent.right;
                    anchors.rightMargin: Constants.GridWidth * 0.25;
					anchors.top: parent.top;
					anchors.topMargin: Constants.GridHeight * 0.25;

                    width: Constants.GridWidth * 0.5;

                    asynchronous: true;
                    fillMode: Image.PreserveAspectFit;
                    clip: true;
                    smooth: true;

                    sourceSize.width: width > height ? height : width;
                    sourceSize.height: width > height ? height : width;
                }
            }

            model: {
                return fallbackCollectionsModel
                /*
                if (aggregatedFeedsModel.articleCount > 0)
                    return aggregatedFeedsModel
                else {
                    return fallbackCollectionsModel
                }
                */
            }

            ScrollDecorator { }
        }
		
		ListItem {
			anchors.top: listView.bottom;

			title: "Templates";
			image: "../images/svg/icon-fileopen-black.svg";
			
			onClicked: {
				base.push( templatesPage,
						  { title: "Templates",
							description: "List of templates"});
			}

		}
    }

    ListModel {
        id: fallbackCollectionsModel;
        ListElement {
            name:"Portfolio";
            image:"../images/collection-portfolio.png";
            description: "<div>Portfolio</div><p>Store your finished works here.</p>";
            updateDate: "Today!";
			cloudSynced: true;
        }
        ListElement {
            name:"Sketches";
            image:"../images/collection-sketches.png";
            description: "<div>Sketches</div><p>Work on rough sketches here.</p>";
            updateDate: "Today!";
			cloudSynced: false;
        }
        ListElement {
            name:"Gallery";
            image:"../images/collection-gallery.png";
            description: "<div>Gallery</div><p>Display your favourite works here.</p>";
            updateDate: "Today!";
			cloudSynced: true;
        }
    }
	
	ListModel {
        id: fallbackTemplatesModel;
        ListElement {
            name:"Comic Templates";
            image:"../images/collection-portfolio.png";
            description: "";
			author:"";
        }
        ListElement {
            name:"Design Templates";
            image:"../images/collection-sketches.png";
            description: "";
        }
        ListElement {
            name:"DSLR Templates";
            image:"../images/collection-gallery.png";
            description: "";
        }
        ListElement {
            name:"Film Templates";
            image:"../images/collection-gallery.png";
            description: "";
        }
        ListElement {
            name:"Texture Templates";
            image:"../images/collection-gallery.png";
            description: "";
        }
    }
	
	Component {
		id: templatesPage;
		Page {
			property string title;
			property string description;
			
			Flickable {
                anchors.fill: parent;
                anchors.leftMargin: Constants.DefaultMargin;
                anchors.rightMargin: Constants.DefaultMargin;
                anchors.bottomMargin: Constants.DefaultMargin;

                contentWidth: width;
                contentHeight: contents.height;

                Column {
                    id: contents;
                    width: parent.width;

                    Item {
                        width: parent.width;
                        height: Constants.GridHeight;

                        Label {
                            anchors {
                                top: parent.top;
                                topMargin: Constants.DefaultMargin;
                            }

                            text: title
							verticalAlignment: Text.AlignTop;
                        }
                    }

                    Label {
                        width: parent.width;
                        height: paintedHeight;

                        elide: Text.ElideNone;
                        wrapMode: Text.WordWrap;
                        horizontalAlignment: Text.AlignJustify;

                        text: description;
                    }
					
					ListView {
						id: templatesListView;
						anchors.fill: parent;
						clip: true;
					
						delegate: ListItem {
							title: model.name;
							image: model.image;
							imageShadow: false;
							imageSmooth: false;
							imageFillMode: Image.PreserveAspectCrop;
							imageCache: false;

							description: model.updateDate;

							onClicked: {
							/*
								base.push( detailsPage,
										  { title: model.title,
											description: model.description,
											updateDate: model.updateDate});
											*/
							}
						}

						model: {
							return fallbackTemplatesModel
							/*
							if (aggregatedFeedsModel.articleCount > 0)
								return aggregatedFeedsModel
							else {
								return fallbackCollectionsModel
							}
							*/
						}

						ScrollDecorator { }
					}
                }

                MouseArea {
                    anchors.fill: parent;
                    onClicked: pageStack.pop();
                }
            }
		}
	}

    Component {
        id: detailsPage;

        Page {

            property string title;
            property string updateDate;
            property string description;

            Flickable {
                anchors.fill: parent;
                anchors.leftMargin: Constants.DefaultMargin;
                anchors.rightMargin: Constants.DefaultMargin;
                anchors.bottomMargin: Constants.DefaultMargin;

                contentWidth: width;
                contentHeight: contents.height;

                Column {
                    id: contents;
                    width: parent.width;

                    Item {
                        width: parent.width;
                        height: Constants.GridHeight;

                        Label {
                            anchors {
                                top: parent.top;
                                topMargin: Constants.DefaultMargin;
                            }

                            text: title
							verticalAlignment: Text.AlignTop;
                        }

                        Label {
                            anchors {
                                bottom: parent.bottom;
                                bottomMargin: Constants.DefaultMargin;
                            }

                            text: updateDate;
                            font.pixelSize: Constants.SmallFontSize;
                            color: Constants.Theme.SecondaryTextColor;
                            verticalAlignment: Text.AlignBottom;
                        }


                    }

                    Label {
                        width: parent.width;
                        height: paintedHeight;

                        elide: Text.ElideNone;
                        wrapMode: Text.WordWrap;
                        horizontalAlignment: Text.AlignJustify;

                        text: description;
                    }
                }

                MouseArea {
                    anchors.fill: parent;
                    onClicked: pageStack.pop();
                }
            }
        }
    }

}
