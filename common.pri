TREEROOT = $$PWD

# change following paths if you want
# where library header files are being copied into their own subdirectory (make install)
INSTALLPATH = $$TREEROOT/include
# where libraries are being copied
LIBSPATH    = $$TREEROOT/lib/
# where are executables are being created
BINPATH     = $$TREEROOT/bin
# where qt unit tests executables are being created. with make check command, tests will be executed automatically when compile time
TESTPATH    = $$TREEROOT/bin/test


# JAKILID specific
DEFINES += JAKILID_VERSION=\\\"1.1\\\"
DEFINES += "JAKILID_DB_VERSION_NUMBER=0x02"


win32:INSTALLPATH = $$replace(INSTALLPATH,/,\\)
INCLUDEPATH += $$INSTALLPATH

contains(TEMPLATE,lib){
    headers.path    = $$INSTALLPATH/$$TARGET/
    headers.files   = $$HEADERS
    INSTALLS       += headers
    DESTDIR         = $$LIBSPATH

    #This option enables qmake to track these dependencies. When this option is enabled, qmake will create a file
    #ending in .prl which will save meta-information about the library (see Library Dependencies for more info).
    #create_prl is required when building a static library
    CONFIG += create_prl

    #copying library headers into install path
    QMAKE_POST_LINK  += mkdir $$INSTALLPATH & mkdir $$INSTALLPATH\\$$TARGET &
    for(hdr, HEADERS) {
        hdr = $$replace(hdr,/,\\)
        hdrdir  =   $$dirname(hdr)
        !contains($$hdrdir,..) {
            QMAKE_POST_LINK  += copy $$replace(_PRO_FILE_PWD_,/,\\)\\$$hdr $$INSTALLPATH\\$$TARGET\\$$hdr /Y &
        }
    }

} else:contains(TEMPLATE,app) {
    #When this is enabled, qmake will process all libraries linked to by the application and find their
    #meta-information(see Library Dependencies for more info).
    #link_prl is required when using a static library.
    CONFIG  += link_prl
    LIBS    += -L$$LIBSPATH
    DESTDIR  = $$BINPATH

    contains(QT,testlib) {
        CONFIG += testcase
        DESTDIR = $$TESTPATH
    }


    # this will add dependcy to libraries which executable uses. If any changes in LIBS occur they will get build again
    for(libFile, LIBS) {
        libFile =   $$replace(libFile,-l,lib)
        libFile =   $$join(libFile,,$$LIBSPATH,.a)
        exists($$libFile) {
            PRE_TARGETDEPS  +=  $$libFile
        }
    }
}
