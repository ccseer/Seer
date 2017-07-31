#include "settingshelper.h"
#include "ccomponent.h"
#include "stable.h"
#include "types.h"

#include <QFont>
#include "QDateTime"
#include "qjsonarray.h"

#define Get(k, grp, hash)                              \
    if (hash.contains(k)) {                            \
        m_settings.beginGroup(grp);                    \
        auto res = m_settings.value(k, hash.value(k)); \
        m_settings.endGroup();                         \
        return res;                                    \
    \
}

#define Set(k, val, grp, hash)       \
    if (hash.contains(k)) {          \
        m_settings.beginGroup(grp);  \
        m_settings.setValue(k, val); \
        m_settings.endGroup();       \
        return;                      \
    \
}

QVariant CSettingsHelper::GetIni(const QString &key)
{
    using namespace Ini;
    if (m_app.contains(key)) {
        return m_settings.value(key, m_app.value(key));
    }

    Get(key, group_text, m_text) Get(key, group_media, m_media)
        Get(key, group_folder, m_folder) Get(key, group_ctrls, m_ctrls)
            Get(key, group_keyboard, m_keys) Get(key, group_others, m_others)
                Get(key, group_pdf, m_pdfs) Get(key, group_web, m_web)

                    if (key == i18n_key_str)
    {
        return m_settings.value(key);
    }
    elprint << "傻傻傻傻傻傻傻傻傻傻B" << key;
    return m_settings.value(key);
}

void CSettingsHelper::SetIni(const QString &key, const QVariant &val)
{
    using namespace Ini;
    if (m_app.contains(key)) {
        m_settings.setValue(key, val);
        return;
    }

    Set(key, val, group_text, m_text) Set(key, val, group_media, m_media) Set(
        key, val, group_folder, m_folder) Set(key, val, group_ctrls, m_ctrls)
        Set(key, val, group_keyboard, m_keys)
            Set(key, val, group_others, m_others)
                Set(key, val, group_pdf, m_pdfs) Set(key, val, group_web, m_web)

                    m_settings.setValue(key, val);
}

CSettingsHelper::CSettingsHelper()
{
    using namespace Ini;
    m_text.insert(text_wrap_b, true);
    m_text.insert(text_line_num_b, false);
    m_text.insert(text_support_list, QStringList());
    m_text.insert(text_zoom_size_i, 0);

    m_media.insert(media_loop_b, true);
    m_media.insert(media_time_b, false);
    m_media.insert(media_mute_b, false);
    m_media.insert(media_recents_list, {});

    m_web.insert(web_zoom_size_d, 1);

    m_pdfs.insert(pdf_is_actualsize_view_b, true);
    m_pdfs.insert(pdf_recents_list, {});

    m_folder.insert(is_folder_treeview_b, true);
    m_folder.insert(is_dir_load_contens_b, true);

    m_others.insert(plugins_data_bytes, QByteArray{});
    m_others.insert(plugins_data_all_bytes, QByteArray{});
    m_others.insert(version_str, QString(""));
    m_others.insert(app_path_str, QString(""));
    m_others.insert(save_temp_folder_b, true);
    m_others.insert(expand_details_property_b, true);
    m_others.insert(detect_type_b, true);
    QString f = "Segoe UI";
    if (QLocale::system().country() == QLocale::China) {
        f = "Microsoft Yahei";
    }
    m_others.insert(app_font_str, f);
    m_others.insert(license_uid_str, "");
    m_others.insert(license_key_str, "");
    m_others.insert(license_time_ll, 0);
    m_others.insert(stay_on_top_b, true);
    m_others.insert(is_keep_wnd_sz_pos_b, false);
    m_others.insert(theme_name_i, (int)Qss_Default);
    m_others.insert(markdown_css_path_str, "");
    m_others.insert(tray_icon_use_white_b, true);
    m_others.insert(tray_icon_hide_b, false);

    m_ctrls.insert(ctrls_app_list, QStringList{});
    m_ctrls.insert(is_show_open_with_default_btn_b, true);
    m_ctrls.insert(is_show_reveal_in_explorer_btn_b, true);

    m_keys.insert(key_get_focus_str, QString(""));  //  QString("Ctrl+Alt+S"));
    m_keys.insert(key_property_str, QString("Ctrl+I"));
    m_keys.insert(key_reveal_in_explorer_str, QString("Ctrl+R"));
    m_keys.insert(key_open_with_def_str, QString("Ctrl+O"));
    m_keys.insert(key_maximum_str, QString("Ctrl+Alt+W"));
    m_keys.insert(key_reload_str, QString("F5"));

    m_keys.insert(key_folder_open_str, QString("Alt+O"));
    m_keys.insert(key_folder_open_seer_str, QString("Alt+S"));
    m_keys.insert(key_folder_reveal_str, QString("Alt+R"));
    m_keys.insert(key_folder_tree_view_str, QString("Alt+T"));
    m_keys.insert(key_folder_column_view_str, QString("Alt+C"));

    m_keys.insert(key_img_rotate_l_str, QString("L"));
    m_keys.insert(key_img_rotate_r_str, QString("R"));
    m_keys.insert(key_img_actual_sz_str, QString("A"));

    m_keys.insert(key_media_forward_str, QString("Right"));
    m_keys.insert(key_media_backward_str, QString("Left"));
    m_keys.insert(key_media_play_str, QString("P"));
    m_keys.insert(key_media_loop_str, QString("L"));
    m_keys.insert(key_media_time_indication_str, QString("T"));
    m_keys.insert(key_media_mute_str, QString("0"));
    m_keys.insert(key_media_go_start_str, QString("Home"));

    m_keys.insert(key_pdf_actual_sz_str, QString("A"));
    m_keys.insert(key_pdf_fit_width_str, QString("W"));

    m_app.insert(autorun_b, false);
    m_app.insert(key_shift_b, false);
    m_app.insert(key_alt_b, false);
    m_app.insert(key_ctrl_b, false);

    m_app.insert(key_shift_f_b, false);
    m_app.insert(key_alt_f_b, false);
    m_app.insert(key_ctrl_f_b, false);

    m_app.insert(openfile_by_dclick_b, true);
    m_app.insert(use_hook_key_close_b, true);
    m_app.insert(tracking_file_b, true);
    m_app.insert(auto_update_b, true);
}
