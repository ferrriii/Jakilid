TEMPLATE = subdirs

SUBDIRS += domain \
    dataaccess \
    application \
    ui \
    test \
    tool

CONFIG  +=  ordered

#UI.depends = Domain
