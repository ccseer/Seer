#include "ctranslationini.h"
#include "settingshelper.h"
#include "stable.h"

#include "qdir.h"
#include "qlocale.h"
#include "qtranslator.h"

bool LoadTr(const QString &qm, QTranslator *translator)
{
    const QString qm_file = qm + ".qm";
    if (!translator->load(g_i18n_path + qm_file)) {
        elprint << "load translator failed." << qm_file;
        translator->deleteLater();
        Translation_Ini::SetCurTranslation("");
        return false;
    }
    Translation_Ini::SetCurTranslation(qm);
    return true;
}

bool Translation_Ini::UpdateTranslation()
{
    const QString cur_lan = GetCurTranslation();
    //不存在
    if (cur_lan.isEmpty() || cur_lan == g_i18n_eng) {
        return true;
    }

    QTranslator *translator = new QTranslator(qApp);
    //上次加载成功,读取最后一次加载的
    if (!LoadTr(cur_lan, translator)) {
        return false;
    }
    if (!qApp->installTranslator(translator)) {
        elprint;
        return false;
    }
    return true;
}

void Translation_Ini::SetCurTranslation(const QString &cur)
{
    QString res = cur;
    if (cur.endsWith(".qm")) {
        res = QString(cur).remove(".qm");
    }
    g_pini->SetIni(Ini::i18n_key_str, res);
}

QString Translation_Ini::GetCurTranslation()
{
    auto res = g_pini->GetIni(Ini::i18n_key_str).toString();
    if (res.endsWith(".qm")) {
        res.remove(".qm");
    }
    return res;
}
