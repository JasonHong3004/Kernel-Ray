#include "graphview.h"
#include "ftracedata.h"
#include "traceview.h"

GraphView::GraphView(QObject *parent, Data *data)
{
    // Initialize properties
    this->parent = parent;
    this->data = data;
    this->taskColorMap = new QMap<QString, QColor*>();
    this->drawingTaskPosition = 5;
    this->cpuColorList = new QList<QColor *>();
    this->cpuColorList->push_back(new QColor(Qt::darkRed));
    this->cpuColorList->push_back(new QColor(Qt::darkGreen));
    this->cpuColorList->push_back(new QColor(Qt::darkBlue));
    this->cpuColorList->push_back(new QColor(Qt::darkCyan));
    this->taskPlottedMap = new QMap<QString, QList<QCPAbstractItem *> *>();

    // Set graph interactions
    setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectItems | QCP::iSelectLegend);

    // Prepare Y axis with CPU number
    this->yAxisTicks << 1 << 2 << 3 << 4;
    this->yAxisLabels << "CPU0" << "CPU1" << "CPU2" << "CPU3";
    this->yAxis->setAutoTicks(false);
    this->yAxis->setAutoTickLabels(false);
    this->yAxis->setTickVector(this->yAxisTicks);
    this->yAxis->setTickVectorLabels(this->yAxisLabels);
    this->yAxis->setRange(0, 5);

    // Prepare X axis
    this->xAxis->setLabel("CPU Jiffies");
    this->xAxis->grid()->setSubGridVisible(true);
    this->xAxis2->setVisible(true);

    // Only zoom X axix in
    axisRect()->setRangeZoom(Qt::Horizontal);
    axisRect()->setupFullAxesBox();

    // Prepare Legend
    this->legend->setVisible(true);
    QFont legendFont = font();
    legendFont.setPointSize(10);
    this->legend->setFont(legendFont);
    this->legend->setSelectedFont(legendFont);
    this->legend->setSelectableParts(QCPLegend::spItems);
    this->legend->setObjectName("Kernel Lay Legend");
}

/*
 * Exp : Slot for drawing task graph which user selected
 * Parmeter : None
 * Return value : None
 * Notice :
 */
void GraphView::drawTaskSlot()
{
    QStringList *checkedTaskList = ((TraceView *)sender())->checkedTaskList;

    // Clear task plotted list
    foreach(QString taskName, this->taskPlottedMap->keys()) {
        QList<QCPAbstractItem *> *itemList = this->taskPlottedMap->value(taskName);
        for(int i=0; i<itemList->size(); i++)
            removeItem(itemList->at(i));
        this->taskPlottedMap->value(taskName)->clear();
    }
    this->taskPlottedMap->clear();

    // Clear and Reset yAxis
    this->yAxisTicks.clear();
    this->yAxisLabels.clear();
    this->yAxisTicks << 1 << 2 << 3 << 4;
    this->yAxisLabels << "CPU0" << "CPU1" << "CPU2" << "CPU3";

//    QRegExp delimiter("(\\n)");
//    for(int i=0; i<checkedTaskList->size(); i++) {
//        QString taskName = checkedTaskList->at(i);
//        int position = i + this->drawingTaskPosition;
//        // Set Y axis
//        this->yAxisTicks << i + this->drawingTaskPosition;
//        this->yAxisLabels << taskName;

//        this->taskPlottedMap->insert(taskName, new QList<QCPAbstractItem *> ());
//        QList<FtraceData *> *taskHistoryList = this->data->taskHistoryMap->value(taskName);
//        for(int j=0; j<this->itemCount(); j++) {
//            QCPAbstractItem *item = this->item(j);
//            QString itemClass(item->metaObject()->className());
//            QStringList info = item->objectName().split(delimiter, QString::SkipEmptyParts);
//            if( !(info.at(1).compare(taskName)) && !(itemClass.compare("QCPItemRect"))) {
//                qDebug() << item->objectName();


//                // CPU number
////                info.at(3).right(1).toInt();
////                QString *string = new QString;
////                string->append()
//                QCPItemRect *taskRect = createRect(((QCPItemRect *)item)->topLeft->coords().x(),
//                                                   ((QCPItemRect *)item)->bottomRight->coords().x(),
//                                                   position-1,
//                                                   "Test",
//                                                   *(this->cpuColorList->at(0))
//                                                   );
//                addItem(taskRect);
//                this->taskPlottedMap->value(taskName)->push_back(taskRect);
//            }
//        }
//    }

    for(int i=0; i<checkedTaskList->size(); i++) {
        QString taskName = checkedTaskList->at(i);
        int position = i + this->drawingTaskPosition;
        // Set Y axix
        this->yAxisTicks << i + this->drawingTaskPosition;
        this->yAxisLabels << taskName;

        this->taskPlottedMap->insert(taskName, new QList<QCPAbstractItem *> ());
        QList<FtraceData *> *taskHistoryList = this->data->taskHistoryMap->value(taskName);
        FtraceData *init, *ftraceData;
        for(int j=0; j<taskHistoryList->size(); j++) {
            if(j == 0) {
                init = taskHistoryList->at(j);

                QCPItemLine *line = createLine(init->getTime()->toDouble(), position-1, *(init->getCpuInfo()), *(this->taskColorMap->value(*(init->getCmd()))));
                addItem( line );
                this->taskPlottedMap->value(taskName)->push_back(line);
                continue;
            }

            ftraceData = taskHistoryList->at(j);

            QCPItemLine *line = createLine(ftraceData->getTime()->toDouble(), position-1, *(ftraceData->getCpuInfo()), *(this->taskColorMap->value(*(ftraceData->getCmd()))));
            addItem( line );
            this->taskPlottedMap->value(taskName)->push_back(line);
            // Change CPU
            if(init->getCpu() != ftraceData->getCpu()) {
                QCPItemRect *rect = createRect(init->getTime()->toDouble(), ftraceData->getTime()->toDouble(), position-1, *(init->getCpuInfo()), *(this->cpuColorList->at(init->getCpu())));
                addItem( rect );
                this->taskPlottedMap->value(taskName)->push_back(rect);
                init = ftraceData;
            }

            // Last item
            if(j == taskHistoryList->size()-1 && init != ftraceData) {
                QCPItemRect *rect = createRect(init->getTime()->toDouble(), ftraceData->getTime()->toDouble(), position-1, *(ftraceData->getCpuInfo()), *(this->cpuColorList->at(init->getCpu())));
                addItem( rect );
                this->taskPlottedMap->value(taskName)->push_back(rect);
            }
        }
    }

    // Set Y axix
    this->yAxis->setTickVector(this->yAxisTicks);
    this->yAxis->setTickVectorLabels(this->yAxisLabels);

    replot();
}


/*
 * Exp : Slot for drawing CPU graph after parsing finished
 * Parmeter : None
 * Return value : None
 * Notice :
 */
void GraphView::drawSlot()
{
    // Clear all item in graph
    clearItems();
    // Set range
    FtraceData *firstData = this->data->ftraceDataList->first();
    FtraceData *lastData = this->data->ftraceDataList->last();

    this->xAxis->setRange(firstData->getTime()->toDouble(), lastData->getTime()->toDouble());
    this->xAxis->setNumberPrecision(13);

    // Clear and Reset yAxis
    this->yAxisTicks.clear();
    this->yAxisLabels.clear();
    this->yAxisTicks << 1 << 2 << 3 << 4;
    this->yAxisLabels << "CPU0" << "CPU1" << "CPU2" << "CPU3";
    this->yAxis->setTickVector(this->yAxisTicks);
    this->yAxis->setTickVectorLabels(this->yAxisLabels);

    //------------------------------------- Draw cpu items ------------------------------------

    this->legend->clear();

    QPen blackPen;
    blackPen.setColor(Qt::black);
    int columnPosition = 0;

    foreach(QString taskName, this->data->taskHistoryMap->keys()) {
        // Create random color for task
        QColor *color = new QColor(rand()%245+10, rand()%245+10, rand()%245+10);
        this->taskColorMap->insert(taskName, color);

        QCPBars *bar = new QCPBars(this->yAxis, this->xAxis);
        bar->setPen(blackPen);
        bar->setBrush(QBrush(*color));
        bar->setName(taskName);
        bar->setObjectName(taskName);

        QCPPlottableLegendItem *legendItem = new QCPPlottableLegendItem(this->legend, bar);
        legendItem->setObjectName(taskName);
        // Set 2 columns for easing to view
        this->legend->addElement(columnPosition/2, columnPosition%2, legendItem);

        columnPosition++;
    }

    for(int i=0; i<4; i++) {
        FtraceData *init, *ftraceData;

        for(int j=0; j<this->data->ftraceDataPerCpuList->at(i)->size(); j++) {
            if(j == 0) {
                init = this->data->ftraceDataPerCpuList->at(i)->at(0);

                addItem( createLine(init->getTime()->toDouble(), i, *(init->getAllInfo()), *(this->taskColorMap->value(*(init->getCmd())))) );
                continue;
            }
            ftraceData = this->data->ftraceDataPerCpuList->at(i)->at(j);

            addItem( createLine(ftraceData->getTime()->toDouble(), i, *(ftraceData->getAllInfo()), *(this->taskColorMap->value(*(ftraceData->getCmd())))) );

            // Change Process
            if(init->getCmd()->compare(*(ftraceData->getCmd()))) {
                addItem( createRect(init->getTime()->toDouble(), ftraceData->getTime()->toDouble(), i, *(init->getAllInfo()), *(this->taskColorMap->value(*(init->getCmd())))) );
                init = ftraceData;
            }

            // Draw last item
            if(j == this->data->ftraceDataPerCpuList->at(i)->size()-1 && init != ftraceData)
                addItem( createRect(init->getTime()->toDouble(), ftraceData->getTime()->toDouble(), i, *(init->getAllInfo()), *(this->taskColorMap->value(*(init->getCmd())))) );
        }
    }

    //---------------------------------------------------------------------------------------------

    connect(this, SIGNAL(legendClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)), this, SLOT(selectAllItemsSlot(QCPLegend*,QCPAbstractLegendItem*)));

    replot();
}

/*
 * Exp : Slot for highlighting all graph items related to legend item which user choosed
 * Parmeter : Graph legend, Selected legend item
 * Return value : None
 * Notice :
 */
void GraphView::selectAllItemsSlot(QCPLegend* legend,QCPAbstractLegendItem* legendItem)
{
    Q_UNUSED(legend);

    if(legendItem) {
        QRegExp delimeter("(\\n)");

        for(int i=0; i<itemCount(); i++) {
            QCPAbstractItem *item = this->item(i);

            if(item->objectName().contains("\n")) {
                QStringList stringList = item->objectName().split(delimeter, QString::SkipEmptyParts);
                if(legendItem->selected() && !(legendItem->objectName().compare(stringList.at(1))))
                    item->setSelected(true);
            }
        }
    } else {
        for(int i=0; i<itemCount(); i++) {
            QCPAbstractItem *item = this->item(i);
            item->setSelected(false);
        }
    }
}

/*
 * Exp : Slot for creating line item
 * Parmeter : Drawing position, CPU center, Info String, Color
 * Return value : Line Item
 * Notice :
 */
QCPItemLine *GraphView::createLine(double position, int center, QString info, QColor color)
{
    QCPItemLine *line = new QCPItemLine(this);
    line->start->setCoords(position, center+1.2);
    line->end->setCoords(position, center+0.7);
    line->setObjectName(info);
    line->setPen(QPen(color));
    line->setTooltip(info);

    return line;
}

/*
 * Exp : Slot for creating rect item
 * Parmeter : Rect left position, Rect right position, CPU center, Info String, Color
 * Return value : Rect Item
 * Notice :
 */
QCPItemRect *GraphView::createRect(double left, double right, int center, QString info, QColor color)
{
    QCPItemRect *rect = new QCPItemRect(this);
    rect->topLeft->setCoords(left, center+1.0);
    rect->bottomRight->setCoords(right, center+0.7);
    rect->setObjectName(info);
    rect->setBrush(QBrush(color));
    rect->setPen(Qt::NoPen);
    rect->setTooltip(info);

    return rect;
}
