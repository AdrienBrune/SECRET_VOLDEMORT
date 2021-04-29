#include "controller.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QString("SECRET_HITLER"));
    app.setOrganizationName(QString("badrien"));
    app.setOrganizationDomain(QString("badrien.fr"));
    Controller w;
    return app.exec();
}
