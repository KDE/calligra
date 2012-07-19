import QtQuick 1.1
import "components"

PageStack {
    width: 1024;
    height: 600;

    initialPage: welcomePage;

    transitionDuration: 500;

    Component { id: welcomePage; WelcomePage { } }
}