# Jakilid
a encrypted password manager

![Jakilid Demo](resource/jakilid-demo.gif)

## Features
- 256-bit AES encryption
- Firefox addon (needs upgrade)
- Flexible user interface
- Multi languages support

## Build and run
Backend of this project has been developed using Qt4 which recently was ported to Qt5. The user interface is developed using Vanilla JS combined with various technologies and frameworks. Some of them are in below list.
- [Qt framework](https://doc.qt.io/qt-5/opensourcelicense.html)
- [SQLCipher](https://www.zetetic.net/sqlcipher/license/)
- jQuery UI
- Websockets
- JSONP
- Pure CSS framework

For building, you need to create SQLCipher driver for Qt.
Project creates two executables. `JakilidService` is the service which provides functionalities to ui. `Jakilid` is the main user interface app.
`create-addon.bat` creates the Firefox addon.

## TODO
- Port translator to Qt5