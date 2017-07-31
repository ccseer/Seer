#include "cfiledialog.h"

#include <QStandardPaths>

QString dir_path
    = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
void GetAbsPath(const QString &res)
{
    dir_path = QFileInfo(res).absoluteDir().path();
}

QString CFileDialog::GetExistingDirectory(QWidget *parent,
                                          const QString &caption,
                                          QFileDialog::Options option)
{
    QString res
        = QFileDialog::getExistingDirectory(parent, caption, dir_path, option);
    if (!res.isEmpty()) {
        GetAbsPath(res);
    }
    return res;
}

QString CFileDialog::GetOpenFilePath(QWidget *parent /*= 0*/,
                                     const QString &caption /*= QString ( )*/,
                                     const QString &filter /*= ""*/,
                                     QFileDialog::Options options /*= 0*/)
{
    QString res = QFileDialog::getOpenFileName(parent, caption, dir_path,
                                               filter, 0, options);
    if (!res.isEmpty()) {
        GetAbsPath(res);
    }
    return res;
}

QString CFileDialog::GetSaveDirPath(QWidget *parent /*= 0*/,
                                    const QString &caption /*= QString( )*/,
                                    const QString &filter /*= QString( )*/,
                                    QString *selectedFilter /*= 0*/,
                                    QFileDialog::Options options /*= 0 */)
{
    QString res = QFileDialog::getSaveFileName(parent, caption, dir_path,
                                               filter, selectedFilter, options);
    if (!res.isEmpty()) {
        GetAbsPath(res);
    }
    return res;
}
