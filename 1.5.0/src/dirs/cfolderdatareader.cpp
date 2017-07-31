#include "cfolderdatareader.h"
#include "ccomponent.h"
#include "cdiritem.h"
#include "cglobaldata.h"
#include "stable.h"
#include "utilities.h"

#include <qt_windows.h>
#include <QDateTime>
#include "qdir.h"
#include "qfileiconprovider.h"

#include <Strsafe.h>

#define g_dir_time_format \
    QString(QLocale::system().dateFormat(QLocale::ShortFormat) + " HH:mm")

// 线程中添加所有数据
void CFolderDataReader::SlotReadData(const QString &dest)
{
    {
        QFileInfo info(dest);
        if (info.isSymLink() && QFileInfo(info.symLinkTarget()).isDir()) {
            m_path = info.symLinkTarget();
        }
        else {
            m_path = info.absoluteFilePath();
        }
    }

    QObject deleter;
    connect(&deleter, &QObject::destroyed, this,
            &CFolderDataReader::deleteLater);

    const auto wpath
        = (const wchar_t *)QDir::toNativeSeparators(m_path).utf16();
    // https://msdn.microsoft.com/en-us/library/windows/desktop/aa365200(v=vs.85).aspx
    size_t length_of_arg;
    // Check that the input path plus 3 is not longer than MAX_PATH.
    // Three characters are for the "\*" plus NULL appended below.
    StringCchLength(wpath, MAX_PATH, &length_of_arg);
    if (length_of_arg > (MAX_PATH - 3)) {
        emit SigDataErr("The input path plus 3 is longer than MAX_PATH!!!");
        elprint << dest;
        return;
    }

    // Prepare string for use with FindFile functions.  First, copy the
    // string to a buffer, then append '\*' to the directory name.
    TCHAR szDir[MAX_PATH];
    StringCchCopy(szDir, MAX_PATH, wpath);
    StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

    // Find the first file in the directory.
    WIN32_FIND_DATA ffd;
    HANDLE hfind = FindFirstFile(szDir, &ffd);
    if (INVALID_HANDLE_VALUE == hfind) {
        emit SigDataErr("INVALID_HANDLE_VALUE!!!");
        elprint << dest;
        return;
    }

    QList<QList<QStandardItem *>> res_folders, res_files;
    QList<CDirItem *> items_heads;

    // data getter
    {
        SHFILEINFO sfi = {0};
        QDir dir(m_path);
        QFileInfo info_item;
        QList<QStandardItem *> list_items;
        QString abs_sub_path, item_type_str, file_name;

        const QString str_folder = tr("File Folder");
        // List all the files in the directory with some info about them.
        do {
            Sleep(0);

            file_name    = QString::fromWCharArray(ffd.cFileName);
            abs_sub_path = dir.absoluteFilePath(file_name);
            info_item.setFile(abs_sub_path);

            if (!info_item.isHidden() && info_item.fileName() != "."
                && info_item.fileName() != "..") {
                list_items.clear();

                auto item_head        = new CDirItem(file_name, abs_sub_path);
                auto item_time_create = new CDirItem(
                    info_item.created().toString(g_dir_time_format));
                auto item_time_mod = new CDirItem(
                    info_item.lastModified().toString(g_dir_time_format));
                items_heads.append(item_head);

                if (info_item.isDir()) {
                    //只添加一个.
                    foreach (const auto &i,
                             QDir(abs_sub_path)
                                 .entryList(QDir::NoDotAndDotDot | QDir::AllDirs
                                            | QDir::Files)) {
                        item_head->appendRow(new CDirItem(i));
                        break;
                    }
                    auto item_size = new CDirItem("");
                    auto item_type = new CDirItem(str_folder);
                    list_items << item_head << item_size << item_type
                               << item_time_create << item_time_mod;
                    res_folders.append(list_items);
                }
                else {
                    sfi = {0};
                    SHGetFileInfo((const wchar_t *)abs_sub_path.utf16(), 0,
                                  &sfi, sizeof(sfi),
                                  SHGFI_USEFILEATTRIBUTES | SHGFI_TYPENAME);
                    item_type_str = QString::fromWCharArray(sfi.szTypeName);

                    auto item_size
                        = new CDirItem(C::Core::Funcs::ConvertSizeNum2String(
                            info_item.size()));
                    auto item_type = new CDirItem(item_type_str);
                    list_items << item_head << item_size << item_type
                               << item_time_create << item_time_mod;
                    res_files.append(list_items);
                }
            }
            if (!m_keep_reading) {
                break;
            }
        } while (FindNextFile(hfind, &ffd));
        FindClose(hfind);
    }

    //线程被关闭
    if (!m_keep_reading) {
        foreach (const auto &i, res_folders) {
            //有子节点的 folder 会一起析构, 整行
            qDeleteAll(i);
        }
        foreach (const auto &i, res_files) {
            qDeleteAll(i);
        }
        qprint << "thread aborted." << dest;
        return;
    }

    //已获取全部数据
    ModelPtr res_data(new CAlignedModel);
    AlignColumns cols;
    cols.insert(1, 1);
    res_data->SetAlignColumn(cols);
    res_data->setHorizontalHeaderLabels(
        QStringList() << tr("Name") << tr("Size") << tr("Type")
                      << tr("Date created") << tr("Date modified"));
    // sort
    {
        foreach (const auto &i, res_folders) {
            res_data->appendRow(i);
        }
        foreach (const auto &i, res_files) {
            res_data->appendRow(i);
        }
    }

    // icons
    if (items_heads.count() < 1000) {
#ifdef Q_OS_WIN
        QIcon res;
        SHFILEINFO sfi = {0};
        QFileIconProvider icon_provider;
#endif
        foreach (auto i, items_heads) {
#ifdef Q_OS_WIN
            sfi = {0};
            SHGetFileInfo(
                (const wchar_t *)QDir::toNativeSeparators(i->GetPath()).utf16(),
                0, &sfi, sizeof(sfi), SHGFI_ICON);
            if (sfi.hIcon) {
                res = QIcon(QtWin::fromHICON(sfi.hIcon));
            }
            if (res.isNull()) {
                res = icon_provider.icon(QFileIconProvider::File);
            }
            i->setIcon(res);
#else
            i->setIcon(GetFileIcon(i->GetPath()));
#endif
        }
    }

    if (!m_keep_reading) {
    }
    else {
        emit SigDataOk(res_data, dest);
    }
}

void CFolderDataReader::StopThread()
{
    m_lock.lock();
    m_keep_reading = false;
    m_lock.unlock();
}
