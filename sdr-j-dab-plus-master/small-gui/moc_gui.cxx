/****************************************************************************
** Meta object code from reading C++ file 'gui.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "gui.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'gui.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_RadioInterface[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      22,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x08,
      36,   15,   15,   15, 0x08,
      62,   15,   15,   15, 0x08,
      76,   15,   15,   15, 0x08,
      93,   15,   15,   15, 0x08,
     112,   15,   15,   15, 0x08,
     139,   15,   15,   15, 0x08,
     166,   15,   15,   15, 0x08,
     177,   15,   15,   15, 0x0a,
     207,   15,   15,   15, 0x0a,
     239,   15,   15,   15, 0x0a,
     270,   15,   15,   15, 0x0a,
     286,   15,   15,   15, 0x0a,
     311,  309,   15,   15, 0x0a,
     337,  309,   15,   15, 0x0a,
     370,  363,   15,   15, 0x0a,
     411,   15,   15,   15, 0x0a,
     433,   15,   15,   15, 0x0a,
     452,   15,   15,   15, 0x0a,
     466,   15,   15,   15, 0x0a,
     481,   15,   15,   15, 0x0a,
     493,   15,   15,   15, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_RadioInterface[] = {
    "RadioInterface\0\0updateTimeDisplay()\0"
    "setStreamOutSelector(int)\0handleReset()\0"
    "abortSystem(int)\0TerminateProcess()\0"
    "set_channelSelect(QString)\0"
    "selectService(QModelIndex)\0setStart()\0"
    "set_fineCorrectorDisplay(int)\0"
    "set_coarseCorrectorDisplay(int)\0"
    "set_avgTokenLengthDisplay(int)\0"
    "clearEnsemble()\0addtoEnsemble(QString)\0"
    ",\0nameofEnsemble(int,char*)\0"
    "addEnsembleChar(char,int)\0,,,,,,\0"
    "channelData(int,int,int,int,int,int,int)\0"
    "show_successRate(int)\0show_ficRatio(int)\0"
    "show_snr(int)\0showScope(int)\0showIQ(int)\0"
    "setSynced(char)\0"
};

void RadioInterface::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        RadioInterface *_t = static_cast<RadioInterface *>(_o);
        switch (_id) {
        case 0: _t->updateTimeDisplay(); break;
        case 1: _t->setStreamOutSelector((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->handleReset(); break;
        case 3: _t->abortSystem((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->TerminateProcess(); break;
        case 5: _t->set_channelSelect((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 6: _t->selectService((*reinterpret_cast< QModelIndex(*)>(_a[1]))); break;
        case 7: _t->setStart(); break;
        case 8: _t->set_fineCorrectorDisplay((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->set_coarseCorrectorDisplay((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->set_avgTokenLengthDisplay((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->clearEnsemble(); break;
        case 12: _t->addtoEnsemble((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 13: _t->nameofEnsemble((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< char*(*)>(_a[2]))); break;
        case 14: _t->addEnsembleChar((*reinterpret_cast< char(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 15: _t->channelData((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< int(*)>(_a[5])),(*reinterpret_cast< int(*)>(_a[6])),(*reinterpret_cast< int(*)>(_a[7]))); break;
        case 16: _t->show_successRate((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 17: _t->show_ficRatio((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 18: _t->show_snr((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 19: _t->showScope((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 20: _t->showIQ((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 21: _t->setSynced((*reinterpret_cast< char(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData RadioInterface::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject RadioInterface::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_RadioInterface,
      qt_meta_data_RadioInterface, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &RadioInterface::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *RadioInterface::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *RadioInterface::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_RadioInterface))
        return static_cast<void*>(const_cast< RadioInterface*>(this));
    return QDialog::qt_metacast(_clname);
}

int RadioInterface::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 22)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 22;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
