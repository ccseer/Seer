#ifndef CFILEDIALOG_H
#define CFILEDIALOG_H

#include <QFileDialog>
#include <QString>
#include <QWidget>

namespace CFileDialog {
QString GetExistingDirectory(QWidget* parent             = 0,
                             const QString& caption      = QString(),
                             QFileDialog::Options option = 0);

QString GetOpenFilePath(QWidget* parent              = 0,
                        const QString& caption       = QString(),
                        const QString& filter        = "",
                        QFileDialog::Options options = 0);

QString GetSaveDirPath(QWidget* parent              = 0,
                       const QString& caption       = QString(),
                       const QString& filter        = QString(),
                       QString* selectedFilter      = 0,
                       QFileDialog::Options options = 0);
};

#endif  // CFILEDIALOG_H
