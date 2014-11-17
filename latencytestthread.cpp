#include "latencytestthread.h"

LatencyTestThread::LatencyTestThread(QProgressDialog *progressDialog, QString command, QProcess *cyclictestProcess)
{
    this->command = command;
    this->isCyclictestFinished = false;
    this->cyclictestProcess = new QProcess;
    this->progressDialog = progressDialog;
    progressDialog->setLabelText(tr("Cyclictest is running"));
}


void LatencyTestThread::run()
{
    this->cyclictestProcess->setStandardOutputFile(QString("/root/cyclictest_output.txt"));
    this->cyclictestProcess->start( this->command );
    this->cyclictestProcess->waitForFinished(-1);
    this->cyclictestProcess->close();
    this->isCyclictestFinished = true;

    while(1){

        if(this->progressDialog->wasCanceled()){
            emit terminateCyclictestSignal();
            break;
        }
        else if (this->isCyclictestFinished){
            emit finishCyclictestSignal();
            break;
        }
    }
}



