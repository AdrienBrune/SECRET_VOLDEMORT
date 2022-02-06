#include "server_ctr.h"
#include <QApplication>

#define VERSION     1
#define REVISION    0

int main(int argc, char *argv[])
{
    sprintf(version, "version %d.%d", VERSION, REVISION);
    QApplication a(argc, argv);
    Server_CTR w;
    w.show();
    return a.exec();
}
