#ifndef DIALOGSIMULATIONCONFIGURE_H
#define DIALOGSIMULATIONCONFIGURE_H

#include <QDialog>
#include "ModelGraphicsScene.h"

namespace Ui {
    class DialogSimulationConfigure;
}

class DialogSimulationConfigure : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSimulationConfigure(QWidget *parent = nullptr);
    ~DialogSimulationConfigure();
    void setModelSimulaion(ModelSimulation * model);
    void setTraceManager(TraceManager * traceManager);
private slots:
    void on_buttonBox_accepted();
    // Number of replication
    void on_spinBox_valueChanged(int arg1);
    void on_comboBox_currentIndexChanged(int index);
    void on_spinBox_2_textChanged(const QString &arg1);
    void on_spinBox_3_textChanged(const QString &arg1);
    void on_comboBox_2_currentIndexChanged(int index);

    void on_plainTextEdit_2_textChanged();
    // Trace Level
    void on_comboBox_3_currentIndexChanged(int index);

    void on_checkBox_stateChanged(int arg1);

private:
    Ui::DialogSimulationConfigure *ui;
    ModelSimulation * ms;
    TraceManager * trace;


    std::string experimentName;
    unsigned int numberOfReplication;
    double replicationLength;
    Util::TimeUnit replicationLengthtimeUnit;
    double warmUpPerid;
    Util::TimeUnit warmUpPeridTimeUnit;
    std::string terminatingCondition;
    TraceManager::Level traceLevel;
    bool initializeSystem;


};

#endif // DIALOGSIMULATIONCONFIGURE_H
