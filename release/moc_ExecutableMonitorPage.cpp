/****************************************************************************
** Meta object code from reading C++ file 'ExecutableMonitorPage.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../ExecutableMonitorPage.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ExecutableMonitorPage.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN21ExecutableMonitorPageE_t {};
} // unnamed namespace

template <> constexpr inline auto ExecutableMonitorPage::qt_create_metaobjectdata<qt_meta_tag_ZN21ExecutableMonitorPageE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "ExecutableMonitorPage",
        "monitoringToggled",
        "",
        "enabled",
        "filterChanged",
        "text",
        "itemActivated",
        "executableName",
        "setDetectedFiles",
        "QList<QStringList>",
        "rows",
        "setAnalysisDetails",
        "fileName",
        "filePath",
        "riskLevel",
        "fileType",
        "fileSize",
        "detection",
        "keyFindings",
        "recommendations",
        "mime",
        "md5",
        "sha256",
        "suspiciousStrings"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'monitoringToggled'
        QtMocHelpers::SignalData<void(bool)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 3 },
        }}),
        // Signal 'filterChanged'
        QtMocHelpers::SignalData<void(const QString &)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 5 },
        }}),
        // Signal 'itemActivated'
        QtMocHelpers::SignalData<void(const QString &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
        // Slot 'setDetectedFiles'
        QtMocHelpers::SlotData<void(const QList<QStringList> &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 9, 10 },
        }}),
        // Slot 'setAnalysisDetails'
        QtMocHelpers::SlotData<void(const QString &, const QString &, const QString &, const QString &, const QString &, const QString &, const QStringList &, const QStringList &, const QString &, const QString &, const QString &, const QStringList &)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 12 }, { QMetaType::QString, 13 }, { QMetaType::QString, 14 }, { QMetaType::QString, 15 },
            { QMetaType::QString, 16 }, { QMetaType::QString, 17 }, { QMetaType::QStringList, 18 }, { QMetaType::QStringList, 19 },
            { QMetaType::QString, 20 }, { QMetaType::QString, 21 }, { QMetaType::QString, 22 }, { QMetaType::QStringList, 23 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ExecutableMonitorPage, qt_meta_tag_ZN21ExecutableMonitorPageE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject ExecutableMonitorPage::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN21ExecutableMonitorPageE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN21ExecutableMonitorPageE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN21ExecutableMonitorPageE_t>.metaTypes,
    nullptr
} };

void ExecutableMonitorPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ExecutableMonitorPage *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->monitoringToggled((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 1: _t->filterChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->itemActivated((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->setDetectedFiles((*reinterpret_cast< std::add_pointer_t<QList<QStringList>>>(_a[1]))); break;
        case 4: _t->setAnalysisDetails((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[5])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[6])),(*reinterpret_cast< std::add_pointer_t<QStringList>>(_a[7])),(*reinterpret_cast< std::add_pointer_t<QStringList>>(_a[8])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[9])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[10])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[11])),(*reinterpret_cast< std::add_pointer_t<QStringList>>(_a[12]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<QStringList> >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ExecutableMonitorPage::*)(bool )>(_a, &ExecutableMonitorPage::monitoringToggled, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (ExecutableMonitorPage::*)(const QString & )>(_a, &ExecutableMonitorPage::filterChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (ExecutableMonitorPage::*)(const QString & )>(_a, &ExecutableMonitorPage::itemActivated, 2))
            return;
    }
}

const QMetaObject *ExecutableMonitorPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ExecutableMonitorPage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN21ExecutableMonitorPageE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ExecutableMonitorPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void ExecutableMonitorPage::monitoringToggled(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void ExecutableMonitorPage::filterChanged(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void ExecutableMonitorPage::itemActivated(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}
QT_WARNING_POP
