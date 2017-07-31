#ifndef CGLOBALDATA_H
#define CGLOBALDATA_H

#include <QApplication>
#include <QList>
#include <QMenu>
#include <QSize>
#include "cctrlsbarapp.h"
#include "csingleton.h"
#include "extensions.h"
#include "qscreen.h"

/***************  values  *********************/

#define AppName_char "Seer"
#define AppName_charL L"Seer"
#define theAppName QObject::tr(AppName_char)
#define web_home "http://1218.io"
#define web_relnote_all "http://1218.io/seer/release-note.html"
#define web_controlbar "http://1218.io/seer/control-bar.html"
#define web_plugin "http://1218.io/seer/plug-in.html"
#define web_update "http://1218.io/app/updates.xml"
#define web_order "http://1218.io/seer/order.html"
#define web_translator "http://1218.io/seer/translators.html"
#define web_designer_website "https://dribbble.com/tangxiangle"
#define web_corey_weibo "http://weibo.com/idealistcorey"
#define web_corey_twitter "https://twitter.com/idealistcorey"
#define web_found_bug "http://github.com/ccseer/Seer"
#define web_plugin_update                                                   \
    "https://raw.githubusercontent.com/ccseer/Seer-plugins/master/release/" \
    "all_v2.json"

#define g_path_help "://help.jpg"

// code
#define g_code_max_t 1024 * 16

// media
#define g_media_recents_max 30

// pdf
#define g_pdf_recents_max 30

// img
#define g_img_max_read_sz 1024 * 1024 * 50

// plugin
#define g_plugin_output_placeholder QString("*SEER_OUTPUT_PATH*")
#define g_plugin_input_placeholder QString("*SEER_INPUT_PATH*")

#define g_plugin_dir qApp->applicationDirPath() + "/plugins/"
#define g_plugin_path_rar g_plugin_dir + "unrar.exe"
#define g_plugin_path_7z g_plugin_dir + "7za.exe"

#define g_name_helper QString("helper.exe")

// CFileProperty
#define g_pp_details QT_TRANSLATE_NOOP("CFileProperty", "Details")

// font_img
namespace fa {
enum icon : int {
    music   = 0xE405,
    arrow_r = 0xE409,
    arrow_d = 0xE313,

    drop_down = 0xE5C5,
    drop_up   = 0xE5C7,

    // controls
    ctrl_pagewidth  = 0xE8D4,
    ctrl_width_blue = 0xE915,

    ctrl_pause = 0xE034,
    ctrl_play  = 0xE037,
    ctrl_mute  = 0xE04F,

    ctrl_code = 0xE86F,
    ctrl_html = 0xE86F,

    // open with default
    ctrl_open_file = 0xE89D,
    //  "2C8"
    ctrl_reveal_file = 0xE89E,
    // open with Seer
    ctrl_seer_file = 0xE417,

    ctrl_folder_column = 0xE8EC,
    ctrl_folder_tree   = 0xE8D2,

    // image rotate right
    ctrl_right = 0xE41A,
    // title bar
    info  = 0xE645,
    close = 0xE5CD,

    // settings
    ini_advanced = 0xE87A,
    ini_controls = 0xE429,
    ini_general  = 0xE8B8,
    ini_keyboard = 0xE312,
    ini_plugins  = 0xE87B,
    ini_type     = 0xE873,
    // feedback_blue
    question   = 0xE887,
    ini_add    = 0xE145,
    ini_edit   = 0xE254,
    ini_trash  = 0xE872,
    ini_choose = 0xE86C,

    face_no_happy = 0xE811,

    confirm = 0xE876,
    // close
    cancel = 0xE5CD,

    // controls
    cb_checked   = 0xE834,
    cb_unchecked = 0xE835,

    // qml
    premium = 0xE8D0,
};

const QString FontMaterial = "Material Icons";

QChar c(fa::icon i);
QFont font(int sz);
QPixmap img(fa::icon id, int sz, const QColor& c);
QPixmap img_choose(fa::icon id, int sz, const QColor& color);
};  // namespace fa

//////////////////////////////////////////////////////////////////////////
#define g_data CSingleton<CGlobalData>::Instance()
struct CGlobalData {
    CGlobalData();

    QScreen* screen;
    QString path;
    QString suffix;
    DisplayType type;
    FileSubType sub_type;
    CExtensions extensions;

    PluginsList plugins;
    CCtrlsBarHelper controls;

    // fixed size
    bool is_resized_by_user;
    bool program_resizing;
};

#endif  // CGLOBALDATA_H
