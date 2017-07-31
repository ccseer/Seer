#ifndef STABLE_H
#define STABLE_H
#include <QDebug>

#define qprint qDebug()
#define eprint __LINE__ << __FILE__ << Q_FUNC_INFO
#define elprint qDebug() << __LINE__ << __FILE__ << Q_FUNC_INFO
#define elprintf qDebug() << __LINE__ << __FILE__
#define qprintl qDebug() << "-------------------------------------"
#define qprintp qDebug() << "+++++++++++++++++++++++++++++++++++++"

#define cfree(_Memory)  \
    \
if(_Memory)             \
    {                   \
        delete _Memory; \
        _Memory = NULL; \
    }

#define qobject_destory_test(obj) \
    \
connect(obj, &QObject::destroyed, \
        [=]() { qDebug() << "destroyed" << obj->objectName(); });
#define qobject_destory_test_tag(obj, tag_str) \
    \
connect(obj, &QObject::destroyed,              \
        [=]() { qDebug() << "destroyed" << tag_str; });

// 通过 qss 设置
// qproperty-title_height: 54;
#define DEFINE_QSS_PROPERTY(type, name)                             \
    \
private:                                                            \
    Q_PROPERTY(type name READ name WRITE set##name DESIGNABLE true) \
    type m_##name;                                                  \
    \
public:                                                             \
    void set##name(const type &a)                                   \
    {                                                               \
        if (a != m_##name) {                                        \
            m_##name = a;                                           \
        }                                                           \
    }                                                               \
    type name() const                                               \
    {                                                               \
        return m_##name;                                            \
    }

#endif  // STABLE_H
