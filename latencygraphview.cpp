#include "latencygraphview.h"

LatencyGraphView::LatencyGraphView(QObject *parent)
{
    this->parent = parent;

    setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend | QCP::iSelectPlottables);

    this->yAxis->setLabel("Count");
    this->yAxis->setTickLabelType(QCPAxis::ltNumber);
    this->yAxis->setScaleLogBase(10);

    // Prepare X axis
    this->xAxis->setTickLabelType(QCPAxis::ltNumber);
    this->xAxis->setAutoTickStep(true);
    this->xAxis->setLabel("Latency");

    // Make left and bottom axes transfer their ranges to right and top axes:
    connect(this->xAxis, SIGNAL(rangeChanged(QCPRange)), this->xAxis2, SLOT(setRange(QCPRange)));

    QPen gridPen;
    gridPen.setStyle(Qt::SolidLine);
    gridPen.setColor(QColor(0, 0, 0, 25));
    this->xAxis->grid()->setPen(gridPen);
    gridPen.setStyle(Qt::DotLine);
    this->yAxis->grid()->setSubGridPen(gridPen);

    // Setup legend
    this->legend->setVisible(true);
    axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignRight);
    this->legend->setBrush(QColor(255, 255, 255, 200));
    QPen legendPen;
    legendPen.setColor(QColor(130, 130, 130, 200));
    this->legend->setBorderPen(legendPen);
    QFont legendFont = font();
    legendFont.setPointSize(10);
    this->legend->setFont(legendFont);
    this->legend->setSelectableParts(QCPLegend::spItems);

    connect(this, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChangeSlot()));
    connect(this, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePressSlot()));
    connect(this, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheelSlot()));
    // make bottom and left axes transfer their ranges to top and right axes:
    connect(this->xAxis, SIGNAL(rangeChanged(QCPRange)), this->xAxis2, SLOT(setRange(QCPRange)));
    connect(this->yAxis, SIGNAL(rangeChanged(QCPRange)), this->yAxis2, SLOT(setRange(QCPRange)));
}

void LatencyGraphView::updateGraphSlot(QList <CyclictestStatData*> *cyclictestStatData, int threadNumber, int deadLineValue)
{
    this->cyclictestStatData = cyclictestStatData;
    this->clearGraphs();
    this->clearItems();

    for( int threadNum = 0 ; threadNum < threadNumber ; threadNum++){
        this->addGraph();
        QPen pen;
        pen.setColor(QColor(0, 0, 255, 200));

        this->graph()->setName("Thread "+QString::number(threadNum));
        this->graph()->setLineStyle(QCPGraph::lsLine);
        this->graph()->setPen(pen);
        this->graph()->setBrush(QBrush(QColor(rand()%245+10, rand()%245+10, rand()%245+10, 150)));

        QVector<double> *latency = new QVector<double>(this->cyclictestStatData->at(threadNum)->max);
        QVector<double> *sampleNumber = new QVector<double>(this->cyclictestStatData->at(threadNum)->max);

        analyzeRedundantDataSet(latency, sampleNumber, threadNum);
    }

    // Draw dead line and text
    QPen deadLinePen(Qt::black, 3, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin);
    QCPItemStraightLine *deadLine = new QCPItemStraightLine(this);
    deadLine->point1->setCoords(deadLineValue, 0);
    deadLine->point2->setCoords(deadLineValue, 1);
    deadLine->setPen(deadLinePen);
    this->addItem(deadLine);

    QCPItemText *deadLineText = new QCPItemText(this);
    deadLineText->position->setCoords(deadLineValue, 0);
    deadLineText->setText("DEAD LINE");
    deadLineText->setFont(QFont(font().family(), 16));
    deadLineText->setSelectable(false);
    deadLineText->setVisible(true);
    this->addItem(deadLineText);

    this->replot();
}

void LatencyGraphView::selectionChangeSlot()
{
    if (this->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || this->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
        this->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || this->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
    {
      this->xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
      this->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    }
    // make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
    if (this->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || this->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
        this->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || this->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
    {
      this->yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
      this->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    }

    // synchronize selection of graphs with selection of corresponding legend items:
    for (int i=0; i<this->graphCount(); ++i)
    {
      QCPGraph *graph = this->graph(i);
      QCPPlottableLegendItem *item = this->legend->itemWithPlottable(graph);
      if (item->selected() || graph->selected())
      {
        item->setSelected(true);
        graph->setSelected(true);
      }
    }
}

void LatencyGraphView::mousePressSlot()
{
    if (this->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
      axisRect()->setRangeDrag(this->xAxis->orientation());
    else if (this->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
      axisRect()->setRangeDrag(this->yAxis->orientation());
    else
      axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void LatencyGraphView::mouseWheelSlot()
{
    if (this->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
      axisRect()->setRangeZoom(this->xAxis->orientation());
    else if (this->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
      axisRect()->setRangeZoom(this->yAxis->orientation());
    else
      axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}


void LatencyGraphView::analyzeRedundantDataSet(QVector<double> *latency, QVector<double> *sampleNumber, int index)
{
    int redundantDataCount = 1;
    int currPointData = 0;

    currPointData = this->cyclictestStatData->at(index)->statDataList->at(0);

    latency->append((this->cyclictestStatData->at(0)->min)-1);        //add dump data to draw graph beautifully
    sampleNumber->append(0);

    for(int i = 1 ; i < this->cyclictestStatData->at(index)->statDataList->size() ; i++ ){
        if(currPointData == this->cyclictestStatData->at(index)->statDataList->at(i)){
            redundantDataCount++;  //redundancy count
            continue;
        }
        else{
            latency->append(this->cyclictestStatData->at(index)->statDataList->at(i-1));
            sampleNumber->append(redundantDataCount);    //set redundancy count
            currPointData = this->cyclictestStatData->at(index)->statDataList->at(i);
            redundantDataCount = 1;                //init redundancy count
        }
    }

    latency->append( this->cyclictestStatData->at(index)->statDataList->at(this->cyclictestStatData->at(index)->statDataList->size()-1));
    sampleNumber->append(redundantDataCount);
    this->graph()->setData(*latency, *sampleNumber);
}



