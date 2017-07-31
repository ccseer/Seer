#include "extensions.h"
#include "cglobaldata.h"
#include "settingshelper.h"
#include "stable.h"

#include <QFileInfo>
#include <QImageReader>
#include <QMovie>

QString CExtensionList::forFilter()
{
    QString s;
    for (int n = 0; n < count(); n++) {
        s = s + "*." + at(n) + " ";
    }
    if (!s.isEmpty())
        s = " (" + s + ")";
    return s;
}

QStringList CExtensionList::forDirFilter()
{
    QStringList l;
    for (int n = 0; n < count(); n++) {
        QString s = "*." + at(n);
        l << s;
    }
    return l;
}

QString CExtensionList::forRegExp()
{
    QString s;
    for (int n = 0; n < count(); n++) {
        if (!s.isEmpty())
            s = s + "|";
        s     = s + "^" + at(n) + "$";
    }
    return s;
}

QString CExtensionList::forDisplay(const QString &space) const
{
    QString s;
    for (int i = 0; i < count(); i++) {
        s.append(at(i)).append(space);
    }
    return s.trimmed();
}

CExtensions::CExtensions(){
#ifdef MEDIA
    {CExtensionList m_video = CExtensionList{} << "avi"
                                               << "vfw"
                                               << "divx"
                                               << "mpg"
                                               << "mpeg"
                                               << "m1v"
                                               << "m2v"
                                               << "mpv"
                                               << "dv"
                                               << "3gp"
                                               << "mov"
                                               << "mp4"
                                               << "m4v"
                                               << "mqv"
                                               << "dat"
                                               << "vcd"
                                               << "ogm"
                                               << "ogv"
                                               << "ogx"
                                               << "asf"
                                               << "wmv"
                                               << "iso"
                                               << "vob"
                                               << "mkv"
                                               << "nsv"
                                               << "ram"
                                               << "flv"
                                               << "rm"
                                               << "swf"
                                               << "mxf"
                                               << "ts"
                                               << "rmvb"
                                               << "dvr-ms"
                                               << "m2t"
                                               << "m2ts"
                                               << "mts"
                                               << "rec"
                                               << "f4v"
                                               << "hdmov"
                                               << "webm"
                                               << "vp8"
                                               << "bik"
                                               << "smk"
                                               << "m4b"
                                               << "wtv";

CExtensionList m_audio = CExtensionList() << "mp3"
                                          << "ogg"
                                          << "oga"
                                          << "wav"
                                          << "wma"
                                          << "aac"
                                          << "ac3"
                                          << "dts"
                                          << "ra"
                                          << "ape"
                                          << "flac"
                                          << "thd"
                                          << "mka"
                                          << "m4a"
                                          << "aiff"
                                          << "aif";

CExtensionList m_medias = CExtensionList() << m_audio << m_video;
m_medias.sort(Qt::CaseInsensitive);
m_all_sub.insert(S_Audio, m_audio);
m_all_sub.insert(S_Video, m_video);
m_all.insert(Media, m_medias);
}
#endif

{
    CExtensionList m_psd = CExtensionList() << "psd"
                                            << "psb";
    // moive ("gif", "mng")
    CExtensionList m_moive;
    foreach (auto i, QMovie::supportedFormats()) {
        m_moive.append(QString(i));
    }
    QStringList res;
    foreach (auto i, QImageReader::supportedImageFormats()) {
        res.append(QString(i));
    }
    foreach (QString i, m_moive) {
        res.removeAll(i);
    }
    CExtensionList m_pic;
    // m_pic ("bmp", "dds", "icns", "ico", "jp2", "jpeg", "jpg", "pbm", "pgm",
    // "png", "ppm", "psb", "psd", "svg", "svgz", "tga", "tif", "tiff", "wbmp",
    // "webp", "xbm", "xpm")
    foreach (QString i, res) {
        m_pic.append(i);
    }
    m_pic.removeOne("psd");
    m_pic.removeOne("psb");
    CExtensionList m_pics = CExtensionList() << m_psd << m_pic << m_moive;
    m_pics.sort(Qt::CaseInsensitive);

    m_all_sub.insert(S_Psd, m_psd);
    m_all_sub.insert(S_Pics, m_pic);
    m_all_sub.insert(S_Movie, m_moive);
    m_all.insert(Pic, m_pics);
}

{
    CExtensionList m_md = CExtensionList() << "md"
                                           << "markdown"
                                           << "mdown"
                                           << "mkdn"
                                           << "mkd"
                                           << "mdwn"
                                           << "mdtxt"
                                           << "mdtext";
    m_all_sub.insert(S_Mrakdown, m_md);

    CExtensionList m_web = CExtensionList() << "html";
    m_all_sub.insert(S_Web, m_web);

    CExtensionList m_webview = CExtensionList() << m_web << m_md;
    m_webview.sort(Qt::CaseInsensitive);
    m_all.insert(Html, m_webview);
}

#ifdef HIGHLIGHT
{
    CExtensionList m_bash = CExtensionList() << "bash"
                                             << "zsh"
                                             << "ebuild"
                                             << "eclass"
                                             << "sh";
    m_all_sub.insert(S_Bash, m_bash);
    CExtensionList m_batch = CExtensionList() << "bat"
                                              << "cmd";
    m_all_sub.insert(S_Batch, m_batch);
    CExtensionList m_cmake = CExtensionList() << "mak"
                                              << "makefile";
    m_all_sub.insert(S_Cmake, m_cmake);
    CExtensionList m_cpp = CExtensionList() << "c"
                                            << "cc"
                                            << "cu"
                                            << "cxx"
                                            << "cpp"
                                            << "c++"
                                            << "h"
                                            << "hh"
                                            << "hpp"
                                            << "h++"
                                            << "i"
                                            << "ii";
    m_all_sub.insert(S_Cpp, m_cpp);
    CExtensionList m_c_sharp = CExtensionList() << "cs"
                                                << "csharp";
    m_all_sub.insert(S_C_Shap, m_c_sharp);
    CExtensionList m_css = CExtensionList() << "css"
                                            << "qss";
    m_all_sub.insert(S_Css, m_css);
    CExtensionList m_diff = CExtensionList() << "diff"
                                             << "patch";
    m_all_sub.insert(S_Diff, m_diff);
    CExtensionList m_fortran_77 = CExtensionList() << "f"
                                                   << "for"
                                                   << "ftn";
    m_all_sub.insert(S_Fortran77, m_fortran_77);
    CExtensionList m_fortran_90 = CExtensionList() << "f95"
                                                   << "f90";
    m_all_sub.insert(S_Fortran90, m_fortran_90);
    CExtensionList m_html = CExtensionList() /*<< "html"*/ << "htm"
                                                           << "xhtml"
                                                           << "php"
                                                           << "php4";
    m_all_sub.insert(S_Html, m_html);
    CExtensionList m_java = CExtensionList() << "java"
                                             << "jsp";
    m_all_sub.insert(S_Java, m_java);
    CExtensionList m_lua = CExtensionList() << "lua";
    m_all_sub.insert(S_Lua, m_lua);
    CExtensionList m_matlab = CExtensionList() << "m";
    m_all_sub.insert(S_Matlab, m_matlab);
    CExtensionList m_perl = CExtensionList() << "pl"
                                             << "perl"
                                             << "cgi"
                                             << "pm"
                                             << "plx"
                                             << "plex";
    m_all_sub.insert(S_Perl, m_perl);
    CExtensionList m_python = CExtensionList() << "py"
                                               << "python"
                                               << "gyp";
    m_all_sub.insert(S_Python, m_python);
    CExtensionList m_sql = CExtensionList() << "sql";
    m_all_sub.insert(S_Sql, m_sql);
    CExtensionList m_tcl = CExtensionList() << "itcl"
                                            << "wish";
    m_all_sub.insert(S_Tcl, m_tcl);
    CExtensionList m_tex = CExtensionList() << "sty"
                                            << "cls";
    m_all_sub.insert(S_Tex, m_tex);
    CExtensionList m_xml = CExtensionList() << "xml"
                                            << "xaml"
                                            << "sgm"
                                            << "sgml"
                                            << "nrm"
                                            << "ent"
                                            << "hdr"
                                            << "hub"
                                            << "dtd"
                                            << "glade"
                                            << "wml"
                                            << "vxml"
                                            << "tld"
                                            << "xsl"
                                            << "ecf"
                                            << "jnlp"
                                            << "xsd"
                                            << "resx"
                                            << "ini";
    m_all_sub.insert(S_Xml, m_xml);

    CExtensionList m_javascript = CExtensionList() << "js"
                                                   << "javascript"
                                                   << "json";
    m_all_sub.insert(S_JavaScript, m_javascript);
    CExtensionList m_ruby = CExtensionList() << "ruby"
                                             << "rb"
                                             << "irb"
                                             << "gemspec"
                                             << "podspec"
                                             << "thor";
    m_all_sub.insert(S_Ruby, m_ruby);

    CExtensionList m_txt = CExtensionList() << "log"
                                            << "srt"
                                            << "lrc"
                                            << "txt"
                                            << "url"
                                            << "tex";
    m_all_sub.insert(S_Txt, m_txt);

    CExtensionList m_codes
        = CExtensionList() << m_bash << m_batch << m_cmake << m_c_sharp << m_cpp
                           << m_css << m_diff << m_fortran_77 << m_fortran_90
                           << m_html << m_lua << m_matlab << m_perl << m_python
                           << m_sql << m_tcl << m_tex << m_xml << m_java
                           << m_javascript << m_ruby << m_txt;

    m_codes.sort(Qt::CaseInsensitive);
    m_codes.append(g_pini->GetIni(Ini::text_support_list).toStringList());

    m_all.insert(Code, m_codes);
}
#endif

{
    CExtensionList m_rar = CExtensionList() << "rar";
    CExtensionList m_zip = CExtensionList() << "zip"
                                            << "zipx"
                                            << "tar"
                                            << "7z"
                                            << "xz"
                                            << "txz"
                                            << "z"
                                            << "taz"
                                            << "tbz2"
                                            << "lzma"
                                            << "lzma86"
                                            << "bzip2"
                                            << "gz"
                                            << "tgz"
                                            << "bz2"
                                            << "jar";
    CExtensionList m_dirs = CExtensionList() << m_rar << m_zip;
    m_dirs.sort(Qt::CaseInsensitive);
    m_all.insert(Dir, m_dirs);
    m_all_sub.insert(S_Rar, m_rar);
    m_all_sub.insert(S_Zip, m_zip);
}

#ifdef PDF
{
    CExtensionList m_pdf = CExtensionList() << "pdf"
                                            << "ai";
    m_pdf.sort(Qt::CaseInsensitive);

    m_all.insert(Pdf, m_pdf);
    m_all_sub.insert(S_PDF, m_pdf);
}
#endif
}

DisplayType CExtensions::GetType(const QString &path)
{
    QFileInfo info(path);
    if (info.isDir()) {
        return Dir;
    }

    const QString &suf = info.suffix().toLower();
    foreach (const auto &i, m_all.values()) {
        if (i.contains(suf)) {
            return m_all.key(i);
        }
    }
    return None;
}

FileSubType CExtensions::GetSubType(const QString &path)
{
    auto info = QFileInfo(path);
    if (info.isDir()) {
        return S_Folder;
    }

    const QString &suf = info.suffix().toLower();
    foreach (const auto &i, m_all_sub.values()) {
        if (i.contains(suf)) {
            return m_all_sub.key(i);
        }
    }
    return S_Unknown;
}
