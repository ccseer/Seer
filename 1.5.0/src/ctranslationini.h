#ifndef CTRANSLATIONINI_H
#define CTRANSLATIONINI_H

#include <QString>
#include "qapplication.h"

#define g_i18n_path QApplication::applicationDirPath() + "/i18n/"
#define g_i18n_eng QString("English")

namespace Translation_Ini {
bool UpdateTranslation();
void SetCurTranslation(const QString& cur);
QString GetCurTranslation();
}

#endif  // CTRANSLATIONINI_H
