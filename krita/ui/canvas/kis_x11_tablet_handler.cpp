#include "kis_x11_tablet_handler.h"

#include <QLibrary>

KisX11TabletHandler::KisX11TabletHandler()
{
    int ndev,
            i,
            j;
    bool gotStylus,
            gotEraser;
    XDeviceInfo *devices = 0, *devs;
    XInputClassInfo *ip;
    XAnyClassPtr any;
    XValuatorInfoPtr v;
    XAxisInfoPtr a;
    XDevice *dev = 0;

    if (X11->ptrXListInputDevices) {
        devices = X11->ptrXListInputDevices(X11->display, &ndev);
        if (!devices)
            qWarning("QApplication: Failed to get list of tablet devices");
    }
    if (!devices)
        ndev = -1;
    QTabletEvent::TabletDevice deviceType;
    for (devs = devices, i = 0; i < ndev && devs; i++, devs++) {
        dev = 0;
        deviceType = QTabletEvent::NoDevice;
        gotStylus = false;
        gotEraser = false;

        if (devs->type == ATOM(XWacomStylus) || devs->type == ATOM(XTabletStylus)) {
            deviceType = QTabletEvent::Stylus;
            if (wacomDeviceName()->isEmpty())
                wacomDeviceName()->append(devs->name);
            gotStylus = true;
        } else if (devs->type == ATOM(XWacomEraser) || devs->type == ATOM(XTabletEraser)) {
            deviceType = QTabletEvent::XFreeEraser;
            gotEraser = true;
        }

        if (deviceType == QTabletEvent::NoDevice)
            continue;

        if (gotStylus || gotEraser) {
            if (X11->ptrXOpenDevice)
                dev = X11->ptrXOpenDevice(X11->display, devs->id);

            if (!dev)
                continue;

            QTabletDeviceData device_data;
            device_data.deviceType = deviceType;
            device_data.eventCount = 0;
            device_data.device = dev;
            device_data.xinput_motion = -1;
            device_data.xinput_key_press = -1;
            device_data.xinput_key_release = -1;
            device_data.xinput_button_press = -1;
            device_data.xinput_button_release = -1;
            device_data.xinput_proximity_in = -1;
            device_data.xinput_proximity_out = -1;
            device_data.widgetToGetPress = 0;

            if (dev->num_classes > 0) {
                for (ip = dev->classes, j = 0; j < dev->num_classes;
                     ip++, j++) {
                    switch (ip->input_class) {
                    case KeyClass:
                        DeviceKeyPress(dev, device_data.xinput_key_press,
                                       device_data.eventList[device_data.eventCount]);
                        if (device_data.eventList[device_data.eventCount])
                            ++device_data.eventCount;
                        DeviceKeyRelease(dev, device_data.xinput_key_release,
                                         device_data.eventList[device_data.eventCount]);
                        if (device_data.eventList[device_data.eventCount])
                            ++device_data.eventCount;
                        break;
                    case ButtonClass:
                        DeviceButtonPress(dev, device_data.xinput_button_press,
                                          device_data.eventList[device_data.eventCount]);
                        if (device_data.eventList[device_data.eventCount])
                            ++device_data.eventCount;
                        DeviceButtonRelease(dev, device_data.xinput_button_release,
                                            device_data.eventList[device_data.eventCount]);
                        if (device_data.eventList[device_data.eventCount])
                            ++device_data.eventCount;
                        break;
                    case ValuatorClass:
                        // I'm only going to be interested in motion when the
                        // stylus is already down anyway!
                        DeviceMotionNotify(dev, device_data.xinput_motion,
                                           device_data.eventList[device_data.eventCount]);
                        if (device_data.eventList[device_data.eventCount])
                            ++device_data.eventCount;
                        ProximityIn(dev, device_data.xinput_proximity_in, device_data.eventList[device_data.eventCount]);
                        if (device_data.eventList[device_data.eventCount])
                            ++device_data.eventCount;
                        ProximityOut(dev, device_data.xinput_proximity_out, device_data.eventList[device_data.eventCount]);
                        if (device_data.eventList[device_data.eventCount])
                            ++device_data.eventCount;
                    default:
                        break;
                    }
                }
            }

            // get the min/max value for pressure!
            any = (XAnyClassPtr) (devs->inputclassinfo);
            for (j = 0; j < devs->num_classes; j++) {
                if (any->c_class == ValuatorClass) {
                    v = (XValuatorInfoPtr) any;
                    a = (XAxisInfoPtr) ((char *) v +
                                        sizeof (XValuatorInfo));
                    device_data.minX = a[0].min_value;
                    device_data.maxX = a[0].max_value;
                    device_data.minY = a[1].min_value;
                    device_data.maxY = a[1].max_value;
                    device_data.minPressure = a[2].min_value;
                    device_data.maxPressure = a[2].max_value;
                    device_data.minTanPressure = 0;
                    device_data.maxTanPressure = 0;
                    device_data.minZ = 0;
                    device_data.maxZ = 0;

                    // got the max pressure no need to go further...
                    break;
                }
                any = (XAnyClassPtr) ((char *) any + any->length);
            } // end of for loop

            tablet_devices()->append(device_data);
        } // if (gotStylus || gotEraser)
    }
    if (X11->ptrXFreeDeviceList)
        X11->ptrXFreeDeviceList(devices);

    QLibrary wacom(QString::fromLatin1("wacomcfg"), 0); // version 0 is the latest release at time of writing this.
    if (wacom.load()) {
        // NOTE: C casts instead of reinterpret_cast for GCC 3.3.x
        ptrWacomConfigInit = (PtrWacomConfigInit)wacom.resolve("WacomConfigInit");
        ptrWacomConfigOpenDevice = (PtrWacomConfigOpenDevice)wacom.resolve("WacomConfigOpenDevice");
        ptrWacomConfigGetRawParam  = (PtrWacomConfigGetRawParam)wacom.resolve("WacomConfigGetRawParam");
        ptrWacomConfigCloseDevice = (PtrWacomConfigCloseDevice)wacom.resolve("WacomConfigCloseDevice");
        ptrWacomConfigTerm = (PtrWacomConfigTerm)wacom.resolve("WacomConfigTerm");

        if (ptrWacomConfigInit == 0 || ptrWacomConfigOpenDevice == 0 || ptrWacomConfigGetRawParam == 0
                || ptrWacomConfigCloseDevice == 0 || ptrWacomConfigTerm == 0) { // either we have all, or we have none.
            ptrWacomConfigInit = 0;
            ptrWacomConfigOpenDevice = 0;
            ptrWacomConfigGetRawParam  = 0;
            ptrWacomConfigCloseDevice = 0;
            ptrWacomConfigTerm = 0;
        }
    }
}

bool KisX11TabletHandler::x11Event(XEvent *event)
{
    if (!qt_xdnd_dragging) {
        QTabletDeviceDataList *tablets = qt_tablet_devices();
        for (int i = 0; i < tablets->size(); ++i) {
            QTabletDeviceData &tab = tablets->operator [](i);
            if (event->type == tab.xinput_motion
            || event->type == tab.xinput_button_release
            || event->type == tab.xinput_button_press
            || event->type == tab.xinput_proximity_in
            || event->type == tab.xinput_proximity_out) {
                widget->translateXinputEvent(event, &tab);
                return 0;
            }
        }
    }

}

//
// XInput Translation Event
//
void fetchWacomToolId(int &deviceType, qint64 &serialId)
{
    if (ptrWacomConfigInit == 0) // we actually have the lib
        return;
    WACOMCONFIG *config = ptrWacomConfigInit(X11->display, 0);
    if (config == 0)
        return;
    WACOMDEVICE *device = ptrWacomConfigOpenDevice (config, wacomDeviceName()->constData());
    if (device == 0)
        return;
    unsigned keys[1];
    int serialInt;
    ptrWacomConfigGetRawParam (device, XWACOM_PARAM_TOOLSERIAL, &serialInt, 1, keys);
    serialId = serialInt;
    int toolId;
    ptrWacomConfigGetRawParam (device, XWACOM_PARAM_TOOLID, &toolId, 1, keys);
    switch(toolId) {
    case 0x007: /* Mouse 4D and 2D */
    case 0x017: /* Intuos3 2D Mouse */
    case 0x094:
    case 0x09c:
        deviceType = QTabletEvent::FourDMouse;
        break;
    case 0x096: /* Lens cursor */
    case 0x097: /* Intuos3 Lens cursor */
        deviceType = QTabletEvent::Puck;
        break;
    case 0x0fa:
    case 0x81b: /* Intuos3 Classic Pen Eraser */
    case 0x82a: /* Eraser */
    case 0x82b: /* Intuos3 Grip Pen Eraser */
    case 0x85a:
    case 0x91a:
    case 0x91b: /* Intuos3 Airbrush Eraser */
    case 0xd1a:
        deviceType = QTabletEvent::XFreeEraser;
        break;
    case 0x112:
    case 0x912:
    case 0x913: /* Intuos3 Airbrush */
    case 0xd12:
        deviceType = QTabletEvent::Airbrush;
        break;
    case 0x012:
    case 0x022:
    case 0x032:
    case 0x801: /* Intuos3 Inking pen */
    case 0x812: /* Inking pen */
    case 0x813: /* Intuos3 Classic Pen */
    case 0x822: /* Pen */
    case 0x823: /* Intuos3 Grip Pen */
    case 0x832: /* Stroke pen */
    case 0x842:
    case 0x852:
    case 0x885: /* Intuos3 Marker Pen */
    default: /* Unknown tool */
        deviceType = QTabletEvent::Stylus;
    }

    /* Close device and return */
    ptrWacomConfigCloseDevice (device);
    ptrWacomConfigTerm(config);
}

struct qt_tablet_motion_data
{
    bool filterByWidget;
    const QWidget *widget;
    const QWidget *etWidget;
    int tabletMotionType;
    bool error; // found a reason to stop searching
};

static Bool qt_mouseMotion_scanner(Display *, XEvent *event, XPointer arg)
{
    qt_tablet_motion_data *data = (qt_tablet_motion_data *) arg;
    if (data->error)
        return false;

    if (event->type == MotionNotify)
        return true;

    data->error = event->type != data->tabletMotionType; // we stop compression when another event gets in between.
    return false;
}

static Bool qt_tabletMotion_scanner(Display *, XEvent *event, XPointer arg)
{
    qt_tablet_motion_data *data = (qt_tablet_motion_data *) arg;
    if (data->error)
        return false;
    if (event->type == data->tabletMotionType) {
        const XDeviceMotionEvent *const motion = reinterpret_cast<const XDeviceMotionEvent*>(event);
        if (data->filterByWidget) {
            const QPoint curr(motion->x, motion->y);
            const QWidget *w = data->etWidget;
            const QWidget *const child = w->childAt(curr);
            if (child) {
                w = child;
            }
            if (w == data->widget)
                return true;
        } else {
            return true;
        }
    }

    data->error = event->type != MotionNotify; // we stop compression when another event gets in between.
    return false;
}

bool translateXinputEvent(const XEvent *ev, QTabletDeviceData *tablet)
{
    Q_ASSERT(tablet != 0);

    QWidget *w = this;
    QPoint global,
        curr;
    QPointF hiRes;
    qreal pressure = 0;
    int xTilt = 0,
        yTilt = 0,
        z = 0;
    qreal tangentialPressure = 0;
    qreal rotation = 0;
    int deviceType = QTabletEvent::NoDevice;
    int pointerType = QTabletEvent::UnknownPointer;
    const XDeviceMotionEvent *motion = 0;
    XDeviceButtonEvent *button = 0;
    const XProximityNotifyEvent *proximity = 0;
    QEvent::Type t;
    Qt::KeyboardModifiers modifiers = 0;
    XID device_id;

    if (ev->type == tablet->xinput_motion) {
        motion = reinterpret_cast<const XDeviceMotionEvent*>(ev);
        t = QEvent::TabletMove;
        global = QPoint(motion->x_root, motion->y_root);
        curr = QPoint(motion->x, motion->y);
        device_id = motion->deviceid;
    } else if (ev->type == tablet->xinput_button_press || ev->type == tablet->xinput_button_release) {
        if (ev->type == tablet->xinput_button_press) {
            t = QEvent::TabletPress;
        } else {
            t = QEvent::TabletRelease;
        }
        button = (XDeviceButtonEvent*)ev;

        global = QPoint(button->x_root, button->y_root);
        curr = QPoint(button->x, button->y);
        device_id = button->deviceid;
    } else { // Proximity
        if (ev->type == tablet->xinput_proximity_in)
            t = QEvent::TabletEnterProximity;
        else
            t = QEvent::TabletLeaveProximity;
        proximity = (const XProximityNotifyEvent*)ev;
        device_id = proximity->deviceid;
    }

    qint64 uid = 0;

    QTabletDeviceDataList *tablet_list = qt_tablet_devices();
    for (int i = 0; i < tablet_list->size(); ++i) {
        const QTabletDeviceData &t = tablet_list->at(i);
        if (device_id == static_cast<XDevice *>(t.device)->device_id) {
            deviceType = t.deviceType;
            if (t.deviceType == QTabletEvent::XFreeEraser) {
                deviceType = QTabletEvent::Stylus;
                pointerType = QTabletEvent::Eraser;
            } else if (t.deviceType == QTabletEvent::Stylus) {
                pointerType = QTabletEvent::Pen;
            }
            break;
        }
    }

    fetchWacomToolId(deviceType, uid);

    QRect screenArea = qApp->desktop()->rect();
    if (motion) {
        xTilt = (short) motion->axis_data[3];
        yTilt = (short) motion->axis_data[4];
        rotation = ((short) motion->axis_data[5]) / 64.0;
        pressure = (short) motion->axis_data[2];
        modifiers = X11->translateModifiers(motion->state);
        hiRes = tablet->scaleCoord(motion->axis_data[0], motion->axis_data[1],
                                    screenArea.x(), screenArea.width(),
                                    screenArea.y(), screenArea.height());
    } else if (button) {
        xTilt = (short) button->axis_data[3];
        yTilt = (short) button->axis_data[4];
        rotation = ((short) button->axis_data[5]) / 64.0;
        pressure = (short) button->axis_data[2];
        modifiers = X11->translateModifiers(button->state);
        hiRes = tablet->scaleCoord(button->axis_data[0], button->axis_data[1],
                                    screenArea.x(), screenArea.width(),
                                    screenArea.y(), screenArea.height());
    } else if (proximity) {
        pressure = 0;
        modifiers = 0;
    }
    if (deviceType == QTabletEvent::Airbrush) {
        tangentialPressure = rotation;
        rotation = 0.;
    }

    if (tablet->widgetToGetPress) {
        w = tablet->widgetToGetPress;
    } else {
        QWidget *child = w->childAt(curr);
        if (child)
            w = child;
    }
    curr = w->mapFromGlobal(global);

    if (t == QEvent::TabletPress) {
        tablet->widgetToGetPress = w;
    } else if (t == QEvent::TabletRelease && tablet->widgetToGetPress) {
        w = tablet->widgetToGetPress;
        curr = w->mapFromGlobal(global);
        tablet->widgetToGetPress = 0;
    }

    QTabletEvent e(t, curr, global, hiRes,
                   deviceType, pointerType,
                   qreal(pressure / qreal(tablet->maxPressure - tablet->minPressure)),
                   xTilt, yTilt, tangentialPressure, rotation, z, modifiers, uid);
    if (proximity) {
        QApplication::sendSpontaneousEvent(qApp, &e);
    } else {
        QApplication::sendSpontaneousEvent(w, &e);
        const bool accepted = e.isAccepted();
        if (!accepted && ev->type == tablet->xinput_motion) {
            // If the widget does not accept tablet events, we drop the next ones from the event queue
            // for this widget so it is not overloaded with the numerous tablet events.
            qt_tablet_motion_data tabletMotionData;
            tabletMotionData.tabletMotionType = tablet->xinput_motion;
            tabletMotionData.widget = w;
            tabletMotionData.etWidget = this;
            // if nothing is pressed, the events are filtered by position
            tabletMotionData.filterByWidget = (tablet->widgetToGetPress == 0);

            bool reinsertMouseEvent = false;
            XEvent mouseMotionEvent;
            while (true) {
                // Find first mouse event since we expect them in pairs inside Qt
                tabletMotionData.error =false;
                if (XCheckIfEvent(X11->display, &mouseMotionEvent, &qt_mouseMotion_scanner, (XPointer) &tabletMotionData)) {
                    reinsertMouseEvent = true;
                } else {
                    break;
                }

                // Now discard any duplicate tablet events.
                tabletMotionData.error = false;
                XEvent dummy;
                while (XCheckIfEvent(X11->display, &dummy, &qt_tabletMotion_scanner, (XPointer) &tabletMotionData)) {
                    // just discard the event
                }
            }

            if (reinsertMouseEvent) {
                XPutBackEvent(X11->display, &mouseMotionEvent);
            }
        }
    }
    return true;
}

