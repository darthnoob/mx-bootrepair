#ifndef MXBOOTREPAIR_H
#define MXBOOTREPAIR_H

#include <QWidget>

namespace Ui {
class mxbootrepair;
}

class mxbootrepair : public QWidget
{
    Q_OBJECT
    
public:
    explicit mxbootrepair(QWidget *parent = 0);
    ~mxbootrepair();
    
private:
    Ui::mxbootrepair *ui;
};

#endif // MXBOOTREPAIR_H
