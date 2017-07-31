#ifndef UTILITIES_H
#define UTILITIES_H

#include <QCryptographicHash>
#include <QIcon>
#include <QSize>
#include <QString>
#include <QUrl>
#include <QtWinExtras/QtWin>

class QScreen;

/***************  functions  *********************/

QString GetExecFilePath(const QString& file);
QIcon GetFileIcon(const QString& file);
QIcon GetFileIcon(const QString& file, bool& is_succeed);

QString GetAppTempPath();
QString GetFilePropertyTempPath();
// QString GetAppDataPath();

bool OpenByDefaultApp(const QUrl& url);
bool OpenByDefaultApp(const QString& path);

QScreen* GetTriggerScreen();

void CopyFile2Clipboard(const QString& path);

bool IsDelFileConfirmEnabled();

// 获取图片 exif 信息
QMap<QString, QString> GetExifInfo(const QString& path);

void GetExeFilePropertyDetails(const QString& fullName,
                               QMap<QString, QString>& ret);

QString Encrypt(const QString& str,
                QCryptographicHash::Algorithm algorithm
                = QCryptographicHash::Sha3_512);

// sz
QSize GetMaxWndSize(QScreen* s);
QSize GetMaxContentSize(QScreen* s);
// GetMaxContentSize -> scale
QSize GetPreviewSize(const QSize& sz_pix);
QSize GetMinWndSz(qreal r);

#endif  // UTILITIES_H
