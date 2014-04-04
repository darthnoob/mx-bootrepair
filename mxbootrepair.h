#ifndef MXBOOTREPAIR_H
#define MXBOOTREPAIR_H

#include <QMessageBox>
#include <QProcess>
#include <QTimer>

namespace Ui {
class mxbootrepair;
}

class mxbootrepair : public QDialog
{
    Q_OBJECT
    
protected:
    QProcess *proc;
    QTimer *timer;

public:
    explicit mxbootrepair(QWidget *parent = 0);
    ~mxbootrepair();

    void refresh();
    void displaySite(QString site);

    void reinstallGRUB();
    void repairGRUB();
    void addDevToCombo();

public slots:
    void procStart();
    void procTime();
    void procDone(int exitCode, QProcess::ExitStatus exitStatus);
    void setConnections(QTimer* timer, QProcess* proc);
    void onStdoutAvailable();

    virtual void on_grubRootButton_toggled();
    virtual void on_buttonOk_clicked();
    virtual void on_buttonAbout_clicked();
    virtual void on_buttonHelp_clicked();


private:
    Ui::mxbootrepair *ui;

};

#endif // MXBOOTREPAIR_H
