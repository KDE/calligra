/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Components project on Qt Labs.
**
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions contained
** in the Technology Preview License Agreement accompanying this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
****************************************************************************/

// Page stack - items are page slots.
var pageStack = [];

// Returns the page stack depth.
function getDepth() {
    return pageStack.length;
}

// Pushes a page on the stack.
// The page can be defined as a component or an item.
// If an item is used then the page will get re-parented.
// Returns the page instance.
function push(page, replace, immediate) {
    // figure out if more than one page is being pushed
    var pages;
    if (page instanceof Array) {
        pages = page;
        page = pages.pop();
    }

    // get the current slot
    var oldSlot = pageStack[pageStack.length - 1];

    // pop the old slot off the stack if this is a replace
    if (oldSlot && replace) {
        pageStack.pop();
    }

    // push any extra defined pages onto the stack
    if (pages) {
        var i;
        for (i = 0; i < pages.length; i++) {
            pageStack.push(createSlot(pages[i]));
        }
    }

    // create a new page slot parented in the page stack
    var slot = createSlot(page);

    // push the page slot onto the stack
    pageStack.push(slot);

    depth = pageStack.length;
    currentPage = slot.page;

    // perform page transition
    immediate = immediate || !oldSlot;
    if (oldSlot) {
        oldSlot.pushExit(replace, immediate);
    }
    slot.pushEnter(replace, immediate);

    return slot.page;
}

// Creates a page slot.
function createSlot(page) {
    var slot = slotComponent.createObject(root);
    if (page.createObject) {
        // page defined as component - instantiate it
        // the slot itself is the owner of the page instance
        page = page.createObject(slot);
        slot.page = page;
        slot.owner = slot;
    } else {
        // page defined as item - reparent to slot
        // the current parent of the page is the owner of the page
        slot.page = page;
        slot.owner = page.parent;
        page.parent = slot;
    }
    return slot;
}

// Pops a page off the stack.
// If page is specified then the stack is unwound to that page.
// Returns the page instance that was popped off the stack.
function pop(page, immediate) {
    // make sure there are enough pages in the stack to pop
    if (pageStack.length > 1) {
        // pop the current slot off the stack and get the next slot
        var oldSlot = pageStack.pop();
        var slot = pageStack[pageStack.length - 1];
        if (page) {
            // an unwind target has been specified - pop until we find it
            while (slot.page != page && pageStack.length > 1) {
                slot.destroy();
                pageStack.pop();
                slot = pageStack[pageStack.length - 1];
            }
        }

        depth = pageStack.length;
        currentPage = slot.page;

        // perform page transition
        oldSlot.popExit(immediate);
        slot.popEnter(immediate);

        return oldSlot.page;
    } else {
        return null;
    }
}

// Clears the page stack.
function clear() {
    var slot;
    while (slot = pageStack.pop()) {
        slot.destroy();
    }
}

