/****************************************************************************
** Meta object code from reading C++ file 'scope.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../includes/scopes-qwt6/scope.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'scope.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Scope[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
       7,    6,    6,    6, 0x05,
      28,    6,    6,    6, 0x05,

 // slots: signature, parameters, type, tag, flags
      50,    6,    6,    6, 0x08,
      67,    6,    6,    6, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Scope[] = {
    "Scope\0\0clickedwithLeft(int)\0"
    "clickedwithRight(int)\0leftClicked(int)\0"
    "rightClicked(int)\0"
};

void Scope::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Scope *_t = static_cast<Scope *>(_o);
        switch (_id) {
        case 0: _t->clickedwithLeft((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->clickedwithRight((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->leftClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->rightClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Scope::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Scope::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Scope,
      qt_meta_data_Scope, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Scope::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Scope::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Scope::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Scope))
        return static_cast<void*>(const_cast< Scope*>(this));
    return QObject::qt_metacast(_clname);
}

int Scope::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void Scope::clickedwithLeft(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Scope::clickedwithRight(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
static const uint qt_meta_data_WaterfallViewer[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
      34,   16,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_WaterfallViewer[] = {
    "WaterfallViewer\0\0leftClicked(int)\0"
    "leftMouseClick(QPointF)\0"
};

void WaterfallViewer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        WaterfallViewer *_t = static_cast<WaterfallViewer *>(_o);
        switch (_id) {
        case 0: _t->leftClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->leftMouseClick((*reinterpret_cast< const QPointF(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData WaterfallViewer::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject WaterfallViewer::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_WaterfallViewer,
      qt_meta_data_WaterfallViewer, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &WaterfallViewer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *WaterfallViewer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *WaterfallViewer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WaterfallViewer))
        return static_cast<void*>(const_cast< WaterfallViewer*>(this));
    if (!strcmp(_clname, "QwtPlotSpectrogram"))
        return static_cast< QwtPlotSpectrogram*>(const_cast< WaterfallViewer*>(this));
    return QObject::qt_metacast(_clname);
}

int WaterfallViewer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void WaterfallViewer::leftClicked(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_SpectrumViewer[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x05,
      33,   15,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
      51,   15,   15,   15, 0x08,
      75,   15,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_SpectrumViewer[] = {
    "SpectrumViewer\0\0leftClicked(int)\0"
    "rightClicked(int)\0leftMouseClick(QPointF)\0"
    "rightMouseClick(QPointF)\0"
};

void SpectrumViewer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SpectrumViewer *_t = static_cast<SpectrumViewer *>(_o);
        switch (_id) {
        case 0: _t->leftClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->rightClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->leftMouseClick((*reinterpret_cast< const QPointF(*)>(_a[1]))); break;
        case 3: _t->rightMouseClick((*reinterpret_cast< const QPointF(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData SpectrumViewer::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SpectrumViewer::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SpectrumViewer,
      qt_meta_data_SpectrumViewer, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SpectrumViewer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SpectrumViewer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SpectrumViewer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SpectrumViewer))
        return static_cast<void*>(const_cast< SpectrumViewer*>(this));
    return QObject::qt_metacast(_clname);
}

int SpectrumViewer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void SpectrumViewer::leftClicked(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SpectrumViewer::rightClicked(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
