import QtQuick 1.1
import "PageStack.js" as Engine

Item {
    id: root

    width: parent ? parent.width : 0
    height: parent ? parent.height : 0

    // Page stack depth.
    property int depth: Engine.getDepth()

    // The currently active page.
    property Item currentPage: null

    // Indicates whether there is an ongoing page transition.
    //property bool busy: (currentPage != null && currentPage && currentPage.parent && currentPage.parent.busy)
    property bool busy: false

    // Pushes a page on the stack.
    // The page can be defined as a component or an item.
    // If an item is used then the page will get re-parented.
    // Returns the page instance.
    function push(page, immediate) {
        return Engine.push(page, false, (immediate != null && immediate != undefined && immediate));
    }

    // Pops a page off the stack.
    // If page is specified then the stack is unwound to that page.
    // Returns the page instance that was popped off the stack.
    function pop(page, immediate) {
        return Engine.pop(page, (immediate != null && immediate != undefined && immediate));
    }

    // Replaces a page on the stack.
    // See push() for details.
    function replace(page, immediate) {
        return Engine.push(page, true, (immediate != null && immediate != undefined && immediate));
    }

    // Clears the page stack.
    function clear() {
        return Engine.clear();
    }

    function contains(page) {
        for (var i = 0; i < Engine.pageStack.length; i++) {
          if (Engine.pageStack[i].page === page) {
              return true;
          }
        }
        return false;
    }

    // Called when the page stack visibility changes.
    onVisibleChanged: {
        if (currentPage != null && currentPage) {
            if (root.visible) {
                // page stack became visible
                __emitPageLifecycleSignal(currentPage, "activating");
                __emitPageLifecycleSignal(currentPage, "activated");
            } else {
                // page stack became invisible
                __emitPageLifecycleSignal(currentPage, "deactivating");
                __emitPageLifecycleSignal(currentPage, "deactivated");
            }
        }
    }

    // Emits a lifecycle signal for a page.
    function __emitPageLifecycleSignal(page, signal) {
        if (page[signal]) {
            page[signal]();
        }
    }

    // Component for page slots.
    Component {
        id: slotComponent

        Item {
            id: slot

            width: parent ? parent.width : 0
            height: parent ? parent.height : 0

            // The states correspond to the different possible positions of the slot.
            state: "hidden"

            // The page held by this slot.
            property Item page: null
            // The owner of the page.
            property Item owner: null

            // Duration of transition animation (in ms)
            property int transitionDuration: 500

            // Tracks whether a transition is ongoing.
            property bool busy: false

            // Performs a push enter transition.
            function pushEnter(replace, immediate) {
                if (!immediate) {
                    state = replace ? "front" : "right";
                }
                state = "";
                page.visible = true;
                if (root.visible && immediate) {
                    __emitPageLifecycleSignal(page, "activating");
                    __emitPageLifecycleSignal(page, "activated");
                }
            }

            // Performs a push exit transition.
            function pushExit(replace, immediate) {
                state = immediate ? "hidden" : (replace ? "back" : "left");
                if (root.visible && immediate) {
                    __emitPageLifecycleSignal(page, "deactivating");
                    __emitPageLifecycleSignal(page, "deactivated");
                }
                if (replace) {
                    slot.destroy(immediate ? 0 : transitionDuration + 100);
                }
            }

            // Performs a pop enter transition.
            function popEnter(immediate) {
                if (!immediate) {
                    state = "left";
                }
                state = "";
                page.visible = true;
                if (root.visible && immediate) {
                    __emitPageLifecycleSignal(page, "activating");
                    __emitPageLifecycleSignal(page, "activated");
                }
            }

            // Performs a pop exit transition.
            function popExit(immediate) {
                state = immediate ? "hidden" : "right";
                if (root.visible && immediate) {
                    __emitPageLifecycleSignal(page, "deactivating");
                    __emitPageLifecycleSignal(page, "deactivated");
                }
                slot.destroy(immediate ? 0 : transitionDuration + 100);
            }

            // Called when a transition has started.
            function transitionStarted() {
                busy = true;
                if (root.visible) {
                    __emitPageLifecycleSignal(page, state == "" ? "activating" : "deactivating");
                }
            }

            // Called when a transition has ended.
            function transitionEnded() {
                busy = false;
                if (root.visible) {
                    __emitPageLifecycleSignal(page, state == "" ? "activated" : "deactivated");
                }
                if (state == "left" || state == "right" || state == "back") {
                    state = "hidden";
                }
            }

            states: [
                // Start state for pop entry, end state for push exit.
                State {
                    name: "left"
                    PropertyChanges { target: slot; x: -width }
                },
                // Start state for push entry, end state for pop exit.
                State {
                    name: "right"
                    PropertyChanges { target: slot; x: width }
                },
                // Start state for replace entry.
                State {
                    name: "front"
                    PropertyChanges { target: slot; scale: 1.5; opacity: 0.0 }
                },
                // End state for replace exit.
                State {
                    name: "back"
                    PropertyChanges { target: slot; scale: 0.5; opacity: 0.0 }
                },
                // Inactive state.
                State {
                    name: "hidden"
                    PropertyChanges { target: slot; visible: false }
                }
            ]

            transitions: [
                // Pop entry and push exit transition.
                Transition {
                    from: ""; to: "left"; reversible: true
                    SequentialAnimation {
                        ScriptAction { script: if (state == "left") { transitionStarted(); } else { transitionEnded(); } }
                        PropertyAnimation { properties: "x"; easing.type: Easing.InOutExpo; duration: transitionDuration }
                        ScriptAction { script: if (state == "left") { transitionEnded(); } else { transitionStarted(); } }
                    }
                },
                // Push entry and pop exit transition.
                Transition {
                    from: ""; to: "right"; reversible: true
                    SequentialAnimation {
                        ScriptAction { script: if (state == "right") { transitionStarted(); } else { transitionEnded(); } }
                        PropertyAnimation { properties: "x"; easing.type: Easing.InOutExpo; duration: transitionDuration }
                        ScriptAction { script: if (state == "right") { transitionEnded(); } else { transitionStarted(); } }
                    }
                },
                // Replace entry transition.
                Transition {
                    from: "front"; to: "";
                    SequentialAnimation {
                        ScriptAction { script: transitionStarted(); }
                        PropertyAnimation { properties: "scale,opacity"; easing.type: Easing.InOutExpo; duration: transitionDuration }
                        ScriptAction { script: transitionEnded(); }
                    }
                },
                // Replace exit transition.
                Transition {
                    from: ""; to: "back";
                    SequentialAnimation {
                        ScriptAction { script: transitionStarted(); }
                        PropertyAnimation { properties: "scale,opacity"; easing.type: Easing.InOutExpo; duration: transitionDuration }
                        ScriptAction { script: transitionEnded(); }
                   }
                }
            ]

            // Called when the slot gets destroyed.
            Component.onDestruction: {
                if (state == "") {
                    // the page is active - deactivate it
                    if (root.visible) {
                        __emitPageLifecycleSignal(page, "deactivating");
                        __emitPageLifecycleSignal(page, "deactivated");
                    }
                }
                if (owner != slot && page != null && page != undefined) {
                    // slot is not the owner of the page - re-parent back to original owner
                    page.visible = false;
                    page.parent = owner;
                }
            }

        }
    }
}
