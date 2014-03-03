/* This file is part of the KDE project
 * Copyright (C) 2014 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

var values = {
    base: {
        base: "#999999",
        text: "#000000",
        header: "#666666",
        headerText: "#ff0000",
    },

    components: {
        button: {
            base: "transparent",
            text: "#ffffff",
            highlight: "transparent",
            checked: Qt.rgba(1.0, 1.0, 1.0, 0.7),
        },
        colorSwatch: {
            border: "silver",
        },
        dialog: {
            modalOverlay: Qt.rgba(0.0, 0.0, 0.0, 0.5),
            background: {
                start: "#F7F8FC",
                stop: "#F0F0FA",
            },
            header: "#9AA1B2",
            headerText: "#ffffff",
            progress: {
                background: "#ffffff",
                border: "silver",
                bar: "gray",
            },
            button: "#9AA1B2",
            buttonText: "#ffffff",
        },
        expandingListView: {
            selection: {
                border: "#ffffff",
                fill: Qt.rgba(1.0, 1.0, 1.0, 0.4),
                text: Qt.rgba(0.0, 0.0, 0.0, 0.65),
            },
            list: {
                background: Qt.rgba(1.0, 1.0, 1.0, 0.4),
                item: "#ffffff",
                itemBorder: "silver",
                itemText: Qt.rgba(0.0, 0.0, 0.0, 0.65),
            },
        },
        header: "#ffffff",
        label: "#323232",
        listItem: {
            background: {
                start: "#FBFBFB",
                stop: "#F0F0F0",
            },
            title: "#000000",
            description: "#333333",
        },
        messageStack: {
            background: "gray",
            border: "silver",
            text: "white",
            button: {
                fill: "gray",
                border: "silver",
            },
        },
    },

    panels: {
        dropArea: {
            fill: Qt.rgba(1.0, 1.0, 1.0, 0.25),
            border: "#ffffff",
        },

        base: {
            base: "#ffff00",
            text: "#000000",
            header: "#00ffff",
            headerText: "#000000",
        },

        presets: {
            base: "#ffff00",
            text: "#000000",
            header: "#00ffff",
            headerText: "#000000",

            preset: {
                active: "#D7D7D7",
                inactive: "transparent",
            }
        },

        layers: {
            base: "#eeeeff",
            text: "#000000",
            header: "#aaaaff",
            headerText: "#000000",
            subheader: "#ffaaaa",
            editorButtons: {
                active: "#EAEAEA",
                inactive: "transparent",
                text: "gray",
                border: "silver",
            },
            layer: {
                active: Qt.rgba(1.0, 1.0, 1.0, 0.5),
                inactive: Qt.rgba(1.0, 1.0, 1.0, 0.2),
                text: "#000000",
                visible: "#ff0000",
                locked: "#00ff00",
            },
        },

        selection: {
            base: "#ffff00",
            text: "#000000",
            header: "#00ffff",
            headerText: "#000000",

            buttons: {
                color: Qt.rgba(1.0, 1.0, 1.0, 0.4),
                text: "black",
                border: "silver"
            }
        },

        filter: {
            base: "#ffff00",
            text: "#000000",
            header: "#00ffff",
            headerText: "#000000",
        },

        color: {
            base: "#ffff00",
            text: "#000000",
            header: "#00ffff",
            headerText: "#000000",
        },

        tool: {
            base: "#ffff00",
            text: "#000000",
            header: "#00ffff",
            headerText: "#000000",
            subheader: "#ffaaaa",
        },

        menu: {
            base: "#00ff00",
            text: "#ffffff",
            buttonHighlight: "#aaaaaa",
        },

        newImage: {
            background: "#ffffff",
            header: {
                start: "#707070",
                stop: "#565656",
                text: "#ffffff",
            }
        },

        openImage: {
            background: "#ffffff",
            header: {
                start: "#707070",
                stop: "#565656",
                text: "#ffffff",
            }
        },
    },

    pages: {
        welcome: {
            background: "#eeffff",
            open:{
                header: {
                    start: "#707000",
                    stop: "#565600",
                    text: "#ffffff",
                },
            },
            create: {
                header: {
                    start: "#005656",
                    stop: "#007070",
                    text: "#ffffff",
                },
            },
            news: {
                header: {
                    start: "#700070",
                    stop: "#560056",
                    text: "#ffffff",
                },
            },
        },
        open: {
            background: "#ffeeff",
            location: "#ffffff",
        },
        save: {
            background: "#ffffee",
            location: "#ffffff",
            footer: "#551111",
        },
    },
}
