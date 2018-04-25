#ifndef COMMON_H
#define COMMON_H
#include <QSettings>
#include <windows.h>
#include <propertylist.h>
#include <QStandardPaths>

#define SIRIUS_VERSION "3.0"
#define max_dist 0.0001

extern QSettings settings;
extern PropertyList params;

QString GetStandartLocation(QStandardPaths::StandardLocation location);

#endif // COMMON_H
