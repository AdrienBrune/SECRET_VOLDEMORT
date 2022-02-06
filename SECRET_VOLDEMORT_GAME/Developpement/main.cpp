#include "controller.h"

#include <QApplication>
#include <QString>

#define VERSION     1
#define REVISION    0

int main(int argc, char *argv[])
{
    sprintf(version, "version %d.%d", VERSION, REVISION);

    QApplication app(argc, argv);
    app.setApplicationName(QString("SECRET_VOLDEMORT"));
    app.setOrganizationName(QString("badrien"));
    app.setOrganizationDomain(QString("badrien.fr"));
    Controller w;
    return app.exec();
}
