#include "dialogsimulationconfigure.h"
#include "ui_dialogsimulationconfigure.h"

DialogSimulationConfigure::DialogSimulationConfigure(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSimulationConfigure)
{
    ui->setupUi(this);
//    experimentName = ms-;
//numberOfReplication = ms->getNumberOfReplications();
//    replicationLength = ms->getReplicationLength();
//    replicationLengthtimeUnit = ms->getReplicationLengthTimeUnit();
}

DialogSimulationConfigure::~DialogSimulationConfigure()
{
    delete ui;
}


void DialogSimulationConfigure::setModelSimulaion(ModelSimulation * model) {
    ms = model;
}

void DialogSimulationConfigure::setTraceManager(TraceManager * traceManager) {
    trace = traceManager;
}


void DialogSimulationConfigure::on_buttonBox_accepted()
{
    ms->setNumberOfReplications(numberOfReplication);
    ms->setReplicationLength(replicationLength, replicationLengthtimeUnit);
    ms->setWarmUpPeriod(warmUpPerid, warmUpPeridTimeUnit);
    ms->setTerminatingCondition(terminatingCondition);
    trace->setTraceLevel(traceLevel);
    ms->setInitializeSystem(initializeSystem);
}

// Number of replication
void DialogSimulationConfigure::on_spinBox_valueChanged(int arg1)
{
    numberOfReplication = arg1;
}

// Replication Length
void DialogSimulationConfigure::on_spinBox_3_textChanged(const QString &arg1)
{
    replicationLength = arg1.toDouble();
}

// Replication Length time unit
void DialogSimulationConfigure::on_comboBox_currentIndexChanged(int index)
{
    switch (index) {
    case 1:
        replicationLengthtimeUnit = Util::TimeUnit::picosecond;
        break;
    case 2:
        replicationLengthtimeUnit = Util::TimeUnit::nanosecond;
        break;
    case 3:
        replicationLengthtimeUnit = Util::TimeUnit::microsecond;
        break;
    case 4:
        replicationLengthtimeUnit = Util::TimeUnit::milisecond;
        break;
    case 5:
        replicationLengthtimeUnit = Util::TimeUnit::second;
        break;
    case 6:
        replicationLengthtimeUnit = Util::TimeUnit::minute;
        break;
    case 7:
        replicationLengthtimeUnit = Util::TimeUnit::hour;
        break;
    case 8:
        replicationLengthtimeUnit = Util::TimeUnit::day;
        break;
    case 9:
        replicationLengthtimeUnit = Util::TimeUnit::week;
        break;
    default:
        replicationLengthtimeUnit = Util::TimeUnit::unknown;
        break;
    }
}

// Warm up Period
void DialogSimulationConfigure::on_spinBox_2_textChanged(const QString &arg1)
{
    warmUpPerid = arg1.toDouble();
}

// Warm up Period time unit
void DialogSimulationConfigure::on_comboBox_2_currentIndexChanged(int index)
{
    switch (index) {
    case 1:
        replicationLengthtimeUnit = Util::TimeUnit::picosecond;
        break;
    case 2:
        replicationLengthtimeUnit = Util::TimeUnit::nanosecond;
        break;
    case 3:
        replicationLengthtimeUnit = Util::TimeUnit::microsecond;
        break;
    case 4:
        replicationLengthtimeUnit = Util::TimeUnit::milisecond;
        break;
    case 5:
        replicationLengthtimeUnit = Util::TimeUnit::second;
        break;
    case 6:
        replicationLengthtimeUnit = Util::TimeUnit::minute;
        break;
    case 7:
        replicationLengthtimeUnit = Util::TimeUnit::hour;
        break;
    case 8:
        replicationLengthtimeUnit = Util::TimeUnit::day;
        break;
    case 9:
        replicationLengthtimeUnit = Util::TimeUnit::week;
        break;
    default:
        replicationLengthtimeUnit = Util::TimeUnit::unknown;
        break;
    }
}

// Terminating Codition
void DialogSimulationConfigure::on_plainTextEdit_2_textChanged()
{
   terminatingCondition = ui->plainTextEdit_2->toPlainText().toStdString();
}

// Trace Level
void DialogSimulationConfigure::on_comboBox_3_currentIndexChanged(int index)
{
   switch (index) {
   case 1:
        traceLevel = TraceManager::Level::L1_errorFatal;
        break;
   case 2:
        traceLevel = TraceManager::Level::L2_results;
        break;
   case 3:
        traceLevel = TraceManager::Level::L3_errorRecover;
        break;
   case 4:
        traceLevel = TraceManager::Level::L4_warning;
        break;
   case 5:
        traceLevel = TraceManager::Level::L5_event;
        break;
   case 6:
        traceLevel = TraceManager::Level::L6_arrival;
        break;
   case 7:
        traceLevel = TraceManager::Level::L7_internal;
        break;
   case 8:
        traceLevel = TraceManager::Level::L8_detailed;
        break;
   case 9:
        traceLevel = TraceManager::Level::L9_mostDetailed;
        break;
   default:
        traceLevel = TraceManager::Level::L0_noTraces;
        break;
   }
}

// InitializeSystem
void DialogSimulationConfigure::on_checkBox_stateChanged(int arg1)
{
   initializeSystem = arg1;
}

