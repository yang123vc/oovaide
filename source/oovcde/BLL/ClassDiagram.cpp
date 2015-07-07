/*
 * ClassDiagram.cpp
 *
 *  Created on: Jul 24, 2013
 *  \copyright 2013 DCBlaha.  Distributed under the GPL.
 */

#include "ClassDiagram.h"
#include "ClassDrawer.h"
#include "DiagramStorage.h"
#include <algorithm>
#include <climits>


void ClassDiagram::initialize(ModelData const &modelData,
        DiagramDrawer &nullDrawer, ClassDrawOptions const &options,
        ClassGraphListener &graphListener,
        OovTaskStatusListener &foregroundTaskStatusListener,
        OovTaskStatusListener &backgroundTaskStatusListener)
    {
    mModelData = &modelData;
    mClassGraph.initialize(options, nullDrawer, graphListener,
            foregroundTaskStatusListener, backgroundTaskStatusListener);
    }

void ClassDiagram::updateGraph(bool reposition)
    {
    mClassGraph.updateGraph(getModelData(), reposition);
    }

void ClassDiagram::restart()
    {
    clearGraphAndAddClass(getLastSelectedClassName(),
            ClassGraph::AN_ClassesAndChildren);
    updateGraph(true);
    }

void ClassDiagram::clearGraphAndAddClass(OovStringRef const className,
        ClassGraph::eAddNodeTypes addType, int depth, bool reposition)
    {
    mClassGraph.clearGraph();
    addClass(className, addType, depth, reposition);
    setLastSelectedClassName(className);
    }

void ClassDiagram::addClass(OovStringRef const className,
        ClassGraph::eAddNodeTypes addType, int depth, bool reposition)
    {
    mClassGraph.addNode(getModelData(), className, addType, depth, reposition);
    }

void ClassDiagram::drawDiagram(DiagramDrawer &diagDrawer)
    {
    ClassDrawer drawer(diagDrawer);
    drawer.setZoom(getDesiredZoom());
    drawer.drawDiagram(mClassGraph);
    }

void ClassDiagram::saveDiagram(FILE *fp)
    {
    NameValueFile file;

    CompoundValue names;
    CompoundValue xPositions;
    CompoundValue yPositions;
    OovString drawingName;
    for(auto const &node : mClassGraph.getNodes())
        {
        if(node.getType())
            {
            if(drawingName.length() == 0)
                {
                drawingName = node.getType()->getName();
                }
            names.addArg(node.getType()->getName());
            }
        else
            {
            names.addArg("Oov-Key");
            }
        OovString num;
        num.appendInt(node.getPosition().x);
        xPositions.addArg(num);

        num.clear();
        num.appendInt(node.getPosition().y);
        yPositions.addArg(num);
        }

    if(drawingName.length() > 0)
        {
        DiagramStorage::setDrawingHeader(file, DST_Class, drawingName);
        file.setNameValue("Names", names.getAsString());
        file.setNameValue("XPositions", xPositions.getAsString());
        file.setNameValue("YPositions", yPositions.getAsString());
        file.writeFile(fp);
        }
    }

void ClassDiagram::loadDiagram(FILE *fp)
    {
    NameValueFile file;
    file.readFile(fp);
    CompoundValue names;
    names.parseString(file.getValue("Names"));
    CompoundValue xPositions;
    xPositions.parseString(file.getValue("XPositions"));
    CompoundValue yPositions;
    yPositions.parseString(file.getValue("YPositions"));
    std::vector<ClassNode> &nodes = getNodes();
    for(size_t i=0; i<names.size(); i++)
        {
        OovString name = names[i];
        if(i == 0)
            {
            // The node at index zero is the graph key, and is not stored in
            // the graph with a name or type.
            // The node at index one is the first class, which is typically the
            // same as the drawing name.
            eDiagramStorageTypes drawingType;
            OovString drawingName;
            DiagramStorage::getDrawingHeader(file, drawingType, drawingName);
            // This adds the key automatically as item index zero.
            // Call this function to set the last selected class name for the journal.
            clearGraphAndAddClass(drawingName, ClassGraph::AN_All,
                    ClassDiagram::DEPTH_SINGLE_CLASS, false);
            int x=0;
            int y=0;
            xPositions[i].getInt(0, INT_MAX, x);
            yPositions[i].getInt(0, INT_MAX, y);
            nodes[0].setPosition(GraphPoint(x, y));
            }
        else
            {
            // This will not add duplicates, so if the name is different
            // from the drawingName, it will be added.
            addClass(name, ClassGraph::AN_All,
                ClassDiagram::DEPTH_SINGLE_CLASS, false);
            }

        auto nodeIter = std::find_if(nodes.begin(), nodes.end(),
            [&name](ClassNode &node)
            { return(node.getType() && name == node.getType()->getName()); });
        if(nodeIter != nodes.end())
            {
            int x=0;
            int y=0;
            xPositions[i].getInt(0, INT_MAX, x);
            yPositions[i].getInt(0, INT_MAX, y);
            nodeIter->setPosition(GraphPoint(x, y));
            }
        }
    }

ClassNode *ClassDiagram::getNode(int x, int y)
    {
    return mClassGraph.getNode(static_cast<int>(x / getDesiredZoom()),
        static_cast<int>(y / getDesiredZoom()));
    }
