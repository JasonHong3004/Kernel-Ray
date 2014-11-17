#include "latencytestview.h"
#include "tabwidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>

LatencyTestView::LatencyTestView(QObject *parent, Data *data)
{

    this->parent = parent;
    this->data = data;

    setMainLayout();

}

void LatencyTestView::setOptionPart(QGridLayout *checkBoxLineEditLayout){

    for(int i = 0 ; i < this->checkBoxList->size() ; i++ )
        checkBoxLineEditLayout->addWidget(this->checkBoxList->at(i), i, 0);
    for(int i = 0 ; i < this->lineEditList->size() ; i++ )
        checkBoxLineEditLayout->addWidget(this->lineEditList->at(i), i, 1);
    checkBoxLineEditLayout->addWidget(this->data->cyclicOptionData->policyTypeComboBox, this->lineEditList->size(), 1);
    checkBoxLineEditLayout->addWidget(this->data->cyclicOptionData->clockTypeComboBox, this->lineEditList->size()+1, 1);
    checkBoxLineEditLayout->addWidget(this->resetButton, this->lineEditList->size()+3, 1);
    checkBoxLineEditLayout->addWidget(this->startButton, this->lineEditList->size()+4, 1);

}

void LatencyTestView::initView()
{

    total_loop      = 0;
    total_sum       = 0;
    total_avg       = 0;
    total_min       = 1000000;
    total_max       = 0;
    total_missCount = 0;
    total_missRate  = 0;

    this->loopLineEdit->setText("");
    this->avgLatencyLineEdit->setText("");
    this->minLatencyLineEdit->setText("");
    this->maxLatencyLineEdit->setText("");
    this->deadLineLineEdit->setText("");
    this->missCountLineEdit->setText("");
    this->missRateLineEdit->setText("");


    for( int i = 0 ; i < this->cyclictestStatDataList->size() ; i++ )
        if(!this->cyclictestStatDataList->at(i)->resetStatData())
            qDebug() << "free is failed";

    this->cyclictestStatDataList->clear();
    this->threadSelectComboBox->clear();
    this->threadSelectComboBox->addItem("Total");


}

void LatencyTestView::startButtonSlot(){

    initView();
    this->progressDialog = setProgressDialog();
    qDebug() << this->data->cyclicOptionData->getCommand();
    this->latencyTestThread = setLatencyTestThread();
    this->latencyTestThread->start();

}

void LatencyTestView::drawGraph(QList <CyclictestStatData*> *cyclictestStatDataList, int threadNumber){

    this->latencyGraphView->updateGraphSlot(cyclictestStatDataList, threadNumber, this->data->cyclicOptionData->lineEditList->at(2)->text().toInt());
}


void LatencyTestView::calculateOutputFileSlot()
{
    qDebug() << "calculate Output File slot";

    infoLineCount = this->data->cyclicOptionData->getInfoLineCount();

    qDebug() << "infolinecount : " << infoLineCount;

    for(int i = 0 ; i < this->data->cyclicOptionData->getThreadNumber() ; i++){
        CyclictestStatData *temp = new CyclictestStatData();    //create a CyclicStatData class per thread
        this->cyclictestStatDataList->push_back(temp);          //add to list
        this->threadSelectComboBox->addItem("Thread #" + QString::number(i));   //add variable to combobox
    }

    qDebug() << "before read";


    lineCount = 0;              //counting line
    CyclictestOptionData *temp; //get some data to calculate total loop
    temp = this->data->cyclicOptionData;

    int interval, distance, loop;

    if( !temp->checkBoxList->at(3)->isChecked() )
        distance = 500;
    else
        distance = temp->lineEditList->at(3)->text().toInt();

    if( !temp->checkBoxList->at(5)->isChecked() )
        interval = 1000;
    else
        interval = temp->lineEditList->at(5)->text().toInt();

    if( !temp->checkBoxList->at(6)->isChecked() )
        loop = 10000;
    else
        loop     = temp->lineEditList->at(6)->text().toInt();


    //calculate total loops

    for(int i = 0 ; i < infoLineCount-2 ; i++ ){
        lineCount += loop / ((interval + i * distance) / interval);
    }

    this->progressDialog->setRange(0, lineCount);
    qDebug() << "linecount : " << lineCount;

    //read output file
    if( readOutputFile() ){
        qDebug() << "Reading output file success";

        if( this->progressDialog->value() < lineCount)
            for( int i = this->progressDialog->value() ; i <= lineCount ; i++ )
                this->progressDialog->setValue(i);

        this->progressDialog->setCancelButtonText(tr("&Done"));
    }
    else{
        qDebug() << "Reading output file failed";
        this->progressDialog->setCancelButtonText(tr("&Failed"));
    }

    qDebug() << "before calculatestatdata deadline";

    //analyze deadline, statistic
    qDebug() << this->cyclictestStatDataList->size();

    for( int i = 0 ; i < this->cyclictestStatDataList->size() ; i++ ){
        if(this->checkBoxList->at(2)->isChecked()){ //when the deadline is set
            int deadline = this->lineEditList->at(2)->text().toInt();
            this->cyclictestStatDataList->at(i)->calculateStatData(deadline);
        }
        else        //when the deadline isn't set
            this->cyclictestStatDataList->at(i)->calculateStatData(0);

        total_loop += this->cyclictestStatDataList->at(i)->loops;
        total_sum  += this->cyclictestStatDataList->at(i)->sum;
        if(total_min > this->cyclictestStatDataList->at(i)->min)
            total_min = this->cyclictestStatDataList->at(i)->min;
        if(total_max < this->cyclictestStatDataList->at(i)->max)
            total_max = this->cyclictestStatDataList->at(i)->max;
        total_missCount += this->cyclictestStatDataList->at(i)->missCount;

    }


    qDebug() << "total loop : " << total_loop;
    total_avg = total_sum / total_loop;
    total_missRate = (double)total_missCount / (double)total_loop;

    qDebug() << "complete calc";

    this->drawGraph(this->cyclictestStatDataList, this->data->cyclicOptionData->getThreadNumber());
}


bool LatencyTestView::readOutputFile(){

    int cpuNumber;
    int latencyData;
    int readCount = 0;

    QFile file("/root/cyclictest_output.txt");
    QString line;
    QTextStream in(&file);
    QStringList tokenList;
    QRegExp delimeter("(\\: |\\ )");

    if(file.open(QIODevice::ReadOnly)){
        for( int i = 0 ; i < infoLineCount ; i++ )
            in.readLine();
        while(!in.atEnd()){
            if(progressDialog->wasCanceled()){
                break;
            }
            this->progressDialog->setValue(readCount++);
            line = in.readLine();
            line = line.simplified();
            tokenList = line.split(delimeter, QString::SkipEmptyParts);
            if(tokenList.size()!=3)
                continue;
            cpuNumber = tokenList.at(0).toInt();
            latencyData = tokenList.at(2).toInt();
            this->cyclictestStatDataList->at(cpuNumber)->statDataList->push_back(latencyData);
        }
        file.close();
        return true;
    }
    else
        return false;
}

QProgressDialog* LatencyTestView::setProgressDialog()
{
    QProgressDialog *progressDialog = new QProgressDialog;
    progressDialog->setCancelButtonText(tr("&Cancel"));
    progressDialog->setWindowTitle(tr("Running Latency Test"));
    progressDialog->show();

    return progressDialog;
}

LatencyTestThread* LatencyTestView::setLatencyTestThread()
{
    LatencyTestThread *latencyTestThread =
            new LatencyTestThread(this->progressDialog, this->data->cyclicOptionData->getCommand(), cyclictestProcess);

    connect(latencyTestThread, SIGNAL(finishCyclictestSignal()), this, SLOT(calculateOutputFileSlot()));
    connect(latencyTestThread, SIGNAL(terminateCyclictestSignal()), this, SLOT(terminateCyclictestSlot()));

    return latencyTestThread;
}

void LatencyTestView::resetButtonSlot(){

    for( int i = 0 ; i < this->checkBoxList->size() ; i++ )
        this->checkBoxList->at(i)->setChecked(false);

    for( int i = 0 ; i < this->lineEditList->size() ; i++ ){
        this->lineEditList->at(i)->setText("");
        this->lineEditList->at(i)->setEnabled(false);
    }
    this->data->cyclicOptionData->policyTypeComboBox->setCurrentIndex(0);
    this->data->cyclicOptionData->clockTypeComboBox->setCurrentIndex(0);
}


void LatencyTestView::terminateCyclictestSlot()
{
    this->latencyTestThread->cyclictestProcess->kill();

    if(this->latencyTestThread->cyclictestProcess->exitStatus() == 1){
        this->latencyTestThread->cyclictestProcess->start( "rm /root/cyclictest_output.txt" );
        qDebug() << "qprocess is terminated";
    }
}



void LatencyTestView::setSelectedThreadStatData(int selectedThread)
{

    CyclictestStatData *temp = new CyclictestStatData;

    if(selectedThread > 0){
        temp = this->cyclictestStatDataList->at(selectedThread-1);
        this->loopLineEdit->setText(QString::number(temp->loops));
        this->avgLatencyLineEdit->setText(QString::number(temp->avg));
        this->minLatencyLineEdit->setText(QString::number(temp->min));
        this->maxLatencyLineEdit->setText(QString::number(temp->max));
        this->deadLineLineEdit->setText(QString::number(temp->deadline));
        this->missCountLineEdit->setText(QString::number(temp->missCount));
        this->missRateLineEdit->setText(QString::number(temp->missRate*100)+"%");
    }
    else if(this->cyclictestStatDataList->size() != 0 && selectedThread == 0){
        temp = this->cyclictestStatDataList->at(0);
        this->loopLineEdit->setText(QString::number(total_loop));
        this->avgLatencyLineEdit->setText(QString::number(total_avg));
        this->minLatencyLineEdit->setText(QString::number(total_min));
        this->maxLatencyLineEdit->setText(QString::number(total_max));
        if(this->checkBoxList->at(2))
            this->deadLineLineEdit->setText(QString::number(temp->deadline));
        this->missCountLineEdit->setText(QString::number(total_missCount));
        qDebug() << total_missCount << total_missRate;
        this->missRateLineEdit->setText(QString::number(total_missRate*100)+"%");
    }
}



void LatencyTestView::setMainLayout(){


    this->latencyGraphView = new LatencyGraphView(this);

    this->cyclictestStatDataList = new QList <CyclictestStatData*>;

    this->checkBoxList = this->data->cyclicOptionData->checkBoxList;
    this->lineEditList = this->data->cyclicOptionData->lineEditList;

    this->startButton = new QPushButton;
    this->startButton->setText("&START");
    this->resetButton = new QPushButton;
    this->resetButton->setText("&RESET");

    connect(this->startButton, SIGNAL(clicked()), this, SLOT(startButtonSlot()));
    connect(this->resetButton, SIGNAL(clicked()), this, SLOT(resetButtonSlot()));


    QGroupBox *checkBoxLineEditGroup = new QGroupBox("Option");

    QGridLayout *checkBoxLineEditLayout = new QGridLayout;
    setOptionPart(checkBoxLineEditLayout);

    checkBoxLineEditGroup->setLayout(checkBoxLineEditLayout);

    QGridLayout *staticDataLayout = new QGridLayout;

    this->threadSelectComboBox = new QComboBox();
    this->threadSelectComboBox->addItem("Total");

    connect(this->threadSelectComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setSelectedThreadStatData(int)));


    this->threadLabel = new QLabel("Thread #");
    this->loopLabel = new QLabel("Loops");
    this->avgLatencyLabel = new QLabel("Average");
    this->minLatencyLabel = new QLabel("Minimum");
    this->maxLatencyLabel = new QLabel("Maximum");
    this->deadLineLabel = new QLabel("Deadline");
    this->missCountLabel = new QLabel("Miss count");
    this->missRateLabel = new QLabel("Miss rate");

    this->avgLatencyLineEdit = new QLineEdit("");
    this->loopLineEdit = new QLineEdit("");
    this->minLatencyLineEdit = new QLineEdit("");
    this->maxLatencyLineEdit = new QLineEdit("");
    this->deadLineLineEdit = new QLineEdit("");
    this->missCountLineEdit = new QLineEdit("");
    this->missRateLineEdit = new QLineEdit("");



    staticDataLayout->addWidget(this->threadLabel, 1, 0);
    staticDataLayout->addWidget(this->loopLabel, 2, 0);
    staticDataLayout->addWidget(this->avgLatencyLabel, 3, 0);
    staticDataLayout->addWidget(this->maxLatencyLabel, 4, 0);
    staticDataLayout->addWidget(this->minLatencyLabel, 5, 0);
    staticDataLayout->addWidget(this->deadLineLabel, 6, 0);
    staticDataLayout->addWidget(this->missCountLabel, 7, 0);
    staticDataLayout->addWidget(this->missRateLabel, 8, 0);



    staticDataLayout->addWidget(this->threadSelectComboBox, 1, 1);
    staticDataLayout->addWidget(this->loopLineEdit, 2, 1);
    staticDataLayout->addWidget(this->avgLatencyLineEdit, 3, 1);
    staticDataLayout->addWidget(this->maxLatencyLineEdit, 4, 1);
    staticDataLayout->addWidget(this->minLatencyLineEdit, 5, 1);
    staticDataLayout->addWidget(this->deadLineLineEdit, 6, 1);
    staticDataLayout->addWidget(this->missCountLineEdit, 7, 1);
    staticDataLayout->addWidget(this->missRateLineEdit, 8, 1);


    QGroupBox *statDataGroup = new QGroupBox("Result Analyzed Data");
    statDataGroup->setLayout(staticDataLayout);

    QSplitter *subSplitter = new QSplitter;
    subSplitter->setOrientation(Qt::Vertical);
    subSplitter->addWidget(checkBoxLineEditGroup);
    subSplitter->addWidget(statDataGroup);

    QSplitter *mainSplitter = new QSplitter;
    mainSplitter->setOrientation(Qt::Horizontal);
    mainSplitter->addWidget(this->latencyGraphView);
    mainSplitter->addWidget(subSplitter);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(mainSplitter);

    setLayout(mainLayout);
}















