#ifndef CKEYBOARDHOOKCONTROLER_H
#define CKEYBOARDHOOKCONTROLER_H

#include "csingleton.h"
#include "qobject.h"

#define g_pkeyboard CSingleton<CKeyboardHookControler>::Instance()

class CKeyboardHookControler : public QObject {
    Q_OBJECT
public:
    struct HookModifiers {
        bool ctrl  = false;
        bool shift = false;
        bool alt   = false;

        bool ctrl_f  = false;
        bool shift_f = false;
        bool alt_f   = false;

        bool is_fs_valid = false;
    };

    explicit CKeyboardHookControler(QObject *parent = 0);
    ~CKeyboardHookControler();

    inline void UpdateHookKey();
    inline const HookModifiers &GetSpaceMods() const
    {
        return m_spcae;
    }

    bool hooking;

    static bool monitor_del_key;

private:
    void InitHook();

    HookModifiers m_spcae;

signals:
    void SigHookFailed();

    void SigKeyPressed(bool full_scr = false);

    void SigDeleteKeyPressed();
};

#endif  // CKEYBOARDHOOKCONTROLER_H
