#ifndef CSETTINGSHELPER_H
#define CSETTINGSHELPER_H

#include "csingleton.h"
#include "utilities.h"

#include <QSettings>
#include <QVariant>

#define REG_AUTO_RUN \
    "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"

// app settings text
namespace Ini {
//            name  _ return type

// app
const QString autorun_b            = "autorun";
const QString auto_update_b        = "auto_update";
const QString openfile_by_dclick_b = "open_with_default";
const QString use_hook_key_close_b = "use_trigger_key_close";
const QString tracking_file_b      = "tracking_file";

// for speed
const QString key_shift_b = "key_shift";
const QString key_alt_b   = "key_alt";
const QString key_ctrl_b  = "key_ctrl";

const QString key_shift_f_b = "key_shift_f";
const QString key_alt_f_b   = "key_alt_f";
const QString key_ctrl_f_b  = "key_ctrl_f";

//
const QString i18n_key_str = "current_lan";

// pdf
const QString group_pdf                = "PDF";
const QString pdf_is_actualsize_view_b = "is_actual_size_view";
// QList<QVariantList>  < path,pos>
const QString pdf_recents_list = "pdf_recents";

// text
const QString group_text        = "Text";
const QString text_wrap_b       = "wrap";
const QString text_line_num_b   = "line_num";
const QString text_support_list = "extra_support";
const QString text_zoom_size_i  = "zoom_size";

// media
const QString group_media  = "Media";
const QString media_loop_b = "repeat";
const QString media_time_b = "time_indication";
const QString media_mute_b = "mute";
// QList<QVariantList>  < path,pos>
const QString media_recents_list = "media_recents";

// Folder
const QString group_folder          = "Folder";
const QString is_folder_treeview_b  = "is_treeview";
const QString is_dir_load_contens_b = "load_dir_contens";

// WEB
const QString group_web       = "WebView";
const QString web_zoom_size_d = "web_zoom";

// Others
const QString group_others              = "Others";
const QString plugins_data_bytes        = "plugins_data";
const QString plugins_data_all_bytes    = "plugins_all_data";
const QString version_str               = "version";
const QString app_path_str              = "app_path";
const QString save_temp_folder_b        = "save_temp_folder";
const QString expand_details_property_b = "expand_details_property";
const QString detect_type_b             = "type_detecting";
const QString app_font_str              = "app_font";
const QString stay_on_top_b             = "stay_on_top";
const QString theme_name_i              = "theme_name";
const QString markdown_css_path_str     = "markdown_css_path";
// bae8cae3ba06e8a7c3e534731e6708474e9a4619108b26a4a5a1f24966e2972d1c054f4489c5aff5ebb1d35ae741e37e59ab5061b8dd0bf1b7d59faa9ae892db
const QString license_uid_str = Encrypt("license uid");
// a1d565e3ffffed5b448632d8dea611513098ef820071e5f7530f4d3c67f78edc0ab3d45cbdba21858a7a65d3f48c00a7d64419807216c18153db443dec8518bb
const QString license_key_str       = Encrypt("license key");
const QString license_time_ll       = Encrypt("license time");
const QString is_keep_wnd_sz_pos_b  = "keep_wnd_sz_pos";
const QString tray_icon_hide_b      = "hide_tray_icon";
const QString tray_icon_use_white_b = "use_white_tray_icon";

// Controls
const QString group_ctrls                      = "Controls";
const QString ctrls_app_list                   = "open_with";
const QString is_show_open_with_default_btn_b  = "open_with_default_btn";
const QString is_show_reveal_in_explorer_btn_b = "reveal_in_explorer_btn";

// Keyboard
const QString group_keyboard             = "Keyboard";
const QString key_get_focus_str          = "get_focus";
const QString key_property_str           = "property";
const QString key_reveal_in_explorer_str = "reveal_in_explorer";
const QString key_open_with_def_str      = "open_with_def";
const QString key_maximum_str            = "maximum_restore_window";
const QString key_reload_str             = "reload";

const QString key_folder_open_str        = "folder_open_with_def";
const QString key_folder_open_seer_str   = "folder_open_with_seer";
const QString key_folder_reveal_str      = "folder_reveal";
const QString key_folder_tree_view_str   = "folder_tree_view";
const QString key_folder_column_view_str = "folder_column_view";

const QString key_img_rotate_l_str  = "img_rotate_left";
const QString key_img_rotate_r_str  = "img_rotate_right";
const QString key_img_actual_sz_str = "img_actual_size";

const QString key_media_forward_str         = "media_forward";
const QString key_media_backward_str        = "media_backward";
const QString key_media_play_str            = "media_play_pause";
const QString key_media_loop_str            = "media_loop";
const QString key_media_time_indication_str = "media_time_indication";
const QString key_media_mute_str            = "media_mute";
const QString key_media_go_start_str        = "media_go_start";

const QString key_pdf_actual_sz_str = "pdf_actual_size";
const QString key_pdf_fit_width_str = "pdf_fit_width";
}
#define g_pini CSingleton<CSettingsHelper>::Instance()
class CSettingsHelper {
public:
    CSettingsHelper();
    QVariant GetIni(const QString& key);
    void SetIni(const QString& key, const QVariant& val);

    QVariant GET(const QString& key,
                 const QString& grp,
                 const QHash<QString, QVariant>& hash);

    QVariantHash& GetKeyGrounpVal()
    {
        return m_keys;
    }

private:
    QVariantHash m_text;
    QVariantHash m_media;
    QVariantHash m_folder;
    QVariantHash m_others;
    QVariantHash m_ctrls;
    QVariantHash m_keys;
    QVariantHash m_pdfs;
    QVariantHash m_web;
    QVariantHash m_app;

    QSettings m_settings;
};

#endif  // CSETTINGSHELPER_H
