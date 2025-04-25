/****************************************************************************
** Meta object code from reading C++ file 'security.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.12)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../MicroPlank_QTVersion/Modules/SecurityModule/security.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'security.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.12. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Security_t {
    QByteArrayData data[7];
    char stringdata0[81];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Security_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Security_t qt_meta_stringdata_Security = {
    {
QT_MOC_LITERAL(0, 0, 8), // "Security"
QT_MOC_LITERAL(1, 9, 21), // "signalToStartSecurity"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 13), // "SendMsgSignal"
QT_MOC_LITERAL(4, 46, 16), // "Message_Inner_T&"
QT_MOC_LITERAL(5, 63, 3), // "msg"
QT_MOC_LITERAL(6, 67, 13) // "DealMsgSignal"

    },
    "Security\0signalToStartSecurity\0\0"
    "SendMsgSignal\0Message_Inner_T&\0msg\0"
    "DealMsgSignal"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Security[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x06 /* Public */,
       3,    1,   30,    2, 0x06 /* Public */,
       6,    0,   33,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void,

       0        // eod
};

void Security::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Security *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->signalToStartSecurity(); break;
        case 1: _t->SendMsgSignal((*reinterpret_cast< Message_Inner_T(*)>(_a[1]))); break;
        case 2: _t->DealMsgSignal(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Security::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Security::signalToStartSecurity)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Security::*)(Message_Inner_T & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Security::SendMsgSignal)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Security::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Security::DealMsgSignal)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Security::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_Security.data,
    qt_meta_data_Security,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Security::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Security::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Security.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Security::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void Security::signalToStartSecurity()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Security::SendMsgSignal(Message_Inner_T & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Security::DealMsgSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
