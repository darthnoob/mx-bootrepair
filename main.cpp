#include <QApplication>
#include "mxbootrepair.h"
#include <qtranslator.h>
#include <qlocale.h>
#include <unistd.h>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon("/usr/share/pixmaps/mx/mx-bootrepair.png"));

    QTranslator qtTran;
    qtTran.load(QString("qt_") + QLocale::system().name());
    a.installTranslator(&qtTran);

    QTranslator appTran;
    appTran.load(QString("mx-bootrepair_") + QLocale::system().name(), "/usr/share/mx-bootrepair/locale");
    a.installTranslator(&appTran);

    if (getuid() == 0) {
        mxbootrepair w;
        w.show();

        return a.exec();

    } else {
        QApplication::beep();
        QMessageBox::critical(0, QString::null,
                              QApplication::tr("You must run this program as root."));
        return 1;
    }
}
