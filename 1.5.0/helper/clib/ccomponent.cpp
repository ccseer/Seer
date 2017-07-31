#include "ccomponent.h"
#include "stable.h"

#include <qscreen.h>
#include <QApplication>
#include <QClipboard>
#include <QCryptographicHash>
#include <QGraphicsOpacityEffect>
#include <QKeyEvent>
#include <QLineEdit>
#include <QMenu>
#include <QPropertyAnimation>
#include <QRegExpValidator>
#include <QStandardItem>
#include <QTextCodec>
#include <QTime>
#include <QTimeLine>
#include <QTranslator>
#include "qprocess.h"
#include "qwindow.h"
#endif

#ifdef DECODE
#include "uchardet.h"
#endif  // DECODE

#ifdef Q_OS_WIN
#include <filesystem>
#include <random>

using namespace std;
using namespace std::tr2::sys;

#include <Shlwapi.h>
#include "windows.h"

#pragma comment(lib, "Shlwapi.lib")
#endif  // Q_OS_WIN


bool C::Core::DirFile::RevealInExplorer(const QString &path)
{
#if defined(Q_OS_WIN)
    const QString explorer = "explorer";
    QStringList param;
    if (!QFileInfo(path).isDir()) {
        param << QLatin1String("/select,");
    }
    param << QDir::toNativeSeparators(path);
    return QProcess::startDetached(explorer, param);
#elif defined(Q_OS_MAC)
    QStringList scriptArgs;
    scriptArgs << QLatin1String("-e")
               << QString::fromLatin1(
                      "tell application \"Finder\" to reveal POSIX file \"%1\"")
                      .arg(path);
    QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs);
    scriptArgs.clear();
    scriptArgs << QLatin1String("-e")
               << QLatin1String("tell application \"Finder\" to activate");
    return QProcess::execute("/usr/bin/osascript", scriptArgs);
#endif
}

QString C::Core::Funcs::ConvertSizeNum2String(qulonglong num, qint8 decimal)
{
    QString res;
#ifdef Q_OS_WIN
    Q_UNUSED(decimal);
    WCHAR out[100] = {0};
    StrFormatByteSize64(num, out, 100);
    res = QString::fromWCharArray(out) /*.replace("字节","Byte")*/;
#else
    const auto kb = 1024;
    const auto mb = kb * 1024;
    const auto gb = mb * 1024;

    // to GB
    if (num > gb) {
        res = QString::number(((float)num) / gb, 'f', decimal).append(" GB");
    }
    // to MB
    else if (num > mb) {
        res = QString::number(((float)num) / mb, 'f', decimal).append(" MB");
    }
    // to kb
    else if (num > kb) {
        res = QString::number(((float)num) / kb, 'f', decimal).append(" KB");
    }
    else {
        res = QString::number(((float)num), 'f', decimal).append(" Byte");
    }
#endif
    return res;
}

#ifdef DECODE
void C::Core::Funcs::GetCharSetName(char *out,
                                    const char *data,
                                    size_t len /*= -1*/)
{
#ifdef Q_OS_WIN
    uchardet_t ucd = uchardet_new();
    auto len_act   = strlen(data);
    if (len_act > len) {
        len_act = len;
    }

    if (uchardet_handle_data(ucd, data, len_act)
        != 0)  // first len bytes used for detection
    {
        uchardet_delete(ucd);
        return;
    }
    uchardet_data_end(ucd);
    const char *charSetName = uchardet_get_charset(ucd);
    strcpy(out, charSetName);
    uchardet_delete(ucd);
#endif
    // QString csn = charSetName;
    // Detected character set:  charSetName
}

QByteArray C::Core::Funcs::GetCharSetName(const QByteArray &data)
{
    uchardet_t ucd = uchardet_new();
    if (uchardet_handle_data(ucd, data.constData(), data.size()) != 0) {
        return QByteArray();
    }

    uchardet_data_end(ucd);
    QByteArray cs(uchardet_get_charset(ucd));
    // uchardet_reset(ucd);
    return cs.trimmed();
}

bool C::Core::Funcs::GetDecodedData(const char *data, QString &res_out)
{
    char set[32] = {0};
    GetCharSetName(set, data);
    // failed
    if (QString(set).isEmpty()) {
        res_out = QString(data);
        return false;
    }
    // QString(set) ==  "UTF-8"
    QTextCodec *codec     = QTextCodec::codecForName(set);  // Set codec
    QTextDecoder *decoder = codec->makeDecoder();           // Decode
    // Get text   ,  might be expensive
    res_out = decoder->toUnicode(data);
    return true;
}
#endif  // DECODE

void C::Core::DirFile::ConvertFile2Bytes(const QString filepath,
                                         QDataStream &stream)
{
    QFileInfo fileinfo(filepath);
    if (!fileinfo.exists() || fileinfo.isDir()) {
        return;
    }

    QFile f(filepath);
    if (!f.open(QFile::ReadOnly)) {
        return;
    }
    //                  文件大小                            文件名 << 文件bytes
    stream << qint64(fileinfo.size()) << fileinfo.fileName();

    QByteArray temp;
    int size = 0;
    while (size != f.size()) {
        temp.clear();
        temp = f.read(1024 * 4);
        size += temp.size();
        stream << temp;
    }

    f.close();
}

void C::Core::DirFile::UncompressMassFile(const QString filepath,
                                          const QString dirpath)
{
    /// 传入解压后的byte数据. 可能是一个文件,也可能是多个文件.
    ///每个文件的数据结构为  ( 总大小+文件名 +文件内容 ) * N
    QFile file(filepath);
    if (!file.open(QFile::ReadOnly)) {
        return;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_2);
    while (!out.atEnd()) {
        // parse one file
        if (!QFileInfo(dirpath).isDir()) {
            return;
        }

        qint64 filesize = 0;
        QString filename;
        out >> filesize >> filename;
        QFile file(filename.prepend(dirpath));
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            return;
        }

        QByteArray block;
        qint64 sum = 0;
        while (1) {
            if (filesize == sum) {
                break;
            }
            block.clear();
            out >> block;
            sum += block.size();
            file.write(block);
        }
        file.close();
    }
    file.close();
}

QString C::Core::DirFile::GetMd5FromFile(const QString &filepath)
{
    QFile in(filepath);
    if (in.open(QFile::ReadOnly)) {
        QCryptographicHash hash(QCryptographicHash::Md5);
        hash.addData(&in);
        auto res = hash.result().toHex();
        in.close();
        return res;
    }

    return "";
}

QString C::Core::DirFile::GetUniqueFilePath(const QString &spcific_dir)
{
    QDir dir(spcific_dir);
    if (!dir.exists()) {
        dir.mkdir(spcific_dir);
    }
    QString path_dir = dir.absolutePath();
    if (!path_dir.endsWith("/")) {
        path_dir.append("/");
    }

    dir.setPath(path_dir);
    QString filename;
    while (1) {
        filename = C::Core::Funcs::GetRandomStr();
        if (dir.entryList(QStringList() << (filename + ".*")).isEmpty()) {
            return path_dir + filename;
        }
    }
    return {};
}

void C::Ui::Funcs::SetDlgCenter(QWidget *wnd, const QRect &rt_screen)
{
    Q_ASSERT(wnd);
    int screen_x = rt_screen.width();
    int screen_y = rt_screen.height();
    QPoint movePoint(screen_x / 2 - wnd->width() / 2 + rt_screen.x(),
                     screen_y / 2 - wnd->height() / 2 + rt_screen.y());
    wnd->move(movePoint);
}

void C::Ui::Funcs::SetDlgCenter2Parent(QWidget *wnd, QWidget *parent)
{
    Q_ASSERT(wnd);
    if (!parent) {
        SetDlgCenter(wnd, qApp->primaryScreen()->availableGeometry());
        return;
    }
    const auto shift_x = (parent->width() - wnd->width()) >> 1;
    const auto shift_y = (parent->height() - wnd->height()) >> 1;
    const auto pos     = parent->mapToGlobal({0, 0});
    wnd->move({pos.x() + shift_x, pos.y() + shift_y});
}

void C::Ui::Funcs::SetLineeditOnlyNumChar(QLineEdit *ed, ushort maxlength)
{
    QRegExp reg("[a-zA-Z0-9]+$");
    QRegExpValidator *vali = new QRegExpValidator(reg, ed);
    ed->setValidator(vali);
    if (maxlength) {
        ed->setMaxLength(maxlength);
    }
}

void C::Ui::Funcs::SetLineeditOnlyNum(QLineEdit *ed, ushort maxlength)
{
    QRegExp reg("[-]*[0-9]+$");
    QRegExpValidator *vali = new QRegExpValidator(reg, ed);
    ed->setValidator(vali);
    if (maxlength) {
        ed->setMaxLength(maxlength);
    }
}

void C::Ui::Funcs::SetLineeditOnlyNum(int bottom,
                                      int top,
                                      QLineEdit *ed,
                                      ushort maxlength)
{
    QIntValidator *vali = new QIntValidator(bottom, top, ed);
    ed->setValidator(vali);
    if (maxlength) {
        ed->setMaxLength(maxlength);
    }
}


void C::Ui::Ani::CloseWndWithFading(QWidget *wnd,
                                    uint duration,
                                    QPropertyAnimation *ani)
{
    if (!wnd->isVisible()) {
        return;
    }
    if (!ani) {
        ani = new QPropertyAnimation;
    }
    QObject::connect(ani, SIGNAL(finished()), wnd, SLOT(close()));
    ani->setTargetObject(wnd);
    ani->setPropertyName("windowOpacity");
    ani->setStartValue(wnd->windowOpacity());
    ani->setEndValue(0);
    ani->setDuration(duration);
    ani->setEasingCurve(QEasingCurve::InCubic);
    ani->start(QAbstractAnimation::DeleteWhenStopped);
}

void C::Ui::Ani::DisplayWndWithOpacity(
    QWidget *wnd, qreal end, qreal start, int duration, QPropertyAnimation *ani)
{
    if (wnd->isVisible()) {
        return;
    }
    if (!ani) {
        ani = new QPropertyAnimation;
    }
    ani->setTargetObject(wnd);
    ani->setPropertyName("windowOpacity");
    ani->setStartValue(start);
    ani->setEndValue(end);
    ani->setDuration(duration);
    ani->start(QAbstractAnimation::DeleteWhenStopped);
}

void C::Ui::Ani::DisplaySubWndWithOpacity(QWidget *sub_wnd,
                                          int duration,
                                          qreal start,
                                          QTimeLine *timer)
{
    if (!sub_wnd) {
        return;
    }
    // init
    auto d = new QGraphicsOpacityEffect;
    d->setOpacity(start);
    sub_wnd->setGraphicsEffect(d);

    // ani
    bool is_new_inside = false;
    if (!timer) {
        is_new_inside = true;
        timer         = new QTimeLine;
    }

    timer->setDuration(duration);
    timer->setFrameRange(0, 100);
    timer->connect(timer, &QTimeLine::valueChanged, [=](qreal val) {
        auto d = new QGraphicsOpacityEffect;
        d->setOpacity(start + (1 - start) * val);
        //会自动 delete 已 set 的 QGraphicsOpacityEffect'
        if (sub_wnd) {
            sub_wnd->setGraphicsEffect(d);
        }
    });
    timer->connect(timer, &QTimeLine::finished, [=]() {
        if (sub_wnd) {
            //恢复效果
            delete sub_wnd->graphicsEffect();
            sub_wnd->repaint();
        }
        if (is_new_inside) {
            delete timer;
        }
    });

    if (is_new_inside) {
        timer->start();
    }
}

void C::Ui::Ani::CloseSubWndWithOpacity(QWidget *sub_wnd,
                                        int duration,
                                        QTimeLine *timer)
{
    // ani
    bool is_new_inside = false;
    if (!timer) {
        is_new_inside = true;
        timer         = new QTimeLine;
    }

    timer->setDuration(duration);
    timer->setFrameRange(0, 100);
    timer->connect(timer, &QTimeLine::valueChanged, [=](qreal val) {
        auto d = new QGraphicsOpacityEffect;
        d->setOpacity(1 - val);
        //会自动 delete 已 set 的 QGraphicsOpacityEffect
        sub_wnd->setGraphicsEffect(d);
    });
    timer->connect(timer, &QTimeLine::finished, [=]() {
        //恢复效果
        sub_wnd->hide();
        delete sub_wnd->graphicsEffect();
        if (is_new_inside) {
            delete timer;
        }
    });

    if (is_new_inside) {
        timer->start();
    }
}

void C::Ui::Ani::WaggleWnd(QWidget *wnd, int duration /*= 6*90 */)
{
    if (!wnd || !wnd->isVisible()) {
        return;
    }

    auto ani       = new QPropertyAnimation(wnd, "pos", wnd);
    const auto dis = 25;
    const auto x   = wnd->pos().x();
    const auto y   = wnd->pos().y();
    ani->setKeyValueAt(0, wnd->pos());
    ani->setKeyValueAt(0.166, QPoint(x - dis, y));
    ani->setKeyValueAt(0.166 * 3, QPoint(x + dis, y));
    ani->setKeyValueAt(0.166 * 5, QPoint(x - dis, y));
    ani->setKeyValueAt(1, QPoint(x, y));
    ani->setDuration(duration);
    ani->start(QPropertyAnimation::DeleteWhenStopped);
}

void C::Ui::TreeModel::GetAllSubItems(const QStandardItem *item,
                                      QList<QStandardItem *> &res,
                                      const int role,
                                      const QVariant &value)
{
    if (!item) {
        return;
    }
    uint child_t = item->rowCount();
    for (uint i = 0; i < child_t; ++i) {
        if (value == item->child(i)->data(role)) {
            //将子节点添加进来. move
            res.append(item->child(i));
        }
        //再递归判断子节点是否还有下级节点
        GetAllSubItems(item->child(i), res, role, value);
    }
}

void C::Ui::TreeModel::GetAllSubItems(const QStandardItem *item,
                                      QList<QStandardItem *> &res)
{
    if (!item) {
        return;
    }
    uint child_t = item->rowCount();
    for (uint i = 0; i < child_t; ++i) {
        //将子节点添加进来.
        res.append(item->child(i));
        //再递归判断子节点是否还有下级节点
        GetAllSubItems(item->child(i), res);
    }
}

int C::Ui::TreeModel::GetMaxIndentation(const QStandardItem *item)
{
    QList<QStandardItem *> res;
    GetAllSubItems(item, res);
    int indentation = 0, max = 0;
    QStandardItem *tmp = NULL;
    foreach (QStandardItem *i, res) {
        if (i && i->hasChildren()) {
            continue;
        }
        tmp = i;
        max = 0;
        while (tmp && res.contains(tmp)) {
            tmp = tmp->parent();
            ++max;
        }
        indentation = qMax(max, indentation);
    }
    return indentation;
}

bool C::Ui::Funcs::MoveWndInsideDesktop(QWidget *target)
{
    //以所有屏幕大小边界
    QRect rt_screen(0, 0, 0, 0);
    foreach (auto i, qApp->screens()) {
        rt_screen = rt_screen.united(i->availableGeometry());
    }

    const QPoint &p = target->pos();
    int x           = p.x();
    if (p.x() < rt_screen.x()) {
        x = rt_screen.x();
    }
    else if (p.x() + target->width() > rt_screen.width() + rt_screen.x()) {
        x = rt_screen.width() + rt_screen.x() - target->width();
    }

    int y = p.y();
    if (p.y() < rt_screen.y()) {
        y = rt_screen.y();
    }
    else if (p.y() + target->height() > rt_screen.height() + rt_screen.y()) {
        y = rt_screen.height() + rt_screen.y() - target->height();
    }

    if (p != QPoint{x, y}) {
        target->move(x, y);
        return true;
    }
    return false;
}


CThread::~CThread()
{
    quit();
    wait();
}
