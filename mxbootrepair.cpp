/*****************************************************************************
 * mxbootrepair.cpp
 *****************************************************************************
 * Copyright (C) 2014 MX Authors
 *
 * Authors: Adrian
 *          MEPIS Community <http://forum.mepiscommunity.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/


#include "mxbootrepair.h"
#include "ui_mxbootrepair.h"

#include <QWebView>

mxbootrepair::mxbootrepair(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::mxbootrepair)
{
    ui->setupUi(this);
    refresh();
}

mxbootrepair::~mxbootrepair()
{
    delete ui;
}


void mxbootrepair::refresh() {
    proc = new QProcess(this);
    timer = new QTimer(this);
    proc->setReadChannel(QProcess::StandardOutput);
    proc->setReadChannelMode(QProcess::MergedChannels);
    ui->stackedWidget->setCurrentIndex(0);
    ui->progressBar->hide();
    ui->progressBar->setValue(0);
    ui->outputBox->setPlainText("");
    ui->outputLabel->setText("");
    ui->buttonOk->setText("Ok");
    ui->buttonOk->setIcon(QIcon("icons/dialog-ok.png"));
    setCursor(QCursor(Qt::ArrowCursor));
}


void mxbootrepair::reinstallGRUB() {
    ui->progressBar->show();
    setCursor(QCursor(Qt::WaitCursor));
    ui->buttonCancel->setEnabled(false);
    ui->buttonOk->setEnabled(false);
    ui->stackedWidget->setCurrentWidget(ui->outputPage);
    QString location = QString(ui->grubBootCombo->currentText()).section(" ", 0, 0);
    QString text = QString("GRUB is being installed on %1 device.").arg(location);
    ui->outputLabel->setText(text);
    setConnections(timer, proc);
    QString cmd = "grub-install /dev/" + location;
    proc->start(cmd);
}

void mxbootrepair::repairGRUB() {
    ui->progressBar->show();
    setCursor(QCursor(Qt::WaitCursor));
    ui->buttonCancel->setEnabled(false);
    ui->buttonOk->setEnabled(false);
    ui->outputLabel->setText("The GRUB configuration file (grub.cfg) is being rebuild.");
    setConnections(timer, proc);
    proc->start("update-grub");
}

//// sync process events ////

void mxbootrepair::procStart() {
    timer->start(100);
}

void mxbootrepair::procTime() {
    int i = ui->progressBar->value() + 1;
    if (i > 100) {
        i = 0;
    }
    ui->progressBar->setValue(i);
}

void mxbootrepair::procDone(int exitCode, QProcess::ExitStatus exitStatus) {
    timer->stop();
    ui->progressBar->setValue(100);
    setCursor(QCursor(Qt::ArrowCursor));
    ui->buttonCancel->setEnabled(true);
    ui->buttonOk->setEnabled(true);
    if (exitCode == 0) {
        if (QMessageBox::information(this, tr("Success"),
                                     tr("Process finished with success.<p><b>Do you want to exit MX Boot Repair?</b>"),
                                     QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Ok){
            qApp->exit(0);
        } else {
            ui->buttonOk->setText("< Back");
            ui->buttonOk->setIcon(QIcon());
        }
    } else {
        QMessageBox::critical(this, tr("Error"),
                              tr("Process finished. Errors have occurred."));
        ui->buttonOk->setText("< Back");
        ui->buttonOk->setIcon(QIcon());
    }
}

// set proc and timer connections
void mxbootrepair::setConnections(QTimer* timer, QProcess* proc) {
    disconnect(timer, SIGNAL(timeout()), 0, 0);
    connect(timer, SIGNAL(timeout()), this, SLOT(procTime()));
    disconnect(proc, SIGNAL(started()), 0, 0);
    connect(proc, SIGNAL(started()), this, SLOT(procStart()));
    disconnect(proc, SIGNAL(finished(int, QProcess::ExitStatus)), 0, 0);
    connect(proc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(procDone(int, QProcess::ExitStatus)));
    disconnect(proc, SIGNAL(readyReadStandardOutput()), 0, 0);
    connect(proc, SIGNAL(readyReadStandardOutput()), this, SLOT(onStdoutAvailable()));
}


// add list of devices to grubBootCombo
void mxbootrepair::addDevToCombo() {
    QString cmd;
    ui->grubBootCombo->clear();
    // add only disks
    if (ui->grubMbrButton->isChecked()) {
        cmd = "/bin/bash -c \"cat /proc/partitions | grep '[h,s,v].[a-z]$' | awk -F \' \' '{print $4}' | sort\"";
    } else { // add partition
        cmd = "/bin/bash -c \"cat /proc/partitions | grep '[h,s,v].[a-z][0-9]' | awk -F \' \' '{print $4}' | sort\"";
    }
    proc->start(cmd);
    proc->waitForFinished();
    QString out = proc->readAllStandardOutput();
    QStringList list = out.split("\n", QString::SkipEmptyParts);
    ui->grubBootCombo->addItems(list);
}

//// slots ////

// update output box on Stdout
void mxbootrepair::onStdoutAvailable() {
    QString out = ui->outputBox->toPlainText() + proc->readAllStandardOutput();
    ui->outputBox->setPlainText(out);
}

// repopulate combo when grubRootButton is toggled
void mxbootrepair::on_grubRootButton_toggled() {
    addDevToCombo();
}

// OK button clicked
void mxbootrepair::on_buttonOk_clicked() {
    if (ui->stackedWidget->currentIndex() == 0) {
        if (ui->reinstallRadioButton->isChecked()) {
            ui->stackedWidget->setCurrentWidget(ui->reinstallPage);
            addDevToCombo();
        } else if (ui->repairRadioButton->isChecked()) {
            ui->stackedWidget->setCurrentWidget(ui->outputPage);
            repairGRUB();
        }
    } else if (ui->stackedWidget->currentWidget() == ui->reinstallPage) {
        reinstallGRUB();
    } else if (ui->stackedWidget->currentWidget() == ui->outputPage) {
        refresh();
    } else {
        qApp->exit(0);
    }
}


// About button clicked
void mxbootrepair::on_buttonAbout_clicked() {
    QMessageBox msgBox(QMessageBox::NoIcon,
                       tr("About MX Boot Repair"), "<p align=\"center\"><b><h2>" +
                       tr("MX Boot Repair") + "</h2></b></p><p align=\"center\">MX14+git20140404</p><p align=\"center\"><h3>" +
                       tr("Simple boot repair program for antiX MX") + "</h3></p><p align=\"center\"><a href=\"http://www.mepiscommunity.org/mx\">http://www.mepiscommunity.org/mx</a><br /></p><p align=\"center\">" +
                       tr("Copyright (c) antiX") + "<br /><br /></p>", 0, this);
    msgBox.addButton(tr("License"), QMessageBox::AcceptRole);
    msgBox.addButton(QMessageBox::Cancel);
    if (msgBox.exec() == QMessageBox::AcceptRole)
        displaySite("file:///usr/local/share/doc/mx-bootrepair-license.html");
}


// Help button clicked
void mxbootrepair::on_buttonHelp_clicked() {
    displaySite("file:///usr/local/share/doc/mxapps.html#bootrepair");
}

// pop up a window and display website
void mxbootrepair::displaySite(QString site) {
    QWidget *window = new QWidget(this, Qt::Dialog);
    window->setWindowTitle(this->windowTitle());
    window->resize(800, 500);
    QWebView *webview = new QWebView(window);
    webview->load(QUrl(site));
    webview->show();
    window->show();
}
