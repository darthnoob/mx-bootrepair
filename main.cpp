#include <QApplication>
#include "mxbootrepair.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    mxbootrepair w;
    w.show();
    
    return a.exec();
}
