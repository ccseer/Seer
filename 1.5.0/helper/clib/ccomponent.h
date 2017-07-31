#ifndef CCOMPONENT_H
#define CCOMPONENT_H

#include <QObject>
#include <QTimer>
#include "qthread.h"

#include <string>
using namespace std;

#include <QFileDialog>
class QLineEdit;
class QMenu;
class QStandardItem;
class QPushButton;
class QPropertyAnimation;
#endif
class QTimeLine;

class CThread : public QThread {
    using QThread::run;

public:
    ~CThread();
};

namespace C {
namespace Core {

namespace DirFile {
/// @note: 在 windwos 下的测试,  Qt 在某些文件夹下测试结果不准确
///          标准库实现结构正确
/// @note: 求文件夹大小.不用递归
void GetDirSize(const QString &rootFolder,
                quint64 &f_size,
                quint64 *dir_t  = nullptr,
                quint64 *file_t = nullptr);
/// @note: 递归实现 + 标准库 .
void GetDirSizeEx(string rootFolder, unsigned long long &f_size);

bool MoveDir(const QString &from_path,
             const QString &to_path,
             bool coverexisting = true);

bool CopyAFile(QString source, QString target, bool coverexisting);

bool CopyDir(const QString from_path,
             const QString to_path,
             bool coverexisting = true);

/// @note: 一个文件转换成一个二进制块.  利用UncompressMassFile可将文件还原
/// @param filepath: 文件地址 ,传入参数
/// @param stream:传出输入流.
/// @note
/// 当filepath文件过大时,readAll系统无法分配足够内存.需要使用输入流分段写入
void ConvertFile2Bytes(const QString filepath, QDataStream &stream);

/// @note  解压缩ConvertFile2Bytes得到的二进制块.还原成文件
///  @param dirpath 保存路径,文件夹路径,二进制块中包含了文件名
///  @param filepath  二进制块路径
void UncompressMassFile(const QString filepath, const QString dirpath);

/// @note 解压缩ConvertFile2Bytes得到的二进制块.还原成文件
///  @param dirpath 保存路径,文件夹路径,二进制块中包含了文件名
///  @param filepath  二进制块路径
QString GetMd5FromFile(const QString &filepath);

/// @note
bool RevealInExplorer(const QString &path);

/// @note 获取唯一文件名
QString GetUniqueFilePath(const QString &dir = ".");
}  // namespace DirFile

namespace Funcs {

/**
 * @brief GetCharSetName
 * @param out   UTF-8   gb18030  ...   might be NULL
 * @param data  QByteArray would be good.
 * @param len   Not the data size, it's the size of datas will be used for
 * detecting
 * @example:
 *          char set[250] = { 0 };
 *          GetCharSetName(set,res, 120);
 *          if ( QString(set).isEmpty() ) return; //failed
 *          //QString(set) ==  "UTF-8"
 *      	QTextCodec *codec = QTextCodec::codecForName(set); // Set codec
 *          QTextDecoder *decoder = codec->makeDecoder(); // Decode
 *   		QString str = decoder->toUnicode(res); // Get text  .   might be
 * expensive
 */
#ifdef DECODE
void GetCharSetName(char *out, const char *data, size_t len = 128);
QByteArray GetCharSetName(const QByteArray &data);

bool GetDecodedData(const char *data, QString &res_out);
#endif
/// @note 将数字转换成 以MB GB 为单位的字符串
QString ConvertSizeNum2String(qulonglong num, qint8 decimal = 2);
}  // namespace Funcs
}  // namespace Core


namespace Ui {
namespace TreeModel {
/// @note
//传入父节点,按从上到下顺序获得该父节点下(不包括此父节点)所有子节点.
//递归实现.
void GetAllSubItems(const QStandardItem *item, QList<QStandardItem *> &res);

/// @overload
///  @param role为 data(int role = Qt::UserRole + 1)
///  @param value为data的值
///  获得该树中特定 role 特定 value 的 item 集合
void GetAllSubItems(const QStandardItem *item,
                    QList<QStandardItem *> &res,
                    const int role,
                    const QVariant &value);

///@note  计算item的children最深的嵌套层次,没有计算item这一层
/// @example
/// item
///         child1
///         child2
///                 child4
///                         child5
///         child3
/// 传入item节点,返回3  @return
int GetMaxIndentation(const QStandardItem *item);
}  // namespace TreeModel

namespace Ani {

///
/// \brief CloseWndWithFading
/// \param wnd 需要关闭的窗口
/// \param ani 若在调用处定义了 slot ,一般用于自定义 Animation 的 finished 槽,
/// 则需要将 ani  作为参数传入.否则函数可省略
///
void CloseWndWithFading(QWidget *wnd,
                        uint duration           = 130,
                        QPropertyAnimation *ani = 0);

/// @example
/// CMyDlg dlg(this);
/// DisplayWndWithOpacity(&dlg);
/// dlg.exec();
void DisplayWndWithOpacity(QWidget *wnd,
                           qreal end               = 0.97,
                           qreal start             = 0,
                           int duration            = 180,
                           QPropertyAnimation *ani = NULL);

/// \brief DisplaySubWndWithOpacity
///  同上 , 但适用于子控件
///  ***  当 timer 参数有值传入时,需要在外部 delete , 即哪里 new 哪里 del
///  ***   It is the caller's responsibility to delete the timer.
void DisplaySubWndWithOpacity(QWidget *sub_wnd,
                              int duration     = 800,
                              qreal start      = 0.3,
                              QTimeLine *timer = NULL);

void CloseSubWndWithOpacity(QWidget *sub_wnd,
                            int duration     = 300,
                            QTimeLine *timer = NULL);


void WaggleWnd(QWidget *wnd, int duration = 6 * 90);
}  // namespace Ani

namespace Funcs {

/// @note 窗口居中
void SetDlgCenter(QWidget *wnd, const QRect &rt_screen);
/// @note wnd center on parent
void SetDlgCenter2Parent(QWidget *wnd, QWidget *parent);

/// @note lineedit只能输入字符和数字
void SetLineeditOnlyNumChar(QLineEdit *ed, ushort maxlength = 0);

/// @note lineedit只能输入字符和数字
void SetLineeditOnlyNum(QLineEdit *ed, ushort maxlength = 0);

/// @overload 带有上下限
void SetLineeditOnlyNum(int bottom,
                        int top,
                        QLineEdit *ed,
                        ushort maxlength = 0);


/// @note if wnd is out of desktop , put it inside
bool MoveWndInsideDesktop(QWidget *target);
}  // namespace Funcs
}  // namespace Ui
#endif
}  // namespace C

#endif  // CCOMPONENT_H
