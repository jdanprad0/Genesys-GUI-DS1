/*
 * The MIT License
 *
 * Copyright 2022 rlcancian.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/*
 * File:   ModelGraphicsScene.cpp
 * Author: rlcancian
 *
 * Created on 16 de fevereiro de 2022, 09:52
 */

//#include <qt5/QtWidgets/qgraphicssceneevent.h>
#include <QtWidgets/qgraphicssceneevent.h>
#include <QTreeWidget>
#include <QMessageBox>
#include <QUndoCommand>
#include "ModelGraphicsScene.h"
#include "ModelGraphicsView.h"
#include "graphicals/GraphicalModelComponent.h"
#include "graphicals/GraphicalComponentPort.h"
#include "graphicals/GraphicalConnection.h"
#include "actions/AddUndoCommand.h"
#include "actions/DeleteUndoCommand.h"
#include "actions/MoveUndoCommand.h"

ModelGraphicsScene::ModelGraphicsScene(qreal x, qreal y, qreal width, qreal height, QObject *parent) : QGraphicsScene(x, y, width, height, parent) {
    // grid
    _grid.interval = TraitsGUI<GScene>::gridInterval; // 20;
    _grid.pen = QPen(TraitsGUI<GScene>::gridColor);	  // QPen(Qt::gray); //TODO: To use TraitsGUI<GScene>::gridColor must solve myrgba first
    _grid.lines = new std::list<QGraphicsLineItem *>();
    _grid.visible = false;

	_grid.pen.setWidth(TraitsGUI<GScene>::gridPenWidth);
	_grid.pen.setStyle(Qt::DotLine);
}

ModelGraphicsScene::ModelGraphicsScene(const ModelGraphicsScene& orig) { // : QGraphicsScene(orig) {
}

ModelGraphicsScene::~ModelGraphicsScene() {}


//-----------------------------------------------------------------------

GraphicalModelComponent* ModelGraphicsScene::addGraphicalModelComponent(Plugin* plugin, ModelComponent* component, QPointF position, QColor color) {
    // cria o componente gráfico
    GraphicalModelComponent* graphComp = new GraphicalModelComponent(plugin, component, position, color);

    // cria as conexoes
    // verifica se tenho um componente selecionado
	if (selectedItems().size() == 1 && plugin->getPluginInfo()->getMinimumInputs() > 0) { // check if there is selected component and crate a connection between them
        GraphicalModelComponent* otherGraphComp = dynamic_cast<GraphicalModelComponent*> (selectedItems().at(0));

        // verifica se conseguiu converter o item selecionado para GraphicalModelComponent
        if (otherGraphComp != nullptr) { // a component is selected
            // pega o componente selecionado
			ModelComponent* otherComp = otherGraphComp->getComponent();

            // numero maximo de possiveis conexoes pela porta de saida
            unsigned int maxOutputsOtherComp = otherGraphComp->getGraphicalOutputPorts().size();

            // verifica se ainda posso criar conexoes com aquele componente
            if (otherGraphComp->getOcupiedOutputPorts() < maxOutputsOtherComp) {
                // caso tenha portas disponíveis, busca qual delas é
                for (unsigned int numPort = 0; numPort < maxOutputsOtherComp; numPort++) {
                    // caso seja um ponteiro vazio, ele esta livre
                    if (otherComp->getConnections()->getConnectionAtPort(numPort) == nullptr) {
                        // create connection (both model and grapically, since model is being built
                        // model
                        otherGraphComp->getComponent()->getConnections()->insertAtPort(numPort, new Connection({component, 0}));

                        //graphically
                        _sourceGraphicalComponentPort = ((GraphicalModelComponent*) selectedItems().at(0))->getGraphicalOutputPorts().at(numPort);
                        GraphicalComponentPort* destport = graphComp->getGraphicalInputPorts().at(0);
                        addGraphicalConnection(_sourceGraphicalComponentPort, destport, numPort, 0);

                        otherGraphComp->setOcupiedOutputPorts(otherGraphComp->getOcupiedOutputPorts() + 1);
                        graphComp->setOcupiedInputPorts(graphComp->getOcupiedInputPorts() + 1);
                        break;
                    }
                }
            }
        // caso seja uma porta que esteja selecionada
		} else {
			GraphicalComponentPort* sourceGraphPort = dynamic_cast<GraphicalComponentPort*> (selectedItems().at(0));
			if (sourceGraphPort != nullptr) { // a specific output port of a component is selected.
				if (sourceGraphPort->getConnections()->size() == 0) {
					// create connection (both model and grapically, since model is being built (ALMOST REPEATED CODE -- REFACTOR)
					otherGraphComp = sourceGraphPort->graphicalComponent();
					// create connection (both model and grapically, since model is being built (ALMOST REPEATED CODE -- REFACTOR)
					// model
					otherGraphComp->getComponent()->getConnections()->insertAtPort(sourceGraphPort->portNum(), new Connection({component, 0}));
					//graphically
					_sourceGraphicalComponentPort = sourceGraphPort;
					GraphicalComponentPort* destport = graphComp->getGraphicalInputPorts().at(0);
                    addGraphicalConnection(_sourceGraphicalComponentPort, destport, sourceGraphPort->portNum(), 0);

                    otherGraphComp->setOcupiedOutputPorts(otherGraphComp->getOcupiedOutputPorts() + 1);
                    graphComp->setOcupiedInputPorts(graphComp->getOcupiedInputPorts() + 1);
                }
			}
		}
	}

    // adiciona o objeto criado na lista de componentes graficos para nao perder a referencia
    _allGraphicalModelComponents.append(graphComp);

    // cria um objeto para undo e redo do add
    // ele propriamente adiciona o objeto na tela
    QUndoCommand *addUndoCommand = new AddUndoCommand(graphComp, this);
    _undoStack->push(addUndoCommand);
}

GraphicalConnection* ModelGraphicsScene::addGraphicalConnection(GraphicalComponentPort* sourcePort, GraphicalComponentPort* destinationPort, unsigned int portSourceConnection, unsigned int portDestinationConnection) {
    GraphicalConnection* graphicconnection = new GraphicalConnection(sourcePort, destinationPort, portSourceConnection, portDestinationConnection);

    addItem(graphicconnection);

    _graphicalConnections->append(graphicconnection);

    //para limpar referencias das conexoes no final
    _allGraphicalConnections.append(graphicconnection);

    //notify graphical model change
	GraphicalModelEvent* modelGraphicsEvent = new GraphicalModelEvent(GraphicalModelEvent::EventType::CREATE, GraphicalModelEvent::EventObjectType::CONNECTION, graphicconnection);
    dynamic_cast<ModelGraphicsView*> (views().at(0))->notifySceneGraphicalModelEventHandler(modelGraphicsEvent);

    return graphicconnection;
}

void ModelGraphicsScene::clearGraphicalModelConnections() {
    // limpa todas as referências das conexões no final
    for (GraphicalConnection* gmc : _allGraphicalConnections) {
        if (gmc) {
            // remove da lista de conexões graficas
            _graphicalConnections->removeOne(gmc);
            _allGraphicalConnections.removeOne(gmc);

            // libera o ponteiro alocado
            delete gmc;
        }
    }
}

void ModelGraphicsScene::addDrawing(QPointF endPoint, bool moving) {
    if (_drawingMode == LINE) {
        //verifica se a linha é muito pequena antes de desenhar
        if (abs(_drawingStartPoint.x() - endPoint.x()) > _grid.interval || abs(_drawingStartPoint.y() - endPoint.y()) > _grid.interval) {

            if (_currentLine != nullptr) {
                removeItem(_currentLine);
                delete _currentLine;
            }

            // Crie e adicione um nova linha à cena
            if (moving) {
                _currentLine = new QGraphicsLineItem(_drawingStartPoint.x(), _drawingStartPoint.y(), endPoint.x(), endPoint.y());
                addItem(_currentLine);
            } else {
                QGraphicsLineItem* line = new QGraphicsLineItem(_drawingStartPoint.x(), _drawingStartPoint.y(), endPoint.x(), endPoint.y());
                line->setFlag(QGraphicsItem::ItemIsSelectable, true);
                line->setFlag(QGraphicsItem::ItemIsMovable, true);
                getGraphicalDrawings()->append(line);
                addItem(line);
            }
        }

    } else if (_drawingMode == TEXT) {
        /*
        QGraphicsTextItem* textItem = new QGraphicsTextItem("Seu Texto Aqui");
        textItem->setFont(QFont("Arial", 12)); // Configurar a fonte (opcional)
        textItem->setPos(endPoint.x(), endPoint.y()); // Definir a posição na cena
        textItem->setDefaultTextColor(Qt::black); // Configurar a cor do texto
        textItem->setTextWidth(30*_grid.interval); // Definir largura máxima para quebrar o texto
        textItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
        textItem->setFlag(QGraphicsItem::ItemIsMovable, true);
        textItem->setVisible(true); */

        //connect(this, &QGraphicsScene::selectionChanged, this, &ModelGraphicsScene::startTextEditing);

    } else if (_drawingMode == RECTANGLE) {
        if (abs(_drawingStartPoint.x() - endPoint.x()) > _grid.interval && abs(_drawingStartPoint.y() - endPoint.y()) > _grid.interval) {
            // Remova o retângulo anterior, se houver
            qreal width = endPoint.x() - _drawingStartPoint.x();
            qreal height = endPoint.y() - _drawingStartPoint.y();
            if (_currentRectangle != nullptr) {
                removeItem(_currentRectangle);
                delete _currentRectangle;
            }

            // Crie e adicione um novo retângulo à cena
            if (moving) {
                _currentRectangle = new QGraphicsRectItem(_drawingStartPoint.x(), _drawingStartPoint.y(), width, height);
                addItem(_currentRectangle);
            } else {
                QGraphicsRectItem* rectangle = new QGraphicsRectItem(_drawingStartPoint.x(), _drawingStartPoint.y(), width, height);
                rectangle->setFlag(QGraphicsItem::ItemIsSelectable, true);
                rectangle->setFlag(QGraphicsItem::ItemIsMovable, true);
                getGraphicalDrawings()->append(rectangle);
                addItem(rectangle);
            }
        }
    } else if (_drawingMode == ELLIPSE) {
        if (abs(_drawingStartPoint.x() - endPoint.x()) > _grid.interval && abs(_drawingStartPoint.y() - endPoint.y()) > _grid.interval) {
            // Remova a ellipse anterior, se houver
            qreal width = endPoint.x() - _drawingStartPoint.x();
            qreal height = endPoint.y() - _drawingStartPoint.y();
            if (_currentEllipse != nullptr) {
                removeItem(_currentEllipse);
                delete _currentEllipse;
            }

            // Crie e adicione uma nova ellipse à cena
            if (moving) {
                _currentEllipse = new QGraphicsEllipseItem(_drawingStartPoint.x(), _drawingStartPoint.y(), width, height);
                addItem(_currentEllipse);
            } else {
                QGraphicsEllipseItem* ellipse = new QGraphicsEllipseItem(_drawingStartPoint.x(), _drawingStartPoint.y(), width, height);
                ellipse->setFlag(QGraphicsItem::ItemIsSelectable, true);
                ellipse->setFlag(QGraphicsItem::ItemIsMovable, true);
                getGraphicalDrawings()->append(ellipse);
                addItem(ellipse);
            }
        }
    } else if (_drawingMode == POLYGON) {
        // Adiciona o primeiro ponto do poligono
        _drawingMode = POLYGON_POINTS;
        _currentPolygonPoints.clear();
        _currentPolygonPoints << endPoint;
        _currentPolygon = new QGraphicsPolygonItem(QPolygonF(_currentPolygonPoints));
        _currentPolygon->setVisible(true);

        addItem(_currentPolygon);
    } else if (_drawingMode == POLYGON_POINTS) {
        removeItem(_currentPolygon);
        _currentPolygonPoints << endPoint;
        _currentPolygon = new QGraphicsPolygonItem(QPolygonF(_currentPolygonPoints));
        _currentPolygon->setVisible(true);
        addItem(_currentPolygon);
    } else if (_drawingMode == POLYGON_FINISHED) {
        _currentPolygon->setFlag(QGraphicsItem::ItemIsSelectable, true);
        _currentPolygon->setFlag(QGraphicsItem::ItemIsMovable, true);
        getGraphicalDrawings()->append(_currentPolygon);
    }
    // Redefina o estado de desenho
    if (!moving && !(_drawingMode == POLYGON) && !(_drawingMode == POLYGON_POINTS)) {
        _drawingMode = NONE;
        _currentRectangle = nullptr;
        _currentLine = nullptr;
        _currentEllipse = nullptr;
        _currentPolygon = nullptr;
    }
}

void ModelGraphicsScene::addAnimation() {

}

void ModelGraphicsScene::startTextEditing() {
    QGraphicsItem* selectedItem = focusItem();
    if (selectedItem) {
        QGraphicsTextItem* textItem = dynamic_cast<QGraphicsTextItem*>(selectedItem);
        if (textItem) {
            textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
            textItem->setFocus(); // Dê foco ao item para iniciar a edição de texto
        }
    }
}


void ModelGraphicsScene::removeModelComponentInModel(GraphicalModelComponent* gmc) {
	ModelComponent* component = gmc->getComponent();
	Model* model = _simulator->getModels()->current();
	model->getComponents()->remove(component);
}

void ModelGraphicsScene::clearGraphicalModelComponents() {
    QList<GraphicalModelComponent*> *componentsInModel = this->graphicalModelComponentItems();

    for (GraphicalModelComponent* gmc : *componentsInModel) {
        removeModelComponentInModel(gmc);
    }

    // limpa todos os componentes no final, desfazendo as conexoes
    for (GraphicalModelComponent* gmc : _allGraphicalModelComponents) {
        if (gmc) {
            for (GraphicalComponentPort* port : gmc->getGraphicalInputPorts()) {
                for (GraphicalConnection* graphConn : *port->getConnections()) {
                    removeGraphicalConnection(graphConn);
                }
            }

            for (GraphicalComponentPort* port : gmc->getGraphicalOutputPorts()) {
                for (GraphicalConnection* graphConn : *port->getConnections()) {
                    removeGraphicalConnection(graphConn);
                }
            }

            // remove da lista de componentes graficos
            _allGraphicalModelComponents.removeOne(gmc);

            // libera o ponteiro alocado
            delete gmc;
        }
    }
}

// esta funcao trata da remocao das conexoes de um componente
void ModelGraphicsScene::clearConnectionsComponent(GraphicalModelComponent* gmc) {
    ModelGraphicsScene::clearInputConnectionsComponent(gmc);
    ModelGraphicsScene::clearOutputConnectionsComponent(gmc);
}

// esta funcao trata da remocao das conexoes de entrada de um componente
void ModelGraphicsScene::clearInputConnectionsComponent(GraphicalModelComponent* gmc) {
    GraphicalConnection *graphConn; // conexao grafica
    GraphicalModelComponent *source; // origiem da conexao

    // varre todas as portas de entrada do componente a ser removido
    for (GraphicalComponentPort* port : gmc->getGraphicalInputPorts()) {
        if (port->getConnections()->size() > 0) {
            // pega a conexao dessa porta
            graphConn = port->getConnections()->at(0);

            if (graphConn != nullptr) {
                // pega a origem da conexao
                source = ModelGraphicsScene::findGraphicalModelComponent(graphConn->getSource()->component->getId());

                if (source != nullptr) {
                    // remove a conexao do componente origem da conexão (conexão gráfica)
                    source->getGraphicalOutputPorts().at(graphConn->getSource()->channel.portNumber)->removeGraphicalConnection(graphConn);
                    // remove a conexao do componente origem da conexão (conexão no modelo)
                    graphConn->getSource()->component->getConnections()->removeAtPort(graphConn->getSource()->channel.portNumber);
                    // uma porta de saida a menos esta sendo ocupada no componente de origem da conexao
                    source->setOcupiedOutputPorts(source->getOcupiedOutputPorts() - 1);
                    // uma porta de entrada a menos esta sendo ocupada no componente de destino da conexao
                    gmc->setOcupiedInputPorts(gmc->getOcupiedInputPorts() - 1);
                    // remove a conexao do componente destino da conexão, que seria o gmc (conexão gráfica)
                    // não remove do modelo, pois o modelo não guarda conexões de entrada
                    port->removeGraphicalConnection(graphConn);

                    // remove da tela
                    removeItem(graphConn);

                    // remove a conexão da lista
                    _graphicalConnections->removeOne(graphConn);
                }
            }
        }
    }

    //notify graphical model change
    GraphicalModelEvent* modelGraphicsEvent = new GraphicalModelEvent(GraphicalModelEvent::EventType::REMOVE, GraphicalModelEvent::EventObjectType::CONNECTION, nullptr); // notify AFTER destroy or BEFORE it?
    dynamic_cast<ModelGraphicsView*> (views().at(0))->notifySceneGraphicalModelEventHandler(modelGraphicsEvent);
}

void ModelGraphicsScene::removeComponent(GraphicalModelComponent* gmc) {
    //remove in model
    removeModelComponentInModel(gmc);

    //limpa as conexoes
    clearConnectionsComponent(gmc);

    //graphically
    removeItem(gmc);
    getGraphicalModelComponents()->removeOne(gmc);

    //notify graphical model change
    GraphicalModelEvent* modelGraphicsEvent = new GraphicalModelEvent(GraphicalModelEvent::EventType::REMOVE, GraphicalModelEvent::EventObjectType::COMPONENT, gmc);
    dynamic_cast<ModelGraphicsView*> (views().at(0))->notifySceneGraphicalModelEventHandler(modelGraphicsEvent);
}

// esta funcao trata da remocao das conexoes de saida de um componente
void ModelGraphicsScene::clearOutputConnectionsComponent(GraphicalModelComponent* gmc) {
    GraphicalConnection *graphConn; // conexao grafica
    GraphicalModelComponent *destination; // destino da conexao

    // varre todas as portas de saida do componente a ser removido
    for (GraphicalComponentPort* port : gmc->getGraphicalOutputPorts()) {
        if (port->getConnections()->size() > 0) {
            // pega a conexao dessa porta
            graphConn = port->getConnections()->at(0);

            if (graphConn != nullptr) {
                // pega o destino da conexao
                destination = this->findGraphicalModelComponent(graphConn->getDestination()->component->getId());

                if (destination != nullptr) {
                    // remove a conexao do componente de destino da conexão (conexão gráfica)
                    // não remove do modelo do componente de destino, pois o modelo não guarda conexões de entrada
                    destination->getGraphicalInputPorts().at(graphConn->getDestination()->channel.portNumber)->removeGraphicalConnection(graphConn);
                    // remove a conexao do componente origem da conexão, que seria o gmc (conexão gráfica)
                    port->removeGraphicalConnection(graphConn);

                    // remove a conexao do componente origem da conexão, que seria o gmc (conexão no modelo)
                    gmc->getComponent()->getConnections()->removeAtPort(graphConn->getSource()->channel.portNumber);

                    // uma porta de entrada a menos esta sendo ocupada no componente de destino da conexao
                    destination->setOcupiedInputPorts(destination->getOcupiedInputPorts() - 1);
                    // uma porta de saida a menos esta sendo ocupada no componente de origem da conexao
                    gmc->setOcupiedOutputPorts(gmc->getOcupiedOutputPorts() - 1);

                    // remove da tela
                    removeItem(graphConn);

                    // remove a conexão da lista
                    _graphicalConnections->removeOne(graphConn);
                }
            }
        }
    }

    //notify graphical model change
    GraphicalModelEvent* modelGraphicsEvent = new GraphicalModelEvent(GraphicalModelEvent::EventType::REMOVE, GraphicalModelEvent::EventObjectType::CONNECTION, nullptr); // notify AFTER destroy or BEFORE it?
    dynamic_cast<ModelGraphicsView*> (views().at(0))->notifySceneGraphicalModelEventHandler(modelGraphicsEvent);
}

// esta funcao trata da conexao dos componentes (necessario que ambos estejam no modelo)
void ModelGraphicsScene::connectComponents(GraphicalConnection* connection, GraphicalModelComponent *source, GraphicalModelComponent *destination) {
    // faz as conexoes
    ModelGraphicsScene::connectSource(connection, source);
    ModelGraphicsScene::connectDestination(connection, destination);

    // adiciona a conexao na lista de conexoes da cena
    _graphicalConnections->append(connection);

    //notify graphical model change
    GraphicalModelEvent* modelGraphicsEvent = new GraphicalModelEvent(GraphicalModelEvent::EventType::REMOVE, GraphicalModelEvent::EventObjectType::CONNECTION, nullptr); // notify AFTER destroy or BEFORE it?
    dynamic_cast<ModelGraphicsView*> (views().at(0))->notifySceneGraphicalModelEventHandler(modelGraphicsEvent);
}

// esta funcao trata da conexao com o componente de origem
bool ModelGraphicsScene::connectSource(GraphicalConnection* connection, GraphicalModelComponent *source) {
    GraphicalModelComponent *src;

    // necessario GraphicalModelComponent do componente, entao...
    // se enviarem o GraphicalModelComponent de origem nao precisa buscar pelo Id componente
    if (source != nullptr)
        src = source;
    // se nao enviarem o GraphicalModelComponent de origem, busca pelo Id do componente
    else
        src = findGraphicalModelComponent(connection->getSource()->component->getId());

    if (src != nullptr) {
        // varre todas as portas de saida do componente de origem, ate encontrar a porta correta
        for (GraphicalComponentPort* port : src->getGraphicalOutputPorts()) {
            // se encontrar a porta correta
            if (port->portNum() == connection->getSource()->channel.portNumber && port->getConnections()->isEmpty()) {
                // adiciona o componente grafico nessa porta
                port->addGraphicalConnection(connection);
                // adiciona a conexao no modelo do componente de origem
                src->getComponent()->getConnections()->insertAtPort(port->portNum(), connection->getDestination());
                break;
            }
        }

        // diz que o componente de origem tem mais uma porta de saida ocupada
        src->setOcupiedOutputPorts(src->getOcupiedOutputPorts() + 1);

        return true;
    }

    return false;
}

// esta funcao trata da conexao com o componente de destino
bool ModelGraphicsScene::connectDestination(GraphicalConnection* connection, GraphicalModelComponent *destination) {
    GraphicalModelComponent *dst;

    // necessario GraphicalModelComponent do componente, entao...
    // se enviarem o GraphicalModelComponent de destina nao precisa buscar pelo Id componente
    if (destination != nullptr)
        dst = destination;
    // se nao enviarem o GraphicalModelComponent de destino, busca pelo Id do componente
    else
        dst = this->findGraphicalModelComponent(connection->getDestination()->component->getId());

    if (dst != nullptr) {
        // varre todas as portas de entrada do componente de destino, ate encontrar a porta correta
        for (GraphicalComponentPort* port : dst->getGraphicalInputPorts()) {
            // se encontrar a porta correta
            if (port->portNum() == connection->getDestination()->channel.portNumber && port->getConnections()->isEmpty()) {
                // adiciona o componente grafico nessa porta
                port->addGraphicalConnection(connection);
                // nao e necessario adicionar a conexao no modelo do componente de destino, pois apenas os componentes de origem a possui
                break;
            }
        }

        // diz que o componente de destino tem mais uma porta de saida ocupada
        dst->setOcupiedInputPorts(dst->getOcupiedOutputPorts() + 1);

        return true;
    }

    return false;
}

void ModelGraphicsScene::removeConnectionInModel(GraphicalConnection* gc) {
	ModelComponent* sourceComp = gc->getSource()->component;
	sourceComp->getConnections()->removeAtPort(gc->getSource()->channel.portNumber);
}

void ModelGraphicsScene::removeGraphicalConnection(GraphicalConnection* gc) {
	// remove in model
    removeConnectionInModel(gc);

    // remove graphically
	removeItem(gc);
    _graphicalConnections->removeOne(gc);

	//notify graphical model change
	GraphicalModelEvent* modelGraphicsEvent = new GraphicalModelEvent(GraphicalModelEvent::EventType::REMOVE, GraphicalModelEvent::EventObjectType::CONNECTION, nullptr); // notify AFTER destroy or BEFORE it?
	dynamic_cast<ModelGraphicsView*> (views().at(0))->notifySceneGraphicalModelEventHandler(modelGraphicsEvent);
}

void ModelGraphicsScene::removeDrawing(QGraphicsItem * item) {
    for (int i = 0 ; i < getGraphicalDrawings()->size(); i++) {
        if (getGraphicalDrawings()->at(i) == item) {
            getGraphicalDrawings()->removeAt(i);
            //removeItem(item);
            delete(item);
        }
    }

    //notify graphical model change
    GraphicalModelEvent* modelGraphicsEvent = new GraphicalModelEvent(GraphicalModelEvent::EventType::CREATE, GraphicalModelEvent::EventObjectType::COMPONENT, item);
    dynamic_cast<ModelGraphicsView*> (views().at(0))->notifySceneGraphicalModelEventHandler(modelGraphicsEvent);
}

void ModelGraphicsScene::removeAnimation() {}


//------------------------------------------------------------------------


// retorna o elemento _grid que é privado
ModelGraphicsScene::GRID *ModelGraphicsScene::grid() {
    return &_grid;
}

// implementação da função clear() da estrutura GRID
void ModelGraphicsScene::GRID::clear() {
    // limpa e libera a memória da lista de linhas
    for (QGraphicsLineItem *line : *lines) {
        delete line;
    }
    lines->clear();

    // volta a visibilidade pra false
    visible = false;
}

void ModelGraphicsScene::showGrid()
{
    // pego a informação se o grid está visível
    // obs.: o grid é criado uma única vez para a cena e habilitado como visível ou não. =

    // se eu quero que o grid fique visível, verifico se o grid já está desenhado ou não
    if (_grid.visible) {
        // se não tenho linhas no grid, eu as desenho
        if (_grid.lines->size() <= 0) {
            // add new grid
            for (int i = sceneRect().left(); i < sceneRect().right(); i += _grid.interval) {
                QGraphicsLineItem *line = addLine(i, sceneRect().top(), i, sceneRect().bottom(), _grid.pen);
                line->setZValue(-1.0);
                line->setVisible(true);
                _grid.lines->insert(_grid.lines->end(), line);
            }
            for (int j = sceneRect().top(); j < sceneRect().bottom(); j += _grid.interval) {
                QGraphicsLineItem *line = addLine(sceneRect().left(), j, sceneRect().right(), j, _grid.pen);
                line->setZValue(-1.0);
                line->setVisible(true);
                _grid.lines->insert(_grid.lines->end(), line);
            }
        }
        // se eu já tenho meu grid desenhado eu apenas o torno visível
        else {
            for (QGraphicsLineItem *line : *_grid.lines) {
                line->setVisible(true);
            }
        }
    }
    // se eu quero esconder o grid eu tiro a visibilidade das linhas
    else {
        for (QGraphicsLineItem *line : *_grid.lines) {
            line->setVisible(false);
        }
    }

    // troco o valor de visible
    _grid.visible = !_grid.visible;
}

void ModelGraphicsScene::setSnapToGrid(bool activated)
{
    _snapToGrid = activated;
}

bool ModelGraphicsScene::getSnapToGrid() {
    return _snapToGrid;
}

void ModelGraphicsScene::snapItemsToGrid()
{
    if (_snapToGrid) {
        // Obtenha a lista de visualizações associadas a esta cena

        QList<QGraphicsItem*>* items = getGraphicalModelComponents();
        int num_items = items->size();

        for (int i = 0; i < num_items; i++) {
            QGraphicsItem* item = items->at(i);

            GraphicalModelComponent* modelItem = dynamic_cast<GraphicalModelComponent*>(item);
            if (modelItem) {
                // Obtenha a posição atual do item
                QPointF itemPos = modelItem->pos();

                // Calcule a nova posição ajustada ao grid
                qreal x = qRound(itemPos.x() / _grid.interval) * _grid.interval;
                qreal y = qRound(itemPos.y() / _grid.interval) * _grid.interval;


                // Verifique se a nova posição está dentro dos limites da cena
                if (x < sceneRect().left()) {
                    x = sceneRect().left();
                }
                else if (x > sceneRect().right()) {
                    x = sceneRect().right();
                }
                if (y < sceneRect().top()) {
                    y = sceneRect().top();
                }
                else if (y > sceneRect().bottom()) {
                    y = sceneRect().bottom();
                }

                //Defina a nova posição ajustada ao grid
                modelItem->setPos(x, y);
            }
        }
    }
}

QUndoStack* ModelGraphicsScene::getUndoStack() {
    return _undoStack;
}

Simulator* ModelGraphicsScene::getSimulator() {
    return _simulator;
}

void ModelGraphicsScene::setUndoStack(QUndoStack* undo) {
    _undoStack = undo;
}


void ModelGraphicsScene::beginConnection() {
	_connectingStep = 1;
	((QGraphicsView*)this->parent())->setCursor(Qt::CrossCursor);
}

void ModelGraphicsScene::groupComponents() {
    int size = selectedItems().size();
    int num_groups = getGraphicalGroups()->size();
    //verifica se algum item selecionado já faz parte de um grupo
    bool component_in_group = false;
    if (size > 1 && num_groups > 0) {
        for (int i = 0; (i < size) && !component_in_group; i++) {  //percorrer todos os itens selecionados
            QGraphicsItem* c = selectedItems().at(i);
            int group_children = c->childItems().size();
            if (group_children > 1) {
                component_in_group = true;
            }
        }
    }
    if (!component_in_group) {

        QList<QGraphicsItem*> group = selectedItems();

        QGraphicsItemGroup* new_group = new QGraphicsItemGroup();

        for (int i = 0; i < group.size(); i++) {
            QGraphicsItem* c = group.at(i);
            new_group->addToGroup(c);
        }

        // Adicione o novo grupo à sua cena
        addItem(new_group);
        new_group->setFlag(QGraphicsItem::ItemIsSelectable, true);
        new_group->setFlag(QGraphicsItem::ItemIsMovable, true);
        // Adicione o grupo à sua lista de grupos (se necessário)
        getGraphicalGroups()->append(new_group);
    }
}

void ModelGraphicsScene::ungroupComponents() {
    int size = selectedItems().size();
    if (size == 1) {
        QGraphicsItem* item = selectedItems().at(0);
        QGraphicsItemGroup* group = dynamic_cast<QGraphicsItemGroup*>(item);

        if (group) {
            // Recupere os itens individuais no grupo
            QList<QGraphicsItem*> itemsInGroup = group->childItems();


            // Adicione novamente os itens individuais à cena
            for (int i = 0; i < itemsInGroup.size(); i++) {
                QGraphicsItem * item = itemsInGroup.at(i);
                //remova item por item do grupo
                group->removeFromGroup(item);
                //adicionar novamente a cena
                addItem(item);
                item->setFlag(QGraphicsItem::ItemIsSelectable, true);
                item->setFlag(QGraphicsItem::ItemIsMovable, true);
            }
            // Remova o grupo da cena
            removeItem(group);
            delete group;

        }
    }
    selectedItems().clear();
}


void ModelGraphicsScene::arranjeModels(int direction) {
    int size = selectedItems().size();
    qreal most_direction;
    qreal most_up;
    qreal most_down;
    qreal most_left;
    qreal most_right;
    qreal middle;
    qreal center;

    if (size >= 2) {
        switch (direction) {
        case 0: //left
            most_direction = sceneRect().right();
            break;
        case 1: //right
            most_direction = sceneRect().left();
            break;
        case 2: //top
            most_direction = sceneRect().bottom();
            break;
        case 3: //bottom
            most_direction = sceneRect().top();
            break;
        case 4: //center
            most_left = sceneRect().right();
            most_right = sceneRect().left();
            for (int i =0; i < size; i++) {
                GraphicalModelComponent* c = dynamic_cast<GraphicalModelComponent*> (selectedItems().at(i));
                qreal item_posX = c->x();
                if (item_posX < most_left) {
                    most_left = item_posX;
                }
                if (item_posX > most_right) {
                    most_right = item_posX;
                }
            }
            center = (most_right + most_left) / 2;
            for (int i =0; i < size; i++) {
                GraphicalModelComponent* c = dynamic_cast<GraphicalModelComponent*> (selectedItems().at(i));
                c->setX(center);
            }
            break;
        case 5: //middle
            most_up = sceneRect().bottom();
            most_down = sceneRect().top();
            for (int i =0; i < size; i++) {
                GraphicalModelComponent* c = dynamic_cast<GraphicalModelComponent*> (selectedItems().at(i));
                qreal item_posY = c->y();
                if (item_posY < most_up) {
                    most_up = item_posY;
                }
                if (item_posY > most_down) {
                    most_down = item_posY;
                }
            }
            middle = (most_up + most_down) / 2;
            for (int i =0; i < size; i++) {
                GraphicalModelComponent* c = dynamic_cast<GraphicalModelComponent*> (selectedItems().at(i));
                c->setY(middle);
            }
            break;
        }
        if (direction < 4) {
            for (int i =0; i < size; i++) {
                GraphicalModelComponent* c = dynamic_cast<GraphicalModelComponent*> (selectedItems().at(i));
                if (direction < 2) {
                    qreal item_pos = c->x();
                    if ((item_pos < most_direction && direction == 0) || (item_pos > most_direction && direction == 1) ) {
                        most_direction = item_pos;
                    }
                } else {
                    qreal item_pos = c->y();
                    if ((item_pos < most_direction && direction == 2) || (item_pos > most_direction && direction == 3) ) {
                        most_direction = item_pos;
                    }
                }
            }
            if (direction < 2) {
                for (int i =0; i < size; i++) {
                    GraphicalModelComponent* c = dynamic_cast<GraphicalModelComponent*> (selectedItems().at(i));
                    c->setX(most_direction);
                }
            } else {
                for (int i =0; i < size; i++) {
                    GraphicalModelComponent* c = dynamic_cast<GraphicalModelComponent*> (selectedItems().at(i));
                    c->setY(most_direction);
                }
            }
        }
    }
}


//-------------------------
// PROTECTED VIRTUAL FUNCTIONS
//-------------------------

void ModelGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) {
	QGraphicsScene::mousePressEvent(mouseEvent);
    if (mouseEvent->button() == Qt::LeftButton) {

        QGraphicsItem* item = this->itemAt(mouseEvent->scenePos(), QTransform());

        if (GraphicalModelComponent *component = dynamic_cast<GraphicalModelComponent *> (item)) {
            component->setOldPosition(component->scenePos());
        }

        if (_connectingStep > 0) {
            if (item != nullptr) {
                GraphicalComponentPort* port = dynamic_cast<GraphicalComponentPort*> (item);
                if (port != nullptr) {
                    if (_connectingStep == 1 && !port->isInputPort()) {
                        _sourceGraphicalComponentPort = port;
                        _connectingStep = 2;
                        return;
                    } else if (_connectingStep == 2 && port->isInputPort()) {
                        _connectingStep = 3;
                        // create connection
                        // in the model
                        ModelComponent* sourceComp = _sourceGraphicalComponentPort->graphicalComponent()->getComponent();
                        ModelComponent* destComp = port->graphicalComponent()->getComponent();
                        sourceComp->getConnections()->insertAtPort(_sourceGraphicalComponentPort->portNum(), new Connection({destComp, port->portNum()}));
                        // graphically
                        GraphicalConnection* graphicconnection = new GraphicalConnection(_sourceGraphicalComponentPort, port);
                        _sourceGraphicalComponentPort->addGraphicalConnection(graphicconnection);
                        port->addGraphicalConnection(graphicconnection);
                        addItem(graphicconnection);
                        //
                        ((ModelGraphicsView *) (this->parent()))->unsetCursor();
                        _connectingStep = 0;
                        return;
                    }
                }
            }
            ((ModelGraphicsView *) (this->parent()))->unsetCursor();
            _connectingStep = 0;
        } else if (_drawingMode != NONE) {
            // Capturar o ponto de início do desenho
            _drawingStartPoint = mouseEvent->scenePos();
            _currentRectangle = nullptr;
            _currentLine = nullptr;
            _currentEllipse = nullptr;

            if (_drawingMode == POLYGON) {
                // Cria o poligono
                addDrawing(_drawingStartPoint, false);
            } else if (_drawingMode == POLYGON_POINTS) {
                // Continue a adicionar pontos ao polígono
                addDrawing(mouseEvent->scenePos(), false);
            } else if (_drawingMode == TEXT) {
                addDrawing(mouseEvent->scenePos(), false);
            }
        }
    }
}

void ModelGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    QGraphicsScene::mouseReleaseEvent(mouseEvent);

    snapItemsToGrid();

    QList<GraphicalModelComponent*> components;
    QList<QPointF> oldPositions;
    QList<QPointF> newPositions;

    foreach (QGraphicsItem* item, this->selectedItems()) {
        GraphicalModelComponent* component = dynamic_cast<GraphicalModelComponent*>(item);
        if (component && component->getOldPosition() != component->scenePos()) {
            components.append(component);
            oldPositions.append(component->getOldPosition());
            newPositions.append(component->scenePos());
        }
    }

    if (components.size() >= 1) {
        QUndoCommand *moveUndoCommand = new MoveUndoCommand(components, this, oldPositions, newPositions);
        _undoStack->push(moveUndoCommand);
    }

    foreach (GraphicalModelComponent* item, components) {
        item->setOldPosition(item->scenePos());
    }


    if (mouseEvent->button() == Qt::LeftButton && _drawingMode != NONE) {
        // Capturar o ponto final da linha
        QPointF drawingEndPoint = mouseEvent->scenePos();
        //Adicionar desenho a tela
        addDrawing(drawingEndPoint, false);
        ((ModelGraphicsView *) (this->parent()))->unsetCursor();
    }
}

void ModelGraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent) {
	QGraphicsScene::mouseDoubleClickEvent(mouseEvent);
	GraphicalComponentPort* port = dynamic_cast<GraphicalComponentPort*> (this->itemAt(mouseEvent->scenePos(), QTransform()));
    if (port != nullptr) { // if doubleclick on a port, then start connecting
		if (!port->isInputPort() && this->_connectingStep == 0) {
			_sourceGraphicalComponentPort = port;
			_connectingStep = 2;

		}
    }
    if (_drawingMode == POLYGON || _drawingMode == POLYGON_POINTS) {
        _drawingMode = POLYGON_FINISHED;
    }
}

void ModelGraphicsScene::wheelEvent(QGraphicsSceneWheelEvent *wheelEvent) {
	QGraphicsScene::wheelEvent(wheelEvent);
    if (_controlIsPressed){
        if (wheelEvent->delta() > 0){
            ((ModelGraphicsView *)(this->parent()))->notifySceneWheelInEventHandler();
        }
        else{
            ((ModelGraphicsView *)(this->parent()))->notifySceneWheelOutEventHandler();
        }
        wheelEvent->accept();
    }
}

QList<QGraphicsItem*>*ModelGraphicsScene::getGraphicalEntities() const {
	return _graphicalEntities;
}

QList<QGraphicsItem*>*ModelGraphicsScene::getGraphicalAnimations() const {
	return _graphicalAnimations;
}

QList<QGraphicsItem*>*ModelGraphicsScene::getGraphicalDrawings() const {
	return _graphicalDrawings;
}

QList<QGraphicsItem*>*ModelGraphicsScene::getGraphicalConnections() const {
	return _graphicalConnections;
}

QList<QGraphicsItem*>*ModelGraphicsScene::getGraphicalModelComponents() const {
	return _graphicalModelComponents;
}

QList<QGraphicsItemGroup*>*ModelGraphicsScene::getGraphicalGroups() const {
    return _graphicalGroups;
}

void ModelGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) {
	QGraphicsScene::mouseMoveEvent(mouseEvent);
	((ModelGraphicsView *) (this->parent()))->notifySceneMouseEventHandler(mouseEvent); // to show coords
	if (_connectingStep > 0) {
		QGraphicsItem* item = this->itemAt(mouseEvent->scenePos(), QTransform());
		if (item != nullptr) {
			GraphicalComponentPort* port = dynamic_cast<GraphicalComponentPort*> (item);
			if (port != nullptr) {
				if (_connectingStep == 1 && !port->isInputPort()) {
					((ModelGraphicsView *) (this->parent()))->setCursor(Qt::PointingHandCursor);
				} else if (_connectingStep == 2 && port->isInputPort()) {
					((ModelGraphicsView *) (this->parent()))->setCursor(Qt::PointingHandCursor);
				}
				return;
			}
		}
		if (_connectingStep > 1) {
			((ModelGraphicsView *) (this->parent()))->setCursor(Qt::ClosedHandCursor);
		} else {
            ((ModelGraphicsView *) (this->parent()))->setCursor(Qt::CrossCursor);
        }
    } else if (_drawingMode != NONE && _drawingMode != POLYGON && _drawingMode != POLYGON_POINTS){
        //mostrar desenho se formando
        QPointF currentPoint = mouseEvent->scenePos();
        addDrawing(currentPoint, true);
        ((ModelGraphicsView *) (this->parent()))->setCursor(Qt::SplitHCursor);
        /*if (_drawingMode == LINE) {
            QPointF currentPoint = mouseEvent->scenePos();
            addDrawing(currentPoint, true);
            ((ModelGraphicsView *) (this->parent()))->setCursor(Qt::SplitHCursor);
        } else if (_drawingMode == RECTANGLE) {
            QPointF currentPoint = mouseEvent->scenePos();
            addDrawing(currentPoint, true);
            ((ModelGraphicsView *) (this->parent()))->setCursor(Qt::SplitHCursor);
        } else if (_drawingMode == ELLIPSE) {
            QPointF currentPoint = mouseEvent->scenePos();
            addDrawing(currentPoint, true);
            ((ModelGraphicsView *) (this->parent()))->setCursor(Qt::ClosedHandCursor);
        }*/
    }
}

void ModelGraphicsScene::focusInEvent(QFocusEvent *focusEvent) {
	QGraphicsScene::focusInEvent(focusEvent);
}

void ModelGraphicsScene::focusOutEvent(QFocusEvent *focusEvent) {
	QGraphicsScene::focusOutEvent(focusEvent);
}

void ModelGraphicsScene::dropEvent(QGraphicsSceneDragDropEvent *event) {
	QGraphicsScene::dropEvent(event);
	if (this->_objectBeingDragged != nullptr) {
        QTreeWidgetItem*    treeItem = /*dynamic_cast<QTreeWidgetItem*>*/(_objectBeingDragged);
		if (treeItem != nullptr) {			
			QColor color = treeItem->foreground(0).color(); // treeItem->textColor(0);
			QString pluginname = treeItem->whatsThis(0);
			Plugin* plugin = _simulator->getPlugins()->find(pluginname.toStdString());
			if (plugin != nullptr) {
				if (plugin->getPluginInfo()->isComponent()) {
					event->setDropAction(Qt::IgnoreAction);
					event->accept();
					// create component in the model
					ModelComponent* component = (ModelComponent*) plugin->newInstance(_simulator->getModels()->current());
					// create graphically
					addGraphicalModelComponent(plugin, component, event->scenePos(), color);
					return;
				}
			}
		}
	}
	event->setAccepted(false);
}

void ModelGraphicsScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *contextMenuEvent) {
	QGraphicsScene::contextMenuEvent(contextMenuEvent);
}

void ModelGraphicsScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event) {
	QGraphicsScene::dragEnterEvent(event);
	//QString name;
	//Plugin* plugin = _simulator->getPlugins()->find(name.toStdString());
	//if(true) {//(plugin != nullptr) {
	event->setDropAction(Qt::CopyAction);
	event->accept();
	//}
}

void ModelGraphicsScene::dragLeaveEvent(QGraphicsSceneDragDropEvent *event) {
	QGraphicsScene::dragLeaveEvent(event);
	event->setDropAction(Qt::CopyAction);
	event->accept();
}

void ModelGraphicsScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event) {
	QGraphicsScene::dragMoveEvent(event);
	event->setDropAction(Qt::CopyAction);
	event->accept();
}

void ModelGraphicsScene::keyPressEvent(QKeyEvent *keyEvent) {
	QGraphicsScene::keyPressEvent(keyEvent);
	QList<QGraphicsItem*> selected = this->selectedItems();
	if (keyEvent->key() == Qt::Key_Delete && selected.size() > 0) {
		QMessageBox::StandardButton reply = QMessageBox::question(this->_parentWidget, "Delete Component", "Are you sure you want to delete the selected components?", QMessageBox::Yes | QMessageBox::No);
		if (reply == QMessageBox::No) {
			return;
        }


        QUndoCommand *deleteUndoCommand = new DeleteUndoCommand(selected, this);
        _undoStack->push(deleteUndoCommand);

//		for (QGraphicsItem* item : selected) {
//			GraphicalModelComponent* gmc = dynamic_cast<GraphicalModelComponent*> (item);
//			if (gmc != nullptr) {
//				// graphically
//                removeComponent(gmc);
//			} else {
//				GraphicalConnection* gc = dynamic_cast<GraphicalConnection*> (item);
//				if (gc != nullptr) {
//                    removeGraphicalConnection(gc);
//                } else {
//                    bool aux = getGraphicalDrawings()->contains(item);
//                    if (aux) {
//                        removeDrawing(item);
//                    }
//                    //Drawing* gc = dynamic_cast<GraphicalConnection*>(item);
//                    //if (gc != nullptr) {
//                    //	removeItem(item);
//                    //	gc->~Drawing();
//                    //}
//                }
//            }

//		}
	}
	_controlIsPressed = (keyEvent->key() == Qt::Key_Control);
}

void ModelGraphicsScene::keyReleaseEvent(QKeyEvent *keyEvent) {
	QGraphicsScene::keyReleaseEvent(keyEvent);
	if (_controlIsPressed)
		_controlIsPressed = (keyEvent->key() != Qt::Key_Control);
}

//--------------------------
//
//--------------------------

void ModelGraphicsScene::setObjectBeingDragged(QTreeWidgetItem* objectBeingDragged) {
	_objectBeingDragged = objectBeingDragged;
}

void ModelGraphicsScene::setSimulator(Simulator *simulator) {
	_simulator = simulator;
}

unsigned short ModelGraphicsScene::connectingStep() const {
	return _connectingStep;
}

void ModelGraphicsScene::setConnectingStep(unsigned short connectingStep) {
	_connectingStep = connectingStep;
}

void ModelGraphicsScene::setParentWidget(QWidget *parentWidget) {
	_parentWidget = parentWidget;
}

void ModelGraphicsScene::setDrawingMode(DrawingMode drawingMode) {
    _drawingMode = drawingMode;
}

QList<GraphicalModelComponent*>* ModelGraphicsScene::graphicalModelComponentItems(){
    QList<GraphicalModelComponent*>* list = new QList<GraphicalModelComponent*>();
    for(QGraphicsItem* item: this->items()) {
        GraphicalModelComponent* gmc = dynamic_cast<GraphicalModelComponent*>(item);
        if (gmc != nullptr) {
            list->append(gmc);
        }
    }
    return list;
}

GraphicalModelComponent* ModelGraphicsScene::findGraphicalModelComponent(Util::identification id){
    QList<GraphicalModelComponent*> allComponents = ModelGraphicsScene::_allGraphicalModelComponents;

    for(GraphicalModelComponent* item: allComponents) {
        if (item->getComponent()->getId() == id) {
            return item;
        }
    }
    return nullptr;
}

//------------------------
// Private
//------------------------

