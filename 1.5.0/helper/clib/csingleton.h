#ifndef CSingleton_H
#define CSingleton_H

//用例 MyClass *singleton = Singleton<MyClass>::Instance();

template <class T>
class CSingleton {
public:
    static T *Instance()
    {
        static T _instance;  // create static instance of our class
        return &_instance;   // return it
    }

private:
    CSingleton()                   = delete;  // hide constructor
    ~CSingleton()                  = delete;  // hide destructor
    CSingleton(const CSingleton &) = delete;  // hide copy constructor
    CSingleton &operator=(const CSingleton &) = delete;  // hide assign op
};
#endif  // Singleton_H
