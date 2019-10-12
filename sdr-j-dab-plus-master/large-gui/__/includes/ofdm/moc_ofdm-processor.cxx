/****************************************************************************
** Meta object code from reading C++ file 'ofdm-processor.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../includes/ofdm/ofdm-processor.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ofdm-processor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ofdm_processor[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x05,
      40,   15,   15,   15, 0x05,
      66,   15,   15,   15, 0x05,
      91,   15,   15,   15, 0x05,
     106,   15,   15,   15, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_ofdm_processor[] = {
    "ofdm_processor\0\0show_fineCorrector(int)\0"
    "show_coarseCorrector(int)\0"
    "show_avgTokenLength(int)\0showScope(int)\0"
    "setSynced(char)\0"
};

void ofdm_processor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ofdm_processor *_t = static_cast<ofdm_processor *>(_o);
        switch (_id) {
        case 0: _t->show_fineCorrector((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->show_coarseCorrector((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->show_avgTokenLength((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->showScope((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->setSynced((*reinterpret_cast< char(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ofdm_processor::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ofdm_processor::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_ofdm_processor,
      qt_meta_data_ofdm_processor, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ofdm_processor::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ofdm_processor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ofdm_processor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ofdm_processor))
        return static_cast<void*>(const_cast< ofdm_processor*>(this));
    return QThread::qt_metacast(_clname);
}

int ofdm_processor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void ofdm_processor::show_fineCorrector(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ofdm_processor::show_coarseCorrector(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ofdm_processor::show_avgTokenLength(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ofdm_processor::showScope(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void ofdm_processor::setSynced(char _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
