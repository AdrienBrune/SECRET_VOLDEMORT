#include "server_ctr.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Server_CTR w;
    w.show();
    return a.exec();
}
