/****************************************************************************
** Meta object code from reading C++ file 'mirics-dongle.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../src/input/mirics-dongle/mirics-dongle.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mirics-dongle.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_miricsDongle[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x08,
      35,   13,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_miricsDongle[] = {
    "miricsDongle\0\0setExternalGain(int)\0"
    "set_KhzOffset(int)\0"
};

void miricsDongle::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        miricsDongle *_t = static_cast<miricsDongle *>(_o);
        switch (_id) {
        case 0: _t->setExternalGain((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->set_KhzOffset((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData miricsDongle::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject miricsDongle::staticMetaObject = {
    { &virtualInput::staticMetaObject, qt_meta_stringdata_miricsDongle,
      qt_meta_data_miricsDongle, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &miricsDongle::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *miricsDongle::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *miricsDongle::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_miricsDongle))
        return static_cast<void*>(const_cast< miricsDongle*>(this));
    if (!strcmp(_clname, "Ui_dongleWidget"))
        return static_cast< Ui_dongleWidget*>(const_cast< miricsDongle*>(this));
    return virtualInput::qt_metacast(_clname);
}

int miricsDongle::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = virtualInput::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
