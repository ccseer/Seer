#-------------------------------------------------
#
# Project created by QtCreator 2015-08-18T10:36:05
#
#-------------------------------------------------

QT       += core gui xml network
QT += quick
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = Seer
TEMPLATE = app
CONFIG +=C++11
QT += winextras
QT += webenginewidgets

#DEFINES += Q_COMPILER_INITIALIZER_LISTS

VERSION=1.5.0
DEFINES += VERSION=\\\"$$VERSION\\\"

OBJECTS_DIR = C:\Users\Corey\Documents\Trash\seer_hole
MOC_DIR =   C:\Users\Corey\Documents\Trash\seer_hole
UI_DIR =   C:\Users\Corey\Documents\Trash\seer_hole
RCC_DIR =   C:\Users\Corey\Documents\Trash\seer_hole

#翻译
TRANSLATIONS+=  C:\Users\Corey\OneDrive\TODO\github\Seer\translation\translation_source\zh_CN.ts   \
                C:\Users\Corey\OneDrive\TODO\github\Seer\translation\translation_source\Italian.ts \
                C:\Users\Corey\OneDrive\TODO\github\Seer\translation\translation_source\Spanish.ts \
                C:\Users\Corey\OneDrive\TODO\github\Seer\translation\translation_source\Swedish.ts \
                C:\Users\Corey\OneDrive\TODO\github\Seer\translation\translation_source\German.ts  \
                C:\Users\Corey\OneDrive\TODO\github\Seer\translation\translation_source\Japanese.ts  \
                C:\Users\Corey\OneDrive\TODO\github\Seer\translation\translation_source\Russian.ts \
                C:\Users\Corey\OneDrive\TODO\github\Seer\translation\translation_source\French.ts \
                C:\Users\Corey\OneDrive\TODO\github\Seer\translation\translation_source\zh_TW.ts
lupdate_only{
    SOURCES = res/qml/*.qml
}


INCLUDEPATH += src helper helper/clib helper/clib/ui src/ui src/settings helper/cparentdlg src/media src/html src/picture src/dirs src/unknown src/pdf src/code libs


SOURCES += main.cpp \
    helper/clib/ccomponent.cpp \
    helper/cparentdlg/cconfirmdlg.cpp \
    helper/cparentdlg/cmsgbox.cpp \
    helper/cparentdlg/cparentdlg.cpp \
    helper/clib/ui/ccomboboxdelegate4separator.cpp \
    helper/cparentdlg/cwndshadoweffecthelper.cpp \
    src/settings/csettingsdlg.cpp \
    src/ckeyboardhookcontroler.cpp \
    src/cmain.cpp \
    src/picture/cpicturemanager.cpp \
    src/media/cplayprocess.cpp \
    src/extensions.cpp \
    src/cglobaldata.cpp \
    src/picture/cpicwnd.cpp \
    src/media/cmediawnd.cpp \
    src/dirs/cdirmaneger.cpp \
    src/dirs/cdirwnd.cpp \
    src/cabstractmanager.cpp \
    src/unknown/cunknownwnd.cpp \
    src/pdf/cpdfwnd.cpp \
    src/code/ccodewnd.cpp \
    src/html/chtmlmanager.cpp \
    src/html/chtmlwnd.cpp \
    src/html/cwebpage.cpp \
    src/settings/csettingswnd4filetype.cpp \
    src/settings/csettingswnd4general.cpp \
    src/settings/csettingswnd4plugins.cpp \
    src/settings/csettingswnd4pluginsadd.cpp \
    src/settings/settingshelper.cpp \
    src/settings/csettingswnd4key.cpp \
    src/dirs/cfilesystemhorizontalwnd.cpp \
    src/picture/cimageview.cpp \
    src/cpluginshelper.cpp \
    src/settings/csettingswnd4explorer.cpp \
    src/ctranslationini.cpp \
    src/settings/csettingswnd4controls.cpp \
    src/cctrlsbarapp.cpp \
    src/utilities.cpp \
    src/picture/cgraphicspixmapitemwithbg.cpp \
    src/dirs/calignedmodel.cpp \
    src/dirs/cdirmodeldatahelper.cpp \
    src/dirs/cdiritem.cpp \
    src/dirs/cdirtreeview.cpp \
    src/dirs/cfolderdatareader.cpp \
    src/picture/cimgreader.cpp \
    src/settings/csettingswnd4pluginsavailable.cpp \
    helper/clib/ui/cclickcheckinglabel.cpp \
    src/ccommandhandler.cpp \
    src/settings/csettingsdlgnavbtn.cpp \
    src/settings/cflagdelegate.cpp \
    src/settings/csettingswnd4controlsedit.cpp \
    helper/clib/ui/cclipboardhelper.cpp \
    helper/clib/ui/ctooltipmenu.cpp \
    helper/clib/ui/cnodashlinedelegate.cpp \
    src/ui/caboutwnd.cpp \
    src/ui/cabstractwnd.cpp \
    src/ui/ccontrolwnd.cpp \
    src/ui/cfileproperty.cpp \
    src/ui/cmainwnd.cpp \
    src/ui/cmainwndex.cpp \
    src/ui/cregdlg.cpp \
    src/ui/csystemtrayicon.cpp \
    src/ui/ctableviewwithcopykey.cpp \
    helper/clib/ui/cfiledialog.cpp \
    src/cthememanager.cpp \
    src/code/qscilexerplain.cpp \
    helper/clib/cscreenadapter.cpp \
    src/ui/cmonitorchecker.cpp


HEADERS  += \
    helper/clib/ccomponent.h \
    helper/clib/csingleton.h \
    helper/clib/stable.h \
    helper/cparentdlg/cconfirmdlg.h \
    helper/cparentdlg/cmsgbox.h \
    helper/cparentdlg/cparentdlg.h \
    helper/cparentdlg/cwndshadoweffecthelper.h \
    helper/clib/ui/ccomboboxdelegate4separator.h \
    src/settings/csettingsdlg.h \
    helper/clib/cappcrashhander.h \
    src/ckeyboardhookcontroler.h \
    src/cmain.h \
    src/picture/cpicturemanager.h \
    src/media/cplayprocess.h \
    src/extensions.h \
    src/cglobaldata.h \
    src/picture/cpicwnd.h \
    src/media/cmediawnd.h \
    src/dirs/cdirmaneger.h \
    src/dirs/cdirwnd.h \
    src/cabstractmanager.h \
    src/unknown/cunknownwnd.h \
    src/pdf/cpdfwnd.h \
    src/code/ccodewnd.h \
    src/html/chtmlmanager.h \
    src/html/chtmlwnd.h \
    src/html/cwebpage.h \
    src/settings/csettingswnd4filetype.h \
    src/settings/csettingswnd4general.h \
    src/settings/csettingswnd4plugins.h \
    src/settings/csettingswnd4pluginsadd.h \
    src/settings/settingshelper.h \
    src/types.h \
    src/settings/csettingswnd4key.h \
    src/dirs/cfilesystemhorizontalwnd.h \
    src/picture/cimageview.h \
    src/cpluginshelper.h \
    src/settings/csettingswnd4explorer.h \
    src/ctranslationini.h \
    src/settings/csettingswnd4controls.h \
    src/cctrlsbarapp.h \
    src/utilities.h \
    src/picture/cgraphicspixmapitemwithbg.h \
    src/dirs/calignedmodel.h \
    src/dirs/cdirmodeldatahelper.h \
    src/dirs/cdiritem.h \
    src/dirs/cdirtreeview.h \
    src/dirs/cfolderdatareader.h \
    src/picture/cimgreader.h \
    src/settings/csettingswnd4pluginsavailable.h \
    helper/clib/ui/cclickcheckinglabel.h \
    src/ccommandhandler.h \
    src/settings/csettingsdlgnavbtn.h \
    src/settings/cflagdelegate.h \
    src/settings/csettingswnd4controlsedit.h \
    helper/clib/ui/cclipboardhelper.h \
    helper/clib/ui/ctooltipmenu.h \
    global_headers.h \
    helper/clib/ui/cnodashlinedelegate.h \
    src/ui/caboutwnd.h \
    src/ui/cabstractwnd.h \
    src/ui/ccontrolwnd.h \
    src/ui/cfileproperty.h \
    src/ui/cmainwnd.h \
    src/ui/cmainwndex.h \
    src/ui/cregdlg.h \
    src/ui/csystemtrayicon.h \
    src/ui/ctableviewwithcopykey.h \
    helper/clib/ui/cfiledialog.h \
    src/cthememanager.h \
    src/code/qscilexerplain.h \
    helper/clib/cscreenadapter.h \
    src/ui/cmonitorchecker.h


FORMS    += \
    helper/cparentdlg/cparentdlg.ui \
    src/settings/csettingsdlg.ui \
    src/dirs/cfilesystemhorizontalwnd.ui \
    src/settings/csettingswnd4filetype.ui \
    src/settings/csettingswnd4general.ui \
    src/settings/csettingswnd4plugins.ui \
    src/settings/csettingswnd4pluginsadd.ui \
    src/settings/csettingswnd4key.ui \
    src/settings/csettingswnd4explorer.ui \
    src/settings/csettingswnd4controls.ui \
    src/settings/csettingswnd4pluginsavailable.ui \
    src/settings/csettingsdlgnavbtn.ui \
    src/settings/csettingswnd4controlsedit.ui \
    src/ui/caboutwnd.ui \
    src/ui/cregdlg.ui

RESOURCES += \
    res/resource.qrc

DISTFILES += \
    seer.rc

RC_FILE = seer.rc

DEFINES += HOOK
include(src/explorers/explorers.pri)
