#include "utilities.h"
#include "cabstractwnd.h"
#include "cglobaldata.h"
#include "cmainwndex.h"
#include "cparentdlg.h"
#include "exif/exif.h"
#include "explorer.h"
#include "settingshelper.h"
#include "stable.h"

#include <qwindow.h>
#include <QClipboard>
#include <QDesktopServices>
#include <QFileInfo>
#include <QMimeData>
#include "qdir.h"
#include "qfileiconprovider.h"
#include "qscreen.h"

#pragma comment(lib, "Version.lib")
#include <Shlobj.h>
#include <shellapi.h>
#include <wtypes.h>

#define g_pp_comments QT_TRANSLATE_NOOP("CFileProperty", "Comments")
#define g_pp_internal_name QT_TRANSLATE_NOOP("CFileProperty", "Internal Name")
#define g_pp_product_name QT_TRANSLATE_NOOP("CFileProperty", "Product Name")
#define g_pp_company_name QT_TRANSLATE_NOOP("CFileProperty", "Company Name")

#define g_pp_legal_copyright \
    QT_TRANSLATE_NOOP("CFileProperty", "Legal Copyright")
#define g_pp_product_version \
    QT_TRANSLATE_NOOP("CFileProperty", "Product Version")
#define g_pp_file_description \
    QT_TRANSLATE_NOOP("CFileProperty", "File Description")
#define g_pp_legal_trademarks \
    QT_TRANSLATE_NOOP("CFileProperty", "Legal Trademarks")

#define g_pp_private_build QT_TRANSLATE_NOOP("CFileProperty", "Private Build")
#define g_pp_file_version QT_TRANSLATE_NOOP("CFileProperty", "File Version")
#define g_pp_original_filename \
    QT_TRANSLATE_NOOP("CFileProperty", "Original Filename")
#define g_pp_special_build QT_TRANSLATE_NOOP("CFileProperty", "Special Build")

bool OpenByDefaultApp(const QUrl &url)
{
    return QDesktopServices::openUrl(url);
}

bool OpenByDefaultApp(const QString &path)
{
    if (!QFileInfo(path).exists()) {
        return false;
    }

    return OpenByDefaultApp(QUrl{"file:///" + path, QUrl::TolerantMode});
}

QSize GetMaxWndSize(QScreen *s)
{
    return s->availableGeometry().size() * 0.95;
}

QSize GetMaxContentSize(QScreen *s)
{
    // 只有 被修改过 后 取到的值才是 正确的.
    // 否则 GetMaxContentSize 会在 UpdateSizeAndPos 之前调用导致 值无效
    foreach (QWidget *i, qApp->topLevelWidgets()) {
        if (auto dlg = qobject_cast<SleekBorderless *>(i)) {
            QSize big_sz = GetMaxWndSize(s);
            if (g_data->is_resized_by_user
                || g_pini->GetIni(Ini::is_keep_wnd_sz_pos_b).toBool()) {
                big_sz = dlg->size();
            }

            return big_sz;
        }
    }
    elprint;
    return GetMaxWndSize(s);
}

QSize GetPreviewSize(const QSize &sz_pix)
{
    const auto sz_max_content = GetMaxContentSize(g_data->screen);
    if (sz_max_content.height() >= sz_pix.height()
        && sz_max_content.width() >= sz_pix.width()) {
        return sz_pix;
    }
    // scale size
    return sz_pix.scaled(sz_max_content, Qt::KeepAspectRatio);
}

QString GetExecFilePath(const QString &file)
{
#ifdef Q_OS_WIN
    auto file_native          = QDir::toNativeSeparators(file).utf16();
    TCHAR exec_path[MAX_PATH] = {0};
    // FindExecutable: Retrieves the name of and handle to the executable (.exe)
    // file associated with a specific document file.
    if (FindExecutable((const wchar_t *)file_native, NULL, exec_path)
        > (HINSTANCE)32) {
        return QString::fromWCharArray(exec_path);
    }
#else
    Q_UNUSED(file);
#endif
    return {};
}

QIcon GetFileIcon(const QString &file, bool &is_succeed)
{
    is_succeed = false;
    QIcon res;
    try {
#ifdef Q_OS_WIN
        CoInitialize(NULL);
        SHFILEINFO sfi   = {0};
        auto file_native = QDir::toNativeSeparators(file).utf16();
        SHGetFileInfo((const wchar_t *)file_native, 0, &sfi, sizeof(sfi),
                      SHGFI_ICON);
        if (sfi.hIcon) {
            res = QIcon(QtWin::fromHICON(sfi.hIcon));
        }
        CoUninitialize();
#else
        QFileInfo fileInfo(file);
        QFileIconProvider fileIconProvider;
        auto icon = fileIconProvider.icon(fileInfo);
        if (!icon.isNull()) {
            res = icon.pixmap(icon.availableSizes().last());
        }
#endif
        if (res.isNull()) {
            QFileIconProvider icon_provider;
            res = icon_provider.icon(QFileIconProvider::File);
        }
        else {
            is_succeed = true;
        }
    }
    catch (...) {
        elprint;
    }
    return res;
}

QIcon GetFileIcon(const QString &file)
{
    bool res;
    return GetFileIcon(file, res);
}

QString GetAppTempPath()
{
    // C:/Users/ccdre_000/AppData/Local/Temp/seer/
    const QString path_temp
        = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/"
          + theAppName + "/";
    if (!QDir(path_temp).exists()) {
        QDir().mkpath(path_temp);
    }
    return path_temp;
}

QString GetFilePropertyTempPath()
{
    const auto res = GetAppTempPath() + "properties/";
    if (!QDir(res).exists()) {
        QDir().mkpath(res);
    }
    return res;
}

QScreen *GetTriggerScreen()
{
    QScreen *screen = qApp->primaryScreen();

    // desktop
    QPoint pos;
    if (Explorers::GetFileIconPosInDesktop(pos)) {
        foreach (auto i, qApp->screens()) {
            if (i->availableGeometry().contains(pos)) {
                return i;
            }
        }
        return screen;
    }

// folder  or  3rd party
#ifdef Q_OS_WIN
    if (auto handl
        = MonitorFromWindow(GetForegroundWindow(), MONITOR_DEFAULTTONEAREST)) {
        MONITORINFOEX info;
        info.cbSize = sizeof(MONITORINFOEX);
        if (GetMonitorInfo(handl, &info)) {
            const auto screen_name = QString::fromWCharArray(info.szDevice);
            foreach (auto i, qApp->screens()) {
                if (screen_name == i->name()) {
                    screen = i;
                    break;
                }
            }
        }
    }
#endif
    return screen;
}

void CopyFile2Clipboard(const QString &path)
{
    if (!QFileInfo::exists(path)) {
        return;
    }
    QMimeData *data = new QMimeData;
    auto list       = QList<QUrl>{} << QUrl::fromLocalFile(path);
    data->setUrls(list);
    qApp->clipboard()->setMimeData(data);
}

const QMap<QString, const char *> &GetExeFilePropertyNames()
{
    static QMap<QString, const char *> map{
        {"Comments", g_pp_comments},
        {"InternalName", g_pp_internal_name},
        {"ProductName", g_pp_product_name},
        {"CompanyName", g_pp_company_name},
        {"LegalCopyright", g_pp_legal_copyright},
        {"ProductVersion", g_pp_product_version},
        {"FileDescription", g_pp_file_description},
        {"LegalTrademarks", g_pp_legal_trademarks},
        {"PrivateBuild", g_pp_private_build},
        {"FileVersion", g_pp_file_version},
        {"OriginalFilename", g_pp_original_filename},
        {"SpecialBuild", g_pp_special_build}};
    return map;
}

void GetExeFilePropertyDetails(const QString &fullName,
                               QMap<QString, QString> &ret)
{
    ret = {};

    //获得文件基础信息
    //--------------------------------------------------------
    DWORD dwLen = GetFileVersionInfoSize(fullName.toStdWString().c_str(), 0);
    if (0 == dwLen) {
        return;
    }

    std::vector<char> lpData(dwLen + 1);
    if (!GetFileVersionInfo(fullName.toStdWString().c_str(), 0, dwLen,
                            &lpData[0])) {
        return;
    }

    LPVOID lpBuffer = NULL;
    UINT uLen       = 0;

    //获得语言和代码页(language and code page)
    //---------------------------------------------------
    if (!VerQueryValue(&lpData[0], (TEXT("\\VarFileInfo\\Translation")),
                       &lpBuffer, &uLen)) {
        return;
    }
    QString strTranslation, str1, str2;
    unsigned short int *p = (unsigned short int *)lpBuffer;
    str1.setNum(*p, 16);
    str1 = "000" + str1;
    strTranslation += str1.mid(str1.size() - 4, 4);
    str2.setNum(*(++p), 16);
    str2 = "000" + str2;
    strTranslation += str2.mid(str2.size() - 4, 4);

    const auto &map = GetExeFilePropertyNames();
    foreach (const auto &i, map.keys()) {
        QString code = "\\StringFileInfo\\" + strTranslation + "\\" + i;
        if (VerQueryValue(&lpData[0], (code.toStdWString().c_str()), &lpBuffer,
                          &uLen)) {
            ret.insert(
                qApp->translate("CFileProperty", map.value(i)),
                QString::fromUtf16((const unsigned short int *)lpBuffer));
        }
    }

    return;
}

bool IsDelFileConfirmEnabled()
{
#ifdef Q_OS_WIN
    SHELLFLAGSTATE opt = {};
    SHGetSettings(&opt, SSF_NOCONFIRMRECYCLE);
    // ==0 : 会弹出确认窗口
    // ==-1: 无弹窗
    return opt.fNoConfirmRecycle == 0;
#endif
    return false;
}

QMap<QString, QString> GetExifInfo(const QString &path)
{
    QMap<QString, QString> res;
    // Read the JPEG file into a buffer

    auto bytes = path.toLatin1();
    if (FILE *fp = fopen(bytes.data(), "rb")) {
        fseek(fp, 0, SEEK_END);
        unsigned long fsize = ftell(fp);
        rewind(fp);
        unsigned char *buf = new unsigned char[fsize];
        if (fread(buf, 1, fsize, fp) != fsize) {
            printf("Can't read file.\n");
            delete[] buf;
            return res;
        }
        fclose(fp);

        // Parse EXIF
        easyexif::EXIFInfo result;
        int code = result.parseFrom(buf, fsize);
        delete[] buf;
        if (!code) {
            res.insert(QT_TRANSLATE_NOOP("CPicWnd", "Camera make"),
                       QString::fromStdString(result.Make));
            res.insert(QT_TRANSLATE_NOOP("CPicWnd", "Camera model"),
                       QString::fromStdString(result.Model));
            res.insert(QT_TRANSLATE_NOOP("CPicWnd", "Software"),
                       QString::fromStdString(result.Software));
            if (result.BitsPerSample)
                res.insert(QT_TRANSLATE_NOOP("CPicWnd", "Bits per sample"),
                           QString::number(result.BitsPerSample));
            if (result.ImageWidth)
                res.insert(QT_TRANSLATE_NOOP("CPicWnd", "Image width"),
                           QString::number(result.ImageWidth));
            if (result.ImageHeight)
                res.insert(QT_TRANSLATE_NOOP("CPicWnd", "Image height"),
                           QString::number(result.ImageHeight));

            res.insert(QT_TRANSLATE_NOOP("CPicWnd", "Image description"),
                       QString::fromStdString(result.ImageDescription));
            //			if (result.Orientation && result.Orientation !=
            // 9)
            //				res.insert(QT_TRANSLATE_NOOP("CPicWnd",
            //"Image  orientation"), QString::number(result.Orientation));

            res.insert(QT_TRANSLATE_NOOP("CPicWnd", "Image copyright"),
                       QString::fromStdString(result.Copyright));

            res.insert(QT_TRANSLATE_NOOP("CPicWnd", "Image datetime"),
                       QString::fromStdString(result.DateTime));
            res.insert(QT_TRANSLATE_NOOP("CPicWnd", "Original datetime"),
                       QString::fromStdString(result.DateTimeOriginal));
            res.insert(QT_TRANSLATE_NOOP("CPicWnd", "Digitize datetime"),
                       QString::fromStdString(result.DateTimeDigitized));
            res.insert(QT_TRANSLATE_NOOP("CPicWnd", "Subsecond time"),
                       QString::fromStdString(result.SubSecTimeOriginal));
            if (result.ExposureTime)
                res.insert(QT_TRANSLATE_NOOP("CPicWnd", "Exposure time"),
                           "1/" + QString::number(
                                      (unsigned)(1.0 / result.ExposureTime))
                               + " s");
            if (result.FNumber)
                res.insert(
                    QT_TRANSLATE_NOOP("CPicWnd", "F-stop"),
                    "f/" + QString::number(result.FNumber, 'f', 1) + "f");
            if (result.ISOSpeedRatings)
                res.insert(QT_TRANSLATE_NOOP("CPicWnd", "ISO speed"),
                           QString::number(result.ISOSpeedRatings));
            if (result.SubjectDistance)
                res.insert(QT_TRANSLATE_NOOP("CPicWnd", "Subject distance"),
                           QString::number(result.SubjectDistance) + " m");
            if (result.ExposureBiasValue)
                res.insert(QT_TRANSLATE_NOOP("CPicWnd", "Exposure bias"),
                           QString::number(result.ExposureBiasValue) + " EV");
            if (result.Flash)
                res.insert(QT_TRANSLATE_NOOP("CPicWnd", "Flash"),
                           /*result.Flash == 0 ? "no flash" :*/ "flash used");
            const QMap<int, QString> mm{
                {1, "average"},    {2, "center weighted average"}, {3, "spot"},
                {4, "multi-spot"}, {5, "multi-segment"},
            };
            res.insert(QT_TRANSLATE_NOOP("CPicWnd", "Metering mode"),
                       mm.value(result.MeteringMode));
            if (result.FocalLength)
                res.insert(QT_TRANSLATE_NOOP("CPicWnd", "Lens focal length"),
                           QString::number(result.FocalLength) + " mm");
            if (result.FocalLengthIn35mm)
                res.insert(QT_TRANSLATE_NOOP("CPicWnd", "35mm focal length"),
                           QString::number(result.FocalLengthIn35mm) + " mm");

            //过滤无效值
            if (result.GeoLocation.Latitude
                || result.GeoLocation.LatComponents.degrees
                || result.GeoLocation.LatComponents.minutes
                || result.GeoLocation.LatComponents.seconds
                || result.GeoLocation.Longitude
                || result.GeoLocation.LonComponents.degrees
                || result.GeoLocation.LonComponents.minutes
                || result.GeoLocation.LonComponents.seconds) {
                res.insert(
                    QT_TRANSLATE_NOOP("CPicWnd", "GPS Latitude"),
                    QString("%1 deg (%2 deg, %3 min, %4 sec %5)")
                        .arg(result.GeoLocation.Latitude)
                        .arg(result.GeoLocation.LatComponents.degrees)
                        .arg(result.GeoLocation.LatComponents.minutes)
                        .arg(result.GeoLocation.LatComponents.seconds)
                        .arg(result.GeoLocation.LatComponents.direction));
                res.insert(
                    QT_TRANSLATE_NOOP("CPicWnd", "GPS Longitude"),
                    QString("%1 deg (%2 deg, %3 min, %4 sec %5)")
                        .arg(result.GeoLocation.Longitude)
                        .arg(result.GeoLocation.LonComponents.degrees)
                        .arg(result.GeoLocation.LonComponents.minutes)
                        .arg(result.GeoLocation.LonComponents.seconds)
                        .arg(result.GeoLocation.LonComponents.direction));
                res.insert(QT_TRANSLATE_NOOP("CPicWnd", "GPS Altitude"),
                           QString::number(result.GeoLocation.Altitude) + " m");
                res.insert(QT_TRANSLATE_NOOP("CPicWnd", "GPS Precision (DOP)"),
                           QString::number(result.GeoLocation.DOP));
            }
            if (result.LensInfo.FocalLengthMin)
                res.insert(
                    QT_TRANSLATE_NOOP("CPicWnd", "Lens min focal length"),
                    QString::number(result.LensInfo.FocalLengthMin) + " mm");
            if (result.LensInfo.FocalLengthMax)
                res.insert(
                    QT_TRANSLATE_NOOP("CPicWnd", "Lens max focal length"),
                    QString::number(result.LensInfo.FocalLengthMax) + " mm");
            if (result.LensInfo.FStopMin)
                res.insert(
                    QT_TRANSLATE_NOOP("CPicWnd", "Lens f-stop min"),
                    "f/" + QString::number(result.LensInfo.FStopMin, 'f', 1));
            if (result.LensInfo.FStopMax)
                res.insert(
                    QT_TRANSLATE_NOOP("CPicWnd", "Lens f-stop max"),
                    "f/" + QString::number(result.LensInfo.FStopMax, 'f', 1));
            res.insert(QT_TRANSLATE_NOOP("CPicWnd", "Lens make"),
                       QString::fromStdString(result.LensInfo.Make));
            res.insert(QT_TRANSLATE_NOOP("CPicWnd", "Lens model"),
                       QString::fromStdString(result.LensInfo.Model));
            if (result.LensInfo.FocalPlaneXResolution)
                res.insert(
                    QT_TRANSLATE_NOOP("CPicWnd", "Focal plane XRes"),
                    QString::number(result.LensInfo.FocalPlaneXResolution));
            if (result.LensInfo.FocalPlaneYResolution)
                res.insert(
                    QT_TRANSLATE_NOOP("CPicWnd", "Focal plane YRes"),
                    QString::number(result.LensInfo.FocalPlaneYResolution));
        }
    }
    return res;
}

QSize GetMinWndSz(qreal r)
{
    return QSize(r * 3 * 160, r * 3 * 120);
}

QString Encrypt(const QString &str, QCryptographicHash::Algorithm algorithm)
{
    QCryptographicHash c(algorithm);
    c.addData(str.toUtf8());
    return c.result().toHex();
}
