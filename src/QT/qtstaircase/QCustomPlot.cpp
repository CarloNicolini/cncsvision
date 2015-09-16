// This file is part of CNCSVision, a computer vision related library
// This software is developed under the grant of Italian Institute of Technology
//
// Copyright (C) 2010-2014 Carlo Nicolini <carlo.nicolini@iit.it>
//
//
// CNCSVision is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
//
// Alternatively, you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CNCSVision is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License or the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License and a copy of the GNU General Public License along with
// CNCSVision. If not, see <http://www.gnu.org/licenses/>.

/***************************************************************************
**                                                                        **
**  QCustomPlot, a simple to use, modern plotting widget for Qt           **
**  Copyright (C) 2012 Emanuel Eichhammer                                 **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Emanuel Eichhammer                                   **
**  Website/Contact: http://www.WorksLikeClockwork.com/                   **
**             Date: 02.02.12                                             **
****************************************************************************/

/*! \mainpage %QCustomPlot Documentation
 
  Below is a brief overview of and guide to the classes and their relations. If you are new to
  QCustomPlot and just want to start using it, it's recommended to look at the examples/tutorials
  at
 
  http://www.WorksLikeClockWork.com/index.php/components/qt-plotting-widget
 
  This documentation is especially helpful when the basic concept of how to use %QCustomPlot is
  clear and you wish to learn more about specific functionality.
 
  \section quickguide Quick Guide
  
  \image html ClassesOverview.png
  
  The central widget which displays the plottables and axes on its surface is QCustomPlot. Usually,
  you don't create the axes yourself, but you use the ones already inside every QCustomPlot
  instance (xAxis, yAxis, xAxis2, yAxis2).

  \section plottables Plottables
  
  \a Plottables are classes that display any kind of data inside the QCustomPlot. They all derive
  from QCPAbstractPlottable. For example, the QCPGraph class is a plottable that displays a graph
  inside the plot with different line styles, scatter styles, filling etc.
  
  Since plotting graphs is such a dominant use case, QCustomPlot has a special interface for working
  with QCPGraph plottables, that makes it very easy to handle them:\n
  You create a new graph with QCustomPlot::addGraph and access them with QCustomPlot::graph.
  
  For all other plottables, you need to use the normal plottable interface:\n
  First, you create an instance of the plottable you want, e.g.
  \code
  QCPCurve *newCurve = new QCPCurve(customPlot->xAxis, customPlot->yAxis);\endcode
  add it to the customPlot with QCustomPlot::addPlottable:
  \code
  customPlot->addPlottable(newCurve);\endcode
  and then modify the properties of the newly created plottable via <tt>newCurve</tt>.
  
  Plottables (including graphs) can be retrieved via QCustomPlot::plottable. Since the return
  type of that function is the abstract base class of all plottables, QCPAbstractPlottable, you
  will probably want to dynamic_cast the returned pointer to the respective plottable subclass.
  (if the dynamic cast returns zero, the plottable wasn't of that specific subclass.)
  
  All further interfacing with plottables (e.g how to set data) is specific to the plottable type.
  See the documentations of the subclasses: QCPGraph, QCPCurve, QCPBars, QCPStatisticalBox.

  \section axes Controlling the Axes
  
  As mentioned, QCustomPlot has four axes by default: \a xAxis (bottom), \a yAxis (left), \a xAxis2
  (top), \a yAxis2 (right).
  
  Their range is handled by the simple QCPRange class. You can set the range with the
  QCPAxis::setRange function. By default, the axes represent a linear scale. To set a logarithmic
  scale, set QCPAxis::setScaleType to QCPAxis::stLogarithmic. The logarithm base can be set freely
  with QCPAxis::setScaleLogBase.
  
  By default, an axis automatically creates and labels ticks in a sensible manner, i.e. with a tick
  interval that's pleasing to the viewer. See the following functions for tick manipulation:\n
  QCPAxis::setTicks, QCPAxis::setAutoTicks, QCPAxis::setAutoTickCount, QCPAxis::setAutoTickStep,
  QCPAxis::setTickLabels, QCPAxis::setTickLabelType, QCPAxis::setTickLabelRotation,
  QCPAxis::setTickStep, QCPAxis::setTickLength,...
  
  Each axis can be given an axis label (e.g. "Voltage [mV]") with QCPAxis::setLabel.
  
  The distance of an axis backbone to the respective QCustomPlot widget border is called its margin.
  Normally, the margins are calculated automatically. To change this, set QCustomPlot::setAutoMargin
  to false and set the margins manually with QCustomPlot::setMargin.
  
  \section legend Plot Legend
  
  Every QCustomPlot owns a QCPLegend (as \a legend). That's a small window inside the plot which
  lists the plottables with an icon of the plottable line/symbol and a description. The Description
  is retrieved from the plottable name (QCPAbstractPlottable::setName). Plottables can be added and
  removed from the legend via \ref QCPAbstractPlottable::addToLegend and \ref
  QCPAbstractPlottable::removeFromLegend. By default, adding a plottable to QCustomPlot
  automatically adds it to the legend, too. This behaviour can be modified with the
  QCustomPlot::setAutoAddPlottableToLegend property.
  
  The QCPLegend provides an interface to access, add and remove legend items directly, too. See
  QCPLegend::item, QCPLegend::itemWithPlottable, QCPLegend::addItem, QCPLegend::removeItem for
  example.
  
  \section performancetweaks Performance Tweaks
  
  Although QCustomPlot is quite fast, some features (transparent fills and antialiasing) cause a
  significant slow down. Here are some thoughts on how to increase performance:
  
  Most performance gets lost in the drawing functions, specifically the drawing of graphs. For
  maximum performance, consider the following (in no particular order):
  
  \li avoid any kind of alpha (transparency), especially in fills \li avoid any kind of
  antialiasing, especially in graph lines (QCustomPlot::setAntialiasedElements)
  \li avoid repeatedly setting the complete data set with setData. Use addData instead, if most
  data points stay unchanged, e.g. in a running measurement.
  \li set the \a copy parameter of the setData functions to false, so only pointers get
  transferred.
  \li on X11 (linux), avoid the (slow) native drawing system, use raster by supplying
  "-graphicssystem raster" as command line argument or calling
  QApplication::setGraphicsSystem("raster") before creating the QApplication object.
  \li on all operating systems, use OpenGL hardware acceleration by supplying "-graphicssystem
  opengl" as command line argument or calling QApplication::setGraphicsSystem("opengl"). If OpenGL
  is available, this will slightly decrease the quality of antialiasing, but extremely increase
  performance especially with alpha (transparent) fills and a large QCustomPlot drawing surface.
  Note however, that the maximum frame rate might be constrained by the vertical sync frequency of
  your monitor (VSync can be disabled in the graphics card driver configuration). So for simple
  plots (potential framerate far above 60 frames per second), OpenGL acceleration might achieve \a
  lower frame rates than the other graphics systems, because they are not capped at the VSync
  frequency.
*/

#include "QCustomPlot.h"

// ================================================================================
// =================== QCPData
// ================================================================================

/*! \class QCPData
  \brief A class holding the data of one single data point for QCPGraph.
  
  The stored data is:
  \li \a key: coordinate on the key axis of this data point
  \li \a value: coordinate on the value axis of this data point
  \li \a keyErrorMinus: negative error in the key dimension (for error bars)
  \li \a keyErrorPlus: positive error in the key dimension (for error bars)
  \li \a valueErrorMinus: negative error in the value dimension (for error bars)
  \li \a valueErrorPlus: positive error in the value dimension (for error bars)
  
  \see QCPDataMap
*/

/*!
  Constructs a data point with key, value and all errors set to zero.
*/
QCPData::QCPData() :
  key(0),
  value(0),
  keyErrorPlus(0),
  keyErrorMinus(0),
  valueErrorPlus(0),
  valueErrorMinus(0)
{
}

// ================================================================================
// =================== QCPCurveData
// ================================================================================

/*! \class QCPCurveData
  \brief A class holding the data of one single data point for QCPCurve.
  
  The stored data is:
  \li \a t: the free parameter of the curve at this curve point (cp. the mathematical vector <em>(x(t), y(t))</em>)
  \li \a key: coordinate on the key axis of this curve point
  \li \a value: coordinate on the value axis of this curve point
  
  \see QCPCurveDataMap
*/

/*!
  Constructs a curve data point with t, key and value set to zero.
*/
QCPCurveData::QCPCurveData() :
  t(0),
  key(0),
  value(0)
{
}


// ================================================================================
// =================== QCPBarData
// ================================================================================

/*! \class QCPBarData
  \brief A class holding the data of one single data point (one bar) for QCPBars.
  
  The stored data is:
  \li \a key: coordinate on the key axis of this bar
  \li \a value: height coordinate on the value axis of this bar
  
  \see QCPBarDataaMap
*/

/*!
  Constructs a bar data point with key and value set to zero.
*/
QCPBarData::QCPBarData() :
  key(0),
  value(0)
{
}

// ================================================================================
// =================== QCPGraph
// ================================================================================

/*! \class QCPGraph
  \brief A plottable representing a graph in a plot.

  Usually QCustomPlot creates it internally via QCustomPlot::addGraph and the resulting instance is
  accessed via QCustomPlot::graph.

  To plot data, assign it with the \ref setData or \ref addData functions.
  
  \section appearance Changing the appearance
  
  The appearance of the graph is mainly determined by the line style, scatter style, brush and pen
  of the graph (\ref setLineStyle, \ref setScatterStyle, \ref setBrush, \ref setPen).
  
  \subsection filling Filling under or between graphs
  
  QCPGraph knows two types of fills: Normal graph fills towards the zero-value-line parallel to
  the key axis of the graph, and fills between two graphs, called channel fills. To enable a fill,
  just set a brush with \ref setBrush which is neither Qt::NoBrush nor fully transparent.
  
  By default, a normal fill towards the zero-value-line will be drawn. To set up a channel fill
  between this graph and another one, call \ref setChannelFillGraph with the other graph as
  parameter.

  \see QCustomPlot::addGraph, QCustomPlot::graph, QCPLegend::addGraph
*/

/*!
  Constructs a graph which uses \a keyAxis as its key axis ("x") and \a valueAxis as its value
  axis ("y"). \a keyAxis and \a valueAxis must both reside in the same QCustomPlot.
  
  The constructed QCPGraph can be added to the plot with QCustomPlot::addPlottable, QCustomPlot
  then takes ownership of the graph.
  
  To directly create a graph inside a plot, you can also use the simpler QCustomPlot::addGraph function.
*/
QCPGraph::QCPGraph(QCPAxis *keyAxis, QCPAxis *valueAxis) :
  QCPAbstractPlottable(keyAxis, valueAxis)
{
  mData = new QCPDataMap;
  mPen.setColor(Qt::blue);
  mPen.setStyle(Qt::SolidLine);
  mErrorPen.setColor(Qt::black);
  mBrush.setColor(Qt::blue);
  mBrush.setStyle(Qt::NoBrush);
  mLineStyle = lsLine;
  mScatterStyle = ssNone;
  mScatterSize = 6;
  mErrorType = etNone;
  mErrorBarSize = 6;
  mErrorBarSkipSymbol = true;
  mChannelFillGraph = 0;
}

QCPGraph::~QCPGraph()
{
  if (mParentPlot)
  {
    // if another graph has a channel fill towards this graph, set it to zero
    for (int i=0; i<mParentPlot->graphCount(); ++i)
    {
      if (mParentPlot->graph(i)->channelFillGraph() == this)
        mParentPlot->graph(i)->setChannelFillGraph(0);
    }
  }
  delete mData;
}

/*!
  Replaces the current data with the provided \a data.
  
  If \a copy is set to true, data points in \a data will only be copied. if false, the plottable
  takes ownership of the passed data and replaces the internal data pointer with it. This is
  significantly faster than copying for large datasets.
*/
void QCPGraph::setData(QCPDataMap *data, bool copy)
{
  if (copy)
  {
    *mData = *data;
  } else
  {
    delete mData;
    mData = data;
  }
}

/*! \overload
  
  Replaces the current data with the provided points in \a key and \a value pairs. The provided
  vectors should have equal length. Else, the number of added points will be the size of the
  smallest vector.
*/
void QCPGraph::setData(const QVector<double> &key, const QVector<double> &value)
{
  mData->clear();
  int n = key.size();
  n = qMin(n, value.size());
  QCPData newData;
  for (int i=0; i<n; ++i)
  {
    newData.key = key[i];
    newData.value = value[i];
    mData->insertMulti(newData.key, newData);
  }
}

/*!
  Replaces the current data with the provided points in \a key and \a value pairs. Additionally the
  symmetrical value error of the data points are set to the values in \a valueError.
  For error bars to show appropriately, see \ref setErrorType.
  The provided vectors should have equal length. Else, the number of added points will be the size of the
  smallest vector.
*/
void QCPGraph::setDataValueError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &valueError)
{
  mData->clear();
  int n = key.size();
  n = qMin(n, value.size());
  n = qMin(n, valueError.size());
  QCPData newData;
  for (int i=0; i<n; ++i)
  {
    newData.key = key[i];
    newData.value = value[i];
    newData.valueErrorMinus = valueError[i];
    newData.valueErrorPlus = valueError[i];
    mData->insertMulti(key[i], newData);
  }
}

/*!
  \overload
  Replaces the current data with the provided points in \a key and \a value pairs. Additionally the
  negative value error of the data points are set to the values in \a valueErrorMinus, the positive
  value error to \a valueErrorPlus.
  For error bars to show appropriately, see \ref setErrorType.
  The provided vectors should have equal length. Else, the number of added points will be the size of the
  smallest vector.
*/
void QCPGraph::setDataValueError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &valueErrorMinus, const QVector<double> &valueErrorPlus)
{
  mData->clear();
  int n = key.size();
  n = qMin(n, value.size());
  n = qMin(n, valueErrorMinus.size());
  n = qMin(n, valueErrorPlus.size());
  QCPData newData;
  for (int i=0; i<n; ++i)
  {
    newData.key = key[i];
    newData.value = value[i];
    newData.valueErrorMinus = valueErrorMinus[i];
    newData.valueErrorPlus = valueErrorPlus[i];
    mData->insertMulti(key[i], newData);
  }
}

/*!
  Replaces the current data with the provided points in \a key and \a value pairs. Additionally the
  symmetrical key error of the data points are set to the values in \a keyError.
  For error bars to show appropriately, see \ref setErrorType.
  The provided vectors should have equal length. Else, the number of added points will be the size of the
  smallest vector.
*/
void QCPGraph::setDataKeyError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &keyError)
{
  mData->clear();
  int n = key.size();
  n = qMin(n, value.size());
  n = qMin(n, keyError.size());
  QCPData newData;
  for (int i=0; i<n; ++i)
  {
    newData.key = key[i];
    newData.value = value[i];
    newData.keyErrorMinus = keyError[i];
    newData.keyErrorPlus = keyError[i];
    mData->insertMulti(key[i], newData);
  }
}

/*!
  \overload
  Replaces the current data with the provided points in \a key and \a value pairs. Additionally the
  negative key error of the data points are set to the values in \a keyErrorMinus, the positive
  key error to \a keyErrorPlus.
  For error bars to show appropriately, see \ref setErrorType.
  The provided vectors should have equal length. Else, the number of added points will be the size of the
  smallest vector.
*/
void QCPGraph::setDataKeyError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &keyErrorMinus, const QVector<double> &keyErrorPlus)
{
  mData->clear();
  int n = key.size();
  n = qMin(n, value.size());
  n = qMin(n, keyErrorMinus.size());
  n = qMin(n, keyErrorPlus.size());
  QCPData newData;
  for (int i=0; i<n; ++i)
  {
    newData.key = key[i];
    newData.value = value[i];
    newData.keyErrorMinus = keyErrorMinus[i];
    newData.keyErrorPlus = keyErrorPlus[i];
    mData->insertMulti(key[i], newData);
  }
}

/*!
  Replaces the current data with the provided points in \a key and \a value pairs. Additionally the
  symmetrical key and value errors of the data points are set to the values in \a keyError and \a valueError.
  For error bars to show appropriately, see \ref setErrorType.
  The provided vectors should have equal length. Else, the number of added points will be the size of the
  smallest vector.
*/
void QCPGraph::setDataBothError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &keyError, const QVector<double> &valueError)
{
  mData->clear();
  int n = key.size();
  n = qMin(n, value.size());
  n = qMin(n, valueError.size());
  n = qMin(n, keyError.size());
  QCPData newData;
  for (int i=0; i<n; ++i)
  {
    newData.key = key[i];
    newData.value = value[i];
    newData.keyErrorMinus = keyError[i];
    newData.keyErrorPlus = keyError[i];
    newData.valueErrorMinus = valueError[i];
    newData.valueErrorPlus = valueError[i];
    mData->insertMulti(key[i], newData);
  }
}

/*!
  \overload
  Replaces the current data with the provided points in \a key and \a value pairs. Additionally the
  negative key and value errors of the data points are set to the values in \a keyErrorMinus and \a valueErrorMinus. The positive
  key and value errors are set to the values in \a keyErrorPlus \a valueErrorPlus.
  For error bars to show appropriately, see \ref setErrorType.
  The provided vectors should have equal length. Else, the number of added points will be the size of the
  smallest vector.
*/
void QCPGraph::setDataBothError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &keyErrorMinus, const QVector<double> &keyErrorPlus, const QVector<double> &valueErrorMinus, const QVector<double> &valueErrorPlus)
{
  mData->clear();
  int n = key.size();
  n = qMin(n, value.size());
  n = qMin(n, valueErrorMinus.size());
  n = qMin(n, valueErrorPlus.size());
  n = qMin(n, keyErrorMinus.size());
  n = qMin(n, keyErrorPlus.size());
  QCPData newData;
  for (int i=0; i<n; ++i)
  {
    newData.key = key[i];
    newData.value = value[i];
    newData.keyErrorMinus = keyErrorMinus[i];
    newData.keyErrorPlus = keyErrorPlus[i];
    newData.valueErrorMinus = valueErrorMinus[i];
    newData.valueErrorPlus = valueErrorPlus[i];
    mData->insertMulti(key[i], newData);
  }
}


/*!
  Sets how the single data points are connected in the plot or how they are represented visually
  apart from the scatter symbol. For scatter-only plots, set \a ls to \ref lsNone and \ref
  setScatterStyle to the desired scatter style.
  \see setScatterStyle
*/
void QCPGraph::setLineStyle(LineStyle ls)
{
  mLineStyle = ls;
}

/*! 
  Sets the visual appearance of single data points in the plot. If set to \ref ssNone, no scatter points
  are drawn (e.g. for line-only-plots with appropriate line style).
  \see ScatterStyle, setLineStyle
*/
void QCPGraph::setScatterStyle(ScatterStyle ss)
{
  mScatterStyle = ss;
}

/*! 
  This defines how big (in pixels) single scatters are drawn, if scatter style (\ref
  setScatterStyle) isn't \ref ssNone, \ref ssDot or \ref ssPixmap. Floating point values are
  allowed for fine grained control over optical appearance with antialiased painting.
  \see ScatterStyle
*/
void QCPGraph::setScatterSize(double size)
{
  mScatterSize = size;
}

/*! 
  If the scatter style (\ref setScatterStyle) is set to ssPixmap, this function defines the QPixmap
  that will be drawn centered on the data point coordinate.
  \see ScatterStyle
*/
void QCPGraph::setScatterPixmap(const QPixmap &pixmap)
{
  mScatterPixmap = pixmap;
}

/*! 
  \see ErrorType
*/
void QCPGraph::setErrorType(ErrorType errorType)
{
  mErrorType = errorType;
}

/*!
  Sets the pen with which the error bars will be drawn.
  \see setErrorBarSize, setErrorType
*/
void QCPGraph::setErrorPen(const QPen &pen)
{
  mErrorPen = pen;
}

/*! 
  Sets the width of the handles at both ends of an error bar in pixels.
*/
void QCPGraph::setErrorBarSize(double size)
{
  mErrorBarSize = size;
}

/*! 
  If \a enabled is set to true, the error bar will not be drawn as a solid line under the scatter symbol but
  leave some free space around the symbol.
  
  This feature uses the current scatter size (\ref setScatterSize) to determine the size of the
  area to leave blank. So when drawing Pixmaps as scatter points (\ref ssPixmap), the scatter size
  must be set manually to a value corresponding to the size of the Pixmap, if the error bars should
  leave gaps to its boundaries.
*/
void QCPGraph::setErrorBarSkipSymbol(bool enabled)
{
  mErrorBarSkipSymbol = enabled;
}

/*! 
  Sets the target graph for filling the area between this graph and \a targetGraph with the current
  brush (\ref setBrush).
  
  When \a targetGraph is set to 0, a normal graph fill will be produced. This means, when the brush
  is not Qt::NoBrush or fully transparent, a fill all the way to the zero-value-line parallel to
  the key axis of this graph will be drawn. To disable any filling, set the brush to Qt::NoBrush.
  \see setBrush
*/
void QCPGraph::setChannelFillGraph(QCPGraph *targetGraph)
{
  // prevent setting channel target to this graph itself:
  if (targetGraph == this)
  {
    qDebug() << FUNCNAME << "targetGraph is self";
    mChannelFillGraph = 0;
    return;
  }
  // prevent setting channel target to a graph not in the plot:
  if (targetGraph && targetGraph->mParentPlot != mParentPlot)
  {
    qDebug() << FUNCNAME << "targetGraph not in same plot";
    mChannelFillGraph = 0;
    return;
  }
  
  mChannelFillGraph = targetGraph;
}

/*!
  Adds the provided data points in \a dataMap to the current data.
  \see removeData
*/
void QCPGraph::addData(const QCPDataMap &dataMap)
{
  mData->unite(dataMap);
}

/*! \overload
  Adds the provided single data point in \a data to the current data.
  \see removeData
*/
void QCPGraph::addData(const QCPData &data)
{
  mData->insertMulti(data.key, data);
}

/*! \overload
  Adds the provided single data point as \a key and \a value pair to the current data.
  \see removeData
*/
void QCPGraph::addData(double key, double value)
{
  QCPData newData;
  newData.key = key;
  newData.value = value;
  mData->insertMulti(newData.key, newData);
}

/*! \overload
  Adds the provided data points as \a key and \a value pairs to the current data.
  \see removeData
*/
void QCPGraph::addData(const QVector<double> &keys, const QVector<double> &values)
{
  int n = qMin(keys.size(), values.size());
  QCPData newData;
  for (int i=0; i<n; ++i)
  {
    newData.key = keys[i];
    newData.value = values[i];
    mData->insertMulti(newData.key, newData);
  }
}

/*!
  Removes all data points with keys smaller than \a key.
  \see addData, clearData
*/
void QCPGraph::removeDataBefore(double key)
{
  QCPDataMap::iterator it = mData->begin();
  while (it != mData->end() && it.key() < key)
    it = mData->erase(it);
}

/*!
  Removes all data points with keys greater than \a key.
  \see addData, clearData
*/
void QCPGraph::removeDataAfter(double key)
{
  if (mData->isEmpty()) return;
  QCPDataMap::iterator it = mData->upperBound(key);
  while (it != mData->end())
    it = mData->erase(it);
}

/*!
  Removes all data points with keys between \a fromKey and \a toKey.
  if \a fromKey is greater or equal to \a toKey, the function does nothing. To remove
  a single data point with known key, use \ref removeData(double key).
  
  \see addData, clearData
*/
void QCPGraph::removeData(double fromKey, double toKey)
{
  if (fromKey >= toKey || mData->isEmpty()) return;
  QCPDataMap::iterator it = mData->upperBound(fromKey);
  QCPDataMap::iterator itEnd = mData->upperBound(toKey);
  while (it != itEnd)
    it = mData->erase(it);
}

/*! \overload
  
  Removes a single data point at \a key. If the position is not known with absolute precision,
  consider using \ref removeData(double fromKey, double toKey) with a small fuzziness interval around
  the suspected position, depeding on the precision with which the key is known.

  \see addData, clearData
*/
void QCPGraph::removeData(double key)
{
  mData->remove(key);
}

/*!
  Removes all data points.
  \see removeData, removeDataAfter, removeDataBefore
*/
void QCPGraph::clearData()
{
  mData->clear();
}

/*! \overload
  
  Allows to define whether error bars are taken into consideration when determining the new axis
  range.
*/
void QCPGraph::rescaleAxes(bool onlyEnlarge, bool includeErrorBars) const
{
  rescaleKeyAxis(onlyEnlarge, includeErrorBars);
  rescaleValueAxis(onlyEnlarge, includeErrorBars);
}

/*! \overload
  
  Allows to define whether error bars (of kind \ref QCPGraph::etKey) are taken into consideration
  when determining the new axis range.
*/
void QCPGraph::rescaleKeyAxis(bool onlyEnlarge, bool includeErrorBars) const
{
  // this code is a copy of QCPAbstractPlottable::rescaleKeyAxis with the only change
  // is that getKeyRange is passed the includeErrorBars value.
  if (mData->isEmpty()) return;

  SignDomain signDomain = sdBoth;
  if (mKeyAxis->scaleType() == QCPAxis::stLogarithmic)
    signDomain = (mKeyAxis->range().upper < 0 ? sdNegative : sdPositive);
  
  bool validRange;
  QCPRange newRange = getKeyRange(validRange, signDomain, includeErrorBars);
  
  if (validRange)
  {
    if (onlyEnlarge)
    {
      if (mKeyAxis->range().lower < newRange.lower)
        newRange.lower = mKeyAxis->range().lower;
      if (mKeyAxis->range().upper > newRange.upper)
        newRange.upper = mKeyAxis->range().upper;
    }
    mKeyAxis->setRange(newRange);
  }
}

/*! \overload
  
  Allows to define whether error bars (of kind \ref QCPGraph::etValue) are taken into consideration
  when determining the new axis range.
*/
void QCPGraph::rescaleValueAxis(bool onlyEnlarge, bool includeErrorBars) const
{
  // this code is a copy of QCPAbstractPlottable::rescaleValueAxis with the only change
  // is that getValueRange is passed the includeErrorBars value.
  if (mData->isEmpty()) return;

  SignDomain signDomain = sdBoth;
  if (mValueAxis->scaleType() == QCPAxis::stLogarithmic)
    signDomain = (mValueAxis->range().upper < 0 ? sdNegative : sdPositive);
  
  bool validRange;
  QCPRange newRange = getValueRange(validRange, signDomain, includeErrorBars);
  
  if (validRange)
  {
    if (onlyEnlarge)
    {
      if (mValueAxis->range().lower < newRange.lower)
        newRange.lower = mValueAxis->range().lower;
      if (mValueAxis->range().upper > newRange.upper)
        newRange.upper = mValueAxis->range().upper;
    }
    mValueAxis->setRange(newRange);
  }
}

/* inherits documentation from base class */
void QCPGraph::draw(QPainter *painter) const
{
  if (!mVisible) return;
  if (mKeyAxis->range().size() <= 0) return;
  if (mData->isEmpty()) return;
  if (mLineStyle == lsNone && mScatterStyle == ssNone) return;
  painter->setClipRect(mKeyAxis->axisRect().united(mValueAxis->axisRect()));
  
  // allocate line and (if necessary) point vectors:
  QVector<QPointF> *lineData = new QVector<QPointF>;
  QVector<QCPData> *pointData = 0;
  if (mScatterStyle != ssNone)
    pointData = new QVector<QCPData>;
  
  // fill vectors with data appropriate to plot style:
  getPlotData(lineData, pointData);

  // draw fill of graph:
  drawFill(painter, lineData);
  
  // draw line:
  if (mLineStyle == lsImpulse)
    drawImpulsePlot(painter, lineData);
  else if (mLineStyle != lsNone)
    drawLinePlot(painter, lineData); // also step plots can be drawn as a line plot
  
  // draw scatters:
  if (pointData)
    drawScatterPlot(painter, pointData);
  
  // free allocated line and point vectors:
  delete lineData;
  if (pointData)
    delete pointData;
}

/* inherits documentation from base class */
void QCPGraph::drawLegendIcon(QPainter *painter, const QRect &rect) const
{
  // draw fill:
  if (mBrush.style() != Qt::NoBrush)
  {
    painter->setRenderHint(QPainter::Antialiasing, mParentPlot->antialiasedElements().testFlag(QCustomPlot::aeGraphs));
    painter->fillRect(rect.left(), rect.top()+rect.height()/2.0, rect.width(), rect.height()/3.0, mBrush);
  }
  // draw line vertically centered:
  if (mLineStyle != lsNone)
  {
    painter->setRenderHint(QPainter::Antialiasing, mParentPlot->antialiasedElements().testFlag(QCustomPlot::aeGraphs));
    painter->setPen(mPen);
    painter->drawLine(rect.left(), rect.top()+rect.height()/2.0, rect.right()+5, rect.top()+rect.height()/2.0); // +5 on x2 else last segment is missing from dashed/dotted pens
  }
  // draw scatter symbol:
  if (mScatterStyle != ssNone)
  {
    if (mScatterStyle == ssPixmap && (mScatterPixmap.size().width() > rect.width() || mScatterPixmap.size().height() > rect.height()))
    {
      // handle pixmap scatters that are larger than legend icon rect separately.
      // We resize them and draw them manually, instead of calling drawScatter:
      QSize newSize = mScatterPixmap.size();
      newSize.scale(rect.size(), Qt::KeepAspectRatio);
      QRect targetRect;
      targetRect.setSize(newSize);
      targetRect.moveCenter(rect.center());
      bool smoothBackup = painter->testRenderHint(QPainter::SmoothPixmapTransform);
      painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
      painter->drawPixmap(targetRect, mScatterPixmap);
      painter->setRenderHint(QPainter::SmoothPixmapTransform, smoothBackup);
    } else // mScatterStyle != ssPixmap
    {
      painter->setPen(mPen);
      painter->setRenderHint(QPainter::Antialiasing, mParentPlot->antialiasedElements().testFlag(QCustomPlot::aeScatters));
      drawScatter(painter, rect.center().x()+1, rect.center().y()+1, mScatterStyle);
    }
  }
}

/*! 
  \internal
  This function branches out to the line style specific "get(...)PlotData" functions, according to the
  line style of the graph.
  \param lineData will be filled with raw points that will be drawn with the according draw functions, e.g. \ref drawLinePlot and \ref drawImpulsePlot.
  These aren't necessarily the original data points, since for step plots for example, additional points are needed for drawing lines that make up steps.
  If the line style of the graph is \ref lsNone, the \a lineData vector will be left untouched.
  \param pointData will be filled with the original data points so \ref drawScatterPlot can draw the scatter symbols accordingly. If no scatters need to be
  drawn, i.e. scatter style is \ref ssNone, pass 0 as \a pointData, and this step will be skipped.
  
  \see getScatterPlotData, getLinePlotData, getStepLeftPlotData, getStepRightPlotData, getStepCenterPlotData, getImpulsePlotData
*/
void QCPGraph::getPlotData(QVector<QPointF> *lineData, QVector<QCPData> *pointData) const
{
  switch(mLineStyle)
  {
    case lsNone: getScatterPlotData(pointData); break;
    case lsLine: getLinePlotData(lineData, pointData); break;
    case lsStepLeft: getStepLeftPlotData(lineData, pointData); break;
    case lsStepRight: getStepRightPlotData(lineData, pointData); break;
    case lsStepCenter: getStepCenterPlotData(lineData, pointData); break;
    case lsImpulse: getImpulsePlotData(lineData, pointData); break;
  }
}

/*! 
  \internal
  If line style is \ref lsNone and scatter style is not \ref ssNone, this function serves at providing the
  visible data points in \a pointData, so the \ref drawScatterPlot function can draw the scatter points
  accordingly.
  
  If line style is not \ref lsNone, this function is not called and the data for the scatter points
  are (if needed) calculated inside the corresponding other "get(...)PlotData" functions.
  \see drawScatterPlot
*/
void QCPGraph::getScatterPlotData(QVector<QCPData> *pointData) const
{
  if (!pointData) return;
  
  // get visible data range:
  QCPDataMap::const_iterator lower, upper;
  int dataCount;
  getVisibleDataBounds(lower, upper, dataCount);
  // prepare vectors:
  if (pointData)
    pointData->resize(dataCount);

  // position data points:
  QCPDataMap::const_iterator it = lower;
  QCPDataMap::const_iterator upperEnd = upper+1;
  int i = 0;
  if (mKeyAxis->orientation() == Qt::Vertical)
  {
    while (it != upperEnd)
    {
      (*pointData)[i] = it.value();
      ++i;
      ++it;
    }
  } else // key axis is horizontal
  {
    while (it != upperEnd)
    {
      (*pointData)[i] = it.value();
      ++i;
      ++it;
    }
  }
}

/*! 
  \internal
  Places the raw data points needed for a normal linearly connected plot in \a lineData.

  As for all plot data retrieval functions, \a pointData just contains all unaltered data (scatter)
  points that are visible, for drawing scatter points, if necessary. If drawing scatter points is
  disabled (i.e. scatter style \ref ssNone), pass 0 as \a pointData, and the function will skip
  filling the vector.
  \see drawLinePlot
*/
void QCPGraph::getLinePlotData(QVector<QPointF> *lineData, QVector<QCPData> *pointData) const
{
  // get visible data range:
  QCPDataMap::const_iterator lower, upper;
  int dataCount;
  getVisibleDataBounds(lower, upper, dataCount);
  // prepare vectors:
  if (lineData)
  { 
    // added 2 to reserve memory for lower/upper fill base points that might be needed for fill
    lineData->reserve(dataCount+2);
    lineData->resize(dataCount);
  }
  if (pointData)
    pointData->resize(dataCount);

  // position data points:
  QCPDataMap::const_iterator it = lower;
  QCPDataMap::const_iterator upperEnd = upper+1;
  int i = 0;
  if (mKeyAxis->orientation() == Qt::Vertical)
  {
    while (it != upperEnd)
    {
      if (pointData)
        (*pointData)[i] = it.value();
      (*lineData)[i].setX(mValueAxis->coordToPixel(it.value().value));
      (*lineData)[i].setY(mKeyAxis->coordToPixel(it.key()));
      ++i;
      ++it;
    }
  } else // key axis is horizontal
  {
    while (it != upperEnd)
    {
      if (pointData)
        (*pointData)[i] = it.value();
      (*lineData)[i].setX(mKeyAxis->coordToPixel(it.key()));
      (*lineData)[i].setY(mValueAxis->coordToPixel(it.value().value));
      ++i;
      ++it;
    }
  }
}

/*! 
  \internal
  Places the raw data points needed for a step plot with left oriented steps in \a lineData.

  As for all plot data retrieval functions, \a pointData just contains all unaltered data (scatter)
  points that are visible, for drawing scatter points, if necessary. If drawing scatter points is
  disabled (i.e. scatter style \ref ssNone), pass 0 as \a pointData, and the function will skip
  filling the vector.
  \see drawLinePlot
*/
void QCPGraph::getStepLeftPlotData(QVector<QPointF> *lineData, QVector<QCPData> *pointData) const
{
  // get visible data range:
  QCPDataMap::const_iterator lower, upper;
  int dataCount;
  getVisibleDataBounds(lower, upper, dataCount);
  // prepare vectors:
  if (lineData)
  {
    // added 2 to reserve memory for lower/upper fill base points that might be needed for fill
    // multiplied by 2 because step plot needs two polyline points per one actual data point
    lineData->reserve(dataCount*2+2);
    lineData->resize(dataCount*2);
  }
  if (pointData)
    pointData->resize(dataCount);
  
  // position data points:
  QCPDataMap::const_iterator it = lower;
  QCPDataMap::const_iterator upperEnd = upper+1;
  int i = 0;
  int ipoint = 0;
  if (mKeyAxis->orientation() == Qt::Vertical)
  {
    double lastValue = mValueAxis->coordToPixel(it.value().value);
    double key;
    while (it != upperEnd)
    {
      if (pointData)
      {
        (*pointData)[ipoint] = it.value();
        ++ipoint;
      }
      key = mKeyAxis->coordToPixel(it.key());
      (*lineData)[i].setX(lastValue);
      (*lineData)[i].setY(key);
      ++i;
      lastValue = mValueAxis->coordToPixel(it.value().value);
      (*lineData)[i].setX(lastValue);
      (*lineData)[i].setY(key);
      ++i;
      ++it;
    }
  } else // key axis is horizontal
  {
    double lastValue = mValueAxis->coordToPixel(it.value().value);
    double key;
    while (it != upperEnd)
    {
      if (pointData)
      {
        (*pointData)[ipoint] = it.value();
        ++ipoint;
      }
      key = mKeyAxis->coordToPixel(it.key());
      (*lineData)[i].setX(key);
      (*lineData)[i].setY(lastValue);
      ++i;
      lastValue = mValueAxis->coordToPixel(it.value().value);
      (*lineData)[i].setX(key);
      (*lineData)[i].setY(lastValue);
      ++i;
      ++it;
    }
  }
}

/*! 
  \internal
  Places the raw data points needed for a step plot with right oriented steps in \a lineData.

  As for all plot data retrieval functions, \a pointData just contains all unaltered data (scatter)
  points that are visible, for drawing scatter points, if necessary. If drawing scatter points is
  disabled (i.e. scatter style \ref ssNone), pass 0 as \a pointData, and the function will skip
  filling the vector.
  \see drawLinePlot
*/
void QCPGraph::getStepRightPlotData(QVector<QPointF> *lineData, QVector<QCPData> *pointData) const
{
  // get visible data range:
  QCPDataMap::const_iterator lower, upper;
  int dataCount;
  getVisibleDataBounds(lower, upper, dataCount);
  // prepare vectors:
  if (lineData)
  {
    // added 2 to reserve memory for lower/upper fill base points that might be needed for fill
    // multiplied by 2 because step plot needs two polyline points per one actual data point
    lineData->reserve(dataCount*2+2);
    lineData->resize(dataCount*2);
  }
  if (pointData)
    pointData->resize(dataCount);
  
  // position points:
  QCPDataMap::const_iterator it = lower;
  QCPDataMap::const_iterator upperEnd = upper+1;
  int i = 0;
  int ipoint = 0;
  if (mKeyAxis->orientation() == Qt::Vertical)
  {
    double lastKey = mKeyAxis->coordToPixel(it.key());
    double value;
    while (it != upperEnd)
    {
      if (pointData)
      {
        (*pointData)[ipoint] = it.value();
        ++ipoint;
      }
      value = mValueAxis->coordToPixel(it.value().value);
      (*lineData)[i].setX(value);
      (*lineData)[i].setY(lastKey);
      ++i;
      lastKey = mKeyAxis->coordToPixel(it.key());
      (*lineData)[i].setX(value);
      (*lineData)[i].setY(lastKey);
      ++i;
      ++it;
    }
  } else // key axis is horizontal
  {
    double lastKey = mKeyAxis->coordToPixel(it.key());
    double value;
    while (it != upperEnd)
    {
      if (pointData)
      {
        (*pointData)[ipoint] = it.value();
        ++ipoint;
      }
      value = mValueAxis->coordToPixel(it.value().value);
      (*lineData)[i].setX(lastKey);
      (*lineData)[i].setY(value);
      ++i;
      lastKey = mKeyAxis->coordToPixel(it.key());
      (*lineData)[i].setX(lastKey);
      (*lineData)[i].setY(value);
      ++i;
      ++it;
    }
  }
}

/*! 
  \internal
  Places the raw data points needed for a step plot with centered steps in \a lineData.

  As for all plot data retrieval functions, \a pointData just contains all unaltered data (scatter)
  points that are visible, for drawing scatter points, if necessary. If drawing scatter points is
  disabled (i.e. scatter style \ref ssNone), pass 0 as \a pointData, and the function will skip
  filling the vector.
  \see drawLinePlot
*/
void QCPGraph::getStepCenterPlotData(QVector<QPointF> *lineData, QVector<QCPData> *pointData) const
{
  // get visible data range:
  QCPDataMap::const_iterator lower, upper;
  int dataCount;
  getVisibleDataBounds(lower, upper, dataCount);
  // prepare vectors:
  if (lineData)
  {
    // added 2 to reserve memory for lower/upper fill base points that might be needed for base fill
    // multiplied by 2 because step plot needs two polyline points per one actual data point
    lineData->reserve(dataCount*2+2);
    lineData->resize(dataCount*2);
  }
  if (pointData)
    pointData->resize(dataCount);
  
  // position points:
  QCPDataMap::const_iterator it = lower;
  QCPDataMap::const_iterator upperEnd = upper+1;
  int i = 0;
  int ipoint = 0;
  if (mKeyAxis->orientation() == Qt::Vertical)
  {
    double lastKey = mKeyAxis->coordToPixel(it.key());
    double lastValue = mValueAxis->coordToPixel(it.value().value);
    double key;
    if (pointData)
    {
      (*pointData)[ipoint] = it.value();
      ++ipoint;
    }
    (*lineData)[i].setX(lastValue);
    (*lineData)[i].setY(lastKey);
    ++it;
    ++i;
    while (it != upperEnd)
    {
      if (pointData)
      {
        (*pointData)[ipoint] = it.value();
        ++ipoint;
      }
      key = (mKeyAxis->coordToPixel(it.key())-lastKey)*0.5 + lastKey;
      (*lineData)[i].setX(lastValue);
      (*lineData)[i].setY(key);
      ++i;
      lastValue = mValueAxis->coordToPixel(it.value().value);
      lastKey = mKeyAxis->coordToPixel(it.key());
      (*lineData)[i].setX(lastValue);
      (*lineData)[i].setY(key);
      ++it;
      ++i;
    }
    (*lineData)[i].setX(lastValue);
    (*lineData)[i].setY(lastKey);
  } else // key axis is horizontal
  {
    double lastKey = mKeyAxis->coordToPixel(it.key());
    double lastValue = mValueAxis->coordToPixel(it.value().value);
    double key;
    if (pointData)
    {
      (*pointData)[ipoint] = it.value();
      ++ipoint;
    }
    (*lineData)[i].setX(lastKey);
    (*lineData)[i].setY(lastValue);
    ++it;
    ++i;
    while (it != upperEnd)
    {
      if (pointData)
      {
        (*pointData)[ipoint] = it.value();
        ++ipoint;
      }
      key = (mKeyAxis->coordToPixel(it.key())-lastKey)*0.5 + lastKey;
      (*lineData)[i].setX(key);
      (*lineData)[i].setY(lastValue);
      ++i;
      lastValue = mValueAxis->coordToPixel(it.value().value);
      lastKey = mKeyAxis->coordToPixel(it.key());
      (*lineData)[i].setX(key);
      (*lineData)[i].setY(lastValue);
      ++it;
      ++i;
    }
    (*lineData)[i].setX(lastKey);
    (*lineData)[i].setY(lastValue);
  }
}

/*! 
  \internal
  Places the raw data points needed for an impulse plot in \a lineData.

  As for all plot data retrieval functions, \a pointData just contains all unaltered data (scatter)
  points that are visible, for drawing scatter points, if necessary. If drawing scatter points is
  disabled (i.e. scatter style \ref ssNone), pass 0 as \a pointData, and the function will skip
  filling the vector.
  \see drawImpulsePlot
*/
void QCPGraph::getImpulsePlotData(QVector<QPointF> *lineData, QVector<QCPData> *pointData) const
{
  // get visible data range:
  QCPDataMap::const_iterator lower, upper;
  int dataCount;
  getVisibleDataBounds(lower, upper, dataCount);
  // prepare vectors:
  if (lineData)
  {
    // no need to reserve 2 extra points, because there is no fill for impulse plot
    lineData->resize(dataCount*2);
  }
  if (pointData)
    pointData->resize(dataCount);
  
  // position data points:
  QCPDataMap::const_iterator it = lower;
  QCPDataMap::const_iterator upperEnd = upper+1;
  int i = 0;
  int ipoint = 0;
  if (mKeyAxis->orientation() == Qt::Vertical)
  {
    double zeroPointX = mValueAxis->coordToPixel(0);
    double key;
    while (it != upperEnd)
    {
      if (pointData)
      {
        (*pointData)[ipoint] = it.value();
        ++ipoint;
      }
      key = mKeyAxis->coordToPixel(it.key());
      (*lineData)[i].setX(zeroPointX);
      (*lineData)[i].setY(key);
      ++i;
      (*lineData)[i].setX(mValueAxis->coordToPixel(it.value().value));
      (*lineData)[i].setY(key);
      ++i;
      ++it;
    }
  } else // key axis is horizontal
  {
    double zeroPointY = mValueAxis->coordToPixel(0);
    double key;
    while (it != upperEnd)
    {
      if (pointData)
      {
        (*pointData)[ipoint] = it.value();
        ++ipoint;
      }
      key = mKeyAxis->coordToPixel(it.key());
      (*lineData)[i].setX(key);
      (*lineData)[i].setY(zeroPointY);
      ++i;
      (*lineData)[i].setX(key);
      (*lineData)[i].setY(mValueAxis->coordToPixel(it.value().value));
      ++i;
      ++it;
    }
  }
}

/*! 
  \internal
  Draws the fill of the graph with the specified brush. If the fill is a normal "base" fill, i.e.
  under the graph toward the zero-value-line, only the \a lineData is required (and two extra points
  at the zero-value-line, which are added by \ref addFillBasePoints and removed by \ref removeFillBasePoints
  after the fill drawing is done).
  
  If the fill is a channel fill between this graph and another graph (mChannelFillGraph), the more complex
  polygon is calculated with the \ref getChannelFillPolygon function.
  \see drawLinePlot
*/
void QCPGraph::drawFill(QPainter *painter, QVector<QPointF> *lineData) const
{
  if (mLineStyle == lsImpulse) return; // fill doesn't make sense for impulse plot
  if (mBrush.style() == Qt::NoBrush || mBrush.color().alpha() == 0) return;
  
  painter->setRenderHint(QPainter::Antialiasing, mParentPlot->antialiasedElements().testFlag(QCustomPlot::aeFills));
  if (!mChannelFillGraph)
  {
    // draw base fill under graph, fill goes all the way to the zero-value-line:
    addFillBasePoints(lineData);
    painter->setPen(Qt::NoPen);
    painter->setBrush(mBrush);
    painter->drawPolygon(QPolygonF(*lineData));
    removeFillBasePoints(lineData);
  } else
  {
    // draw channel fill between this graph and mChannelFillGraph:
    painter->setPen(Qt::NoPen);
    painter->setBrush(mBrush);
    painter->drawPolygon(getChannelFillPolygon(lineData));
  }
}

/*! 
  \internal
  Draws scatter symbols at every data point passed in \a pointData. scatter symbols are independent of
  the line style and are always drawn if scatter style is not \ref ssNone. Hence, the \a pointData vector
  is outputted by all "get(...)PlotData" functions, together with the (line style dependent) line data.
  \see drawLinePlot, drawImpulsePlot
*/
void QCPGraph::drawScatterPlot(QPainter *painter, QVector<QCPData> *pointData) const
{
  // draw error bars:
  if (mErrorType != etNone)
  {
    painter->setRenderHint(QPainter::Antialiasing, mParentPlot->antialiasedElements().testFlag(QCustomPlot::aeErrorBars));
    painter->setPen(mErrorPen);
    if (mKeyAxis->orientation() == Qt::Vertical)
    {
      for (int i=0; i<pointData->size(); ++i)
        drawError(painter, mValueAxis->coordToPixel(pointData->at(i).value), mKeyAxis->coordToPixel(pointData->at(i).key), pointData->at(i));
    } else
    {
      for (int i=0; i<pointData->size(); ++i)
        drawError(painter, mKeyAxis->coordToPixel(pointData->at(i).key), mValueAxis->coordToPixel(pointData->at(i).value), pointData->at(i));
    }
  }
  
  // draw scatter point symbols:
  painter->setRenderHint(QPainter::Antialiasing, mParentPlot->antialiasedElements().testFlag(QCustomPlot::aeScatters));
  painter->setPen(mPen);
  painter->setBrush(mBrush);
  if (mKeyAxis->orientation() == Qt::Vertical)
  {
    for (int i=0; i<pointData->size(); ++i)
      drawScatter(painter, mValueAxis->coordToPixel(pointData->at(i).value), mKeyAxis->coordToPixel(pointData->at(i).key), mScatterStyle);
  } else
  {
    for (int i=0; i<pointData->size(); ++i)
      drawScatter(painter, mKeyAxis->coordToPixel(pointData->at(i).key), mValueAxis->coordToPixel(pointData->at(i).value), mScatterStyle);
  }
}

/*! 
  \internal
  Draws line graphs from the provided data. It connects all points in \a lineData, which
  was created by one of the "get(...)PlotData" functions for line styles that require simple line
  connections between the point vector they create. These are for example \ref getLinePlotData, \ref
  getStepLeftPlotData, \ref getStepRightPlotData and \ref getStepCenterPlotData.
  \see drawScatterPlot, drawImpulsePlot
*/
void QCPGraph::drawLinePlot(QPainter *painter, QVector<QPointF> *lineData) const
{
  // draw line of graph:
  if (mPen.style() != Qt::NoPen && mPen.color().alpha() != 0)
  {
    painter->setRenderHint(QPainter::Antialiasing, mParentPlot->antialiasedElements().testFlag(QCustomPlot::aeGraphs));
    painter->setPen(mPen);
    painter->setBrush(Qt::NoBrush);
    painter->drawPolyline(QPolygonF(*lineData));
  }
}

/*! 
  \internal
  Draws impulses graphs from the provided data, i.e. it connects all line pairs in \a lineData, which was
  created by \ref getImpulsePlotData.
  \see drawScatterPlot, drawLinePlot
*/
void QCPGraph::drawImpulsePlot(QPainter *painter, QVector<QPointF> *lineData) const
{
  // draw impulses:
  if (mPen.style() != Qt::NoPen && mPen.color().alpha() != 0)
  {
    painter->setRenderHint(QPainter::Antialiasing, mParentPlot->antialiasedElements().testFlag(QCustomPlot::aeGraphs));
    painter->setPen(mPen);
    painter->setBrush(Qt::NoBrush);
    painter->drawLines(*lineData);
  }
}

/*! 
  \internal
  Called by the scatter plot drawing function (\ref drawScatterPlot) to draw single point representations at the
  pixel positions \a x and \a y in the scatter style \a style.
  
  \warning this function changes the brush of the painter for scatter styles \ref ssCircle, \ref ssDisc and \ref ssSquare
  in order to draw clear (ssSquare, ssCircle) and filled (ssDisc) shapes.
*/
void QCPGraph::drawScatter(QPainter *painter, double x, double y, ScatterStyle style) const
{
  // If you change this correction, make sure pdf exported scatters are properly centered in error bars!
  // There seems to be some kind of discrepancy for different paint devices here.
  if (style == ssCross || style == ssPlus)
  {
    x = x-0.7; // paint system correction, else, we don't get pixel exact matches (Qt problem)
    y = y-0.4; // paint system correction, else, we don't get pixel exact matches (Qt problem)
  }
  
  double w = mScatterSize/2.0;
  switch (style)
  {
    case ssDot:
    {
      painter->drawPoint(QPointF(x, y));
      break;
    }
    case ssCross:
    {
      painter->drawLine(QLineF(x-w, y-w, x+w, y+w));
      painter->drawLine(QLineF(x-w, y+w, x+w, y-w));
      break;
    }
    case ssPlus:
    {
      painter->drawLine(QLineF(x-w, y, x+w, y));
      painter->drawLine(QLineF(x, y+w, x, y-w));
      break;
    }
    case ssCircle:
    {
      painter->setBrush(Qt::NoBrush);
      painter->drawEllipse(x-w,y-w,mScatterSize,mScatterSize);
      break;
    }
    case ssDisc:
    {
      painter->setBrush(QBrush(painter->pen().color()));
      painter->drawEllipse(QPointF(x,y), w, w);
      break;
    }
    case ssSquare:
    {
      painter->setBrush(Qt::NoBrush);
      painter->drawRect(x-w,y-w,mScatterSize,mScatterSize);
      break;
    }
    case ssStar:
    {
      painter->drawLine(QLineF(x-w, y, x+w, y));
      painter->drawLine(QLineF(x, y+w, x, y-w));
      painter->drawLine(QLineF(x-w*0.707, y-w*0.707, x+w*0.707, y+w*0.707));
      painter->drawLine(QLineF(x-w*0.707, y+w*0.707, x+w*0.707, y-w*0.707));
      break;
    }
    case ssTriangle:
    {
      painter->drawLine(QLineF(x-w, y+0.755*w, x+w, y+0.755*w));
      painter->drawLine(QLineF(x+w, y+0.755*w, x, y-0.977*w));
      painter->drawLine(QLineF(x, y-0.977*w, x-w, y+0.755*w));
      break;
    }
    case ssTriangleInverted:
    {
      painter->drawLine(QLineF(x-w, y-0.755*w, x+w, y-0.755*w));
      painter->drawLine(QLineF(x+w, y-0.755*w, x, y+0.977*w));
      painter->drawLine(QLineF(x, y+0.977*w, x-w, y-0.755*w));
      break;
    }
    case ssCrossSquare:
    {
      painter->setBrush(Qt::NoBrush);
      painter->drawLine(QLineF(x-w, y-w, x+w*0.95, y+w*0.95));
      painter->drawLine(QLineF(x-w, y+w*0.95, x+w*0.95, y-w));
      painter->drawRect(x-w,y-w,mScatterSize,mScatterSize);
      break;
    }
    case ssPlusSquare:
    {
      painter->setBrush(Qt::NoBrush);
      painter->drawLine(QLineF(x-w, y, x+w*0.95, y));
      painter->drawLine(QLineF(x, y+w, x, y-w));
      painter->drawRect(x-w,y-w,mScatterSize,mScatterSize);
      break;
    }
    case ssCrossCircle:
    {
      painter->setBrush(Qt::NoBrush);
      painter->drawLine(QLineF(x-w*0.707, y-w*0.707, x+w*0.67, y+w*0.67));
      painter->drawLine(QLineF(x-w*0.707, y+w*0.67, x+w*0.67, y-w*0.707));
      painter->drawEllipse(x-w,y-w,mScatterSize,mScatterSize);
      break;
    }
    case ssPlusCircle:
    {
      painter->setBrush(Qt::NoBrush);
      painter->drawLine(QLineF(x-w, y, x+w, y));
      painter->drawLine(QLineF(x, y+w, x, y-w));
      painter->drawEllipse(x-w,y-w,mScatterSize,mScatterSize);
      break;
    }
    case ssPeace:
    {
      painter->setBrush(Qt::NoBrush);
      painter->drawLine(QLineF(x, y-w, x, y+w));
      painter->drawLine(QLineF(x, y, x-w*0.707, y+w*0.707));
      painter->drawLine(QLineF(x, y, x+w*0.707, y+w*0.707));
      painter->drawEllipse(x-w,y-w,mScatterSize,mScatterSize);
      break;
    }
    case ssPixmap:
    {
      painter->drawPixmap(x-mScatterPixmap.width()*0.5, y-mScatterPixmap.height()*0.5, mScatterPixmap);
      // if something in here is changed, adapt ssPixmap scatter style case in drawLegendIcon(), too
      break;
    }
    default: break;
  }
}

/*! 
  \internal
  called by the scatter drawing function (\ref drawScatterPlot) to draw the error bars on one data
  point. \a x and \a y pixel positions of the data point are passed since they are already known in
  pixel coordinates in the drawing function, so we save some extra coordToPixel transforms here. \a
  data is therefore only used for the errors, not key and value.
*/
void QCPGraph::drawError(QPainter *painter, double x, double y, const QCPData &data) const
{
  double a, b; // positions of error bar bounds in pixels
  double barWidthHalf = mErrorBarSize*0.5;
  double skipSymbolMargin = mScatterSize*1.25; // pixels left blank per side, when mErrorBarSkipSymbol is true
  
  if (!mParentPlot->antialiasedElements().testFlag(QCustomPlot::aeErrorBars))
  {
    x = x-0.9; // paint system correction, else, we don't get pixel exact matches (Qt problem)
    y = y-0.9; // paint system correction, else, we don't get pixel exact matches (Qt problem)
  }
  
  if (mKeyAxis->orientation() == Qt::Vertical)
  {
    // draw key error vertically and value error horizontally
    if (mErrorType == etKey || mErrorType == etBoth)
    {
      a = mKeyAxis->coordToPixel(data.key-data.keyErrorMinus);
      b = mKeyAxis->coordToPixel(data.key+data.keyErrorPlus);
      if (mKeyAxis->rangeReversed())
        qSwap(a,b);
      // draw spine:
      if (mErrorBarSkipSymbol)
      {
        if (a-y > skipSymbolMargin) // don't draw spine if error is so small it's within skipSymbolmargin
          painter->drawLine(QLineF(x, a, x, y+skipSymbolMargin));
        if (y-b > skipSymbolMargin) 
          painter->drawLine(QLineF(x, y-skipSymbolMargin, x, b));
      } else
        painter->drawLine(QLineF(x, a, x, b));
      // draw handles:
      painter->drawLine(QLineF(x-barWidthHalf, a, x+barWidthHalf, a));
      painter->drawLine(QLineF(x-barWidthHalf, b, x+barWidthHalf, b));
    }
    if (mErrorType == etValue || mErrorType == etBoth)
    {
      a = mValueAxis->coordToPixel(data.value-data.valueErrorMinus);
      b = mValueAxis->coordToPixel(data.value+data.valueErrorPlus);
      if (mValueAxis->rangeReversed())
        qSwap(a,b);
      // draw spine:
      if (mErrorBarSkipSymbol)
      {
        if (x-a > skipSymbolMargin) // don't draw spine if error is so small it's within skipSymbolmargin
          painter->drawLine(QLineF(a, y, x-skipSymbolMargin, y));
        if (b-x > skipSymbolMargin)
          painter->drawLine(QLineF(x+skipSymbolMargin, y, b, y));
      } else
        painter->drawLine(QLineF(a, y, b, y));
      // draw handles:
      painter->drawLine(QLineF(a, y-barWidthHalf, a, y+barWidthHalf));
      painter->drawLine(QLineF(b, y-barWidthHalf, b, y+barWidthHalf));
    }
  } else
  {
    // draw value error vertically and key error horizontally
    if (mErrorType == etKey || mErrorType == etBoth)
    {
      a = mKeyAxis->coordToPixel(data.key-data.keyErrorMinus);
      b = mKeyAxis->coordToPixel(data.key+data.keyErrorPlus);
      if (mKeyAxis->rangeReversed())
        qSwap(a,b);
      // draw spine:
      if (mErrorBarSkipSymbol)
      {
        if (x-a > skipSymbolMargin) // don't draw spine if error is so small it's within skipSymbolmargin
          painter->drawLine(QLineF(a, y, x-skipSymbolMargin, y));
        if (b-x > skipSymbolMargin)
          painter->drawLine(QLineF(x+skipSymbolMargin, y, b, y));
      } else
        painter->drawLine(QLineF(a, y, b, y));
      // draw handles:
      painter->drawLine(QLineF(a, y-barWidthHalf, a, y+barWidthHalf));
      painter->drawLine(QLineF(b, y-barWidthHalf, b, y+barWidthHalf));
    }
    if (mErrorType == etValue || mErrorType == etBoth)
    {
      a = mValueAxis->coordToPixel(data.value-data.valueErrorMinus);
      b = mValueAxis->coordToPixel(data.value+data.valueErrorPlus);
      if (mValueAxis->rangeReversed())
        qSwap(a,b);
      // draw spine:
      if (mErrorBarSkipSymbol)
      {
        if (a-y > skipSymbolMargin) // don't draw spine if error is so small it's within skipSymbolmargin
          painter->drawLine(QLineF(x, a, x, y+skipSymbolMargin));
        if (y-b > skipSymbolMargin)
          painter->drawLine(QLineF(x, y-skipSymbolMargin, x, b));
      } else
        painter->drawLine(QLineF(x, a, x, b));
      // draw handles:
      painter->drawLine(QLineF(x-barWidthHalf, a, x+barWidthHalf, a));
      painter->drawLine(QLineF(x-barWidthHalf, b, x+barWidthHalf, b));
    }
  }
}

/*! 
  \internal
  called by the specific plot data generating functions "get(...)PlotData" to determine
  which data range is visible, so only that needs to be processed.
  
  \param[out] lower returns an iterator to the lowest data point that needs to be taken into account
  when plotting. Note that in order to get a clean plot all the way to the edge of the axes, \a lower
  may still be outside the visible range.
  \param[out] upper returns an iterator to the highest data point. Same as before, \a upper may also
  lie outside of the visible range.
  \param[out] count number of data points that need plotting, i.e. points between \a lower and \a upper,
  including them. This is useful for allocating the array of QPointFs in the specific drawing functions.
*/
void QCPGraph::getVisibleDataBounds(QCPDataMap::const_iterator &lower, QCPDataMap::const_iterator &upper, int &count) const
{
  // get visible data range as QMap iterators
  QCPDataMap::const_iterator lbound = mData->lowerBound(mKeyAxis->range().lower);
  QCPDataMap::const_iterator ubound = mData->upperBound(mKeyAxis->range().upper)-1;
  bool lowoutlier = lbound != mData->constBegin(); // indicates whether there exist points below axis range
  bool highoutlier = ubound+1 != mData->constEnd(); // indicates whether there exist points above axis range
  lower = (lowoutlier ? lbound-1 : lbound); // data pointrange that will be actually drawn
  upper = (highoutlier ? ubound+1 : ubound); // data pointrange that will be actually drawn
  
  // count number of points in range lower to upper (including them), so we can allocate array for them in draw functions:
  QCPDataMap::const_iterator it = lower;
  count = 1;
  while (it != upper)
  {
    ++it;
    ++count;
  }
}

/*! 
  \internal
  The line data vector generated by e.g. getLinePlotData contains only the line
  that connects the data points. If the graph needs to be filled, two additional points
  need to be added at the value-zero-line in the lower and upper key positions, the graph
  reaches. This function calculates these points and adds them to the end of \a lineData.
  Since the fill is typically drawn before the line stroke, these added points need to
  be removed again after the fill is done, with the removeFillBasePoints function.
  
  The expanding of \a lineData by two points will not cause unnecessary memory reallocations,
  because the data vector generation functions (getLinePlotData etc.) reserve two extra points
  when they allocate memory for \a lineData.
  \see removeFillBasePoints, lowerFillBasePoint, upperFillBasePoint
*/
void QCPGraph::addFillBasePoints(QVector<QPointF> *lineData) const
{
  // append points that close the polygon fill at the key axis:
  if (mKeyAxis->orientation() == Qt::Vertical)
  {
    *lineData << upperFillBasePoint(lineData->last().y());
    *lineData << lowerFillBasePoint(lineData->first().y());
  } else
  {
    *lineData << upperFillBasePoint(lineData->last().x());
    *lineData << lowerFillBasePoint(lineData->first().x());
  }
}

/*! 
  \internal
  removes the two points from \a lineData that were added by addFillBasePoints.
  \see addFillBasePoints, lowerFillBasePoint, upperFillBasePoint
*/
void QCPGraph::removeFillBasePoints(QVector<QPointF> *lineData) const
{
  lineData->remove(lineData->size()-2, 2);
}

/*! 
  \internal
  called by addFillBasePoints to conveniently assign the point which closes the fill
  polygon on the lower side of the zero-value-line parallel to the key axis.
  The logarithmic axis scale case is a bit special, since the zero-value-line in pixel coordinates
  is in positive or negative infinity. So this case is handled separately by just closing the
  fill polygon on the axis which lies in the direction towards the zero value.
  
  \param lowerKey pixel position of the lower key of the point. Depending on whether the key axis
  is horizontal or vertical, \a lowerKey will end up as the x or y value of the returned point,
  respectively.
  \see upperFillBasePoint, addFillBasePoints
*/
QPointF QCPGraph::lowerFillBasePoint(double lowerKey) const
{
  QPointF point;
  if (mValueAxis->scaleType() == QCPAxis::stLinear)
  {
    if (mKeyAxis->axisType() == QCPAxis::atLeft)
    {
      point.setX(mValueAxis->coordToPixel(0));
      point.setY(lowerKey);
    } else if (mKeyAxis->axisType() == QCPAxis::atRight)
    {
      point.setX(mValueAxis->coordToPixel(0));
      point.setY(lowerKey);
    } else if (mKeyAxis->axisType() == QCPAxis::atTop)
    {
      point.setX(lowerKey);
      point.setY(mValueAxis->coordToPixel(0));
    } else if (mKeyAxis->axisType() == QCPAxis::atBottom)
    {
      point.setX(lowerKey);
      point.setY(mValueAxis->coordToPixel(0));
    }
  } else // mValueAxis->mScaleType == QCPAxis::stLogarithmic
  {
    // In logarithmic scaling we can't just draw to value zero so we just fill all the way
    // to the axis which is in the direction towards zero
    if (mKeyAxis->orientation() == Qt::Vertical)
    {
      if ((mValueAxis->range().upper < 0 && !mValueAxis->rangeReversed()) ||
          (mValueAxis->range().upper > 0 && mValueAxis->rangeReversed())) // if range is negative, zero is on opposite side of key axis
        point.setX(mKeyAxis->axisRect().right());
      else
        point.setX(mKeyAxis->axisRect().left());
      point.setY(lowerKey);
    } else if (mKeyAxis->axisType() == QCPAxis::atTop || mKeyAxis->axisType() == QCPAxis::atBottom)
    {
      point.setX(lowerKey);
      if ((mValueAxis->range().upper < 0 && !mValueAxis->rangeReversed()) ||
          (mValueAxis->range().upper > 0 && mValueAxis->rangeReversed())) // if range is negative, zero is on opposite side of key axis
        point.setY(mKeyAxis->axisRect().top());
      else
        point.setY(mKeyAxis->axisRect().bottom());
    }
  }
  return point;
}

/*! 
  \internal
  called by addFillBasePoints to conveniently assign the point which closes the fill
  polygon on the upper side of the zero-value-line parallel to the key axis. The logarithmic axis
  scale case is a bit special, since the zero-value-line in pixel coordinates is in positive or
  negative infinity. So this case is handled separately by just closing the fill polygon on the
  axis which lies in the direction towards the zero value.
  
  \param upperKey pixel position of the upper key of the point. Depending on whether the key axis
  is horizontal or vertical, \a upperKey will end up as the x or y value of the returned point,
  respectively.
  \see lowerFillBasePoint, addFillBasePoints
*/
QPointF QCPGraph::upperFillBasePoint(double upperKey) const
{
  QPointF point;
  if (mValueAxis->scaleType() == QCPAxis::stLinear)
  {
    if (mKeyAxis->axisType() == QCPAxis::atLeft)
    {
      point.setX(mValueAxis->coordToPixel(0));
      point.setY(upperKey);
    } else if (mKeyAxis->axisType() == QCPAxis::atRight)
    {
      point.setX(mValueAxis->coordToPixel(0));
      point.setY(upperKey);
    } else if (mKeyAxis->axisType() == QCPAxis::atTop)
    {
      point.setX(upperKey);
      point.setY(mValueAxis->coordToPixel(0));
    } else if (mKeyAxis->axisType() == QCPAxis::atBottom)
    {
      point.setX(upperKey);
      point.setY(mValueAxis->coordToPixel(0));
    }
  } else // mValueAxis->mScaleType == QCPAxis::stLogarithmic
  {
    // In logarithmic scaling we can't just draw to value 0 so we just fill all the way
    // to the axis which is in the direction towards 0
    if (mKeyAxis->orientation() == Qt::Vertical)
    {
      if ((mValueAxis->range().upper < 0 && !mValueAxis->rangeReversed()) ||
          (mValueAxis->range().upper > 0 && mValueAxis->rangeReversed())) // if range is negative, zero is on opposite side of key axis
        point.setX(mKeyAxis->axisRect().right());
      else
        point.setX(mKeyAxis->axisRect().left());
      point.setY(upperKey);
    } else if (mKeyAxis->axisType() == QCPAxis::atTop || mKeyAxis->axisType() == QCPAxis::atBottom)
    {
      point.setX(upperKey);
      if ((mValueAxis->range().upper < 0 && !mValueAxis->rangeReversed()) ||
          (mValueAxis->range().upper > 0 && mValueAxis->rangeReversed())) // if range is negative, zero is on opposite side of key axis
        point.setY(mKeyAxis->axisRect().top());
      else
        point.setY(mKeyAxis->axisRect().bottom());
    }
  }
  return point;
}

/*!
  \internal
  Generates the polygon needed for drawing channel fills between this graph (data passed via \a
  lineData) and the graph specified by mChannelFillGraph (data generated by calling its \ref
  getPlotData function). May return an empty polygon if the key ranges have no overlap or fill
  target graph and this graph don't have same orientation (i.e. both key axes horizontal or both
  key axes vertical). For increased performance (due to implicit sharing), keep the returned QPolygonF
  const.
*/
const QPolygonF QCPGraph::getChannelFillPolygon(const QVector<QPointF> *lineData) const
{
  if (mChannelFillGraph->mKeyAxis->orientation() != mKeyAxis->orientation())
    return QPolygonF(); // don't have same axis orientation, can't fill that (Note: if keyAxis fits, valueAxis will fit too, because it's always orthogonal to keyAxis)
  
  if (lineData->isEmpty()) return QPolygonF();
  QVector<QPointF> otherData;
  mChannelFillGraph->getPlotData(&otherData, 0);
  if (otherData.isEmpty()) return QPolygonF();
  QVector<QPointF> thisData;
  thisData.reserve(lineData->size()+otherData.size()); // because we will join both vectors at end of this function
  for (int i=0; i<lineData->size(); ++i) // don't use the vector<<(vector),  it squeezes internally, which ruins the performance tuning with reserve()
    thisData << lineData->at(i);
  
  // pointers to be able to swap them, depending which data range needs cropping:
  QVector<QPointF> *staticData = &thisData;
  QVector<QPointF> *croppedData = &otherData;
  
  // crop both vectors to ranges in which the keys overlap (which coord is key, depends on axisType):
  if (mKeyAxis->orientation() == Qt::Horizontal)
  {
    // x is key
    // if an axis range is reversed, the data point keys will be descending. Reverse them, since following algorithm assumes ascending keys:
    if (staticData->first().x() > staticData->last().x())
    {
      int size = staticData->size();
      for (int i=0; i<size/2; ++i)
        qSwap((*staticData)[i], (*staticData)[size-1-i]);
    }
    if (croppedData->first().x() > croppedData->last().x())
    {
      int size = croppedData->size();
      for (int i=0; i<size/2; ++i)
        qSwap((*croppedData)[i], (*croppedData)[size-1-i]);
    }
    // crop lower bound:
    if (staticData->first().x() < croppedData->first().x()) // other one must be cropped
      qSwap(staticData, croppedData);
    int lowBound = findIndexBelowX(croppedData, staticData->first().x());
    if (lowBound == -1) return QPolygonF(); // key ranges have no overlap
    croppedData->remove(0, lowBound);
    // set lowest point of cropped data to fit exactly key position of first static data
    // point via linear interpolation:
    if (croppedData->size() < 2) return QPolygonF(); // need at least two points for interpolation
    double slope;
    if (croppedData->at(1).x()-croppedData->at(0).x() != 0)
      slope = (croppedData->at(1).y()-croppedData->at(0).y())/(croppedData->at(1).x()-croppedData->at(0).x());
    else
      slope = 0;
    (*croppedData)[0].setY(croppedData->at(0).y()+slope*(staticData->first().x()-croppedData->at(0).x()));
    (*croppedData)[0].setX(staticData->first().x());
    
    // crop upper bound:
    if (staticData->last().x() > croppedData->last().x()) // other one must be cropped
      qSwap(staticData, croppedData);
    int highBound = findIndexAboveX(croppedData, staticData->last().x());
    if (highBound == -1) return QPolygonF(); // key ranges have no overlap
    croppedData->remove(highBound+1, croppedData->size()-(highBound+1));
    // set highest point of cropped data to fit exactly key position of last static data
    // point via linear interpolation:
    if (croppedData->size() < 2) return QPolygonF(); // need at least two points for interpolation
    int li = croppedData->size()-1; // last index
    if (croppedData->at(li).x()-croppedData->at(li-1).x() != 0)
      slope = (croppedData->at(li).y()-croppedData->at(li-1).y())/(croppedData->at(li).x()-croppedData->at(li-1).x());
    else
      slope = 0;
    (*croppedData)[li].setY(croppedData->at(li-1).y()+slope*(staticData->last().x()-croppedData->at(li-1).x()));
    (*croppedData)[li].setX(staticData->last().x());
  } else // mKeyAxis->orientation() == Qt::Vertical
  {
    // y is key
    // similar to "x is key" but switched x,y. Further, lower/upper meaning is inverted compared to x,
    // because in pixel coordinates, y increases from top to bottom, not bottom to top like data coordinate.
    // if an axis range is reversed, the data point keys will be descending. Reverse them, since following algorithm assumes ascending keys:
    if (staticData->first().y() < staticData->last().y())
    {
      int size = staticData->size();
      for (int i=0; i<size/2; ++i)
        qSwap((*staticData)[i], (*staticData)[size-1-i]);
    }
    if (croppedData->first().y() < croppedData->last().y())
    {
      int size = croppedData->size();
      for (int i=0; i<size/2; ++i)
        qSwap((*croppedData)[i], (*croppedData)[size-1-i]);
    }
    // crop lower bound:
    if (staticData->first().y() > croppedData->first().y()) // other one must be cropped
      qSwap(staticData, croppedData);
    int lowBound = findIndexAboveY(croppedData, staticData->first().y());
    if (lowBound == -1) return QPolygonF(); // key ranges have no overlap
    croppedData->remove(0, lowBound);
    // set lowest point of cropped data to fit exactly key position of first static data
    // point via linear interpolation:
    if (croppedData->size() < 2) return QPolygonF(); // need at least two points for interpolation
    double slope;
    if (croppedData->at(1).y()-croppedData->at(0).y() != 0) // avoid division by zero in step plots
      slope = (croppedData->at(1).x()-croppedData->at(0).x())/(croppedData->at(1).y()-croppedData->at(0).y());
    else
      slope = 0;
    (*croppedData)[0].setX(croppedData->at(0).x()+slope*(staticData->first().y()-croppedData->at(0).y()));
    (*croppedData)[0].setY(staticData->first().y());
    
    // crop upper bound:
    if (staticData->last().y() < croppedData->last().y()) // other one must be cropped
      qSwap(staticData, croppedData);
    int highBound = findIndexBelowY(croppedData, staticData->last().y());
    if (highBound == -1) return QPolygonF(); // key ranges have no overlap
    croppedData->remove(highBound+1, croppedData->size()-(highBound+1));
    // set highest point of cropped data to fit exactly key position of last static data
    // point via linear interpolation:
    if (croppedData->size() < 2) return QPolygonF(); // need at least two points for interpolation
    int li = croppedData->size()-1; // last index
    if (croppedData->at(li).y()-croppedData->at(li-1).y() != 0) // avoid division by zero in step plots
      slope = (croppedData->at(li).x()-croppedData->at(li-1).x())/(croppedData->at(li).y()-croppedData->at(li-1).y());
    else
      slope = 0;
    (*croppedData)[li].setX(croppedData->at(li-1).x()+slope*(staticData->last().y()-croppedData->at(li-1).y()));
    (*croppedData)[li].setY(staticData->last().y());
  }
  
  // return joined:
  for (int i=otherData.size()-1; i>=0; --i) // insert reversed, otherwise the polygon will be twisted
    thisData << otherData.at(i);
  return QPolygonF(thisData);
}

/*! 
  \internal
  Finds the smallest index of \a data, whose points x value is just above \a x.
  Assumes x values in \a data points are ordered ascending, as is the case
  when plotting with horizontal key axis.
  Used to calculate the channel fill polygon, see \ref getChannelFillPolygon.
*/
int QCPGraph::findIndexAboveX(const QVector<QPointF> *data, double x) const
{
  for (int i=data->size()-1; i>=0; --i)
  {
    if (data->at(i).x() < x)
    {
      if (i<data->size()-1)
        return i+1;
      else
        return data->size()-1;
    }
  }
  return -1;
}

/*! 
  \internal
  Finds the greatest index of \a data, whose points x value is just below \a x.
  Assumes x values in \a data points are ordered ascending, as is the case
  when plotting with horizontal key axis.
  Used to calculate the channel fill polygon, see \ref getChannelFillPolygon.
*/
int QCPGraph::findIndexBelowX(const QVector<QPointF> *data, double x) const
{
  for (int i=0; i<data->size(); ++i)
  {
    if (data->at(i).x() > x)
    {
      if (i>0)
        return i-1;
      else
        return 0;
    }
  }
  return -1;
}

/*! 
  \internal
  Finds the smallest index of \a data, whose points y value is just above \a y.
  Assumes y values in \a data points are ordered descending, as is the case
  when plotting with vertical key axis.
  Used to calculate the channel fill polygon, see \ref getChannelFillPolygon.
*/
int QCPGraph::findIndexAboveY(const QVector<QPointF> *data, double y) const
{
  for (int i=0; i<data->size(); ++i)
  {
    if (data->at(i).y() < y)
    {
      if (i>0)
        return i-1;
      else
        return 0;
    }
  }
  return -1;
}

/*! 
  \internal
  Finds the greatest index of \a data, whose points y value is just below \a y.
  Assumes y values in \a data points are ordered descending, as is the case
  when plotting with vertical key axis.
  Used to calculate the channel fill polygon, see \ref getChannelFillPolygon.
*/
int QCPGraph::findIndexBelowY(const QVector<QPointF> *data, double y) const
{
  for (int i=data->size()-1; i>=0; --i)
  {
    if (data->at(i).y() > y)
    {
      if (i<data->size()-1)
        return i+1;
      else
        return data->size()-1;
    }
  }
  return -1;
}

/* inherits documentation from base class */
QCPRange QCPGraph::getKeyRange(bool &validRange, SignDomain inSignDomain) const
{
  // just call the specialized version which takes an additional argument whether error bars
  // should also be taken into consideration for range calculation. We set this to true here.
  return getKeyRange(validRange, inSignDomain, true);
}

/* inherits documentation from base class */
QCPRange QCPGraph::getValueRange(bool &validRange, SignDomain inSignDomain) const
{
  // just call the specialized version which takes an additional argument whether error bars
  // should also be taken into consideration for range calculation. We set this to true here.
  return getValueRange(validRange, inSignDomain, true);
}

/*! \overload
  Allows to specify whether the error bars should be included in the range calculation.
  
  \see getKeyRange(bool &validRange, SignDomain inSignDomain)
*/
QCPRange QCPGraph::getKeyRange(bool &validRange, SignDomain inSignDomain, bool includeErrors) const
{
  QCPRange range;
  bool haveLower = false;
  bool haveUpper = false;
  
  double current, currentErrorMinus, currentErrorPlus;
  
  if (inSignDomain == sdBoth) // range may be anywhere
  {
    QCPDataMap::const_iterator it = mData->constBegin();
    while (it != mData->constEnd())
    {
      current = it.value().key;
      currentErrorMinus = (includeErrors ? it.value().keyErrorMinus : 0);
      currentErrorPlus = (includeErrors ? it.value().keyErrorPlus : 0);
      if (current-currentErrorMinus < range.lower || !haveLower)
      {
        range.lower = current-currentErrorMinus;
        haveLower = true;
      }
      if (current+currentErrorPlus > range.upper || !haveUpper)
      {
        range.upper = current+currentErrorPlus;
        haveUpper = true;
      }
      it++;
    }
  } else if (inSignDomain == sdNegative) // range may only be in the negative sign domain
  {
    QCPDataMap::const_iterator it = mData->constBegin();
    while (it != mData->constEnd())
    {
      current = it.value().key;
      currentErrorMinus = (includeErrors ? it.value().keyErrorMinus : 0);
      currentErrorPlus = (includeErrors ? it.value().keyErrorPlus : 0);
      if ((current-currentErrorMinus < range.lower || !haveLower) && current-currentErrorMinus < 0)
      {
        range.lower = current-currentErrorMinus;
        haveLower = true;
      }
      if ((current+currentErrorPlus > range.upper || !haveUpper) && current+currentErrorPlus < 0)
      {
        range.upper = current+currentErrorPlus;
        haveUpper = true;
      }
      if (includeErrors) // in case point is in valid sign domain but errobars stretch beyond it, we still want to geht that point.
      {
        if ((current < range.lower || !haveLower) && current < 0)
        {
          range.lower = current;
          haveLower = true;
        }
        if ((current > range.upper || !haveUpper) && current < 0)
        {
          range.upper = current;
          haveUpper = true;
        }
      }
      it++;
    }
  } else if (inSignDomain == sdPositive) // range may only be in the positive sign domain
  {
    QCPDataMap::const_iterator it = mData->constBegin();
    while (it != mData->constEnd())
    {
      current = it.value().key;
      currentErrorMinus = (includeErrors ? it.value().keyErrorMinus : 0);
      currentErrorPlus = (includeErrors ? it.value().keyErrorPlus : 0);
      if ((current-currentErrorMinus < range.lower || !haveLower) && current-currentErrorMinus > 0)
      {
        range.lower = current-currentErrorMinus;
        haveLower = true;
      }
      if ((current+currentErrorPlus > range.upper || !haveUpper) && current+currentErrorPlus > 0)
      {
        range.upper = current+currentErrorPlus;
        haveUpper = true;
      }
      if (includeErrors) // in case point is in valid sign domain but errobars stretch beyond it, we still want to get that point.
      {
        if ((current < range.lower || !haveLower) && current > 0)
        {
          range.lower = current;
          haveLower = true;
        }
        if ((current > range.upper || !haveUpper) && current > 0)
        {
          range.upper = current;
          haveUpper = true;
        }
      }
      it++;
    }
  }
  
  validRange = haveLower && haveUpper;
  return range;
}

/*! \overload
  Allows to specify whether the error bars should be included in the range calculation.
  
  \see getValueRange(bool &validRange, SignDomain inSignDomain)
*/
QCPRange QCPGraph::getValueRange(bool &validRange, SignDomain inSignDomain, bool includeErrors) const
{
  QCPRange range;
  bool haveLower = false;
  bool haveUpper = false;
  
  double current, currentErrorMinus, currentErrorPlus;
  
  if (inSignDomain == sdBoth) // range may be anywhere
  {
    QCPDataMap::const_iterator it = mData->constBegin();
    while (it != mData->constEnd())
    {
      current = it.value().value;
      currentErrorMinus = (includeErrors ? it.value().valueErrorMinus : 0);
      currentErrorPlus = (includeErrors ? it.value().valueErrorPlus : 0);
      if (current-currentErrorMinus < range.lower || !haveLower)
      {
        range.lower = current-currentErrorMinus;
        haveLower = true;
      }
      if (current+currentErrorPlus > range.upper || !haveUpper)
      {
        range.upper = current+currentErrorPlus;
        haveUpper = true;
      }
      it++;
    }
  } else if (inSignDomain == sdNegative) // range may only be in the negative sign domain
  {
    QCPDataMap::const_iterator it = mData->constBegin();
    while (it != mData->constEnd())
    {
      current = it.value().value;
      currentErrorMinus = (includeErrors ? it.value().valueErrorMinus : 0);
      currentErrorPlus = (includeErrors ? it.value().valueErrorPlus : 0);
      if ((current-currentErrorMinus < range.lower || !haveLower) && current-currentErrorMinus < 0)
      {
        range.lower = current-currentErrorMinus;
        haveLower = true;
      }
      if ((current+currentErrorPlus > range.upper || !haveUpper) && current+currentErrorPlus < 0)
      {
        range.upper = current+currentErrorPlus;
        haveUpper = true;
      }
      if (includeErrors) // in case point is in valid sign domain but errobars stretch beyond it, we still want to get that point.
      {
        if ((current < range.lower || !haveLower) && current < 0)
        {
          range.lower = current;
          haveLower = true;
        }
        if ((current > range.upper || !haveUpper) && current < 0)
        {
          range.upper = current;
          haveUpper = true;
        }
      }
      it++;
    }
  } else if (inSignDomain == sdPositive) // range may only be in the positive sign domain
  {
    QCPDataMap::const_iterator it = mData->constBegin();
    while (it != mData->constEnd())
    {
      current = it.value().value;
      currentErrorMinus = (includeErrors ? it.value().valueErrorMinus : 0);
      currentErrorPlus = (includeErrors ? it.value().valueErrorPlus : 0);
      if ((current-currentErrorMinus < range.lower || !haveLower) && current-currentErrorMinus > 0)
      {
        range.lower = current-currentErrorMinus;
        haveLower = true;
      }
      if ((current+currentErrorPlus > range.upper || !haveUpper) && current+currentErrorPlus > 0)
      {
        range.upper = current+currentErrorPlus;
        haveUpper = true;
      }
      if (includeErrors) // in case point is in valid sign domain but errobars stretch beyond it, we still want to geht that point.
      {
        if ((current < range.lower || !haveLower) && current > 0)
        {
          range.lower = current;
          haveLower = true;
        }
        if ((current > range.upper || !haveUpper) && current > 0)
        {
          range.upper = current;
          haveUpper = true;
        }
      }
      it++;
    }
  }
  
  validRange = haveLower && haveUpper;
  return range;
}


// ================================================================================
// =================== QCPRange
// ================================================================================
/*! \class QCPRange
  \brief Represents the range an axis is encompassing.
  
  contains a \a lower and \a upper double value and provides convenience input, output and
  modification functions.
  
  \see QCPAxis::setRange
*/

/*! 
  Minimum range size (\a upper - \a lower) the range changing functions will accept. Smaller
  intervals would cause errors due to the 11-bit exponent of double precision numbers,
  corresponding to a minimum magnitude of roughly 1e-308.
  \see validRange, maxRange
*/
const double QCPRange::minRange = 1e-280;

/*! 
  Maximum values (negative and positive) the range will accept in range-changing functions.
  Larger absolute values would cause errors due to the 11-bit exponent of double precision numbers,
  corresponding to a maximum magnitude of roughly 1e308.
  Since the number of planck-volumes in the entire visible universe is only ~1e183, this should
  be enough.
  \see validRange, minRange
*/
const double QCPRange::maxRange = 1e250;

/*! 
  Constructs a range with \a lower and \a upper set to zero.
*/
QCPRange::QCPRange() :
  lower(0),
  upper(0)
{
}

/*! \overload
  Constructs a range with the specified \a lower and \a upper values.
*/
QCPRange::QCPRange(double lower, double upper)
{
  this->lower = lower;
  this->upper = upper;
}

/*! 
  Returns the size of the range, i.e. \a upper-\a lower
*/
double QCPRange::size() const
{
  return upper-lower;
}

/*! 
  Returns the center of the range, i.e. (\a upper+\a lower)*0.5
*/
double QCPRange::center() const
{
  return (upper+lower)*0.5;
}

/*! 
  Makes sure \a lower is numerically smaller than \a upper. If this is not the case, the values
  are swapped.
*/
void QCPRange::normalize()
{
  if (lower > upper)
    qSwap(lower, upper);
}

/*! 
  Returns a sanitized version of the range. Sanitized means for logarithmic scales, that
  the range won't span the positive and negative sign domain, i.e. contain zero. Further
  \a lower will always be numerically smaller (or equal) to \a upper.
  
  If the original range does span positive and negative sign domains or contains zero,
  the returned range will try to approximate the original range as good as possible.
  If the positive interval of the original range is wider than the negative interval, the
  returned range will only contain the positive interval, with lower bound set to \a rangeFac or
  \a rangeFac *\a upper, whichever is closer to zero. Same procedure is used if the negative interval
  is wider than the positive interval, this time by changing the \a upper bound.
*/
QCPRange QCPRange::sanitizedForLogScale() const
{
  double rangeFac = 1e-3;
  QCPRange sanitizedRange(lower, upper);
  sanitizedRange.normalize();
  // can't have range spanning negative and positive values in log plot, so change range to fix it
  //if (qFuzzyCompare(sanitizedRange.lower+1, 1) && !qFuzzyCompare(sanitizedRange.upper+1, 1))
  if (sanitizedRange.lower == 0.0 && sanitizedRange.upper != 0.0)
  {
    // case lower is 0
    if (rangeFac < sanitizedRange.upper*rangeFac)
      sanitizedRange.lower = rangeFac;
    else
      sanitizedRange.lower = sanitizedRange.upper*rangeFac;
  } //else if (!qFuzzyCompare(lower+1, 1) && qFuzzyCompare(upper+1, 1))
  else if (sanitizedRange.lower != 0.0 && sanitizedRange.upper == 0.0)
  {
    // case upper is 0
    if (-rangeFac > sanitizedRange.lower*rangeFac)
      sanitizedRange.upper = -rangeFac;
    else
      sanitizedRange.upper = sanitizedRange.lower*rangeFac;
  } else if (sanitizedRange.lower < 0 && sanitizedRange.upper > 0)
  {
    // find out whether negative or positive interval is wider to decide which sign domain will be chosen
    if (-sanitizedRange.lower > sanitizedRange.upper)
    {
      // negative is wider, do same as in case upper is 0
      if (-rangeFac > sanitizedRange.lower*rangeFac)
        sanitizedRange.upper = -rangeFac;
      else
        sanitizedRange.upper = sanitizedRange.lower*rangeFac;
    } else
    {
      // positive is wider, do same as in case lower is 0
      if (rangeFac < sanitizedRange.upper*rangeFac)
        sanitizedRange.lower = rangeFac;
      else
        sanitizedRange.lower = sanitizedRange.upper*rangeFac;
    }
  }
  // due to normalization, case lower>0 && upper<0 should never occur, because that implies upper<lower
  return sanitizedRange;
}

/*! 
  Returns a sanitized version of the range. Sanitized means for linear scales, that
  \a lower will always be numerically smaller (or equal) to \a upper.
*/
QCPRange QCPRange::sanitizedForLinScale() const
{
  QCPRange sanitizedRange(lower, upper);
  sanitizedRange.normalize();
  return sanitizedRange;
}

/*! 
  Checks, whether the specified range is within valid bounds, which are defined
  as QCPRange::maxRange and QCPRange::minRange.
  A valid range means:
  \li range bounds within -maxRange and maxRange
  \li range size above minRange
  \li range size below maxRange
*/
bool QCPRange::validRange(double lower, double upper)
{
  /*
  return (lower > -maxRange &&
          upper < maxRange &&
          fabs(lower-upper) > minRange &&
          (lower < -minRange || lower > minRange) &&
          (upper < -minRange || upper > minRange));
          */
  return (lower > -maxRange &&
          upper < maxRange &&
          fabs(lower-upper) > minRange &&
          fabs(lower-upper) < maxRange);
}

/*! 
  \overload
  Checks, whether the specified range is within valid bounds, which are defined
  as QCPRange::maxRange and QCPRange::minRange.
  A valid range means:
  \li range bounds within -maxRange and maxRange
  \li range size above minRange
  \li range size below maxRange
*/
bool QCPRange::validRange(const QCPRange &range)
{
  /*
  return (range.lower > -maxRange &&
          range.upper < maxRange &&
          fabs(range.lower-range.upper) > minRange &&
          fabs(range.lower-range.upper) < maxRange &&
          (range.lower < -minRange || range.lower > minRange) &&
          (range.upper < -minRange || range.upper > minRange));
          */
  return (range.lower > -maxRange &&
          range.upper < maxRange &&
          fabs(range.lower-range.upper) > minRange &&
          fabs(range.lower-range.upper) < maxRange);
}


// ================================================================================
// =================== QCPLegend
// ================================================================================

/*! \class QCPLegend
  \brief Manages a legend inside a QCustomPlot.

  Doesn't need to be instantiated externally, rather access QCustomPlot::legend
*/

/*!
  Constructs a new QCPLegend instance with \a parentPlot as the containing plot and default
  values. Under normal usage, QCPLegend needn't be instantiated outside of QCustomPlot.
  Access QCustomPlot::legend to modify the legend (set to invisible by default, see \ref
  setVisible).
*/
QCPLegend::QCPLegend(QCustomPlot *parentPlot) : 
  QObject(parentPlot)
{
  mParentPlot = parentPlot;
  setVisible(true);
  setBorderPen(QPen(Qt::black));
  setIconBorderPen(Qt::NoPen);
  setBrush(QBrush(Qt::white));
  setFont(parentPlot->font());
  setPositionStyle(psTopRight);
  setSize(100, 28);
  setMinimumSize(100, 0);
  setAutoSize(true);
  
  setMargin(12, 12, 12, 12);
  setPadding(8, 8, 3, 3);
  setIconSize(32, 18);
  setItemSpacing(3);
  setIconTextPadding(7);
}

QCPLegend::~QCPLegend()
{
  clearItems();
}

/*!
  Sets the pen, the border of the entire legend is drawn with.
*/
void QCPLegend::setBorderPen(const QPen &pen)
{
  mBorderPen = pen;
}

/*!
  Sets the brush of the legend background.
*/
void QCPLegend::setBrush(const QBrush &brush)
{
  mBrush = brush;
}

/*!
  Sets the default font of legend text. Legend items that draw text (e.g. the name of a graph) will
  use this font by default. However, a different font can be specified on a per-item-basis by
  accessing the specific legend item.
*/
void QCPLegend::setFont(const QFont &font)
{
  mFont = font;
  for (int i=0; i<mItems.size(); ++i)
    mItems.at(i)->setFont(mFont);
}

/*!
  Sets the position style of the legend. If the \a legendPositionStyle is not \ref psManual, the
  position is found automatically depending on the specific \a legendPositionStyle and the
  legend margins. If \a legendPositionStyle is \ref psManual, the exact pixel position of the
  legend must be specified via \ref setPosition. Margins have no effect in that case.
  \see setMargin
*/
void QCPLegend::setPositionStyle(PositionStyle legendPositionStyle)
{
  mPositionStyle = legendPositionStyle;
}

/*!
  Sets the exact pixel Position of the legend inside the QCustomPlot widget, if \ref
  setPositionStyle is set to \ref psManual. Margins have no effect in that case.
*/
void QCPLegend::setPosition(const QPoint &pixelPosition)
{
  mPosition = pixelPosition;
}

/*!
  Sets whether the size of the legend should be calculated automatically to fit all the content
  (plus padding), or whether the size must be specified manually with \ref setSize.
  
  If the autoSize mechanism is enabled, the legend will have the smallest possible size to still
  display all its content. For items with text wrapping (QCPPlottableLegendItem::setTextWrap) this
  means, they would become very compressed, i.e. wrapped at every word. To prevent this, set a
  reasonable \ref setMinimumSize width.
*/
void QCPLegend::setAutoSize(bool on)
{
  mAutoSize = on;
}

/*!
  Sets the size of the legend. Setting the size manually with this function only has an effect, if
  \ref setAutoSize is set to false.
  
  If you want to control the minimum size (or the text-wrapping width) while still leaving the
  autoSize mechanism enabled, consider using \ref setMinimumSize.
  
  \see setAutoSize, setMinimumSize
*/
void QCPLegend::setSize(const QSize &size)
{
  mSize = size;
}

/*! \overload
*/
void QCPLegend::setSize(int width, int height)
{
  mSize = QSize(width, height);
}

/*!
  Sets the minimum size of the legend when \ref setAutoSize is enabled.
  
  If text wrapping is enabled in the legend items (e.g. \ref QCPPlottableLegendItem::setTextWrap), this minimum \a size defines the width
  at which the wrapping will occur. Note that the wrapping will happen only at word boundaries, so the actual size might
  still be bigger than the \a size given here, but not smaller.
  
  If \ref setAutoSize is not enabled, the minimum \a size is ignored. Setting a smaller legend size with \ref setSize manually, is not prevented.
  
  \see setAutoSize, setSize, QCPPlottableLegendItem::setTextWrap
*/
void QCPLegend::setMinimumSize(const QSize &size)
{
  mMinimumSize = size;
}

/*! \overload
*/
void QCPLegend::setMinimumSize(int width, int height)
{
  mMinimumSize = QSize(width, height);
}

/*!
  Sets the visibility of the legend.
*/
void QCPLegend::setVisible(bool on)
{
  mVisible = on;
}

/*!
  Sets the left padding of the legend. Padding is the space by what the legend box is made larger
  than minimally needed for the content to fit. I.e. it's the space left blank on each side inside
  the legend.
*/
void QCPLegend::setPaddingLeft(int padding)
{
  mPaddingLeft = padding;
}

/*!
  Sets the right padding of the legend. Padding is the space by what the legend box is made larger
  than minimally needed for the content to fit. I.e. it's the space left blank on each side inside
  the legend.
*/
void QCPLegend::setPaddingRight(int padding)
{
  mPaddingRight = padding;
}

/*!
  Sets the top padding of the legend. Padding is the space by what the legend box is made larger
  than minimally needed for the content to fit. I.e. it's the space left blank on each side inside
  the legend.
*/
void QCPLegend::setPaddingTop(int padding)
{
  mPaddingTop = padding;
}

/*!
  Sets the bottom padding of the legend. Padding is the space by what the legend box is made larger
  than minimally needed for the content to fit. I.e. it's the space left blank on each side inside
  the legend.
*/
void QCPLegend::setPaddingBottom(int padding)
{
  mPaddingBottom = padding;
}

/*!
  Sets the padding of the legend. Padding is the space by what the legend box is made larger than
  minimally needed for the content to fit. I.e. it's the space left blank on each side inside the
  legend.
*/
void QCPLegend::setPadding(int left, int right, int top, int bottom)
{
  mPaddingLeft = left;
  mPaddingRight = right;
  mPaddingTop = top;
  mPaddingBottom = bottom;
}

/*!
  Sets the left margin of the legend. Margins are the distances the legend will keep to the axis
  rect, when \ref setPositionStyle is not \ref psManual.
*/
void QCPLegend::setMarginLeft(int margin)
{
  mMarginLeft = margin;
}

/*!
  Sets the right margin of the legend. Margins are the distances the legend will keep to the axis
  rect, when \ref setPositionStyle is not \ref psManual.
*/
void QCPLegend::setMarginRight(int margin)
{
  mMarginRight = margin;
}

/*!
  Sets the top margin of the legend. Margins are the distances the legend will keep to the axis
  rect, when \ref setPositionStyle is not \ref psManual.
*/
void QCPLegend::setMarginTop(int margin)
{
  mMarginTop = margin;
}

/*!
  Sets the bottom margin of the legend. Margins are the distances the legend will keep to the axis
  rect, when \ref setPositionStyle is not \ref psManual.
*/
void QCPLegend::setMarginBottom(int margin)
{
  mMarginBottom = margin;
}

/*!
  Sets the margin of the legend. Margins are the distances the legend will keep to the axis rect,
  when \ref setPositionStyle is not \ref psManual.
*/
void QCPLegend::setMargin(int left, int right, int top, int bottom)
{
  mMarginLeft = left;
  mMarginRight = right;
  mMarginTop = top;
  mMarginBottom = bottom;
}

/*!
  Sets the vertical space between two legend items in the legend.
  
  \see setIconTextPadding, setPadding
*/
void QCPLegend::setItemSpacing(int spacing)
{
  mItemSpacing = spacing;
}

/*!
  Sets the default size of legend icons. Legend items that draw an icon (e.g. a visual
  representation of the graph) will use this size by default. However, different values can be
  specified on a per-item-basis by accessing the specific legend item.
*/
void QCPLegend::setIconSize(const QSize &size)
{
  mIconSize = size;
}

/*! \overload
*/
void QCPLegend::setIconSize(int width, int height)
{
  mIconSize.setWidth(width);
  mIconSize.setHeight(height);
}

/*!
  Sets the default horizontal space in pixels between the legend icon and the text next to it.
  Legend items that draw an icon (e.g. a visual representation of the graph) and text (e.g. the
  name of the graph) will use this space by default. However, different values can be specified on a
  per-item-basis by accessing the specific legend item.
  
  \see setItemSpacing
*/
void QCPLegend::setIconTextPadding(int padding)
{
  mIconTextPadding = padding;
}

/*!
  Sets the default pen used to draw a border around each legend icon. Legend items that draw an
  icon (e.g. a visual representation of the graph) will use this pen by default. However, different
  values can be specified on a per-item-basis by accessing the specific legend item.
  
  If no border is wanted, set this to \a Qt::NoPen.
*/
void QCPLegend::setIconBorderPen(const QPen &pen)
{
  mIconBorderPen = pen;
}

/*!
  Returns the item with index \a i.
  
  \see itemCount
*/
QCPAbstractLegendItem *QCPLegend::item(int index) const
{
  if (index >= 0 && index < mItems.size())
    return mItems[index];
  else
    return 0;
}

/*!
  Returns the QCPPlottableLegendItem which is associated with \a plottable (e.g. a \ref QCPGraph*).
  If such an item isn't in the legend, returns 0.
  
  \see hasItemWithPlottable
*/
QCPPlottableLegendItem *QCPLegend::itemWithPlottable(const QCPAbstractPlottable *plottable) const
{
  for (int i=0; i<mItems.size(); ++i)
  {
    if (QCPPlottableLegendItem *lip = dynamic_cast<QCPPlottableLegendItem*>(mItems.at(i)))
    {
      if (lip->plottable() == plottable)
        return lip;
    }
  }
  return 0;
}

/*!
  Returns the number of items currently in the legend.
  \see item
*/
int QCPLegend::itemCount() const
{
  return mItems.size();
}

/*!
  Returns whether the legend contains \a item.
*/
bool QCPLegend::hasItem(QCPAbstractLegendItem *item) const
{
  return mItems.contains(item);
}

/*!
  Returns whether the legend contains a QCPPlottableLegendItem which is associated with \a plottable (e.g. a \ref QCPGraph*).
  If such an item isn't in the legend, returns false.
  
  \see itemWithPlottable
*/
bool QCPLegend::hasItemWithPlottable(const QCPAbstractPlottable *plottable) const
{
  return itemWithPlottable(plottable);
}

/*!
  Adds \a item to the legend, if it's not present already.
  
  Returns true on sucess, i.e. if the item wasn't in the list already and has been successfully added.
  
  The legend takes ownership of the item.
*/
bool QCPLegend::addItem(QCPAbstractLegendItem *item)
{
  if (!mItems.contains(item))
  {
    mItems.append(item);
    return true;
  } else
    return false;
}

/*!
  Removes the item with index \a index from the legend.

  Returns true, if successfull.
  
  \see itemCount, clearItems
*/
bool QCPLegend::removeItem(int index)
{
  if (index >= 0 && index < mItems.size())
  {
    delete mItems.at(index);
    mItems.removeAt(index);
    return true;
  } else
    return false;
}

/*! \overload
  
  Removes \a item from the legend.

  Returns true, if successfull.
  
  \see clearItems
*/
bool QCPLegend::removeItem(QCPAbstractLegendItem *item)
{
  return removeItem(mItems.indexOf(item));
}

/*!
  Removes all items from the legend.
*/
void QCPLegend::clearItems()
{
  qDeleteAll(mItems);
  mItems.clear();
}

/*!
  If \ref setAutoSize is true, the size needed to fit all legend contents is calculated and applied.
  Finally, the automatic positioning of the legend is performed, depending on the \ref
  setPositionStyle setting.
*/
void  QCPLegend::reArrange()
{
  if (mAutoSize)
  {
    calculateAutoSize();
  }
  calculateAutoPosition();
}

/*!
  \internal
  Draws the legend with the provided \a painter.
*/
void QCPLegend::draw(QPainter *painter)
{
  if (!mVisible) return;
  painter->save();
  painter->setBrush(mBrush);
  painter->setPen(mBorderPen);
  // draw background rect:
  painter->drawRect(QRect(mPosition, mSize));
  // draw legend items:
  painter->setClipRect(QRect(mPosition, mSize).adjusted(1, 1, 0, 0));
  painter->setPen(QPen());
  painter->setBrush(Qt::NoBrush);
  int currentTop = mPosition.y()+mPaddingTop;
  for (int i=0; i<mItems.size(); ++i)
  {
    QSize itemSize = mItems.at(i)->size(QSize(mSize.width(), 0));
    painter->save(); // this might be user subclass, so we save painter outside - just in case
    mItems.at(i)->draw(painter, QRect(QPoint(mPosition.x()+mPaddingLeft, currentTop), itemSize));
    painter->restore();
    currentTop += itemSize.height()+mItemSpacing;
  }
  painter->restore();
}

/*! \internal 
  
  Goes through similar steps as \ref draw and calculates the width and height needed to
  fit all items and padding in the legend. The new calculated size is then applied to the mSize of
  this legend.
*/
void QCPLegend::calculateAutoSize()
{
  int width = mMinimumSize.width()-mPaddingLeft-mPaddingRight; // start with minimum width and only expand from there
  int currentTop;
  bool repeat = true;
  int repeatCount = 0;
  while (repeat && repeatCount < 3) // repeat until we find self-consistent width (usually 2 runs)
  {
    repeat = false;
    currentTop = mPaddingTop;
    for (int i=0; i<mItems.size(); ++i)
    {
      QSize s = mItems.at(i)->size(QSize(width, 0));
      currentTop += s.height();
      if (i < mItems.size()-1) // vertical spacer for all but last item
        currentTop += mItemSpacing;
      if (width < s.width())
      {
        width = s.width();
        repeat = true; // changed width, so need a new run with new width to let other items adapt their height to that new width
      }
    }
    repeatCount++;
  }
  if (repeat)
    qDebug() << FUNCNAME << "hit repeat limit for iterative width calculation";
  currentTop += mPaddingBottom;
  width += mPaddingLeft+mPaddingRight;
  
  mSize.setWidth(width);
  if (currentTop > mMinimumSize.height())
    mSize.setHeight(currentTop);
  else
    mSize.setHeight(mMinimumSize.height());
}

/*!
  \internal
  Sets the position dependant on the \ref setPositionStyle setting and the margins.
*/
void QCPLegend::calculateAutoPosition()
{
  if (mPositionStyle == psTopLeft)
  {
    mPosition = mParentPlot->mAxisRect.topLeft() + QPoint(mMarginLeft, mMarginTop);
  } else if (mPositionStyle == psTop)
  {
    mPosition = mParentPlot->mAxisRect.topLeft() + QPoint(mParentPlot->mAxisRect.width()/2.0-mSize.width()/2.0, mMarginTop);
  } else if (mPositionStyle == psTopRight)
  {
    mPosition = mParentPlot->mAxisRect.topRight() + QPoint(-mMarginRight-mSize.width(), mMarginTop);
  } else if (mPositionStyle == psRight)
  {
    mPosition = mParentPlot->mAxisRect.topRight() + QPoint(-mMarginRight-mSize.width(), mParentPlot->mAxisRect.height()/2.0-mSize.height()/2.0);
  } else if (mPositionStyle == psBottomRight)
  {
    mPosition = mParentPlot->mAxisRect.bottomRight() + QPoint(-mMarginRight-mSize.width(), -mMarginBottom-mSize.height());
  } else if (mPositionStyle == psBottom)
  {
    mPosition = mParentPlot->mAxisRect.bottomLeft() + QPoint(mParentPlot->mAxisRect.width()/2.0-mSize.width()/2.0, -mMarginBottom-mSize.height());
  } else if (mPositionStyle == psBottomLeft)
  {
    mPosition = mParentPlot->mAxisRect.bottomLeft() + QPoint(mMarginLeft, -mMarginBottom-mSize.height());
  } else if (mPositionStyle == psLeft)
  {
    mPosition = mParentPlot->mAxisRect.topLeft() + QPoint(mMarginLeft, mParentPlot->mAxisRect.height()/2.0-mSize.height()/2.0);
  }
}


// ================================================================================
// =================== QCPAxis
// ================================================================================

/*! \class QCPAxis
  \brief Manages a single axis inside a QCustomPlot.

  Usually doesn't need to be instantiated externally. Access %QCustomPlot's axes via
  QCustomPlot::xAxis (bottom), QCustomPlot::yAxis (left), QCustomPlot::xAxis2 (top) and
  QCustomPlot::yAxis2 (right).
*/

/*!
  Constructs an Axis instance of Type \a type inside \a parentPlot.
*/
QCPAxis::QCPAxis(QCustomPlot *parentPlot, AxisType type)
{
  mParentPlot = parentPlot;
  mTickVector = new QVector<double>;
  mSubTickVector = new QVector<double>;
  mTickVectorLabels = new QVector<QString>;
  setAxisType(type);
  setAxisRect(parentPlot->axisRect()); 
  setScaleType(stLinear);
  setScaleLogBase(10);
  
  setVisible(true);
  setRange(0, 5);
  setRangeReversed(false);
  
  setTicks(true);
  setTickStep(1);
  setAutoTickCount(6);
  setAutoTicks(true);
  setAutoTickLabels(true);
  setAutoTickStep(true);
  setTickLabelFont(parentPlot->font());
  setTickLength(5);
  setTickPen(QPen(Qt::black));
  setTickLabels(true);
  setTickLabelType(ltNumber);
  setTickLabelRotation(0);
  setDateTimeFormat("hh:mm:ss\ndd.MM.yy");
  setNumberFormat("gbd");
  setNumberPrecision(6);
  setLabel("");
  setLabelFont(parentPlot->font());
  
  setAutoSubTicks(true);
  setSubTickCount(4);
  setSubTickLength(2);
  setSubTickPen(QPen(Qt::black));
  
  QPen gPen;
  gPen.setColor(QColor(200,200,200));
  gPen.setStyle(Qt::DotLine);
  setGridPen(gPen);
  setGrid(true);
  QPen subgPen;
  subgPen.setColor(QColor(220,220,220));
  subgPen.setStyle(Qt::DotLine);
  setSubGridPen(subgPen);
  setSubGrid(false);
  QPen zlinePen;
  zlinePen.setColor(QColor(200,200,200));
  setZeroLinePen(zlinePen);
  setBasePen(QPen(Qt::black));
  
  setPadding(0);
  if (type == atTop)
  {
    setTickLabelPadding(3);
    setLabelPadding(6);
  } else if (type == atRight)
  {
    setTickLabelPadding(7);
    setLabelPadding(12);
  } else if (type == atBottom)
  {
    setTickLabelPadding(3);
    setLabelPadding(3);
  } else if (type == atLeft)
  {
    setTickLabelPadding(5);
    setLabelPadding(10);
  }
}

QCPAxis::~QCPAxis()
{
  delete mTickVector;
  delete mTickVectorLabels;
  delete mSubTickVector;
}

/*!
  Returns the number Format.
  \see setNumberFormat
*/
QString QCPAxis::numberFormat() const
{
  QString result;
  result.append(mNumberFormatChar);
  if (mNumberBeautifulPowers)
  {
    result.append("b");
    if (mNumberMultiplyCross)
      result.append("c");
  }
  return result;
}

/*!
  \internal
  Sets the axis type. Together with the current axis rect (see \ref setAxisRect), this determines
  the orientation and position of the axis. Depending on \a type, ticks, tick labels, and label are
  drawn on corresponding sides of the axis base line.
*/
void QCPAxis::setAxisType(AxisType type)
{
  mAxisType = type;
  mOrientation = (type == atBottom || type == atTop) ? Qt::Horizontal : Qt::Vertical;
}

/*!
  \internal
  Sets the axis rect. The axis uses this rect to position itself within the plot,
  together with the information of its type (\ref setAxisType). Theoretically it's possible to give
  a plot's axes different axis rects (e.g. for gaps between them), however, they are currently all
  synchronized by the QCustomPlot::setAxisRect function.
*/
void QCPAxis::setAxisRect(const QRect &rect)
{
  mAxisRect = rect;
}

/*!
  Sets whether the axis uses a linear scale or a logarithmic scale. If \a type is set to \ref
  stLogarithmic, the logarithm base can be set with \ref setScaleLogBase. In logarithmic axis
  scaling, major tick marks appear at all powers of the logarithm base. Properties like tick step
  (\ref setTickStep) don't apply in logarithmic scaling. If you wish a decimal base but less major
  ticks, consider choosing a logarithm base of 100, 1000 or even higher.
  
  If \a type is \ref stLogarithmic and the number format (\ref setNumberFormat) uses the 'b' option
  (beautifully typeset decimal powers), the display usually is "1 [multiplication sign] 10
  [superscript] n", which looks unnatural for logarithmic scaling (the "1 [multiplication sign]"
  part). To only display the decimal power, set the number precision to zero with
  \ref setNumberPrecision.
*/
void QCPAxis::setScaleType(ScaleType type)
{
  mScaleType = type;
  if (mScaleType == stLogarithmic)
    mRange = mRange.sanitizedForLogScale();
}

/*!
  If \ref setScaleType is set to \ref stLogarithmic, \a base will be the logarithm base of the
  scaling. In logarithmic axis scaling, major tick marks appear at all powers of \a base.
  
  Properties like tick step (\ref setTickStep) don't apply in logarithmic scaling. If you wish a decimal base but
  less major ticks, consider choosing \a base 100, 1000 or even higher.
*/
void QCPAxis::setScaleLogBase(double base)
{
  if (base > 1)
  {
    mScaleLogBase = base;
    mScaleLogBaseLogInv = 1.0/log(mScaleLogBase); // buffer for faster baseLog() calculation
  } else
    qDebug() << FUNCNAME << "Invalid logarithmic scale base (must be greater 1):" << base;
}

/*!
  Sets the range of the axis.
  
  This slot may be connected with the \a rangeChanged signal of another axis in order for the
  ranges to be synchronized (in one direction).
  
  To invert the direction of an axis range, use \ref setRangeReversed.
*/
void QCPAxis::setRange(const QCPRange &range)
{
  if (!QCPRange::validRange(range)) return;
  if (mScaleType == stLogarithmic)
  {
    mRange = range.sanitizedForLogScale();
  } else
  {
    mRange = range.sanitizedForLinScale();
  }
  emit rangeChanged(mRange);
}


/*!
  \overload
  Sets the lower and upper bound of the axis range.
  
  There is also a slot to set a range, see \ref setRange(const QCPRange &range).
*/
void QCPAxis::setRange(double lower, double upper)
{
  if (!QCPRange::validRange(lower, upper)) return;
  mRange.lower = lower;
  mRange.upper = upper;
  if (mScaleType == stLogarithmic)
  {
    mRange = mRange.sanitizedForLogScale();
  } else
  {
    mRange = mRange.sanitizedForLinScale();
  }
  emit rangeChanged(mRange);
}

/*!
  \overload
  Sets the range of the axis.

  \param position the \a position coordinate indicates together with the \a alignment parameter, where
  the new range will be positioned.
  \param size defines the size (upper-lower) of the new axis range.
  \param alignment determines how \a position is to be interpreted.\n
  If \a alignment is Qt::AlignLeft, \a position will be the lower bound of the range.\n
  If \a alignment is Qt::AlignRight, \a position will be the upper bound of the range.\n
  If \a alignment is Qt::AlignCenter, the new range will be centered around \a position.\n
  Any other values for \a alignment will default to Qt::AlignCenter.
*/
void QCPAxis::setRange(double position, double size, Qt::AlignmentFlag alignment)
{
  if (alignment == Qt::AlignLeft)
    setRange(position, position+size);
  else if (alignment == Qt::AlignRight)
    setRange(position-size, position);
  else // alignment == Qt::AlignCenter
    setRange(position-size/2.0, position+size/2.0);
}

/*!
  Sets the lower bound of the axis range, independently of the upper bound.
  \see setRange
*/
void QCPAxis::setRangeLower(double lower)
{
  mRange.lower = lower;
  if (mScaleType == stLogarithmic)
  {
    mRange = mRange.sanitizedForLogScale();
  } else
  {
    mRange = mRange.sanitizedForLinScale();
  }
  emit rangeChanged(mRange);
}

/*!
  Sets the upper bound of the axis range, independently of the lower bound.
  \see setRange
*/
void QCPAxis::setRangeUpper(double upper)
{
  mRange.upper = upper;
  if (mScaleType == stLogarithmic)
  {
    mRange = mRange.sanitizedForLogScale();
  } else
  {
    mRange = mRange.sanitizedForLinScale();
  }
  emit rangeChanged(mRange);
}

/*!
  Sets whether the axis range (direction) is displayed reversed. Normally, the values on horizontal
  axes increase left to right, on vertical axes bottom to top. When \a reversed is set to true, the
  direction of increasing values is inverted. Note that the range and data interface stays the same
  for reversed axes, e.g. the \a lower part of the \ref setRange interface will still reference the
  mathematically smaller number than the \a upper part.
*/
void QCPAxis::setRangeReversed(bool reversed)
{
  mRangeReversed = reversed;
}

/*!
  Sets whether the axis (including all its decoration, i.e. labels and grid lines) is visible.
  An invisible axis doesn't mean a non functional axis. Graphs attached to an invisible axis can still
  be plotted/moved/zoomed normally as if the axis was visible.
*/
void QCPAxis::setVisible(bool on)
{
  mVisible = on;
}

/*!
  Sets whether the grid lines are visible.
  \see setSubGrid, setGridPen
*/
void QCPAxis::setGrid(bool show)
{
  mGrid = show;
}

/*!
  Sets whether the sub grid lines are visible.
  \see setGrid, setSubGridPen
*/
void QCPAxis::setSubGrid(bool show)
{
  mSubGrid = show;
}

/*!
  Sets whether the tick positions should be calculated automatically (either from an automatically
  generated tick step or a tick step provided manually via \ref setTickStep, see \ref setAutoTickStep).
  
  If \a on is set to false, you must provide the tick positions manually via \ref setTickVector.
  For these manual ticks you may let QCPAxis generate the appropriate labels automatically
  by setting/leaving \ref setAutoTickLabels true. If you also wish to control the displayed labels
  manually, set \ref setAutoTickLabels to false and provide the label strings with \ref setTickVectorLabels.
*/
void QCPAxis::setAutoTicks(bool on)
{
  mAutoTicks = on;
}

/*!
  When \ref setAutoTickStep is true, \a approximateCount determines how many ticks should be generated
  in the visible range approximately.
*/
void QCPAxis::setAutoTickCount(int approximateCount)
{
  mAutoTickCount = approximateCount;
}

/*!
  Sets whether the tick labels are generated automatically depending on the tick label type
  (\ref ltNumber or \ref ltDateTime).
  
  If \a on is set to false, you should provide the tick labels via \ref setTickVectorLabels.
  (This is usually used in a combination with \ref setAutoTicks set to false for complete control over
   tick positions and labels, e.g. when the ticks should be at multiples of pi and show n*pi as tick labels.)
*/
void QCPAxis::setAutoTickLabels(bool on)
{
  mAutoTickLabels = on;
}

/*!
  Sets whether the tick step, i.e. the interval between two (major) ticks, is calculated
  automatically. If \a on is set to true, the axis finds a tick step that is reasonable for human
  readable plots. This means the tick step mantissa is chosen such that it's either a multiple of
  two or ends in 0.5. The number of ticks the algorithm aims for within the visible range can be
  set with \ref setAutoTickCount. It's not guaranteed that this number of ticks is met exactly, but
  approximately within a tolerance of two or three.
  
  If \a on is set to false, you may set the tick step manually with \ref setTickStep.
*/
void QCPAxis::setAutoTickStep(bool on)
{
  mAutoTickStep = on;
}

/*!
  Sets whether then number of sub ticks in one tick interval is determined automatically.
  This works, as long as the tick step mantissa is a multiple of 0.5 (which it is, when
  \ref setAutoTickStep is enabled).\n
  When \a on is set to false, you may set the sub tick count with \ref setSubTickCount manually.
*/
void QCPAxis::setAutoSubTicks(bool on)
{
  mAutoSubTicks = on;
}

/*!
  Sets whether tick marks are displayed. Setting \a show to false does not imply, that tick labels
  are invisible, too. To achieve that, see \ref setTickLabels.
*/
void QCPAxis::setTicks(bool show)
{
  mTicks = show;
}

/*!
  Sets whether tick labels are displayed.
*/
void QCPAxis::setTickLabels(bool show)
{
  mTickLabels = show;
}

/*!
  Sets the distance between the axis base line (or any tick marks pointing outward) and the tick labels.
  \see setLabelPadding, setPadding
*/
void QCPAxis::setTickLabelPadding(int padding)
{
  mTickLabelPadding = padding;
}

/*!
  Sets whether the tick labels display numbers or dates/times.\n
  If \a type is set to \ref ltNumber, the format specifications of \ref setNumberFormat apply.\n
  If \a type is set to \ref ltDateTime, the format specifications of \ref setDateTimeFormat apply.\n
  In QCustomPlot, date/time coordinates are double numbers representing the seconds since 1970-01-01T00:00:00 UTC.
  This format can be retrieved from QDateTime objects with the QDateTime::toTime_t() function. Since this
  only gives a resolution of one second, there is also the QDateTime::toMSecsSinceEpoch() function which
  returns the timespan described above in milliseconds. Divide its return value by 1000.0 to get a value with
  the format needed for date/time plotting, this time with a resolution of one millisecond.
*/
void QCPAxis::setTickLabelType(LabelType type)
{
  mTickLabelType = type;
}

/*!
  Sets the font of the tick labels, i.e. the numbers drawn next to tick marks.
*/
void QCPAxis::setTickLabelFont(const QFont &font)
{
  mTickLabelFont = font;
}

/*!
  Sets the rotation of the tick labels, i.e. the numbers drawn next to tick marks. If \a degrees
  is zero, the labels are drawn normally. Else, the tick labels are drawn rotated by \a degrees
  clockwise. The specified angle is bound to values from -90 to 90 degrees.
*/
void QCPAxis::setTickLabelRotation(double degrees)
{
  mTickLabelRotation = qBound(-90.0, degrees, 90.0);
}

/*!
  Sets the format in which dates and times are displayed as tick labels, if \ref setTickLabelType is \ref ltDateTime.
  for details about the \a format string, see the documentation of QDateTime::toString().
  Newlines can be inserted with "\n".
*/
void QCPAxis::setDateTimeFormat(const QString &format)
{
  mDateTimeFormat = format;
}

/*!
  Sets the number format for the numbers drawn as tick labels (if tick label type is \ref
  ltNumber). This \a formatCode is an extended version of the format code used e.g. by
  QString::number() and QLocale::toString(). For reference about that, see the "Argument Formats"
  section in the detailed description of the QString class. \a formatCode is a string of one, two
  or three characters. The first character is identical to the normal format code used by Qt. In
  short, this means: 'e'/'E' scientific format, 'f' fixed format, 'g'/'G' scientific or fixed,
  whichever is shorter.
  
  The second and third characters are optional and specific to QCustomPlot:\n
  If the first char was 'e' or 'g', numbers are/might be displayed in the scientific format, e.g.
  "5.5e9", which is ugly in a plot. So when the second char of \a formatCode is set to 'b' (for
  "beautiful"), those exponential numbers are formatted in a more natural way, i.e. "5.5
  [multiplication sign] 10 [superscript] 9". By default, the multiplication sign is a centered dot.
  If instead a cross should be shown (as is usual in the USA), the third char of \a formatCode can
  be set to 'c'. The inserted multiplication signs are the UTF-8 characters 215 (0xD7) for the
  cross and 183 (0xB7) for the dot.
  
  If the scale type (\ref setScaleType) is \ref stLogarithmic and the \a formatCode uses the 'b'
  option (beautifully typeset decimal powers), the display usually is "1 [multiplication sign] 10
  [superscript] n", which looks unnatural for logarithmic scaling (the "1 [multiplication sign]"
  part). To only display the decimal power, set the number precision to zero with \ref
  setNumberPrecision.
  
  Examples for \a formatCode:
  \li \c g normal format code behaviour. If number is small, fixed format is used, if number is large,
  normal scientific format is used
  \li \c gb If number is small, fixed format is used, if number is large, scientific format is used with
  beautifully typeset decimal powers and a dot as multiplication sign
  \li \c ebc All numbers are in scientific format with beautifully typeset decimal power and a cross as
  multiplication sign
  \li \c fb illegal format code, since fixed format doesn't support (or need) beautifully typeset decimal
  powers. Format code will be reduced to 'f'.
  \li \c hello illegal format code, since first char is not 'e', 'E', 'f', 'g' or 'G'. Current format
  code will not be changed.
*/
void QCPAxis::setNumberFormat(const QString &formatCode)
{
  if (formatCode.length() < 1) return;
  
  // interpret first char as number format char:
  QString allowedFormatChars = "eEfgG";
  if (allowedFormatChars.contains(formatCode.at(0)))
  {
    mNumberFormatChar = formatCode.at(0).toAscii();
  } else
  {
    qDebug() << FUNCNAME << "Invalid number format code (first char not in 'eEfgG'):" << formatCode;
    return;
  }
  if (formatCode.length() < 2)
  {
    mNumberBeautifulPowers = false;
    mNumberMultiplyCross = false;
    return;
  }
  
  // interpret second char as indicator for beautiful decimal powers:
  if (formatCode.at(1) == 'b' && (mNumberFormatChar == 'e' || mNumberFormatChar == 'g'))
  {
    mNumberBeautifulPowers = true;
  } else
  {
    qDebug() << FUNCNAME << "Invalid number format code (second char not 'b' or first char neither 'e' nor 'g'):" << formatCode;
    return;
  }
  if (formatCode.length() < 3)
  {
    mNumberMultiplyCross = false;
    return;
  }
  
  // interpret third char as indicator for dot or cross multiplication symbol:
  if (formatCode.at(2) == 'c')
  {
    mNumberMultiplyCross = true;
  } else if (formatCode.at(2) == 'd')
  {
    mNumberMultiplyCross = false;
  } else
  {
    qDebug() << FUNCNAME << "Invalid number format code (third char neither 'c' nor 'd'):" << formatCode;
    return;
  }
}

/*!
  Sets the precision of the numbers drawn as tick labels. See QLocale::toString(double i, char f,
  int prec) for details. The effect of precisions are most notably for number Format starting with
  'e', see \ref setNumberFormat

  If the scale type (\ref setScaleType) is \ref stLogarithmic and the number format (\ref
  setNumberFormat) uses the 'b' format code (beautifully typeset decimal powers), the display
  usually is "1 [multiplication sign] 10 [superscript] n", which looks unnatural for logarithmic
  scaling (the "1 [multiplication sign]" part). To only display the decimal power, set \a precision
  to zero.
*/
void QCPAxis::setNumberPrecision(int precision)
{
  mNumberPrecision = precision;
}

/*!
  If \ref setAutoTickStep is set to false, use this function to set the tick step manually.
  The tick step is the interval between (major) ticks, in plot coordinates.
  \see setSubTickCount
*/
void QCPAxis::setTickStep(double step)
{
  mTickStep = step;
}

/*!
  If you want full control over what ticks (and possibly labels) the axes show, this function is
  used to set the coordinates at which ticks will appear.\ref setAutoTicks must be disabled, else
  the provided tick vector will be overwritten with automatically generated tick coordinates. The
  labels of the ticks can either be generated automatically when \ref setAutoTickLabels is left
  enabled, or be set manually with \ref setTickVectorLabels, when \ref setAutoTickLabels is disabled.
  
  \param vec a pointer to the vector containing the positions of the ticks
  \param copy if this is set to true, the provided \a vec is copied to the internal tick vector. In
  this case, QCPAxis does not take ownership of \a vec. If it's set to false, the internal
  tick vector is deleted and replaced by \a vec, QCPAxis takes ownership of the vector
  memory \a vec points to. The latter might give slight performance benefits.
  \see setTickVectorLabels
*/
void QCPAxis::setTickVector(QVector<double> *vec, bool copy)
{
  if (copy)
  {
    *mTickVector = *vec;
  } else
  {
    delete mTickVector;
    mTickVector = vec;
  }
}

/*!
  If you want full control over what ticks and labels the axes show, this function
  is used to set a number of QStrings that will be displayed at the tick positions
  which you need to provide with \ref setTickVector. These two vectors should have
  the same size.
  (Note that you need to disable \ref setAutoTicks and \ref setAutoTickLabels first.)
  
  \param vec a pointer to the vector containing the labels of the ticks
  \param copy if this is set to true, the provided \a vec is copied to the internal label vector.
  In this case QCPAxis does not take ownership of \a vec. If it's set to false, the
  internal label vector is deleted and replaced by \a vec, QCPAxis takes ownership of the
  vector memory \a vec points to. The latter might give slight performance benefits.
  \see
  setTickVector
*/
void QCPAxis::setTickVectorLabels(QVector<QString> *vec, bool copy)
{
  if (copy)
  {
    *mTickVectorLabels = *vec;
  } else
  {
    delete mTickVectorLabels;
    mTickVectorLabels = vec;
  }
}

/*!
  Sets the length of the ticks in pixels. \a inside is the length the ticks will reach inside the
  plot and \a outside is the length they will reach outside the plot. If \a outside is greater than
  zero, the tick labels will increase their distance to the axis accordingly, so they won't collide
  with the ticks.
  \see setSubTickLength
*/
void QCPAxis::setTickLength(int inside, int outside)
{
  mTickLengthIn = inside;
  mTickLengthOut = outside;
}

/*!
  Sets the number of sub ticks in one (major) tick step. A sub tick count of three for example,
  divides the tick intervals in four sub intervals.
  
  By default, the number of sub ticks is chosen automatically in a reasonable manner as long as
  the mantissa of the tick step is a multiple of 0.5 (which it is, when \ref setAutoTickStep is enabled).
  If you want to disable automatic sub ticks and use this function to set the count manually, see
  \ref setAutoSubTicks.
*/
void QCPAxis::setSubTickCount(int count)
{
  mSubTickCount = count;
}

/*!
  Sets the length of the subticks in pixels. \a inside is the length the subticks will reach inside the
  plot and \a outside is the length they will reach outside the plot. If \a outside is greater than
  zero, the tick labels will increase their distance to the axis accordingly, so they won't collide
  with the ticks.
  \see setTickLength
*/
void QCPAxis::setSubTickLength(int inside, int outside)
{
  mSubTickLengthIn = inside;
  mSubTickLengthOut = outside;
}

/*!
  Sets the pen, the axis base line itself is drawn with.
*/
void QCPAxis::setBasePen(const QPen &pen)
{
  mBasePen = pen;
}

/*!
  Sets the pen, grid lines are drawn with.
  \see setSubGridPen
*/
void QCPAxis::setGridPen(const QPen &pen)
{
  mGridPen = pen;
}

/*!
  Sets the pen, the sub grid lines are drawn with.
  (By default, subgrid drawing needs to be enabled first with \ref setSubGrid.)
  \see setGridPen
*/
void QCPAxis::setSubGridPen(const QPen &pen)
{
  mSubGridPen = pen;
}

/*!
  Sets the pen with which a single grid-like line will be drawn at value position zero. The line
  will be drawn instead of a grid line at that position, and not on top. To disable the drawing of
  a zero-line, set \a pen to Qt::NoPen. Then, if \ref setGrid is enabled, a grid line will be
  drawn instead.
  \see setGrid, setGridPen
*/
void QCPAxis::setZeroLinePen(const QPen &pen)
{
  mZeroLinePen = pen;
}

/*!
  Sets the pen, tick marks will be drawn with.
  \see setTickLength
*/
void QCPAxis::setTickPen(const QPen &pen)
{
  mTickPen = pen;
}

/*!
  Sets the pen, subtick marks will be drawn with.
  \see setSubTickCount, setSubTickLength
*/
void QCPAxis::setSubTickPen(const QPen &pen)
{
  mSubTickPen = pen;
}

/*!
  Sets the font of the axis label.
*/
void QCPAxis::setLabelFont(const QFont &font)
{
  mLabelFont = font;
}

/*!
  Sets the axis label that will be shown below/above or next to the axis, depending on its orientation.
*/
void QCPAxis::setLabel(const QString &str)
{
  mLabel = str;
}

/*!
  Sets the distance between the tick labels and the axis label.
  \see setTickLabelPadding, setPadding
*/
void QCPAxis::setLabelPadding(int padding)
{
  mLabelPadding = padding;
}

/*!
  Sets the padding of the axis.

  When \ref QCustomPlot::setAutoMargin is enabled, the padding is the additional distance to the
  respective widget border, that is left blank. If \a padding is zero (default), the auto margin
  mechanism will find a margin that the axis label (or tick label, if no axis label is set) barely
  fits inside the QCustomPlot widget. To give the label closest to the border some freedom,
  increase \a padding.
  
  The axis padding has no meaning if \ref QCustomPlot::setAutoMargin is disabled.
  
  \see setLabelPadding, setTickLabelPadding
*/
void QCPAxis::setPadding(int padding)
{
  mPadding = padding;
}

/*!
  If the scale type (\ref setScaleType) is \ref stLinear, \a diff is added to the lower and upper bounds of the range. The range is
  simply moved by \a diff.
  If the scale type is \ref stLogarithmic, the range bounds are multiplied by \a diff. This corresponds to an
  apparent "linear" move in logarithmic scaling by a distance of log(diff).
*/
void QCPAxis::moveRange(double diff)
{
  if (mScaleType == stLinear)
  {
    mRange.lower += diff;
    mRange.upper += diff;
  } else // mScaleType == stLogarithmic
  {
    mRange.lower *= diff;
    mRange.upper *= diff;
  }
  emit rangeChanged(mRange);
}

/*!
  Scales the range of this axis by \a factor around the coordinate \a center. For example, if \a
  factor is 2.0, \a center is 1.0, then the axis range will double its size, and the point at
  coordinate 1.0 won't have changed it's position in the QCustomPlot widget (i.e. coordinates
  around 1.0 will have moved symmetrically closer to 1.0).
*/
void QCPAxis::scaleRange(double factor, double center)
{
  
  if (mScaleType == stLinear)
  {
    QCPRange newRange;
    newRange.lower = (mRange.lower-center)*factor + center;
    newRange.upper = (mRange.upper-center)*factor + center;
    if (QCPRange::validRange(newRange))
      mRange = newRange.sanitizedForLinScale();
  } else // mScaleType == stLogarithmic
  {
    if ((mRange.upper < 0 && center < 0) || (mRange.upper > 0 && center > 0)) // make sure center has same sign as range
    {
      QCPRange newRange;
      newRange.lower = pow(mRange.lower/center, factor)*center;
      newRange.upper = pow(mRange.upper/center, factor)*center;
      if (QCPRange::validRange(newRange))
        mRange = newRange.sanitizedForLogScale();
    } else
      qDebug() << FUNCNAME << "center of scaling operation doesn't lie in same logarithmic sign domain as range:" << center;
  }
  emit rangeChanged(mRange);
}

/*!
  Sets the range of this axis to have a certain scale \a ratio to \a otherAxis. For example, if \a
  ratio is 1, this axis is the \a yAxis and \a otherAxis is \a xAxis, graphs plotted with those axes
  will appear in a 1:1 ratio, independent of the aspect ratio the axis rect has. This is an
  operation that changes the range of this axis once, it doesn't fix the scale ratio indefinitely.
  Consequently calling this function in the constructor won't have the desired effect, since the
  widget's dimensions aren't defined yet, and a resizeEvent will follow.
*/
void QCPAxis::setScaleRatio(const QCPAxis *otherAxis, double ratio)
{
  int otherPixelSize, ownPixelSize;
  
  if (otherAxis->orientation() == Qt::Horizontal)
    otherPixelSize = otherAxis->mAxisRect.width();
  else
    otherPixelSize = otherAxis->mAxisRect.height();
  
  if (orientation() == Qt::Horizontal)
    ownPixelSize = mAxisRect.width();
  else
    ownPixelSize = mAxisRect.height();
  
  double newRangeSize = ratio*otherAxis->mRange.size()*ownPixelSize/(double)otherPixelSize;
  setRange(range().center(), newRangeSize, Qt::AlignCenter);
}

/*!
  \internal
  Transforms \a value (in pixel coordinates of the QCustomPlot widget) to axis coordinates.
*/
double QCPAxis::pixelToCoord(double value) const
{
  if (orientation() == Qt::Horizontal)
  {
    if (mScaleType == stLinear)
    {
      if (mRangeReversed)
        return -(value-mAxisRect.left())/(double)mAxisRect.width()*mRange.size()+mRange.upper;
      else
        return (value-mAxisRect.left())/(double)mAxisRect.width()*mRange.size()+mRange.lower;
    } else // mScaleType == stLogarithmic
    {
      if (mRangeReversed)
        return pow(mRange.upper/mRange.lower, (mAxisRect.left()-value)/(double)mAxisRect.width())*mRange.upper;
      else
        return pow(mRange.upper/mRange.lower, (value-mAxisRect.left())/(double)mAxisRect.width())*mRange.lower;
    }
  } else // orientation() == Qt::Vertical
  {
    if (mScaleType == stLinear)
    {
      if (mRangeReversed)
        return -(mAxisRect.bottom()-value)/(double)mAxisRect.height()*mRange.size()+mRange.upper;
      else
        return (mAxisRect.bottom()-value)/(double)mAxisRect.height()*mRange.size()+mRange.lower;
    } else // mScaleType == stLogarithmic
    {
      if (mRangeReversed)
        return pow(mRange.upper/mRange.lower, (value-mAxisRect.bottom())/(double)mAxisRect.height())*mRange.upper;
      else
        return pow(mRange.upper/mRange.lower, (mAxisRect.bottom()-value)/(double)mAxisRect.height())*mRange.lower;
    }
  }
}

/*!
  \internal
  Transforms \a value (in coordinates of the axis) to pixel coordinates of the QCustomPlot widget.
*/
double QCPAxis::coordToPixel(double value) const
{
  if (orientation() == Qt::Horizontal)
  {
    if (mScaleType == stLinear)
    {
      if (mRangeReversed)
        return (mRange.upper-value)/mRange.size()*mAxisRect.width()+mAxisRect.left();
      else
        return (value-mRange.lower)/mRange.size()*mAxisRect.width()+mAxisRect.left();
    } else // mScaleType == stLogarithmic
    {
      if (value >= 0 && mRange.upper < 0) // invalid value for logarithmic scale, just draw it outside visible range
        return mRangeReversed ? mAxisRect.left()-200 : mAxisRect.right()+200;
      else if (value <= 0 && mRange.upper > 0) // invalid value for logarithmic scale, just draw it outside visible range
        return mRangeReversed ? mAxisRect.right()+200 : mAxisRect.left()-200;
      else
      {
        if (mRangeReversed)
          return baseLog(mRange.upper/value)/baseLog(mRange.upper/mRange.lower)*mAxisRect.width()+mAxisRect.left();
        else
          return baseLog(value/mRange.lower)/baseLog(mRange.upper/mRange.lower)*mAxisRect.width()+mAxisRect.left();
      }
    }
  } else // orientation() == Qt::Vertical
  {
    if (mScaleType == stLinear)
    {
      if (mRangeReversed)
        return mAxisRect.bottom()-(mRange.upper-value)/mRange.size()*mAxisRect.height();
      else
        return mAxisRect.bottom()-(value-mRange.lower)/mRange.size()*mAxisRect.height();
    } else // mScaleType == stLogarithmic
    {     
      if (value >= 0 && mRange.upper < 0) // invalid value for logarithmic scale, just draw it outside visible range
        return mRangeReversed ? mAxisRect.bottom()+200 : mAxisRect.top()-200;
      else if (value <= 0 && mRange.upper > 0) // invalid value for logarithmic scale, just draw it outside visible range
        return mRangeReversed ? mAxisRect.top()-200 : mAxisRect.bottom()+200;
      else
      {
        if (mRangeReversed)
          return mAxisRect.bottom()-baseLog(mRange.upper/value)/baseLog(mRange.upper/mRange.lower)*mAxisRect.height();
        else
          return mAxisRect.bottom()-baseLog(value/mRange.lower)/baseLog(mRange.upper/mRange.lower)*mAxisRect.height();
      }
    }
  }
}

/*!
  \internal
  This function is called before the grid and axis is drawn, in order to prepare the tick
  vector, sub tick vector and tick label vector. If \ref setAutoTicks is set to true, appropriate
  tick values are determined automatically via \ref generateAutoTicks. If it's set to false, the
  signal ticksRequest is emitted, which can be used to provide external tick positions. Then the
  sub tick vectors and tick label vectors are created.
*/
void QCPAxis::generateTickVectors()
{
  if ((!mTicks && !mTickLabels && !mGrid) || mRange.size() <= 0) return;
  
  // fill tick vectors, either by auto generating or by notifying user to fill the vectors himself
  if (mAutoTicks)
  {
    generateAutoTicks();
  } else
  {
    emit ticksRequest();
  }
  
  if (mTickVector->isEmpty())
  {
    mSubTickVector->clear();
    return;
  }
  
  // generate subticks between ticks:
  mSubTickVector->resize((mTickVector->size()-1)*mSubTickCount);
  if (mSubTickCount > 0)
  {
    double subTickStep = 0;
    double subTickPosition = 0;
    int subTickIndex = 0;
    bool done = false;
    for (int i=1; i<mTickVector->size(); ++i)
    {
      subTickStep = (mTickVector->at(i)-mTickVector->at(i-1))/(double)(mSubTickCount+1);
      for (int k=1; k<=mSubTickCount; ++k)
      {
        subTickPosition = mTickVector->at(i-1) + k*subTickStep;
        if (subTickPosition < mRange.lower)
          continue;
        if (subTickPosition > mRange.upper)
        {
          done = true;
          break;
        }
        (*mSubTickVector)[subTickIndex] = subTickPosition;
        subTickIndex++;
      }
      if (done) break;
    }
    mSubTickVector->resize(subTickIndex);
  }

  // generate tick labels according to tick positions:
  mExponentialChar = mParentPlot->locale().exponential();   // will be needed when drawing the numbers generated here, in drawTickLabel()
  mPositiveSignChar = mParentPlot->locale().positiveSign(); // will be needed when drawing the numbers generated here, in drawTickLabel()
  if (mAutoTickLabels)
  {
    int vecsize = mTickVector->size();
    mTickVectorLabels->resize(vecsize);
    if (mTickLabelType == ltNumber)
    {
      for (int i=0; i<vecsize; ++i)
        (*mTickVectorLabels)[i] = mParentPlot->locale().toString(mTickVector->at(i), mNumberFormatChar, mNumberPrecision);
    } else if (mTickLabelType == ltDateTime)
    {
      for (int i=0; i<vecsize; ++i)
        (*mTickVectorLabels)[i] = mParentPlot->locale().toString(QDateTime::fromTime_t(mTickVector->at(i)), mDateTimeFormat);
    }
  } else // mAutoTickLabels == false
  {
    if (mAutoTicks) // ticks generated automatically, but not ticklabels, so emit ticksRequest here for labels
    {
      emit ticksRequest();
    }
    // make sure provided tick label vector has correct (minimal) length:
    if (mTickVectorLabels->size() < mTickVector->size())
      mTickVectorLabels->resize(mTickVector->size());
  }
}

/*!
  \internal
  If \ref setAutoTicks is set to true, this function is called by \ref generateTickVectors to
  generate reasonable tick positions (and subtick count). The algorithm tries to create approximately
  <tt>mAutoTickCount</tt> ticks (set via \ref setAutoTickCount), taking into account, that tick
  mantissas that are divisable by two or end in .5 are nice to look at and practical in linear
  scales. If the scale is logarithmic, one tick is generated at every power of the current
  logarithm base, set via \ref setScaleLogBase.
*/
void QCPAxis::generateAutoTicks()
{
  if (mScaleType == stLinear)
  {
    if (mAutoTickStep)
    {
      // Generate tick positions according to linear scaling:
      mTickStep = mRange.size()/(double)mAutoTickCount; // mAutoTickCount ticks on average
      double magnitudeFactor = pow( 10.0, floor(log10(mTickStep))); // get magnitude factor e.g. 0.01, 1, 10, 1000 etc.
      double tickStepMantissa = mTickStep/magnitudeFactor;
      if (tickStepMantissa < 5)
      {
        // round digit after decimal point to 0.5
        mTickStep = (int)(tickStepMantissa*2)/2.0*magnitudeFactor;
      } else
      {
        // round to first digit in multiples of 2
        mTickStep = (int)((tickStepMantissa/10.0)*5)/5.0*10*magnitudeFactor;
      }
    }
    if (mAutoSubTicks)
      mSubTickCount = calculateAutoSubTickCount(mTickStep);
    // Generate tick positions according to mTickStep:
    int firstStep = floor(mRange.lower/mTickStep);
    int lastStep = ceil(mRange.upper/mTickStep);
    int tickcount = lastStep-firstStep+1;
    if (tickcount < 0) tickcount = 0;
    mTickVector->resize(tickcount);
    for (int i=0; i<tickcount; ++i)
    {
      (*mTickVector)[i] = (firstStep+i)*mTickStep;
    }
  } else // mScaleType == stLogarithmic
  {
    // Generate tick positions according to logbase scaling:
    if (mRange.lower > 0 && mRange.upper > 0) // positive range
    {
      double lowerMag = basePow((int)floor(baseLog(mRange.lower)));
      double currentMag = lowerMag;
      mTickVector->clear();
      mTickVector->append(currentMag);
      while (currentMag < mRange.upper && currentMag > 0) // currentMag might be zero for ranges ~1e-300, just cancel in that case
      {
        currentMag *= mScaleLogBase;
        mTickVector->append(currentMag);
      }
    } else if (mRange.lower < 0 && mRange.upper < 0) // negative range
    {
      double lowerMag = -basePow((int)ceil(baseLog(-mRange.lower)));
      double currentMag = lowerMag;
      mTickVector->clear();
      mTickVector->append(currentMag);
      while (currentMag < mRange.upper && currentMag < 0) // currentMag might be zero for ranges ~1e-300, just cancel in that case
      {
        currentMag /= mScaleLogBase;
        mTickVector->append(currentMag);
      }
    } else // invalid range for logarithmic scale, because lower and upper have different sign
    {
      mTickVector->clear();
      qDebug() << FUNCNAME << "Invalid range for logarithmic plot: " << mRange.lower << "-" << mRange.upper;
    }
  }
}

/*!
  \internal
  Called by generateAutoTicks when \ref setAutoSubTicks is set to true. Depending on the
  \a tickStep between two major ticks on the axis, a different number of sub ticks is appropriate.
  For Example taking 4 sub ticks for a \a tickStep of 1 makes more sense than taking 5 sub ticks,
  because this corresponds to a sub tick step of 0.2, instead of the less intuitive 0.16666. Note
  that a subtick count of 4 means dividing the major tick step into 5 sections.
  
  This is implemented by a hand made lookup for integer tick steps as well as fractional tick steps with
  a fractional part of (approximately) 0.5.
  If a tick step is different (i.e. has no fractional part close to 0.5), the currently set
  sub tick count (\ref setSubTickCount) is returned.
*/
int QCPAxis::calculateAutoSubTickCount(double tickStep) const
{
  int result = mSubTickCount; // default to current setting, if no proper value can be found
  
  // get mantissa of tickstep:
  double ten = 10.0;
  double magnitudeFactor = pow(ten, floor(log10(tickStep)) ); // get magnitude factor e.g. 0.01, 1, 10, 1000 etc.
  double tickStepMantissa = tickStep/magnitudeFactor;
  
  // separate integer and fractional part of mantissa:
  double epsilon = 0.01;
  double intPartf;
  int intPart;
  double fracPart = modf(tickStepMantissa, &intPartf);
  intPart = intPartf;
  
  // handle cases with (almost) integer mantissa:
  if (fracPart < epsilon || 1.0-fracPart < epsilon)
  {
    if (1.0-fracPart < epsilon)
      intPart++;
    switch (intPart)
    {
      case 1: result = 4; break; // 1.0 -> 0.2 substep
      case 2: result = 3; break; // 2.0 -> 0.5 substep
      case 3: result = 2; break; // 3.0 -> 1.0 substep
      case 4: result = 3; break; // 4.0 -> 1.0 substep
      case 5: result = 4; break; // 5.0 -> 1.0 substep
      case 6: result = 2; break; // 6.0 -> 2.0 substep
      case 7: result = 6; break; // 7.0 -> 1.0 substep
      case 8: result = 3; break; // 8.0 -> 2.0 substep
      case 9: result = 2; break; // 9.0 -> 3.0 substep
    }
  } else
  {
    // handle cases with significantly fractional mantissa:
    if (qAbs(fracPart-0.5) < epsilon) // *.5 mantissa
    {
      switch (intPart)
      {
        case 1: result = 2; break; // 1.5 -> 0.5 substep
        case 2: result = 4; break; // 2.5 -> 0.5 substep
        case 3: result = 4; break; // 3.5 -> 0.7 substep
        case 4: result = 2; break; // 4.5 -> 1.5 substep
        case 5: result = 4; break; // 5.5 -> 1.1 substep (won't occur with autoTickStep from here on)
        case 6: result = 4; break; // 6.5 -> 1.3 substep
        case 7: result = 2; break; // 7.5 -> 2.5 substep
        case 8: result = 4; break; // 8.5 -> 1.7 substep
        case 9: result = 4; break; // 9.5 -> 1.9 substep
      }
    }
    // if mantissa fraction isnt 0.0 or 0.5, don't bother finding good sub tick marks, leave default
  }
  
  return result;
}

/*!
  \internal
  Draws grid lines belonging to ticks of this axis, spanning over the complete
  axis rect. Also draws the zeroline, if \ref setZeroLinePen wasnt passed a QPen(Qt::NoPen).
  Called by QCustomPlot::draw for each axis.
*/
void QCPAxis::drawGrid(QPainter *painter)
{
  if (!mVisible || (!mGrid && mZeroLinePen.style() == Qt::NoPen)) return;
  painter->save();
  int lowTick, highTick;
  visibleTickBounds(lowTick, highTick);
  int t; // helper variable, result of coordinate-to-pixel transforms
  if (orientation() == Qt::Horizontal)
  {
    // draw zeroline:
    int zeroLineIndex = -1;
    if (mZeroLinePen.style() != Qt::NoPen && mRange.lower < 0 && mRange.upper > 0)
    {
      painter->setRenderHint(QPainter::Antialiasing, mParentPlot->antialiasedElements().testFlag(QCustomPlot::aeZeroLine));
      painter->setPen(mZeroLinePen);
      double epsilon = mRange.size()*1E-6; // for comparing double to zero
      for (int i=lowTick; i <= highTick; ++i)
      {
        if (fabs(mTickVector->at(i)) < epsilon)
        {
          zeroLineIndex = i;
          t = coordToPixel(mTickVector->at(i)); // x
          painter->drawLine(t, mAxisRect.bottom(), t, mAxisRect.top());
          break;
        }
      }
    }
    // draw grid lines:
    if (mGrid)
    {
      painter->setRenderHint(QPainter::Antialiasing, mParentPlot->antialiasedElements().testFlag(QCustomPlot::aeGrid));
      painter->setPen(mGridPen);
      for (int i=lowTick; i <= highTick; ++i)
      {
        if (i == zeroLineIndex) continue; // don't draw a gridline on top of the zeroline
        t = coordToPixel(mTickVector->at(i)); // x
        painter->drawLine(t, mAxisRect.bottom(), t, mAxisRect.top());
      }
    }
  } else
  {
    // draw zeroline:
    int zeroLineIndex = -1;
    if (mZeroLinePen.style() != Qt::NoPen && mRange.lower < 0 && mRange.upper > 0)
    {
      painter->setRenderHint(QPainter::Antialiasing, mParentPlot->antialiasedElements().testFlag(QCustomPlot::aeZeroLine));
      painter->setPen(mZeroLinePen);
      double epsilon = mRange.size()*1E-6; // for comparing double to zero
      for (int i=lowTick; i <= highTick; ++i)
      {
        if (fabs(mTickVector->at(i)) < epsilon)
        {
          zeroLineIndex = i;
          t = coordToPixel(mTickVector->at(i)); // y
          painter->drawLine(mAxisRect.left(), t, mAxisRect.right(), t);
          break;
        }
      }
    }
    // draw grid lines:
    if (mGrid)
    {
      painter->setRenderHint(QPainter::Antialiasing, mParentPlot->antialiasedElements().testFlag(QCustomPlot::aeGrid));
      painter->setPen(mGridPen);
      for (int i=lowTick; i <= highTick; ++i)
      {
        if (i == zeroLineIndex) continue; // don't draw a gridline on top of the zeroline
        t = coordToPixel(mTickVector->at(i)); // y
        painter->drawLine(mAxisRect.left(), t, mAxisRect.right(), t);
      }
    }
  }
  painter->restore();
}

/*!
  \internal
  The function to draw subgrid lines belonging to subticks of this axis, spanning over the complete
  axis rect. Called by QCustomPlot::draw for each axis.
*/
void QCPAxis::drawSubGrid(QPainter *painter)
{
  if (!mVisible || !mSubGrid || !mGrid) return;
  painter->save();
  painter->setRenderHint(QPainter::Antialiasing, mParentPlot->antialiasedElements().testFlag(QCustomPlot::aeSubGrid));
  
  int t; // helper variable, result of coordinate-to-pixel transforms
  painter->setPen(mSubGridPen);
  if (orientation() == Qt::Horizontal)
  {
    for (int i=0; i<mSubTickVector->size(); ++i)
    {
      t = coordToPixel(mSubTickVector->at(i)); // x
      painter->drawLine(t, mAxisRect.bottom(), t, mAxisRect.top());
    }
  } else
  {
    for (int i=0; i<mSubTickVector->size(); ++i)
    {
      t = coordToPixel(mSubTickVector->at(i)); // y
      painter->drawLine(mAxisRect.left(), t, mAxisRect.right(), t);
    }
  }
  painter->restore();
}

/*!
  \internal
  The main draw function of an axis, called by QCustomPlot::draw for each axis. Draws
  axis baseline, major ticks, subticks, tick labels and axis label.
*/
void QCPAxis::drawAxis(QPainter *painter)
{
  if (!mVisible) return;
  painter->save();
  painter->setRenderHint(QPainter::Antialiasing, mParentPlot->antialiasedElements().testFlag(QCustomPlot::aeAxes));
  QPoint origin;
  if (mAxisType == atLeft)
    origin = mAxisRect.bottomLeft();
  else if (mAxisType == atRight)
    origin = mAxisRect.bottomRight();
  else if (mAxisType == atTop)
    origin = mAxisRect.topLeft();
  else if (mAxisType == atBottom)
    origin = mAxisRect.bottomLeft();
  
  int xCor = 0, yCor = 0; // paint system correction, for pixel exact matches (affects baselines and ticks of top/right axes)
  if (mAxisType == atTop)
    yCor = -1;
  else if (mAxisType == atRight)
    xCor = 1;
  
  int margin = 0;
  int lowTick, highTick;
  visibleTickBounds(lowTick, highTick);
  int t; // helper variable, result of coordinate-to-pixel transforms

  /* draw axes */
  // baselines:
  painter->setPen(mBasePen);
  if (orientation() == Qt::Horizontal)
    painter->drawLine(origin+QPoint(xCor, yCor), origin+QPoint(mAxisRect.width()+xCor, yCor));
  else
    painter->drawLine(origin+QPoint(xCor, yCor), origin+QPoint(xCor, -mAxisRect.height()+yCor));
  
  // ticks:
  if (mTicks)
  {
    painter->setPen(mTickPen);
    // direction of ticks ("inward" is right for left axis and left for right axis)
    int tickDir = (mAxisType == atBottom || mAxisType == atRight) ? -1 : 1;
    if (orientation() == Qt::Horizontal)
    {
      for (int i=lowTick; i <= highTick; ++i)
      {
        t = coordToPixel(mTickVector->at(i)); // x
        painter->drawLine(t+xCor, origin.y()-mTickLengthOut*tickDir+yCor, t+xCor, origin.y()+mTickLengthIn*tickDir+yCor);
      }
    } else
    {
      for (int i=lowTick; i <= highTick; ++i)
      {
        t = coordToPixel(mTickVector->at(i)); // y
        painter->drawLine(origin.x()-mTickLengthOut*tickDir+xCor, t+yCor, origin.x()+mTickLengthIn*tickDir+xCor, t+yCor);
      }
    }
  }
  
  // subticks:
  if (mTicks && mSubTickCount > 0)
  {
    painter->setPen(mSubTickPen);
    // direction of ticks ("inward" is right for left axis and left for right axis)
    int tickDir = (mAxisType == atBottom || mAxisType == atRight) ? -1 : 1;
    if (orientation() == Qt::Horizontal)
    {
      for (int i=0; i<mSubTickVector->size(); ++i) // no need to check bounds because subticks are always only created inside current mRange
      {
        t = coordToPixel(mSubTickVector->at(i));
        painter->drawLine(t+xCor, origin.y()-mSubTickLengthOut*tickDir+yCor, t+xCor, origin.y()+mSubTickLengthIn*tickDir+yCor);
      }
    } else
    {
      for (int i=0; i<mSubTickVector->size(); ++i)
      {
        t = coordToPixel(mSubTickVector->at(i));
        painter->drawLine(origin.x()-mSubTickLengthOut*tickDir+xCor, t+yCor, origin.x()+mSubTickLengthIn*tickDir+xCor, t+yCor);
      }
    }
  }
  margin += qMax(0, qMax(mTickLengthOut, mSubTickLengthOut));
  
  // tick labels:
  QSize tickLabelsSize; // size of largest tick label, for offset calculation of axis label
  if (mTickLabels)
  {
    margin += mTickLabelPadding;
    painter->setFont(mTickLabelFont);
    for (int i=lowTick; i <= highTick; ++i)
    {
      t = coordToPixel(mTickVector->at(i));
      drawTickLabel(painter, t, margin, mTickVectorLabels->at(i), &tickLabelsSize);
    }
  }
  if (orientation() == Qt::Horizontal)
    margin += tickLabelsSize.height();
  else
    margin += tickLabelsSize.width();

  // axis label:
  if (!mLabel.isEmpty())
  {
    margin += mLabelPadding;
    painter->setFont(mLabelFont);
    QRect bounds;
    bounds = painter->fontMetrics().boundingRect(0, 0, 0, 0, Qt::TextDontClip, mLabel);
    if (mAxisType == atLeft)
    {
      QTransform oldTransform = painter->transform();
      painter->translate((origin.x()-margin-bounds.height()), origin.y());
      painter->rotate(-90);
      painter->drawText(0, 0, mAxisRect.height(), bounds.height(), Qt::TextDontClip | Qt::AlignCenter, mLabel);
      painter->setTransform(oldTransform);
    }
    else if (mAxisType == atRight)
    {
      QTransform oldTransform = painter->transform();
      painter->translate((origin.x()+margin+bounds.height()), origin.y()-mAxisRect.height());
      painter->rotate(90);
      painter->drawText(0, 0, mAxisRect.height(), bounds.height(), Qt::TextDontClip | Qt::AlignCenter, mLabel);
      painter->setTransform(oldTransform);
    }
    else if (mAxisType == atTop)
      painter->drawText(origin.x(), origin.y()-margin-bounds.height(), mAxisRect.width(), bounds.height(), Qt::TextDontClip | Qt::AlignCenter, mLabel);
    else if (mAxisType == atBottom)
      painter->drawText(origin.x(), origin.y()+margin, mAxisRect.width(), bounds.height(), Qt::TextDontClip | Qt::AlignCenter, mLabel);
  }

  painter->restore();
}

/*!
  \internal
  Draws a single tick label with the provided \a painter. The tick label is always bound
  to an axis in one direction (distance to axis in that direction is however controllable via \a
  distanceToAxis in pixels). The position in the other direction is passed in the \a position
  parameter. Hence for the bottom axis, \a position would indicate the horizontal pixel position
  (not coordinate!), at which the label should be drawn.
  
  In order to draw the axis label after all the tick labels in a position, that doesn't overlap with
  the tick labels, we need to know the largest tick label size. This is done by passing
  a \a tickLabelsSize to all \ref drawTickLabel calls during the process of drawing all tick labels of one axis.
  \a tickLabelSize is only expanded, if the drawn label exceeds the value \a tickLabelsSize
  currently holds.
  
  This function is also responsible for turning ugly exponential numbers "5.5e9" into a more beautifully
  typeset format "5.5 [multiplication sign] 10 [superscript] 9".
  This feature is controlled with \ref setNumberFormat.
*/
void QCPAxis::drawTickLabel(QPainter *painter, double position, int distanceToAxis, const QString &text, QSize *tickLabelsSize)
{
  // warning: if you change anything here, also adapt getMaxTickLabelSize() accordingly!
  
  // determine whether beautiful decimal powers should be used
  bool useBeautifulPowers = false;
  int ePos = -1;
  if (mAutoTickLabels && mNumberBeautifulPowers && mTickLabelType == ltNumber)
  {
    ePos = text.indexOf('e');
    if (ePos > -1)
      useBeautifulPowers = true;
  }
  
  // calculate text bounding rects and do string preparation for beautiful decimal powers:
  QRect bounds, baseBounds, expBounds;
  QString basePart, expPart;
  QFont expFont;
  if (useBeautifulPowers)
  {
    // split string parts for part of number/symbol that will be drawn normally and part that will be drawn as exponent:
    basePart = text.left(ePos);
    // in log scaling, we want to turn "1*10^n" into "10^n", else add multiplication sign and decimal base:
    if (mScaleType == stLogarithmic && basePart == "1")
      basePart = "10";
    else
      basePart += (mNumberMultiplyCross ? QString(QChar(215)) : QString(QChar(183))) + "10";
    expPart = text.mid(ePos+1);
    // clip "+" and leading zeros off expPart:
    while (expPart.at(1) == '0' && expPart.length() > 2) // length > 2 so we leave one zero when numberFormatChar is 'e'
      expPart.remove(1, 1);
    if (expPart.at(0) == mPositiveSignChar)
      expPart.remove(0, 1);
    // prepare smaller font for exponent:
    expFont = painter->font();
    expFont.setPointSize(expFont.pointSize()*0.75);
    // calculate bounding rects of base part, exponent part and total one:
    baseBounds = painter->fontMetrics().boundingRect(0, 0, 0, 0, Qt::TextDontClip, basePart);
    QFontMetrics expFontMetrics(expFont);
    expBounds = expFontMetrics.boundingRect(0, 0, 0, 0, Qt::TextDontClip, expPart);
    bounds = baseBounds.adjusted(0, 0, expBounds.width(), 0);
  } else // useBeautifulPowers == false
  {
    bounds = painter->fontMetrics().boundingRect(0, 0, 0, 0, Qt::TextDontClip | Qt::AlignHCenter, text);
  }
  
  // if using rotated tick labels, transform bounding rect, too:
  QRect rotatedBounds = bounds;
  if (!qFuzzyCompare(mTickLabelRotation+1.0, 1.0))
  {
    QTransform transform;
    transform.rotate(mTickLabelRotation);
    rotatedBounds = transform.mapRect(bounds);
  }
  // expand passed tickLabelsSize if current tick label is larger:
  if (rotatedBounds.width() > tickLabelsSize->width()) 
    tickLabelsSize->setWidth(rotatedBounds.width());
  if (rotatedBounds.height() > tickLabelsSize->height())
    tickLabelsSize->setHeight(rotatedBounds.height());
  
  // calculate coordinates (non-trivial, for best visual appearance):
  // short explanation for bottom axis: The anchor, i.e. the point in the label that is placed horizontally under the
  //   corresponding tick is always on the label side that is closer to the axis (e.g. the left side of the text when we're
  //   rotating clockwise). On that side, the height-edge is halved and the resulting median point is defined the anchor. This way,
  //   a 90 degree rotated text will be centered under the tick (i.e. displaced horizontally by half its height). At the
  //   same time, a 45 degree rotated text will "point toward" its tick, as is typical for rotated tick labels.
  bool doRotation = !qFuzzyCompare(mTickLabelRotation+1.0, 1.0);
  double angle = mTickLabelRotation/180.0*M_PI;
  int x=0,y=0;
  if (mAxisType == atLeft)
  {
    if (doRotation)
    {
      if (mTickLabelRotation > 0)
      {
        x = mAxisRect.left()-cos(angle)*bounds.width()-distanceToAxis;
        y = position-sin(angle)*bounds.width()-cos(angle)*bounds.height()/2.0;
      } else
      {
        x = mAxisRect.left()-cos(-angle)*bounds.width()-sin(-angle)*bounds.height()-distanceToAxis;
        y = position+sin(-angle)*bounds.width()-cos(-angle)*bounds.height()/2.0;
      }
    } else
    {
      x = mAxisRect.left()-bounds.width()-distanceToAxis;
      y = position-bounds.height()/2.0;
    }
  } else if (mAxisType == atRight)
  {
    if (doRotation)
    {
      if (mTickLabelRotation > 0)
      {
        x = mAxisRect.right()+sin(angle)*bounds.height()+distanceToAxis;
        y = position-cos(angle)*bounds.height()/2.0;
      } else
      {
        x = mAxisRect.right()+distanceToAxis;
        y = position-cos(-angle)*bounds.height()/2.0;
      }
    } else
    {
      x = mAxisRect.right()+distanceToAxis;
      y = position-bounds.height()/2.0;
    }
  } else if (mAxisType == atTop)
  {
    if (doRotation)
    {
      if (mTickLabelRotation > 0)
      {
        x = position-cos(angle)*bounds.width()+sin(angle)*bounds.height()/2.0;
        y = mAxisRect.top()-sin(angle)*bounds.width()-cos(angle)*bounds.height()-distanceToAxis;
      } else
      {
        x = position-sin(-angle)*bounds.height()/2.0;
        y = mAxisRect.top()-cos(-angle)*bounds.height()-distanceToAxis;
      }
    } else
    {
      x = position-bounds.width()/2.0;
      y = mAxisRect.top()-bounds.height()-distanceToAxis;
    }
  } else if (mAxisType == atBottom)
  {
    if (doRotation)
    {
      if (mTickLabelRotation > 0)
      {
        x = position+sin(angle)*bounds.height()/2.0;
        y = mAxisRect.bottom()+distanceToAxis;
      } else
      {
        x = position-cos(-angle)*bounds.width()-sin(-angle)*bounds.height()/2.0;
        y = mAxisRect.bottom()+sin(-angle)*bounds.width()+distanceToAxis;
      }
    } else
    {
      x = position-bounds.width()/2.0;
      y = mAxisRect.bottom()+distanceToAxis;
    }
  }
  
  // if label would be partly clipped by widget border on sides, don't draw it:
  if (orientation() == Qt::Horizontal)
  {
    if (x+bounds.width() > mParentPlot->mViewport.right() ||
        x < mParentPlot->mViewport.left())
      return;
  } else
  {
    if (y+bounds.height() > mParentPlot->mViewport.bottom() ||
        y < mParentPlot->mViewport.top())
      return;
  }
  
  // transform painter to position/rotation:
  QTransform oldTransform = painter->transform();
  painter->translate(x, y);
  if (doRotation)
    painter->rotate(mTickLabelRotation);
  // draw text:
  if (useBeautifulPowers)
  {
    // draw base:
    painter->drawText(0, 0, 0, 0, Qt::TextDontClip, basePart);
    // draw exponent:
    painter->setFont(expFont);
    painter->drawText(baseBounds.width()+1, 0, expBounds.width(), expBounds.height(), Qt::TextDontClip,  expPart);
    painter->setFont(mTickLabelFont);
  } else // useBeautifulPowers == false
  {
    painter->drawText(0, 0, bounds.width(), bounds.height(), Qt::TextDontClip | Qt::AlignHCenter, text);
  }
  
  // reset rotation/translation transform to what it was before:
  painter->setTransform(oldTransform);
}

/*!
  \internal
  Simulates the steps done by \ref drawTickLabel by calculating bounding boxes of the text
  label to be drawn, depending on number format etc.
  Since we only want the largest tick label for the margin calculation, the passed \a tickLabelsSize
  isn't overridden with the calculated label size, but it's only expanded, if it's
  currently set to a smaller width/height.
*/
void QCPAxis::getMaxTickLabelSize(const QFont &font, const QString &text,  QSize *tickLabelsSize) const
{
  // This function does the same as drawTickLabel but omits the actual drawing
  // changes involve creating extra QFontMetrics instances for font, since painter->fontMetrics() isn't available
  
  // determine whether beautiful powers should be used
  bool useBeautifulPowers = false;
  int ePos=-1;
  if (mAutoTickLabels && mNumberBeautifulPowers && mTickLabelType == ltNumber)
  {
    ePos = text.indexOf(mExponentialChar);
    if (ePos > -1)
      useBeautifulPowers = true;
  }
  
  // calculate and draw text, depending on whether beautiful powers are applicable or not:
  QRect bounds, baseBounds, expBounds;
  QString basePart, expPart;
  QFont expFont;
  if (useBeautifulPowers)
  {
    // split string parts for part of number/symbol that will be drawn normally and part that will be drawn as exponent:
    basePart = text.left(ePos);
    // in log scaling, we want to turn "1*10^n" into "10^n", else add multiplication sign and decimal base:
    if (mScaleType == stLogarithmic && basePart == "1")
      basePart = "10";
    else
      basePart += (mNumberMultiplyCross ? QString(QChar(215)) : QString(QChar(183))) + "10";
    expPart = text.mid(ePos+1);
    // clip "+" and leading zeros off expPart:
    while (expPart.at(1) == '0' && expPart.length() > 2) // length > 2 so we leave one zero when numberFormatChar is 'e'
      expPart.remove(1, 1);
    if (expPart.at(0) == mPositiveSignChar)
      expPart.remove(0, 1);
    // prepare smaller font for exponent:
    expFont = font;
    expFont.setPointSize(expFont.pointSize()*0.75);
    // calculate bounding rects of base part, exponent part and total one:
    QFontMetrics baseFontMetrics(font);
    baseBounds = baseFontMetrics.boundingRect(0, 0, 0, 0, Qt::TextDontClip, basePart);
    QFontMetrics expFontMetrics(expFont);
    expBounds = expFontMetrics.boundingRect(0, 0, 0, 0, Qt::TextDontClip, expPart);
    bounds = baseBounds.adjusted(0, 0, expBounds.width(), 0); 
  } else // useBeautifulPowers == false
  {
    QFontMetrics fontMetrics(font);
    bounds = fontMetrics.boundingRect(0, 0, 0, 0, Qt::TextDontClip | Qt::AlignHCenter, text);
  }
  
  // if rotated tick labels, transform bounding rect, too:
  QRect rotatedBounds = bounds;
  if (!qFuzzyCompare(mTickLabelRotation+1.0, 1.0))
  {
    QTransform transform;
    transform.rotate(mTickLabelRotation);
    rotatedBounds = transform.mapRect(bounds);
  }
  // expand passed tickLabelsSize if current tick label is larger:
  if (rotatedBounds.width() > tickLabelsSize->width()) 
    tickLabelsSize->setWidth(rotatedBounds.width());
  if (rotatedBounds.height() > tickLabelsSize->height())
    tickLabelsSize->setHeight(rotatedBounds.height());
}

/*!
  \internal
  Returns via \a lowIndex and \a highIndex, which ticks in the current tick vector
  are visible in the current range. The return values are indices of the
  tick vector, not the positions of the ticks themselves.
  The actual use of this function is when we have an externally provided tick vector, which
  might exceed far beyond the currently displayed range, and would cause unnecessary calculations
  e.g. of subticks.
*/
void QCPAxis::visibleTickBounds(int &lowIndex, int &highIndex) const
{
  lowIndex = 0;
  highIndex = -1;
  // make sure only ticks that are in visible range are returned
  for (int i=0; i < mTickVector->size(); ++i)
  {
    lowIndex = i;
    if (mTickVector->at(i) >= mRange.lower) break;
  }
  for (int i=mTickVector->size()-1; i >= 0; --i)
  {
    highIndex = i;
    if (mTickVector->at(i) <= mRange.upper) break;
  }
}

/*!
  \internal
  A log function with the base mScaleLogBase, used mostly for coordinate transforms in
  logarithmic scales with arbitrary log base. Uses the buffered mScaleLogBaseLogInv for faster
  calculation. This is set to <tt>1.0/log(mScaleLogBase)</tt> in \ref setScaleLogBase.
  \see basePow, setScaleLogBase, setScaleType
*/
double QCPAxis::baseLog(double value) const
{
  return log(value)*mScaleLogBaseLogInv;
}

/*!
  \internal
  A power function with the base mScaleLogBase, used mostly for coordinate transforms in
  logarithmic scales with arbitrary log base.
  \see baseLog, setScaleLogBase, setScaleType
*/
double QCPAxis::basePow(double value) const
{
  return pow(mScaleLogBase, value);
}

/*!
  \internal
  Simulates the steps of \ref drawAxis by calculating all appearing text bounding boxes. From this
  information, the appropriate margin for this axis is determined, so nothing is drawn beyond the
  widget border in the actual \ref drawAxis function (if \ref QCustomPlot::setAutoMargin is set to
  true).
  The margin consists of: tick label padding, tick label size, label padding, label size.
  The return value is the calculated margin for this axis. Thus, an axis with axis type
  \ref atLeft will return an appropriate left margin, \ref atBottom will return an appropriate
  bottom margin and so forth.
  \warning if anything is changed in this function, make sure it's synchronized with
  the actual drawing function \ref drawAxis.
*/
int QCPAxis::calculateMargin() const
{
  // run through similar steps as QCPAxis::drawAxis, and caluclate margin needed to fit axis and its labels
  int margin = 0;
  
  if (mVisible)
  {
    int lowTick, highTick;
    visibleTickBounds(lowTick, highTick);
    
    // get length of tick marks reaching outside axis rect:
    margin += qMax(0, qMax(mTickLengthOut, mSubTickLengthOut));
    // calculate size of tick labels:
    QSize tickLabelsSize(0, 0);
    if (mTickLabels)
    {
      for (int i=lowTick; i <= highTick; ++i)
      {
        getMaxTickLabelSize(mTickLabelFont, mTickVectorLabels->at(i), &tickLabelsSize);
      }
      if (orientation() == Qt::Horizontal)
        margin += tickLabelsSize.height() + mTickLabelPadding;
      else
        margin += tickLabelsSize.width() + mTickLabelPadding;
    }
    
    // calculate size of axis label (only height needed, because left/right labels are rotated by 90 degrees):
    if (!mLabel.isEmpty())
    {
      QFontMetrics fontMetrics(mLabelFont);
      QRect bounds;
      bounds = fontMetrics.boundingRect(0, 0, 0, 0, Qt::TextDontClip | Qt::AlignHCenter | Qt::AlignVCenter, mLabel);
      margin += bounds.height() + mLabelPadding;
    }
  }
  
  margin += mPadding;
  
  if (margin < 15) // need a bit of margin if no axis text is shown at all (i.e. only baseline and tick lines, or no axis at all)
    margin = 15;
  return margin;
}


// ================================================================================
// =================== QCustomPlot
// ================================================================================

/*! \class QCustomPlot
  \brief The central class which is also the QWidget which displays the plot and interacts with the user.
  
  For tutorials on how to use QCustomPlot, see the website\n
  http://www.WorksLikeClockWork.com/index.php/components/qt-plotting-widget
*/

/*!
  Constructs a QCustomPlot and sets reasonable default values.
  Four axes are created at the bottom, left, top and right sides (xAxis, yAxis, xAxis2, yAxis2),
  however, only the bottom and left axes are set to be visible.
  The legend is also set to be invisible initially.
*/
QCustomPlot::QCustomPlot(QWidget *parent) :
    QWidget(parent)
{  
  setMouseTracking(true);
  QLocale currentLocale = locale();
  currentLocale.setNumberOptions(QLocale::OmitGroupSeparator);
  setLocale(currentLocale);
  
  buffer = QPixmap(size());
  mViewport = rect();
  mDragging = false;
  QFont titleFont;
  titleFont.setPointSize(14);
  titleFont.setBold(true);
  setTitleFont(titleFont);
  setTitle("");
  setColor(Qt::white);
  setAntialiasedElements(aeGraphs | aeScatters | aeFills);
  legend = new QCPLegend(this);
  legend->setVisible(false);
  setAutoAddPlottableToLegend(true);
  xAxis = new QCPAxis(this, QCPAxis::atBottom);
  yAxis = new QCPAxis(this, QCPAxis::atLeft);
  xAxis2 = new QCPAxis(this, QCPAxis::atTop);
  yAxis2 = new QCPAxis(this, QCPAxis::atRight);
  xAxis2->setGrid(false);
  yAxis2->setGrid(false);
  xAxis2->setZeroLinePen(Qt::NoPen);
  yAxis2->setZeroLinePen(Qt::NoPen);
  xAxis2->setVisible(false);
  yAxis2->setVisible(false);
  setAxisBackground(QPixmap());
  setAxisBackgroundScaled(true);
  setAxisBackgroundScaledMode(Qt::KeepAspectRatioByExpanding);
  
  setRangeDragAxes(xAxis, yAxis);
  setRangeZoomAxes(xAxis, yAxis);
  setRangeDrag(0);
  setRangeZoom(0);
  setRangeZoomFactor(0.85);
  
  setMargin(0, 0, 0, 0);
  setAutoMargin(true);
  replot();
}

QCustomPlot::~QCustomPlot()
{
  clearPlottables();
  delete legend;
  delete xAxis;
  delete yAxis;
  delete xAxis2;
  delete yAxis2;
}

/*!
  Returns the range drag axis of the \a orientation provided
  \see setRangeDragAxes
*/
QCPAxis *QCustomPlot::rangeDragAxis(Qt::Orientation orientation)
{
  return (orientation == Qt::Horizontal ? mRangeDragHorzAxis : mRangeDragVertAxis);
}

/*!
  Returns the range zoom axis of the \a orientation provided
  \see setRangeZoomAxes
*/
QCPAxis *QCustomPlot::rangeZoomAxis(Qt::Orientation orientation)
{
  return (orientation == Qt::Horizontal ? mRangeZoomHorzAxis : mRangeZoomVertAxis);
}

/*!
  Returns the range zoom factor of the \a orientation provided
  \see setRangeZoomFactor
*/
double QCustomPlot::rangeZoomFactor(Qt::Orientation orientation)
{
  return (orientation == Qt::Horizontal ? mRangeZoomFactorHorz : mRangeZoomFactorVert);
}

/*!
  Sets the plot title which will be drawn centered at the top of the widget.
  The title position is not dependant on the actual position of the axes. However, if
  \ref setAutoMargin is set to true, the top margin will be adjusted appropriately,
  so the top axis labels/tick labels will not overlap with the title.
*/
void QCustomPlot::setTitle(const QString &title)
{
  mTitle = title;
}

/*!
  Sets the font of the plot title
  \see setTitle
*/
void QCustomPlot::setTitleFont(const QFont &font)
{
  mTitleFont = font;
}

/*!
  An alternative way to set the margins, by directly setting the wanted axis rect. The rect
  will be translated into appropriate margin values.
  \warning setting the axis rect with this function does not guarantee that the axis rect
  will stay like this indefinitely. In QCustomPlot, margins are the fixed values (if \ref
  setAutoMargin is false). Consequently, the axis rect is automatically changed when the
  widget size changes, but the margins (distances between axis rect edges and widget/viewport
  rect edges) stay the same.
  \see setMargin
*/
void QCustomPlot::setAxisRect(const QRect &arect)
{
  mMarginLeft = arect.left()-mViewport.left();
  mMarginRight = mViewport.right()-arect.right();
  mMarginTop = arect.top()-mViewport.top();
  mMarginBottom = mViewport.bottom()-arect.bottom();
  updateAxisRect();
}

/*!
  Sets the left margin manually. Will only have effect, if \ref setAutoMargin is set to false.
  see \ref setMargin for an explanation of what margins mean in QCustomPlot.
*/
void QCustomPlot::setMarginLeft(int margin)
{
  mMarginLeft = margin;
  updateAxisRect();
}

/*!
  Sets the right margin manually. Will only have effect, if \ref setAutoMargin is set to false.
  see \ref setMargin for an explanation of what margins mean in QCustomPlot.
*/
void QCustomPlot::setMarginRight(int margin)
{
  mMarginRight = margin;
  updateAxisRect();
}

/*!
  Sets the top margin manually. Will only have effect, if \ref setAutoMargin is set to false.
  see \ref setMargin for an explanation of what margins mean in QCustomPlot.
*/
void QCustomPlot::setMarginTop(int margin)
{
  mMarginTop = margin;
  updateAxisRect();
}

/*!
  Sets the bottom margin manually. Will only have effect, if \ref setAutoMargin is set to false.
  see \ref setMargin for an explanation of what margins mean in QCustomPlot.
*/
void QCustomPlot::setMarginBottom(int margin)
{
  mMarginBottom = margin;
  updateAxisRect();
}

/*!
  Sets the margins manually. Will only have effect, if \ref setAutoMargin is set to false.
  The margins are the distances in pixels between the axes box and the viewport box.
  The viewport box normally is the entire QCustomPlot widget or the entire image, if
  using one of the export functions. Positive margin values always mean the axes box
  is shrinked, going inward from the sides of the viewport box.
*/
void QCustomPlot::setMargin(int left, int right, int top, int bottom)
{
  mMarginLeft = left;
  mMarginRight = right;
  mMarginTop = top;
  mMarginBottom = bottom;
  updateAxisRect();
}

/*!
  Sets whether the margins are calculated automatically depeding on the sizes
  of the tick labels, axis labels, paddings etc.
  If disabled, the margins must be set manually with the \a setMargin functions.
  \see setMargin, QCPAxis::setLabelPadding, QCPAxis::setTickLabelPadding
*/
void QCustomPlot::setAutoMargin(bool enabled)
{
  mAutoMargin = enabled;
}

/*!
  Sets the background color of the QCustomPlot widget.
*/
void QCustomPlot::setColor(const QColor &color)
{
  mColor = color;
}

/*!
  Sets which axis orientation may be range dragged by the user with mouse interaction.
  What orientation corresponds to which specific axis can be set with
  \ref setRangeDragAxes(QCPAxis *horizontal, QCPAxis *vertical). By
  default, the horizontal axis is the bottom axis (xAxis) and the vertical axis
  is the left axis (yAxis).
  To disable range dragging entirely, pass 0 as \a orientations. To enable range dragging
  for both directions, pass <tt>Qt::Horizontal | Qt::Vertical</tt> as \a orientations.
*/
void QCustomPlot::setRangeDrag(Qt::Orientations orientations)
{
  mRangeDrag = orientations;
}

/*!
  Sets which axis orientation may be zoomed by the user with the mouse wheel.
  What orientation corresponds to which specific axis can be set with
  \ref setRangeZoomAxes(QCPAxis *horizontal, QCPAxis *vertical). By
  default, the horizontal axis is the bottom axis (xAxis) and the vertical axis
  is the left axis (yAxis).
  To disable range zooming entirely, pass 0 as \a orientations. To enable range zooming
  for both directions, pass <tt>Qt::Horizontal | Qt::Vertical</tt> as \a orientations.
*/
void QCustomPlot::setRangeZoom(Qt::Orientations orientations)
{
  mRangeZoom = orientations;
}

/*!
  Sets the axes whose range will be dragged when \ref setRangeDrag enables mouse range dragging
  on the QCustomPlot widget.
*/
void QCustomPlot::setRangeDragAxes(QCPAxis *horizontal, QCPAxis *vertical)
{
  if (horizontal)
    mRangeDragHorzAxis = horizontal;
  if (vertical)
    mRangeDragVertAxis = vertical;
}

/*!
  Sets the axes whose range will be zoomed when \ref setRangeZoom enables mouse wheel zooming on the
  QCustomPlot widget. The two axes can be zoomed with different strengths, when different factors
  are passed to \ref setRangeZoomFactor(double horizontalFactor, double verticalFactor).
*/
void QCustomPlot::setRangeZoomAxes(QCPAxis *horizontal, QCPAxis *vertical)
{
  if (horizontal)
    mRangeZoomHorzAxis = horizontal;
  if (vertical)
    mRangeZoomVertAxis = vertical;
}

/*!
  Sets how strong one rotation step of the mouse wheel zooms, when range zoom was activated with
  \ref setRangeZoom. The two parameters \a horizontalFactor and \a verticalFactor provide a way to
  let the horizontal axis zoom at different rates as the vertical axis. Which axis is horizontal
  and which is vertical, can be set with \ref setRangeZoomAxes. When the zoom factor is greater
  than one, scrolling the mouse wheel backwards (towards the user) will zoom in. For zoom factors
  smaller than one, scrolling the mouse wheel forwards zooms in.
*/
void QCustomPlot::setRangeZoomFactor(double horizontalFactor, double verticalFactor)
{
  mRangeZoomFactorHorz = horizontalFactor;
  mRangeZoomFactorVert = verticalFactor;
}

/*!
  \overload
  Sets how strong one rotation step of the mouse wheel zooms, when range zoom was activated with
  \ref setRangeZoom. The zoom \a factor is applied to both horizontal and vertical range zooming.
  When the zoom factor is greater than one, scrolling the mouse wheel backwards (towards the user)
  will zoom in. For zoom factors smaller than one, scrolling the mouse wheel forwards zooms in.
*/
void QCustomPlot::setRangeZoomFactor(double factor)
{
  mRangeZoomFactorHorz = factor;
  mRangeZoomFactorVert = factor;
}

/*!
  Sets which elements are drawn antialiased as an or combination of \ref AntialiasedElement.
*/
void QCustomPlot::setAntialiasedElements(const AntialiasedElements &antialiasedElements)
{
  mAntialiasedElements = antialiasedElements;
}

/*!
  Sets whether the specified \a antialiasedElement is drawn antialiased.
*/
void QCustomPlot::setAntialiasedElement(AntialiasedElement antialiasedElement, bool enabled)
{
  if (!enabled && mAntialiasedElements.testFlag(antialiasedElement))
    mAntialiasedElements &= ~antialiasedElement;
  else if (enabled && !mAntialiasedElements.testFlag(antialiasedElement))
    mAntialiasedElements |= antialiasedElement;
}

/*!
  If set to true, adding a plottable (e.g. a graph) to the QCustomPlot automatically also adds the
  newly created plottable to the legend.
  
  \see addPlottable, addGraph, QCPLegend::addItem
*/
void QCustomPlot::setAutoAddPlottableToLegend(bool on)
{
  mAutoAddPlottableToLegend = on;
}

/*!
  Sets \a pm as the axis background pixmap. The axis background pixmap will be drawn inside the current
  axis rect, before anything else (e.g. the axes themselves, grids, graphs, etc.) is drawn.
  If the provided pixmap doesn't have the same size as the axis rect, scaling can be enabled with \ref setAxisBackgroundScaled
  and the scaling mode (i.e. whether and how the aspect ratio is preserved) can be set with \ref setAxisBackgroundScaledMode.
  To set all these options in one call, consider using the overloaded version of this function.
  \see setAxisBackgroundScaled, setAxisBackgroundScaledMode
*/
void QCustomPlot::setAxisBackground(const QPixmap &pm)
{
  mAxisBackground = pm;
  mScaledAxisBackground = QPixmap();
}

/*!
  \overload
  Allows setting the background pixmap, whether it shall be scaled and how it shall be scaled in one call.
  \see setAxisBackground(const QPixmap &pm), setAxisBackgroundScaled, setAxisBackgroundScaledMode
*/
void QCustomPlot::setAxisBackground(const QPixmap &pm, bool scaled, Qt::AspectRatioMode mode)
{
  mAxisBackground = pm;
  mScaledAxisBackground = QPixmap();
  mAxisBackgroundScaled = scaled;
  mAxisBackgroundScaledMode = mode;
}

/*!
  Sets whether the axis background pixmap shall be scaled to fit the current axis rect or not. If
  \a scaled is set to true, you may control whether and how the aspect ratio of the original pixmap is
  preserved with \ref setAxisBackgroundScaledMode.
  
  Note that the scaled version of the original pixmap is buffered, so there is no performance penalty
  on replots, when enabling the scaling. (Except of course, the axis rect is continuously
  changed, but that's not very likely.)
  
  \see setAxisBackground, setAxisBackgroundScaledMode
*/
void QCustomPlot::setAxisBackgroundScaled(bool scaled)
{
  mAxisBackgroundScaled = scaled;
}

/*!
  If scaling of the axis background pixmap is enabled (\ref setAxisBackgroundScaled), use this function to
  define whether and how the aspect ratio of the original pixmap passed to \ref setAxisBackground is preserved.
  \see setAxisBackground, setAxisBackgroundScaled
*/
void QCustomPlot::setAxisBackgroundScaledMode(Qt::AspectRatioMode mode)
{
  mAxisBackgroundScaledMode = mode;
}

/*!
  Returns the plottable with \a index. If the index is invalid, returns 0.
  
  There is an overloaded version of this function with no parameter which returns the last created
  plottable, see QCustomPlot::plottable()
  
  \see plottableCount, addPlottable
*/
QCPAbstractPlottable *QCustomPlot::plottable(int index)
{
  if (index >= 0 && index < mPlottables.size())
  {
    return mPlottables.at(index);
  } else
  {
    qDebug() << FUNCNAME << "index out of bounds:" << index;
    return 0;
  }
}

/*! \overload
  
  Returns the last plottable, that was created with \ref addPlottable. If there are no plottables in the plot,
  returns 0.
  
  \see plottableCount, addPlottable
*/
QCPAbstractPlottable *QCustomPlot::plottable()
{
  if (!mPlottables.isEmpty())
  {
    return mPlottables.last();
  } else
    return 0;
}

/*!
  Adds the specified plottable to the plot and, if \ref setAutoAddPlottableToLegend is enabled, to the legend.
  QCustomPlot takes ownership of the plottable.
  
  Returns true on success, i.e. when \a plottable wasn't already added to the plot and
  the parent plot of \a plottable is this QCustomPlot (the latter is controlled by what
  axes the plottable was passed in the constructor).
  
  \see plottable, plottableCount, removePlottable, clearPlottables
*/
bool QCustomPlot::addPlottable(QCPAbstractPlottable *plottable)
{
  if (!mPlottables.contains(plottable) && plottable->parentPlot() == this)
  {
    mPlottables.append(plottable);
    // possibly add plottable to legend:
    if (mAutoAddPlottableToLegend)
      plottable->addToLegend();
    // special handling for QCPGraphs to maintain the simple graph interface:
    if (QCPGraph *graph = dynamic_cast<QCPGraph*>(plottable))
      mGraphs.append(graph);
    return true;
  } else
  {
    //qDebug() << FUNCNAME << "plottable either already in list or not created with this QCustomPlot as parent:" << (int)plottable;
    return false;
  }
}

/*!
  Removes the specified plottable from the plot and, if necessary, from the legend.
  
  Returns true on success.
  
  \see addPlottable, clearPlottables
*/
bool QCustomPlot::removePlottable(QCPAbstractPlottable *plottable)
{
  if (mPlottables.contains(plottable))
  {
    // remove plottable from legend:
    plottable->removeFromLegend();
    // special handling for QCPGraphs to maintain the simple graph interface:
    if (QCPGraph *graph = dynamic_cast<QCPGraph*>(plottable))
      mGraphs.removeOne(graph);
    // remove plottable:
    delete plottable;
    mPlottables.removeOne(plottable);
    return true;
  } else
  {
    //qDebug() << FUNCNAME << "plottable not in list:" << (int)plottable;
    return false;
  }
}

/*! \overload
  
  Removes the plottable by its \a index.
*/
bool QCustomPlot::removePlottable(int index)
{
  if (index >= 0 && index < mPlottables.size())
    return removePlottable(mPlottables[index]);
  else
  {
    
    qDebug() << FUNCNAME << "index out of bounds:" << index;
    return false;
  }
}

/*!
  Removes all graphs from the plot (and the legend, if necessary).
  
  Returns the number of graphs removed.
  
  \see removePlottable
*/
int QCustomPlot::clearPlottables()
{
  int c = mPlottables.size();
  for (int i=c-1; i >= 0; --i)
    removePlottable(mPlottables[i]);
  return c;
}

/*!
  Returns the number of currently existing plottables in the plot
  
  \see plottable, addPlottable
*/
int QCustomPlot::plottableCount() const
{
  return mPlottables.size();
}

/*!
  Returns the graph with \a index. If the index is invalid, returns 0.
  
  There is an overloaded version of this function with no parameter which returns the last created
  graph, see QCustomPlot::graph()
  
  \see graphCount, addGraph
*/
QCPGraph *QCustomPlot::graph(int index) const
{
  if (index >= 0 && index < mGraphs.size())
  {
    return mGraphs.at(index);
  } else
  {
    qDebug() << FUNCNAME << "index out of bounds:" << index;
    return 0;
  }
}

/*! \overload
  
  Returns the last graph, that was created with \ref addGraph. If there are no graphs in the plot,
  returns 0.
  
  \see graphCount, addGraph
*/
QCPGraph *QCustomPlot::graph() const
{
  if (!mGraphs.isEmpty())
  {
    return mGraphs.last();
  } else
    return 0;
}

/*!
  Creates a new graph inside the plot. If \a keyAxis and \a valueAxis are left unspecified, the
  bottom (xAxis) is used as key and the left (yAxis) is used as value. \a keyAxis and \a valueAxis
  must reside in the same QCustomPlot.
  
  \param keyAxis the axis that will be used as key axis for the graph (typically "x")
  \param valueAxis the axis that will be used as value axis for the graph (typically "y")
  
  Returns a pointer to the newly created graph.
  
  \see graph, graphCount, removeGraph, clearGraphs
*/
QCPGraph *QCustomPlot::addGraph(QCPAxis *keyAxis, QCPAxis *valueAxis)
{
  if (!keyAxis) keyAxis = xAxis;
  if (!valueAxis) valueAxis = yAxis;
  QCPGraph *newGraph = new QCPGraph(keyAxis, valueAxis);
  if (addPlottable(newGraph))
  {
    newGraph->setName("Graph "+QString::number(mGraphs.size()));
    return newGraph;
  } else
  {
    delete newGraph;
    return 0;
  }
}

/*!
  Removes the specified \a graph from the plot and, if necessary, from the legend. If
  any other graphs in the plot have a channel fill set towards the removed graph, the channel fill
  property of those graphs is reset to zero (no channel fill).
  
  Returns true on success.
  
  \see clearGraphs
*/
bool QCustomPlot::removeGraph(QCPGraph *graph)
{
  return removePlottable(graph);
}

/*! \overload
  
  Removes the graph by its \a index.
*/
bool QCustomPlot::removeGraph(int index)
{
  if (index >= 0 && index < mGraphs.size())
    return removeGraph(mGraphs[index]);
  else
    return false;
}

/*!
  Removes all graphs from the plot (and the legend, if necessary).
  Returns the number of graphs removed.
  \see removeGraph
*/
int QCustomPlot::clearGraphs()
{
  int c = mGraphs.size();
  for (int i=c-1; i >= 0; --i)
    removeGraph(mGraphs[i]);
  return c;
}

/*!
  Returns the number of currently existing graphs in the plot
  
  \see graph, addGraph
*/
int QCustomPlot::graphCount() const
{
  return mGraphs.size();
}

void QCustomPlot::setNumberOfGraphs(int ngraphs)
{
    this->clearGraphs();
    for (int i=0; i<ngraphs; i++)
        this->addGraph();
}

/*!
  Causes a complete replot (axes, labels, graphs, etc.) into the internal buffer QPixmap. Finally,
  update() is called, to redraw the buffer on the QCustomPlot widget surface.
*/
void QCustomPlot::replot()
{
  QPainter painter(&buffer);
  if (!painter.isActive()) // might happen if QCustomPlot has width or height zero
  {
    qDebug() << FUNCNAME << "Couldn't activate painter on buffer";
    return;
  }
  painter.fillRect(rect(), mColor);
  draw(&painter);
  update();
}

/*!
  Convenience function to make the top and right axes visible and assign them the following
  properties from their corresponding bottom/left axes:
  
  \li range (\ref QCPAxis::setRange)
  \li range reversed (\ref QCPAxis::setRangeReversed)
  \li scale type (\ref QCPAxis::setScaleType)
  \li scale log base  (\ref QCPAxis::setScaleLogBase)
  \li ticks (\ref QCPAxis::setTicks)
  \li auto (major) tick count (\ref QCPAxis::setAutoTickCount)
  \li sub tick count (\ref QCPAxis::setSubTickCount)
  \li auto sub ticks (\ref QCPAxis::setAutoSubTicks)
  \li tick step (\ref QCPAxis::setTickStep)
  \li auto tick step (\ref QCPAxis::setAutoTickStep)
  
  Tick labels (\ref QCPAxis::setTickLabels) however, is always set to false.

  This function does \a not connect the rangeChanged signals of the bottom and left axes to the \ref
  QCPAxis::setRange slots of the top and right axes in order to synchronize the ranges permanently.
*/
void QCustomPlot::setupFullAxesBox()
{
  xAxis2->setVisible(true);
  yAxis2->setVisible(true);
  
  xAxis2->setTickLabels(false);
  yAxis2->setTickLabels(false);
  
  xAxis2->setAutoSubTicks(xAxis->autoSubTicks());
  yAxis2->setAutoSubTicks(yAxis->autoSubTicks());
  
  xAxis2->setAutoTickCount(xAxis->autoTickCount());
  yAxis2->setAutoTickCount(yAxis->autoTickCount());
  
  xAxis2->setAutoTickStep(xAxis->autoTickStep());
  yAxis2->setAutoTickStep(yAxis->autoTickStep());
  
  xAxis2->setScaleType(xAxis->scaleType());
  yAxis2->setScaleType(yAxis->scaleType());
  
  xAxis2->setScaleLogBase(xAxis->scaleLogBase());
  yAxis2->setScaleLogBase(yAxis->scaleLogBase());
  
  xAxis2->setTicks(xAxis->ticks());
  yAxis2->setTicks(yAxis->ticks());
  
  xAxis2->setSubTickCount(xAxis->subTickCount());
  yAxis2->setSubTickCount(yAxis->subTickCount());
  
  xAxis2->setTickStep(xAxis->tickStep());
  yAxis2->setTickStep(yAxis->tickStep());
  
  xAxis2->setRange(xAxis->range());
  yAxis2->setRange(yAxis->range());
  
  xAxis2->setRangeReversed(xAxis->rangeReversed());
  yAxis2->setRangeReversed(yAxis->rangeReversed());
}

/*!
  Rescales the axes such that all plottables (e.g. graphs) in the plot are fully visible.
  It does this by calling \ref QCPAbstractPlottable::rescaleAxes on all plottables.
  
  \see QCPAbstractPlottable::rescaleAxes
*/
void QCustomPlot::rescaleAxes()
{
  if (mPlottables.isEmpty()) return;
  
  mPlottables.at(0)->rescaleAxes(false); // onlyEnlarge disabled on first plottable
  for (int i=1; i<mPlottables.size(); ++i)
    mPlottables.at(i)->rescaleAxes(true);  // onlyEnlarge enabled on all other plottables
}

/*!
  Saves a PDF with the vectorized plot to the file \a fileName. The axis ratio as well as the scale
  of texts and lines will be derived from the specified \a width and \a height. This means, the
  output will look like the normal on-screen output of a QCustomPlot widget with the corresponding
  pixel width and height. If either \a width or \a height is zero, the exported image will have
  the same dimensions as the QCustomPlot widget currently has.

  \a noCosmeticPen disables the use of cosmetic pens when drawing to the PDF file. Cosmetic pens
  are pens with numerical width 0, which are always drawn as a one pixel wide line, no matter what
  zoom factor is set in the PDF-Viewer. For more information about cosmetic pens, see QPainter and
  QPen documentation.

  \warning
  \li If you plan on editing the exported PDF file with a vector graphics editor like
  Inkscape, it is advised to set \a noCosmeticPen to true to avoid losing those cosmetic lines
  (which might be quite many, because cosmetic pens are the default for e.g. axes and tick marks).
  \li If calling this function inside the constructor of the parent of the QCustomPlot widget
  (i.e. the MainWindow constructor, if QCustomPlot is inside the MainWindow), always provide
  explicit non-zero widths and heights. If you leave \a width or \a height as 0 (default), this
  function uses the current width and height of the QCustomPlot widget. However, in Qt, these
  aren't defined yet inside the constructor, so you would get an image that has strange
  widths/heights.
  
  \see savePng
*/
void QCustomPlot::savePdf(const QString &fileName, bool noCosmeticPen, int width, int height)
{
  int newWidth, newHeight;
  if (width == 0 || height == 0)
  {
    newWidth = this->width();
    newHeight = this->height();
  } else
  {
    newWidth = width;
    newHeight = height;
  }
  
  QPrinter printer(QPrinter::ScreenResolution);
  printer.setOutputFileName(fileName);
  printer.setFullPage(true);
  QRect oldViewport = mViewport;
  mViewport = QRect(0, 0, newWidth, newHeight);
  updateAxisRect();
  printer.setPaperSize(mViewport.size(), QPrinter::DevicePixel);
  QPainter printpainter(&printer);
  printpainter.setWindow(mViewport);
  printpainter.setRenderHint(QPainter::NonCosmeticDefaultPen, noCosmeticPen);
  draw(&printpainter);
  mViewport = oldViewport;
  updateAxisRect();
}

/*
  Function for providing svg export. Requires the QtSvg module
  This is Not tested and will require some modifications!
  
void QCustomPlot::saveSvg(const QString &fileName)
{  
  QSvgGenerator generator;
  generator.setFileName(fileName);
  generator.setSize(QSize(200, 200));
  generator.setViewBox(QRect(0, 0, 200, 200));
  generator.setTitle("");
  generator.setDescription("");
  QPainter painter(&generator);
  draw(&painter);
}
*/

/*!
  Saves a PNG image file to \a fileName on disc. The output plot will have the dimensions \a width
  and \a height in pixels. If either \a width or \a height is zero, the exported image will have
  the same dimensions as the QCustomPlot widget currently has. Line widths and texts etc. are not
  scaled up when larger widths/heights are used. If you want that effect, consider the scaled
  version of this function.
  
  \warning If calling this function inside the constructor of the parent of the QCustomPlot widget
  (i.e. the MainWindow constructor, if QCustomPlot is inside the MainWindow), always provide
  explicit non-zero widths and heights. If you leave \a width or \a height as 0 (default), this
  function uses the current width and height of the QCustomPlot widget. However, in Qt, these
  aren't defined yet inside the constructor, so you would get an image that has strange
  widths/heights.
  
  \see savePngScaled
*/
void QCustomPlot::savePng(const QString &fileName, int width, int height)
{  
  int newWidth, newHeight;
  if (width == 0 || height == 0)
  {
    newWidth = this->width();
    newHeight = this->height();
  } else
  {
    newWidth = width;
    newHeight = height;
  }
  
  QPixmap pngBuffer(newWidth, newHeight);
  QPainter painter(&pngBuffer);
  painter.fillRect(pngBuffer.rect(), mColor);
  QRect oldViewport = mViewport;
  mViewport = QRect(0, 0, newWidth, newHeight);
  updateAxisRect();
  draw(&painter);
  mViewport = oldViewport;
  updateAxisRect();
  pngBuffer.save(fileName);
}

/*!
  Saves a PNG image file to \a fileName on disc. The output plot will have a base \a width and \a
  height which is then scaled by factor \a scale. If you for example set both \a width and \a height to
  100 and \a scale to 2, you will end up with a PNG file of size 200*200 in which all graphical
  elements are scaled up by factor 2 (line widths, texts, etc.). This scaling is done not by
  stretching a 100*100 image but by actually scaling the painter, so the result will have full
  200*200 pixel resolution.

  \warning
  \li If calling this function inside the constructor of the parent of the QCustomPlot widget
  (i.e. the MainWindow constructor, if QCustomPlot is inside the MainWindow), always provide
  explicit non-zero widths and heights. If you leave \a width or \a height as 0 (default), this
  function uses the current width and height of the QCustomPlot widget. However, in Qt, these
  aren't defined yet inside the constructor, so you would get an image that has strange
  widths/heights.
  \li When using the raster graphicssystem: There currently is a bug in the Qt painting system that
  prevents proper scaling of pen widths when using the raster graphicssystem. So if you use this
  function and need properly scaled pens widths, consider using a different graphicssystem than
  raster (or apply a non-zero pen width to all pens, since the bug only occurs for cosmetic pens).
  
  \see savePng
*/
void QCustomPlot::savePngScaled(const QString &fileName, double scale, int width, int height)
{  
  int newWidth, newHeight;
  if (width == 0 || height == 0)
  {
    newWidth = this->width();
    newHeight = this->height();
  } else
  {
    newWidth = width;
    newHeight = height;
  }
  
  int scaledWidth = scale*newWidth;
  int scaledHeight = scale*newHeight;
  
  QPixmap pngBuffer(scaledWidth, scaledHeight);
  QPainter painter(&pngBuffer);
  painter.setRenderHint(QPainter::NonCosmeticDefaultPen);
  painter.fillRect(pngBuffer.rect(), mColor);
  QRect oldViewport = mViewport;
  mViewport = QRect(0, 0, newWidth, newHeight);
  updateAxisRect();
  painter.scale(scale, scale);
  draw(&painter);
  mViewport = oldViewport;
  updateAxisRect();
  pngBuffer.save(fileName);
}

/*! \internal
  
  Event handler for when the QCustomPlot widget needs repainting.
  This does not cause a replot, but draws the internal buffer QPixmap on
  the widget surface.
*/
void QCustomPlot::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);
  QPainter painter(this);
  painter.drawPixmap(0, 0, buffer);
}

/*! \internal
  
  Event handler for a resize of the QCustomPlot widget. Causes the
  internal buffer QPixmap to be resized to the new size. The
  viewport and the axis rect are resized appropriately. Finally a
  replot is performed.
*/
void QCustomPlot::resizeEvent(QResizeEvent *event)
{
  // resize and repaint the buffer:
  buffer = QPixmap(event->size());
  mViewport = rect();
  updateAxisRect();
  replot();
}

/*! \internal
  
  emits the mouseDoubleClick signal
*/
void QCustomPlot::mouseDoubleClickEvent(QMouseEvent *event)
{
  emit mouseDoubleClick(event);
}

/*! \internal
  
  Event handler for when a mouse button is pressed. If the left
  mouse button is pressed, the mDragging flag is set to true and
  some anchor points are set that are needed to determine the distance
  that the range was dragged in the mouseMoveEvent.
  Finally, the mousePress signal is emitted.
  
  \see mouseMoveEvent, mouseReleaseEvent
*/
void QCustomPlot::mousePressEvent(QMouseEvent *event)
{
  if (event->buttons() & Qt::LeftButton)
  {
    mDragging = true;
    mDragStart = event->pos();
    mDragStartHorzRange = mRangeDragHorzAxis->range();
    mDragStartVertRange = mRangeDragVertAxis->range();
  } else
    mDragging = false;
  emit mousePress(event);
}

/*! \internal
  
  Event handler for when the cursor is moved. First, the mouseMove signal is
  emitted. Then the built-in range dragging functionality is handled.
  
  \see mousePressEvent, mouseReleaseEvent
*/
void QCustomPlot::mouseMoveEvent(QMouseEvent *event)
{
  emit mouseMove(event);
  if (mDragging)
  {
    if (mRangeDrag.testFlag(Qt::Horizontal))
    {
      if (mRangeDragHorzAxis->mScaleType == QCPAxis::stLinear)
      {
        double diff = mRangeDragHorzAxis->pixelToCoord(mDragStart.x()) - mRangeDragHorzAxis->pixelToCoord(event->pos().x());
        mRangeDragHorzAxis->setRange(mDragStartHorzRange.lower+diff, mDragStartHorzRange.upper+diff);
      } else if (mRangeDragHorzAxis->mScaleType == QCPAxis::stLogarithmic)
      {
        double diff = mRangeDragHorzAxis->pixelToCoord(mDragStart.x()) / mRangeDragHorzAxis->pixelToCoord(event->pos().x());
        mRangeDragHorzAxis->setRange(mDragStartHorzRange.lower*diff, mDragStartHorzRange.upper*diff);
      }
    }
    if (mRangeDrag.testFlag(Qt::Vertical))
    {
      if (mRangeDragVertAxis->mScaleType == QCPAxis::stLinear)
      {
        double diff = mRangeDragVertAxis->pixelToCoord(mDragStart.y()) - mRangeDragVertAxis->pixelToCoord(event->pos().y());
        mRangeDragVertAxis->setRange(mDragStartVertRange.lower+diff, mDragStartVertRange.upper+diff);
      } else if (mRangeDragVertAxis->mScaleType == QCPAxis::stLogarithmic)
      {
        double diff = mRangeDragVertAxis->pixelToCoord(mDragStart.y()) / mRangeDragVertAxis->pixelToCoord(event->pos().y());
        mRangeDragVertAxis->setRange(mDragStartVertRange.lower*diff, mDragStartVertRange.upper*diff);
      }
    }
    if (mRangeDrag != 0) // if either vertical or horizontal drag was enabled, do a replot
      replot();
  }
}

/*! \internal
  
  Event handler for when a mouse button is released. First, the mDragging flag is set
  to false (used by range dragging functionality) and then the mouseRelease signal is
  emitted.
  
  \see mousePressEvent, mouseMoveEvent
*/
void QCustomPlot::mouseReleaseEvent(QMouseEvent *event)
{
  mDragging = false;
  emit mouseRelease(event);
}

/*! \internal
  
  Event handler for mouse wheel events. First, the mouseWheel signal is emitted.
  If rangeZoom is Qt::Horizontal, Qt::Vertical or both, the ranges of the axes defined as
  rangeZoomHorzAxis and rangeZoomVertAxis are scaled. The center of the scaling
  operation is the current cursor position inside the plot. The scaling factor
  is dependant on the mouse wheel delta (which direction the wheel was rotated)
  to provide a natural zooming feel. The Strength of the zoom can be controlled via
  \ref setRangeZoomFactor.
  
  Note, that event->delta() is usually +/-120 for single rotation steps. However, if the mouse
  wheel is turned rapidly, many steps may bunch up to one event, so the event->delta() may then be
  multiples of 120. This is taken into account here, by calculating \a wheelSteps and using it as
  exponent of the range zoom factor. This takes care of the wheel direction automatically, by
  inverting the factor, when the wheel step is negative (f^-1 = 1/f).
*/
void QCustomPlot::wheelEvent(QWheelEvent *event)
{
  emit mouseWheel(event);
  if (mRangeZoom != 0)
  {
    double factor;
    double wheelSteps = event->delta()/120.0; // a single step delta is +/-120 usually
    if (mRangeZoom.testFlag(Qt::Horizontal))
    {
      factor = pow(mRangeZoomFactorHorz, wheelSteps);
      mRangeZoomHorzAxis->scaleRange(factor, mRangeZoomHorzAxis->pixelToCoord(event->pos().x()));
    }
    if (mRangeZoom.testFlag(Qt::Vertical))
    {
      factor = pow(mRangeZoomFactorVert, wheelSteps);
      mRangeZoomVertAxis->scaleRange(factor, mRangeZoomVertAxis->pixelToCoord(event->pos().y()));
    }
    replot();
  }
}

/*! \internal
  
  This is the main draw function which first generates the tick vectors of all axes,
  calculates and applies appropriate margins if autoMargin is true and finally draws
  all elements with the passed \a painter. (axis background, title, subgrid, grid, axes, plottables)
*/
void QCustomPlot::draw(QPainter *painter)
{

  // draw title:
  QRect titleBounds;
  if (!mTitle.isEmpty())
  {
    painter->setFont(mTitleFont);
    titleBounds = painter->fontMetrics().boundingRect(0, 0, titleBounds.width(), titleBounds.height(), Qt::TextDontClip | Qt::AlignHCenter | Qt::AlignVCenter, mTitle);
    painter->drawText(mViewport.left(), mViewport.top(), mViewport.width(), titleBounds.height(), Qt::TextDontClip | Qt::AlignHCenter | Qt::AlignVCenter, mTitle);
  }  
  
  // prepare values of ticks and tick strings:
  xAxis->generateTickVectors();
  yAxis->generateTickVectors();
  xAxis2->generateTickVectors();
  yAxis2->generateTickVectors();
  
  // set auto margin such that tick/axis labels etc. are not clipped:
  if (mAutoMargin)
  {
    setMargin(yAxis->calculateMargin(),
              yAxis2->calculateMargin(),
              xAxis2->calculateMargin()+titleBounds.height(),
              xAxis->calculateMargin());
  }
  
  // draw axis background:
  drawAxisBackground(painter);
  
  // draw grids (and zerolines):
  xAxis->drawSubGrid(painter);
  yAxis->drawSubGrid(painter);
  xAxis2->drawSubGrid(painter);
  yAxis2->drawSubGrid(painter);
  xAxis->drawGrid(painter);
  yAxis->drawGrid(painter);
  xAxis2->drawGrid(painter);
  yAxis2->drawGrid(painter);
  
  // draw all plottables:
  for (int i=0; i < mPlottables.size(); ++i)
  {
    painter->save(); // since this might be user subclass, we save painter outside - just in case
    mPlottables.at(i)->draw(painter);
    painter->restore();
  }
  
  // draw axes, ticks and axis labels:
  xAxis->drawAxis(painter);
  yAxis->drawAxis(painter);
  xAxis2->drawAxis(painter);
  yAxis2->drawAxis(painter);
  
  // draw legend:
  legend->reArrange();
  legend->draw(painter);
}

/*! \internal

  If an axis background is provided via \ref setAxisBackground, this function first buffers the
  scaled version depending on \ref setAxisBackgroundScaled and \ref setAxisBackgroundScaledMode and
  then draws it inside the current axisRect with the provided \a painter. The scaled version is
  buffered in mScaledAxisBackground to prevent the need for rescaling at every redraw. It is only
  updated, when the axisRect has changed in a way that requires a rescale of the background pixmap
  (this is dependant on the \ref setAxisBackgroundScaledMode), or when a differend axis backgroud
  was set.
  
  \see draw, setAxisBackground, setAxisBackgroundScaled, setAxisBackgroundScaledMode
*/
void QCustomPlot::drawAxisBackground(QPainter *painter)
{
  if (!mAxisBackground.isNull())
  {
    if (mAxisBackgroundScaled)
    {
      // check whether mScaledAxisBackground needs to be updated:
      QSize scaledSize(mAxisBackground.size());
      scaledSize.scale(mAxisRect.size(), mAxisBackgroundScaledMode);
      if (mScaledAxisBackground.size() != scaledSize)
        mScaledAxisBackground = mAxisBackground.scaled(mAxisRect.size(), mAxisBackgroundScaledMode, Qt::SmoothTransformation);
      painter->drawPixmap(mAxisRect.topLeft(), mScaledAxisBackground, QRect(0, 0, mAxisRect.width(), mAxisRect.height()) & mScaledAxisBackground.rect());
    } else
    {
      painter->drawPixmap(mAxisRect.topLeft(), mAxisBackground, QRect(0, 0, mAxisRect.width(), mAxisRect.height()));
    }
  }
}

/*! \internal
  
  calculates mAxisRect by applying the margins inward to mViewport. The axisRect is then
  passed on to all axes via QCPAxis::setAxisRect
  
  \see setMargin, setAxisRect
*/
void QCustomPlot::updateAxisRect()
{
  mAxisRect = mViewport.adjusted(mMarginLeft, mMarginTop, -mMarginRight, -mMarginBottom);
  xAxis->setAxisRect(mAxisRect);
  yAxis->setAxisRect(mAxisRect);
  xAxis2->setAxisRect(mAxisRect);
  yAxis2->setAxisRect(mAxisRect);
}


// ================================================================================
// =================== QCPAbstractPlottable
// ================================================================================

/*! \class QCPAbstractPlottable
  \brief The abstract base class for all data representing objects in a plot.

  It defines a very basic interface like name, pen, brush, visibility etc. Since this class is
  abstract, it can't be instantiated. Use one of the subclasses or create a subclass yourself (see
  below), to create new ways of displaying data.
  
  All further specifics are in the subclasses, for example:
  \li A normal graph with possibly a line, scatter points and error bars is displayed by \ref QCPGraph
  (typically created with \ref QCustomPlot::addGraph).
  \li A parametric curve can be displayed with \ref QCPCurve.
  \li A stackable bar chart can be achieved with \ref QCPBars.
  \li A box of a statistical box plot is created with \ref QCPStatisticalBox.
  
  \section subclassing Creating own plottables
  
  To create an own plottable, you implement a subclass of QCPAbstractPlottable. These are the pure
  virtual functions, you must implement:
  \li \ref clearData
  \li \ref draw
  \li \ref drawLegendIcon
  \li \ref getKeyRange
  \li \ref getValueRange
  
  See the documentation of those functions for what they need to do.
  
  For drawing your plot, you can use the \ref coordsToPixels functions to translate a point in plot
  coordinates to pixel coordinates. This function is quite convenient, because it takes the
  orientation of the key and value axes into account for you (x and y are swapped when the key axis
  is vertical and the value axis horizontal). If you are worried about performance (i.e. you need
  to translate many points in a loop like QCPGraph), you can directly use \ref
  QCPAxis::coordToPixel. However, you must then take care about the orientation of the axis
  yourself.
  
  From QCPAbstractPlottable you inherit the following members you may use:

  \li QCustomPlot *\b mParentPlot: A pointer to the parent QCustomPlot instance. This is adopted from
  the axes that are passed in the constructor.
  \li QString \b mName: The name of the plottable.
  \li bool \b mVisible: Whether the plot is visible or not. When this is false, you shouldn't draw the data in the \ref draw function (\ref draw is always called, no matter what mVisible is).
  \li QPen \b mPen: The generic pen of the plottable. You should use this pen for the most prominent data representing lines in the plottable (e.g QCPGraph uses this pen for its graph lines and scatters)
  \li QBrush \b mBrush: The generic brush of the plottable. You should use this brush for the most prominent fillable structures in the plottable (e.g. QCPGraph uses this brush to control filling under the graph)
  \li QCPAxis *\b mKeyAxis, *\b mValueAxis: The key and value axes this plottable is attached to. Call their QCPAxis::coordToPixel functions to translate coordinates to pixels in either the key or value dimension.
*/

/* start of documentation of pure virtual functions */

/*! \fn void QCPAbstractPlottable::clearData() = 0
  Clears all data in the plottable.
*/

/*! \fn void QCPAbstractPlottable::draw(QPainter *painter) const = 0
  \internal
  
  Draws this plottable with the provided \a painter. Called by \ref QCustomPlot::draw on all its
  plottables.
*/

/*! \fn void QCPAbstractPlottable::drawLegendIcon(QPainter *painter, const QRect &rect) const = 0
  \internal
  
  called by QCPLegend::draw (via QCPPlottableLegendItem::draw) to create a graphical representation
  of this plottable inside \a rect, next to the plottable name.
*/

/*! \fn QCPRange QCPAbstractPlottable::getKeyRange(bool &validRange, SignDomain inSignDomain) const = 0
  \internal
  
  called by rescaleAxes functions to get the full data key bounds. For logarithmic plots, one can
  set \a inSignDomain to either \ref sdNegative or \ref sdPositive in order to restrict the
  returned range to that sign domain. E.g. when only negative range is wanted, set \a inSignDomain
  to \ref sdNegative and all positive points will be ignored for range calculation. For no
  restriction, just set \a inSignDomain to \ref sdBoth (default). \a validRange is an output
  parameter that indicates whether a proper range could be found or not. If this is false, you
  shouldn't use the returned range (e.g. no points in data).
  
  \see rescaleAxes, getValueRange
*/

/*! \fn QCPRange QCPAbstractPlottable::getValueRange(bool &validRange, SignDomain inSignDomain) const = 0
  \internal
  
  called by rescaleAxes functions to get the full data value bounds. For logarithmic plots, one can
  set \a inSignDomain to either \ref sdNegative or \ref sdPositive in order to restrict the
  returned range to that sign domain. E.g. when only negative range is wanted, set \a inSignDomain
  to \ref sdNegative and all positive points will be ignored for range calculation. For no
  restriction, just set \a inSignDomain to \ref sdBoth (default). \a validRange is an output
  parameter that indicates whether a proper range could be found or not. If this is false, you
  shouldn't use the returned range (e.g. no points in data).
  
  \see rescaleAxes, getKeyRange
*/

/* end of documentation of pure virtual functions */

/*!
  Constructs an abstract plottable which uses \a keyAxis as its key axis ("x") and \a valueAxis as
  its value axis ("y"). \a keyAxis and \a valueAxis must reside in the same QCustomPlot instance.
  
  Since QCPAbstractPlottable is an abstract class that defines the basic interface to plottables
  (i.e. any form of data representation inside a plot, like graphs, curves etc.), it can't be
  directly instantiated.
  
  You probably want one of the subclasses like \ref QCPGraph and \ref QCPCurve instead.
  \see setkeyAxis, setValueAxis
*/
QCPAbstractPlottable::QCPAbstractPlottable(QCPAxis *keyAxis, QCPAxis *valueAxis) :
  mParentPlot(keyAxis->parentPlot()),
  mName(""),
  mVisible(true),
  mPen(Qt::black),
  mBrush(Qt::NoBrush),
  mKeyAxis(keyAxis),
  mValueAxis(valueAxis)
{
  if (keyAxis->parentPlot() != valueAxis->parentPlot())
    qDebug() << FUNCNAME << "Parent plot of keyAxis is not the same as that of valueAxis.";
  if (keyAxis->orientation() == valueAxis->orientation())
    qDebug() << FUNCNAME << "keyAxis and valueAxis must be orthogonal to each other.";
}

/*!
   The name is the textual representation of this plottable as it is displayed in the QCPLegend of
   the parent QCustomPlot. It may contain any utf-8 characters, including newlines.
*/
void QCPAbstractPlottable::setName(const QString &name)
{
  mName = name;
}

/*!
  If the plottable visibility is set to false, it won't be drawn to the plot surface.
  It will still appear in a \ref QCPLegend that it's associated with, though.
*/
void QCPAbstractPlottable::setVisible(bool visible)
{
  mVisible = visible;
}

/*!
  The pen is used to draw basic lines that make up the plottable representation in the
  plot.
  
  For example, the \ref QCPGraph subclass draws its graph lines and scatter points
  with this pen.

  \see setBrush
*/
void QCPAbstractPlottable::setPen(const QPen &pen)
{
  mPen = pen;
}

/*!
  The brush is used to draw basic fills of the plottable representation in the
  plot. The Fill can be a color, gradient or texture, see the usage of QBrush.
  
  For example, the \ref QCPGraph subclass draws the fill under the graph with this brush, when
  it's not set to Qt::NoBrush.

  \see setPen
*/
void QCPAbstractPlottable::setBrush(const QBrush &brush)
{
  mBrush = brush;
}

/*!
  The key axis of a plottable can be set to any axis of a QCustomPlot, as long as it is orthogonal
  to the plottable's value axis. The typical mathematical choice is to use the x-axis
  (QCustomPlot::xAxis) as key axis and the y-axis (QCustomPlot::yAxis) as value axis.

  \see setValueAxis
*/
void QCPAbstractPlottable::setKeyAxis(QCPAxis *axis)
{
  mKeyAxis = axis;
}

/*!
  The value axis of a plottable can be set to any axis of a QCustomPlot, as long as it is
  orthogonal to the plottable's key axis. The typical mathematical choice is to use the x-axis
  (QCustomPlot::xAxis) as key axis and the y-axis (QCustomPlot::yAxis) as value axis.

  \see setKeyAxis
*/
void QCPAbstractPlottable::setValueAxis(QCPAxis *axis)
{
  mValueAxis = axis;
}

/*!
  Rescales the key and value axes associated with this plottable to contain all displayed data, so
  the whole plottable is visible. If the scaling of an axis is logarithmic, rescaleAxes will make
  sure not to rescale to an illegal range i.e. a range containing different signs and/or zero.
  Instead it will stay in the current sign domain and ignore all entities of the plottable that lie
  outside of that domain.
  
  \a onlyEnlarge makes sure the ranges are only expanded, never reduced. So it's possible to show
  multiple plottables in their entirety by multiple calls to rescaleAxes where the first call has
  \a onlyEnlarge set to false (the default), and all subsequent set to true.
*/
void QCPAbstractPlottable::rescaleAxes(bool onlyEnlarge) const
{
  rescaleKeyAxis(onlyEnlarge);
  rescaleValueAxis(onlyEnlarge);
}

/*!
  Rescales the key axis of the plottable so the whole plottable is visible.
  
  See \ref rescaleAxes for detailed behaviour.
*/
void QCPAbstractPlottable::rescaleKeyAxis(bool onlyEnlarge) const
{
  SignDomain signDomain = sdBoth;
  if (mKeyAxis->scaleType() == QCPAxis::stLogarithmic)
    signDomain = (mKeyAxis->range().upper < 0 ? sdNegative : sdPositive);
  
  bool validRange;
  QCPRange newRange = getKeyRange(validRange, signDomain);
  
  if (validRange)
  {
    if (onlyEnlarge)
    {
      if (mKeyAxis->range().lower < newRange.lower)
        newRange.lower = mKeyAxis->range().lower;
      if (mKeyAxis->range().upper > newRange.upper)
        newRange.upper = mKeyAxis->range().upper;
    }
    mKeyAxis->setRange(newRange);
  }
}

/*!
  Rescales the value axis of the plottable so the whole plottable is visible.
  
  See \ref rescaleAxes for detailed behaviour.
*/
void QCPAbstractPlottable::rescaleValueAxis(bool onlyEnlarge) const
{
  SignDomain signDomain = sdBoth;
  if (mValueAxis->scaleType() == QCPAxis::stLogarithmic)
    signDomain = (mValueAxis->range().upper < 0 ? sdNegative : sdPositive);
  
  bool validRange;
  QCPRange newRange = getValueRange(validRange, signDomain);
  
  if (validRange)
  {
    if (onlyEnlarge)
    {
      if (mValueAxis->range().lower < newRange.lower)
        newRange.lower = mValueAxis->range().lower;
      if (mValueAxis->range().upper > newRange.upper)
        newRange.upper = mValueAxis->range().upper;
    }
    mValueAxis->setRange(newRange);
  }
}

/*!
  Adds this plottable to the legend of the parent QCustomPlot.
    
  Normally, a QCPPlottableLegendItem is created and inserted into the legend. If the plottable
  needs a more specialized representation in the plot, this function will take this into account
  and instead create the specialized subclass of QCPAbstractLegendItem.
    
  Returns true on success, i.e. when a legend item associated with this plottable isn't already in
  the legend.
    
  \see removeFromLegend, QCPLegend::addItem
*/
bool QCPAbstractPlottable::addToLegend() const
{
  if (!mParentPlot->legend->hasItemWithPlottable(this))
  {
    mParentPlot->legend->addItem(new QCPPlottableLegendItem(mParentPlot->legend, this));
    return true;
  } else
    return false;
}

/*! 
  Removes the plottable from the legend of the parent QCustomPlot. This means the
  QCPAbstractLegendItem (usually a QCPPlottableLegendItem) that is associated with this plottable
  is removed.
    
  Returns true on success, i.e. if a legend item associated with this plottable was found and
  removed from the legend.
    
  \see addToLegend, QCPLegend::removeItem
*/
bool QCPAbstractPlottable::removeFromLegend() const
{
  if (QCPPlottableLegendItem *lip = mParentPlot->legend->itemWithPlottable(this))
    return mParentPlot->legend->removeItem(lip);
  else
    return false;
}

/*! \internal
  
  Convenience function for transforming a key/value pair to pixels on the QCustomPlot surface,
  taking the orientations of the axes associated with this plottable into account (e.g. whether key
  becomes x or y).
  
  \a key and \a value are transformed to the coodinates in pixels and are written to \a x and \a y.
    
  \see QCPAxis::coordToPixel
*/
void QCPAbstractPlottable::coordsToPixels(double key, double value, double &x, double &y) const
{
  if (mKeyAxis->orientation() == Qt::Horizontal)
  {
    x = mKeyAxis->coordToPixel(key);
    y = mValueAxis->coordToPixel(value);
  } else
  {
    y = mKeyAxis->coordToPixel(key);
    x = mValueAxis->coordToPixel(value);
  }
}

/*! \internal 
  \overload
  Returns the input as pixel coordinates in a QPointF.
  \see QCPAxis::coordToPixel
*/
const QPointF QCPAbstractPlottable::coordsToPixels(double key, double value) const
{
  if (mKeyAxis->orientation() == Qt::Horizontal)
    return QPointF(mKeyAxis->coordToPixel(key), mValueAxis->coordToPixel(value));
  else
    return QPointF(mValueAxis->coordToPixel(value), mKeyAxis->coordToPixel(key));
}


// ================================================================================
// =================== QCPAbstractLegendItem
// ================================================================================
/*! \class QCPAbstractLegendItem
  \brief The abstract base class for all items in a QCPLegend.
  
  It defines a very basic interface to items in a QCPLegend. For representing plottables in the
  legend, the subclass QCPPlottableLegendItem is more suitable.
  
  Only derive directly from this class when you need absolute freedom (i.e. a legend item that's
  not associated with a plottable).

  You must implement the following pure virtual functions:
  \li \ref draw
  \li \ref size
  
  You inherit the following members you may use:
  \li QCPLegend *\b mParentLegend: A pointer to the parent QCPLegend.
  \li QFont \b mFont: The generic font of the item. You should use this font for all or at least the most prominent text of the item.
*/

/* start documentation of pure virtual functions */

/*! \fn void QCPAbstractLegendItem::draw(QPainter *painter, const QRect &rect) const = 0;
  
  Draws this legend item with \a painter inside the specified \a rect. The \a rect typically has
  the size which was returned from a preceding \ref size call.
*/

/*! \fn QSize QCPAbstractLegendItem::size(const QSize &targetSize) const = 0;

  Returns the size this item occupies in the legend. The legend will adapt its layout with the help
  of this function. If this legend item can have a variable width (e.g. auto-wrapping text), this
  function tries to find a size with a width close to the width of \a targetSize. The height of \a
  targetSize only may have meaning in specific sublasses. Typically, it's ignored.
*/

/* end documentation of pure virtual functions */

/*!
  Constructs a QCPAbstractLegendItem and associates it with the QCPLegend \a parent. This does not
  cause the item to be added to \a parent, so \ref QCPLegend::addItem must be called separately.
*/
QCPAbstractLegendItem::QCPAbstractLegendItem(QCPLegend *parent) : 
  mParentLegend(parent),
  mFont(parent->font())
{
}

/*!
  Sets the default font of this specific legend item to \a font.
  
  \see QCPLegend::setFont
*/
void QCPAbstractLegendItem::setFont(const QFont &font)
{
  mFont = font;
}


// ================================================================================
// =================== QCPPlottableLegendItem
// ================================================================================
/*! \class QCPPlottableLegendItem
  \brief A legend item representing a plottable with an icon and the plottable name.
  
  This is the standard legend item for plottables. It displays an icon of the plottable next to the
  plottable name. The icon is drawn by the respective plottable itself (\ref
  QCPAbstractPlottable::drawLegendIcon), and tries to give an intuitive symbol for the plottable.
  For example, the QCPGraph draws a centered horizontal line with a single scatter point in the
  middle and filling (if enabled) below.
  
  Legend items of this type are always associated with one plottable (retrievable via the
  plottable() function and settable with the constructor). You may change the font of the plottable
  name with \ref setFont. If \ref setTextWrap is set to true, the plottable name will wrap at the
  right legend boundary (see \ref QCPLegend::setMinimumSize). Icon padding and border pen is taken
  from the parent QCPLegend, see \ref QCPLegend::setIconBorderPen and \ref
  QCPLegend::setIconTextPadding.

  The function \ref QCPAbstractPlottable::addToLegend/\ref QCPAbstractPlottable::removeFromLegend
  creates/removes legend items of this type in the default implementation. However, these functions
  may be reimplemented such that a different kind of legend item (e.g a direct subclass of
  QCPAbstractLegendItem) is used for that plottable.
*/

/*!
  Creates a new legend item associated with \a plottable.
  
  Once it's created, it can be added to the legend via \ref QCPLegend::addItem.
  
  A more convenient way of adding/removing a plottable to/from the legend is via the functions \ref
  QCPAbstractPlottable::addToLegend and \ref QCPAbstractPlottable::removeFromLegend.
*/
QCPPlottableLegendItem::QCPPlottableLegendItem(QCPLegend *parent, const QCPAbstractPlottable *plottable) :
  QCPAbstractLegendItem(parent),
  mPlottable(plottable)
{
  // take default values from parent legend:
  mIconSize = parent->iconSize();
  mIconBorderPen = parent->iconBorderPen();
  mIconTextPadding = parent->iconTextPadding();
  mTextWrap = false;
}

/*!
  Sets whether the text of the legend item is wrapped at word boundaries to fit the with of the
  legend.
  
  To prevent the legend autoSize feature (QCPLegend::setAutoSize) from compressing the text too
  strong by wrapping it very often, set an appropriate minimum width with
  QCPLegend::setMinimumSize.
*/
void QCPPlottableLegendItem::setTextWrap(bool wrap)
{
  mTextWrap = wrap;
}

/*! \internal
  
  Draws the item with \a painter into \a rect.

  The width of the passed rect is used as text wrapping width, when \ref setTextWrap is enabled.
  The height is ignored. The rect is not equivalent to a clipping rect (overpainting is not
  prevented), so you should set an appropriate clipping rect on the painter before calling this
  function. Ideally, the width of the rect should be the result of a preceding call to \ref size.
*/
void QCPPlottableLegendItem::draw(QPainter *painter, const QRect &rect) const
{
  if (!mPlottable) return;
  painter->setFont(mFont);
  QRect textRect;
  QRect iconRect(rect.topLeft(), mIconSize);
  if (mTextWrap)
  {
    // take width from rect since our text should wrap there (only icon must fit at least):
    textRect = painter->fontMetrics().boundingRect(0, 0, rect.width()-mIconTextPadding-mIconSize.width(), rect.height(), Qt::TextDontClip | Qt::TextWordWrap, mPlottable->name());
    if (textRect.height() < mIconSize.height()) // text smaller than icon, center text vertically in icon height
    {
      painter->drawText(rect.x()+mIconSize.width()+mIconTextPadding, rect.y(), rect.width()-mIconTextPadding-mIconSize.width(), mIconSize.height(), Qt::TextDontClip | Qt::TextWordWrap, mPlottable->name());
    } else // text bigger than icon, position top of text with top of icon
    {
      painter->drawText(rect.x()+mIconSize.width()+mIconTextPadding, rect.y(), rect.width()-mIconTextPadding-mIconSize.width(), textRect.height(), Qt::TextDontClip | Qt::TextWordWrap, mPlottable->name());
    }
  } else
  {
    // text can't wrap (except with explicit newlines), center at current item size (icon size)
    textRect = painter->fontMetrics().boundingRect(0, 0, 0, rect.height(), Qt::TextDontClip, mPlottable->name());
    if (textRect.height() < mIconSize.height()) // text smaller than icon, center text vertically in icon height
    {
      painter->drawText(rect.x()+mIconSize.width()+mIconTextPadding, rect.y(), rect.width(), mIconSize.height(), Qt::TextDontClip, mPlottable->name());
    } else // text bigger than icon, position top of text with top of icon
    {
      painter->drawText(rect.x()+mIconSize.width()+mIconTextPadding, rect.y(), rect.width(), textRect.height(), Qt::TextDontClip, mPlottable->name());
    }
  }
  // draw icon:
  painter->save();
  painter->setClipRect(iconRect, Qt::IntersectClip);
  mPlottable->drawLegendIcon(painter, iconRect);
  painter->restore();
  // draw icon border:
  if (mIconBorderPen.style() != Qt::NoPen)
  {
    painter->setPen(mIconBorderPen);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(iconRect);
  }
}

/*! \internal
  
  Calculates and returns the size of this item. If \ref setTextWrap is enabled, the width of \a
  targetSize will be used as the text wrapping width. This does not guarantee, that the width of
  the returned QSize is the same as the width of \a targetSize, since wrapping occurs only at word
  boundaries. So a single word that extends beyond the width of \a targetSize, will stretch the
  returned QSize accordingly.
  
  The height of \a targetSize is ignored. The height of the returned QSize is either the height
  of the icon or the height of the text bounding box, whichever is larger.
*/
QSize QCPPlottableLegendItem::size(const QSize &targetSize) const
{
  if (!mPlottable) return QSize();
  QSize result(0, 0);
  QRect textRect;
  QFontMetrics fontMetrics(mFont);
  if (mTextWrap)
  {
    // take width from targetSize since our text can wrap (Only icon must fit at least):
    textRect = fontMetrics.boundingRect(0, 0, targetSize.width()-mIconTextPadding-mIconSize.width(), mIconSize.height(), Qt::TextDontClip | Qt::TextWordWrap, mPlottable->name());
  } else
  {
    // text can't wrap (except with explicit newlines), center at current item size (icon size)
    textRect = fontMetrics.boundingRect(0, 0, 0, mIconSize.height(), Qt::TextDontClip, mPlottable->name());
  }
  result.setWidth(mIconSize.width() + mIconTextPadding + textRect.width());
  result.setHeight(qMax(textRect.height(), mIconSize.height()));
  return result;
}

// ================================================================================
// =================== QCPCurve
// ================================================================================
/*! \class QCPCurve
  \brief A plottable representing a parametric curve in a plot.

  To plot data, assign it with the \ref setData or \ref addData functions.
  
  \section appearance Changing the appearance
  
  The appearance of the curve is determined by the pen and the brush (\ref setPen, \ref setBrush).
  \section usage Usage
  
  Like all data representing objects in QCustomPlot, the QCPCurve is a plottable (QCPAbstractPlottable). So
  the plottable-interface of QCustomPlot applies (QCustomPlot::plottable, QCustomPlot::addPlottable, QCustomPlot::removePlottable, etc.) 
  
  Usually, you first create an instance:
  \code
  QCPCurve *newCurve = new QCPCurve(customPlot->xAxis, customPlot->yAxis);\endcode
  add it to the customPlot with QCustomPlot::addPlottable:
  \code
  customPlot->addPlottable(newCurve);\endcode
  and then modify the properties of the newly created plottable, e.g.:
  \code
  newCurve->setName("Fermat's Spiral");
  newCurve->setData(tData, xData, yData);\endcode
*/

/*!
  Constructs a curve which uses \a keyAxis as its key axis ("x") and \a valueAxis as its value
  axis ("y"). \a keyAxis and \a valueAxis must both reside in the same QCustomPlot.
  
  The constructed QCPCurve can be added to the plot with QCustomPlot::addPlottable, QCustomPlot
  then takes ownership of the graph.
*/
QCPCurve::QCPCurve(QCPAxis *keyAxis, QCPAxis *valueAxis) :
  QCPAbstractPlottable(keyAxis, valueAxis)
{
  mData = new QCPCurveDataMap;
  mPen.setColor(Qt::blue);
  mPen.setStyle(Qt::SolidLine);
  mBrush.setColor(Qt::blue);
  mBrush.setStyle(Qt::NoBrush);
}

QCPCurve::~QCPCurve()
{
  delete mData;
}

/*!
  Replaces the current data with the provided \a data.
  
  If \a copy is set to true, data points in \a data will only be copied. if false, the plottable
  takes ownership of the passed data and replaces the internal data pointer with it. This is
  significantly faster than copying for large datasets.
*/
void QCPCurve::setData(QCPCurveDataMap *data, bool copy)
{
  if (copy)
  {
    *mData = *data;
  } else
  {
    delete mData;
    mData = data;
  }
}

/*! \overload
  
  Replaces the current data with the provided points in \a t, \a key and \a value tuples. The
  provided vectors should have equal length. Else, the number of added points will be the size of
  the smallest vector.
*/
void QCPCurve::setData(const QVector<double> &t, const QVector<double> &key, const QVector<double> &value)
{
  mData->clear();
  int n = t.size();
  n = qMin(n, key.size());
  n = qMin(n, value.size());
  QCPCurveData newData;
  for (int i=0; i<n; ++i)
  {
    newData.t = t[i];
    newData.key = key[i];
    newData.value = value[i];
    mData->insertMulti(newData.t, newData);
  }
}

/*! \overload
  
  Replaces the current data with the provided \a key and \a value pairs. The t parameter
  of each data point will be set to the integer index of the respective key/value pair.
*/
void QCPCurve::setData(const QVector<double> &key, const QVector<double> &value)
{
  mData->clear();
  int n = key.size();
  n = qMin(n, value.size());
  QCPCurveData newData;
  for (int i=0; i<n; ++i)
  {
    newData.t = i; // no t vector given, so we assign t the index of the key/value pair
    newData.key = key[i];
    newData.value = value[i];
    mData->insertMulti(newData.t, newData);
  }
}

/*!
  Adds the provided data points in \a dataMap to the current data.
  \see removeData
*/
void QCPCurve::addData(const QCPCurveDataMap &dataMap)
{
  mData->unite(dataMap);
}

/*! \overload
  Adds the provided single data point in \a data to the current data.
  \see removeData
*/
void QCPCurve::addData(const QCPCurveData &data)
{
  mData->insertMulti(data.t, data);
}

/*! \overload
  Adds the provided single data point as \a t, \a key and \a value tuple to the current data
  \see removeData
*/
void QCPCurve::addData(double t, double key, double value)
{
  QCPCurveData newData;
  newData.t = t;
  newData.key = key;
  newData.value = value;
  mData->insertMulti(newData.t, newData);
}

/*! \overload
  
  Adds the provided single data point as \a key and \a value pair to the current data The t
  parameter of the data point is set to the t of the last data point plus 1. If there is no last
  data point, t will be set to 0.
  
  \see removeData
*/
void QCPCurve::addData(double key, double value)
{
  QCPCurveData newData;
  if (!mData->isEmpty())
    newData.t = (mData->constEnd()-1).key()+1;
  else
    newData.t = 0;
  newData.key = key;
  newData.value = value;
  mData->insertMulti(newData.t, newData);
}

/*! \overload
  Adds the provided data points as \a t, \a key and \a value tuples to the current data.
  \see removeData
*/
void QCPCurve::addData(const QVector<double> &ts, const QVector<double> &keys, const QVector<double> &values)
{
  int n = ts.size();
  n = qMin(n, keys.size());
  n = qMin(n, values.size());
  QCPCurveData newData;
  for (int i=0; i<n; ++i)
  {
    newData.t = ts[i];
    newData.key = keys[i];
    newData.value = values[i];
    mData->insertMulti(newData.t, newData);
  }
}

/*!
  Removes all data points with curve parameter t smaller than \a t.
  \see addData, clearData
*/
void QCPCurve::removeDataBefore(double t)
{
  QCPCurveDataMap::iterator it = mData->begin();
  while (it != mData->end() && it.key() < t)
    it = mData->erase(it);
}

/*!
  Removes all data points with curve parameter t greater than \a t.
  \see addData, clearData
*/
void QCPCurve::removeDataAfter(double t)
{
  if (mData->isEmpty()) return;
  QCPCurveDataMap::iterator it = mData->upperBound(t);
  while (it != mData->end())
    it = mData->erase(it);
}

/*!
  Removes all data points with curve parameter t between \a fromt and \a tot. if \a fromt is
  greater or equal to \a tot, the function does nothing. To remove a single data point with known
  t, use \ref removeData(double t).
  
  \see addData, clearData
*/
void QCPCurve::removeData(double fromt, double tot)
{
  if (fromt >= tot || mData->isEmpty()) return;
  QCPCurveDataMap::iterator it = mData->upperBound(fromt);
  QCPCurveDataMap::iterator itEnd = mData->upperBound(tot);
  while (it != itEnd)
    it = mData->erase(it);
}

/*! \overload
  
  Removes a single data point at curve parameter \a t. If the position is not known with absolute
  precision, consider using \ref removeData(double fromt, double tot) with a small fuzziness
  interval around the suspected position, depeding on the precision with which the curve parameter
  is known.
  
  \see addData, clearData
*/
void QCPCurve::removeData(double t)
{
  mData->remove(t);
}

/*!
  Removes all data points.
  \see removeData, removeDataAfter, removeDataBefore
*/
void QCPCurve::clearData()
{
  mData->clear();
}

/* inherits documentation from base class */
void QCPCurve::draw(QPainter *painter) const
{
  if (!mVisible) return;
  if (mData->isEmpty()) return;
  painter->setClipRect(mKeyAxis->axisRect().united(mValueAxis->axisRect()));
  
  // allocate line vector:
  QVector<QPointF> *lineData = new QVector<QPointF>;
  // fill with curve data:
  getCurveData(lineData);
  // draw curve fill:
  if (mBrush.style() != Qt::NoBrush && mBrush.color().alpha() != 0)
  {
    painter->setRenderHint(QPainter::Antialiasing, mParentPlot->antialiasedElements().testFlag(QCustomPlot::aeFills));
    painter->setPen(Qt::NoPen);
    painter->setBrush(mBrush);
    painter->drawPolygon(QPolygonF(*lineData));
  }
  // draw curve line:
  if (mPen.style() != Qt::NoPen && mPen.color().alpha() != 0)
  {
    painter->setRenderHint(QPainter::Antialiasing, mParentPlot->antialiasedElements().testFlag(QCustomPlot::aeGraphs));
    painter->setPen(mPen);
    painter->setBrush(Qt::NoBrush);
    painter->drawPolyline(QPolygonF(*lineData));
  }
  // free allocated line data:
  delete lineData;
}

/* inherits documentation from base class */
void QCPCurve::drawLegendIcon(QPainter *painter, const QRect &rect) const
{
  // draw fill:
  if (mBrush.style() != Qt::NoBrush)
  {
    painter->setRenderHint(QPainter::Antialiasing, mParentPlot->antialiasedElements().testFlag(QCustomPlot::aeGraphs));
    painter->fillRect(rect.left(), rect.top()+rect.height()/2.0, rect.width(), rect.height()/3.0, mBrush);
  }
  // draw line vertically centered:
  painter->setRenderHint(QPainter::Antialiasing, mParentPlot->antialiasedElements().testFlag(QCustomPlot::aeGraphs));
  painter->setPen(mPen);
  painter->drawLine(rect.left(), rect.top()+rect.height()/2.0, rect.right()+5, rect.top()+rect.height()/2.0); // +5 on x2 else last segment is missing from dashed/dotted pens
}

/*! \internal
  
  called by QCPCurve::draw to generate a point vector which represents the line of the curve. Line
  segments that aren't visible in the current axis rect are handled in an optimized way.
*/
void QCPCurve::getCurveData(QVector<QPointF> *lineData) const
{
  /* Edges of axis rect R divide outside space into 9 regions:
     1__|_4_|__7  
     2__|_R_|__8
     3  | 6 |  9 
     General idea: If the two points of a line segment are in the same region (that is not R), the line segment is omitted.
     The region inside R has index 5.
  */
  lineData->reserve(mData->size());
  QCPCurveDataMap::const_iterator it;
  int lastRegion = 5;
  int currentRegion = 5;
  double RLeft = mKeyAxis->range().lower;
  double RRight = mKeyAxis->range().upper;
  double RBottom = mValueAxis->range().lower;
  double RTop = mValueAxis->range().upper;
  double x, y; // current key/value
  bool addedLastAlready = true;
  bool firstPoint = true; // first point must always be drawn, to make sure fill works correctly
  for (it = mData->constBegin(); it != mData->constEnd(); ++it)
  {
    x = it.value().key;
    y = it.value().value;
    // determine current region:
    if (x < RLeft) // region 123
    {
      if (y > RTop)
        currentRegion = 1;
      else if (y < RBottom)
        currentRegion = 3;
      else
        currentRegion = 2;
    } else if (x > RRight) // region 789
    {
      if (y > RTop)
        currentRegion = 7;
      else if (y < RBottom)
        currentRegion = 9;
      else
        currentRegion = 8;
    } else // region 456
    {
      if (y > RTop)
        currentRegion = 4;
      else if (y < RBottom)
        currentRegion = 6;
      else
        currentRegion = 5;
    }
    
    /*
      Watch out, the next part is very tricky, since it modifies the curve such that it seems like
      the whole thing is still drawn, but actually the points outside the axisRect are simplified
      ("optimized") greatly. There are some subtle special cases when line segments are large and
      thereby each subsequent point may be in a different region or even skip some.
    */
    // determine whether to keep current point:
    if (currentRegion == 5 || (firstPoint && mBrush.style() != Qt::NoBrush)) // current is in R, add current and last if it wasn't added already
    {
      if (!addedLastAlready) // in case curve just entered R, make sure the last point outside R is also drawn correctly
        lineData->append(coordsToPixels((it-1).value().key, (it-1).value().value)); // add last point to vector
      else if (lastRegion != 5) // added last already. If that's the case, we probably added it at optimized position. So go back and make sure it's at original position
      {
        if (!firstPoint) // because on firstPoint, currentRegion is 5 and addedLastAlready is true, although there is no last point
          lineData->replace(lineData->size()-1, coordsToPixels((it-1).value().key, (it-1).value().value));
      }
      lineData->append(coordsToPixels(it.value().key, it.value().value)); // add current point to vector
      addedLastAlready = true; // so in next iteration, we don't add this point twice
    } else if (currentRegion != lastRegion) // changed region, add current and last if not added already
    {
      // using outsideCoordsToPixels instead of coorsToPixels for optimized point placement (places points just outside axisRect instead of potentially far away)
      
      // if we're coming from R or we skip diagonally over the edge regions (so line might still be visible in R), we can't place points optimized
      if (lastRegion == 5 || // coming from R
          ((lastRegion==2 && currentRegion==4) || (lastRegion==4 && currentRegion==2)) || // skip top left diagonal
          ((lastRegion==4 && currentRegion==8) || (lastRegion==8 && currentRegion==4)) || // skip top right diagonal
          ((lastRegion==8 && currentRegion==6) || (lastRegion==6 && currentRegion==8)) || // skip bottom right diagonal
          ((lastRegion==6 && currentRegion==2) || (lastRegion==2 && currentRegion==6))    // skip bottom left diagonal
          )
      {
        // always add last point if not added already, original:
        if (!addedLastAlready)
          lineData->append(coordsToPixels((it-1).value().key, (it-1).value().value));
        // add current point, original:
        lineData->append(coordsToPixels(it.value().key, it.value().value));
      } else // no special case that forbids optimized point placement, so do it:
      {
        // always add last point if not added already, optimized:
        if (!addedLastAlready)
          lineData->append(outsideCoordsToPixels((it-1).value().key, (it-1).value().value, currentRegion));
        // add current point, optimized:
        lineData->append(outsideCoordsToPixels(it.value().key, it.value().value, currentRegion));
      }
      addedLastAlready = true; // so that if next point enters 5, or crosses another region boundary, we don't add this point twice
    } else // neither in R, nor crossed a region boundary, skip current point
    {
      addedLastAlready = false;
    }
    lastRegion = currentRegion;
    firstPoint = false;
  }
  // If curve ends outside R, we want to add very last point so the fill looks like it should when the curve started inside R:
  if (lastRegion != 5 && mBrush.style() != Qt::NoBrush && !mData->isEmpty())
    lineData->append(coordsToPixels((mData->constEnd()-1).value().key, (mData->constEnd()-1).value().value));
}

/*! \internal
  
  This is a specialized \ref coordsToPixels function for points that are outside the visible
  axisRect and just crossing a boundary (since \ref getCurveData reduces non-visible curve segments
  to those line segments that cross region boundaries, see documentation there). It only uses the
  coordinate parallel to the region boundary to the axisRect. The other coordinate is picked 10
  pixels outside the axisRect. Together with the optimization in \ref getCurveData this improves
  performance for large curves (or zoomed in ones) significantly while keeping the illusion the
  whole curve is still being drawn for the viewer.
*/
QPointF QCPCurve::outsideCoordsToPixels(double key, double value, int region) const
{
  int margin = 10;
  QRect axisRect = mKeyAxis->axisRect().united(mValueAxis->axisRect());
  QPointF result = coordsToPixels(key, value);
  switch (region)
  {
    case 2: result.setX(axisRect.left()-margin); break; // left
    case 8: result.setX(axisRect.right()+margin); break; // right
    case 4: result.setY(axisRect.top()-margin); break; // top
    case 6: result.setY(axisRect.bottom()+margin); break; // bottom
    case 1: result.setX(axisRect.left()-margin);
            result.setY(axisRect.top()-margin); break; // top left
    case 7: result.setX(axisRect.right()+margin);
            result.setY(axisRect.top()-margin); break; // top right
    case 9: result.setX(axisRect.right()+margin);
            result.setY(axisRect.bottom()+margin); break; // bottom right
    case 3: result.setX(axisRect.left()-margin);
            result.setY(axisRect.bottom()+margin); break; // bottom left
  }
  return result;
}

/* inherits documentation from base class */
QCPRange QCPCurve::getKeyRange(bool &validRange, SignDomain inSignDomain) const
{
  QCPRange range;
  bool haveLower = false;
  bool haveUpper = false;
  
  double current;
  
  QCPCurveDataMap::const_iterator it = mData->constBegin();
  while (it != mData->constEnd())
  {
    current = it.value().key;
    if (inSignDomain == sdBoth || (inSignDomain == sdNegative && current < 0) || (inSignDomain == sdPositive && current > 0))
    {
      if (current < range.lower || !haveLower)
      {
        range.lower = current;
        haveLower = true;
      }
      if (current > range.upper || !haveUpper)
      {
        range.upper = current;
        haveUpper = true;
      }
    }
    ++it;
  }
  
  validRange = haveLower && haveUpper;
  return range;
}

/* inherits documentation from base class */
QCPRange QCPCurve::getValueRange(bool &validRange, SignDomain inSignDomain) const
{
  QCPRange range;
  bool haveLower = false;
  bool haveUpper = false;
  
  double current;
  
  QCPCurveDataMap::const_iterator it = mData->constBegin();
  while (it != mData->constEnd())
  {
    current = it.value().value;
    if (inSignDomain == sdBoth || (inSignDomain == sdNegative && current < 0) || (inSignDomain == sdPositive && current > 0))
    {
      if (current < range.lower || !haveLower)
      {
        range.lower = current;
        haveLower = true;
      }
      if (current > range.upper || !haveUpper)
      {
        range.upper = current;
        haveUpper = true;
      }
    }
    ++it;
  }
  
  validRange = haveLower && haveUpper;
  return range;
}

// ================================================================================
// =================== QCPBars
// ================================================================================
/*! \class QCPBars
  \brief A plottable representing a bar chart in a plot.

  To plot data, assign it with the \ref setData or \ref addData functions.
  
  \section appearance Changing the appearance
  
  The appearance of the bars is determined by the pen and the brush (\ref setPen, \ref setBrush).
  
  Bar charts are stackable. This means, Two QCPBars plottables can be placed ontop of eachother
  (see \ref QCPBars::moveAbove). Then, when two bars are at the same key position, they will appear
  stacked.
  
  \section usage Usage
  
  Like all data representing objects in QCustomPlot, the QCPBars is a plottable
  (QCPAbstractPlottable). So the plottable-interface of QCustomPlot applies
  (QCustomPlot::plottable, QCustomPlot::addPlottable, QCustomPlot::removePlottable, etc.)
  
  Usually, you first create an instance:
  \code
  QCPBars *newBars = new QCPBars(customPlot->xAxis, customPlot->yAxis);\endcode
  add it to the customPlot with QCustomPlot::addPlottable:
  \code
  customPlot->addPlottable(newBars);\endcode
  and then modify the properties of the newly created plottable, e.g.:
  \code
  newBars->setName("Country population");
  newBars->setData(xData, yData);\endcode
*/

/*! \fn QCPBars *QCPBars::barBelow() const
  Returns the bars plottable that is directly below this bars plottable.
  If there is no such plottable, returns 0.
  
  \see barAbove, moveBelow, moveAbove
*/

/*! \fn QCPBars *QCPBars::barAbove() const
  Returns the bars plottable that is directly above this bars plottable.
  If there is no such plottable, returns 0.
  
  \see barBelow, moveBelow, moveAbove
*/

/*!
  Constructs a bar chart which uses \a keyAxis as its key axis ("x") and \a valueAxis as its value
  axis ("y"). \a keyAxis and \a valueAxis must both reside in the same QCustomPlot.
  
  The constructed QCPBars can be added to the plot with QCustomPlot::addPlottable, QCustomPlot
  then takes ownership of the bar chart.
*/

QCPBars::QCPBars(QCPAxis *keyAxis, QCPAxis *valueAxis) :
  QCPAbstractPlottable(keyAxis, valueAxis),
  mBarBelow(0),
  mBarAbove(0)
{
  mData = new QCPBarDataMap;
  mPen.setColor(Qt::blue);
  mPen.setStyle(Qt::SolidLine);
  mBrush.setColor(QColor(40, 50, 255, 30));
  mBrush.setStyle(Qt::SolidPattern);
  mWidth = 0.75;
}

QCPBars::~QCPBars()
{
  if (mBarBelow || mBarAbove)
    connectBars(mBarBelow, mBarAbove); // take this bar out of any stacking
  delete mData;
}

/*!
  Sets the width of the bars in plot (key) coordinates.
*/
void QCPBars::setWidth(double width)
{
  mWidth = width;
}

/*!
  Replaces the current data with the provided \a data.
  
  If \a copy is set to true, data points in \a data will only be copied. if false, the plottable
  takes ownership of the passed data and replaces the internal data pointer with it. This is
  significantly faster than copying for large datasets.
*/
void QCPBars::setData(QCPBarDataMap *data, bool copy)
{
  if (copy)
  {
    *mData = *data;
  } else
  {
    delete mData;
    mData = data;
  }
}

/*! \overload
  
  Replaces the current data with the provided points in \a key and \a value tuples. The
  provided vectors should have equal length. Else, the number of added points will be the size of
  the smallest vector.
*/
void QCPBars::setData(const QVector<double> &key, const QVector<double> &value)
{
  mData->clear();
  int n = key.size();
  n = qMin(n, value.size());
  QCPBarData newData;
  for (int i=0; i<n; ++i)
  {
    newData.key = key[i];
    newData.value = value[i];
    mData->insertMulti(newData.key, newData);
  }
}

/*!
  Moves this bars plottable below \a bars. In other words, the bars of this plottable will appear
  below the bars of \a bars. The move target \a bars must use the same key and value axis as this
  plottable.
  
  Inserting into and removing from existing bar stacking is handled gracefully. If \a bars already
  has a bars object below itself, this bars object is inserted between the two. If this bars object
  is already between two other bars, the two other bars will be stacked ontop of each other after
  the operation.
  
  To remove this bars plottable from any stacking, set \a bars to 0.
  
  \see moveBelow, barAbove, barBelow
*/
void QCPBars::moveBelow(QCPBars *bars)
{
  if (bars == this) return;
  if (bars->keyAxis() != mKeyAxis || bars->valueAxis() != mValueAxis)
  {
    qDebug() << FUNCNAME << "passed QCPBars* doesn't have same key and value axis as this QCPBars";
    return;
  }
  // remove from stacking:
  connectBars(mBarBelow, mBarAbove); // Note: also works if one (or both) of them is 0
  // if new bar given, insert this bar below it:
  if (bars)
  {
    if (bars->mBarBelow)
      connectBars(bars->mBarBelow, this);
    connectBars(this, bars);
  }
}

/*!
  Moves this bars plottable above \a bars. In other words, the bars of this plottable will appear
  above the bars of \a bars. The move target \a bars must use the same key and value axis as this
  plottable.
  
  Inserting into and removing from existing bar stacking is handled gracefully. If \a bars already
  has a bars object below itself, this bars object is inserted between the two. If this bars object
  is already between two other bars, the two other bars will be stacked ontop of each other after
  the operation.
  
  To remove this bars plottable from any stacking, set \a bars to 0.
  
  \see moveBelow, barBelow, barAbove
*/
void QCPBars::moveAbove(QCPBars *bars)
{
  if (bars == this) return;
  if (bars && (bars->keyAxis() != mKeyAxis || bars->valueAxis() != mValueAxis))
  {
    qDebug() << FUNCNAME << "passed QCPBars* doesn't have same key and value axis as this QCPBars";
    return;
  }
  // remove from stacking:
  connectBars(mBarBelow, mBarAbove); // Note: also works if one (or both) of them is 0
  // if new bar given, insert this bar above it:
  if (bars)
  {
    if (bars->mBarAbove)
      connectBars(this, bars->mBarAbove);
    connectBars(bars, this);
  }
}

/*!
  Adds the provided data points in \a dataMap to the current data.
  \see removeData
*/
void QCPBars::addData(const QCPBarDataMap &dataMap)
{
  mData->unite(dataMap);
}

/*! \overload
  Adds the provided single data point in \a data to the current data.
  \see removeData
*/
void QCPBars::addData(const QCPBarData &data)
{
  mData->insertMulti(data.key, data);
}

/*! \overload
  Adds the provided single data point as \a key and \a value tuple to the current data
  \see removeData
*/
void QCPBars::addData(double key, double value)
{
  QCPBarData newData;
  newData.key = key;
  newData.value = value;
  mData->insertMulti(newData.key, newData);
}

/*! \overload
  Adds the provided data points as \a key and \a value tuples to the current data.
  \see removeData
*/
void QCPBars::addData(const QVector<double> &keys, const QVector<double> &values)
{
  int n = keys.size();
  n = qMin(n, values.size());
  QCPBarData newData;
  for (int i=0; i<n; ++i)
  {
    newData.key = keys[i];
    newData.value = values[i];
    mData->insertMulti(newData.key, newData);
  }
}

/*!
  Removes all data points with key smaller than \a key.
  \see addData, clearData
*/
void QCPBars::removeDataBefore(double key)
{
  QCPBarDataMap::iterator it = mData->begin();
  while (it != mData->end() && it.key() < key)
    it = mData->erase(it);
}

/*!
  Removes all data points with key greater than \a key.
  \see addData, clearData
*/
void QCPBars::removeDataAfter(double key)
{
  if (mData->isEmpty()) return;
  QCPBarDataMap::iterator it = mData->upperBound(key);
  while (it != mData->end())
    it = mData->erase(it);
}

/*!
  Removes all data points with key between \a fromKey and \a toKey. if \a fromKey is
  greater or equal to \a toKey, the function does nothing. To remove a single data point with known
  key, use \ref removeData(double key).
  
  \see addData, clearData
*/
void QCPBars::removeData(double fromKey, double toKey)
{
  if (fromKey >= toKey || mData->isEmpty()) return;
  QCPBarDataMap::iterator it = mData->upperBound(fromKey);
  QCPBarDataMap::iterator itEnd = mData->upperBound(toKey);
  while (it != itEnd)
    it = mData->erase(it);
}

/*! \overload
  
  Removes a single data point at \a key. If the position is not known with absolute precision,
  consider using \ref removeData(double fromKey, double toKey) with a small fuzziness interval
  around the suspected position, depeding on the precision with which the key is known.
  
  \see addData, clearData
*/
void QCPBars::removeData(double key)
{
  mData->remove(key);
}

/*!
  Removes all data points.
  \see removeData, removeDataAfter, removeDataBefore
*/
void QCPBars::clearData()
{
  mData->clear();
}

/* inherits documentation from base class */
void QCPBars::draw(QPainter *painter) const
{
  if (!mVisible || mData->isEmpty()) return;
  painter->setClipRect(mKeyAxis->axisRect().united(mValueAxis->axisRect()));
  
  QCPBarDataMap::const_iterator it;
  for (it = mData->constBegin(); it != mData->constEnd(); ++it)
  {
    if (it.key()+mWidth*0.5 < mKeyAxis->range().lower || it.key()-mWidth*0.5 > mKeyAxis->range().upper)
      continue;
    QPolygonF barPolygon = getBarPolygon(it.key(), it.value().value);
    // draw bar fill:
    if (mBrush.style() != Qt::NoBrush && mBrush.color().alpha() != 0)
    {
      painter->setRenderHint(QPainter::Antialiasing, mParentPlot->antialiasedElements().testFlag(QCustomPlot::aeFills));
      painter->setPen(Qt::NoPen);
      painter->setBrush(mBrush);
      painter->drawPolygon(barPolygon);
    }
    // draw bar line:
    if (mPen.style() != Qt::NoPen && mPen.color().alpha() != 0)
    {
      painter->setRenderHint(QPainter::Antialiasing, mParentPlot->antialiasedElements().testFlag(QCustomPlot::aeGraphs));
      painter->setPen(mPen);
      painter->setBrush(Qt::NoBrush);
      painter->drawPolyline(barPolygon);
    }
  }
}

/* inherits documentation from base class */
void QCPBars::drawLegendIcon(QPainter *painter, const QRect &rect) const
{
  // draw filled rect:
  painter->setBrush(mBrush);
  painter->setPen(mPen);
  painter->setRenderHint(QPainter::Antialiasing, mParentPlot->antialiasedElements().testFlag(QCustomPlot::aeGraphs));
  QRect r = QRect(0, 0, rect.width()*0.67, rect.height()*0.67);
  r.moveCenter(rect.center());
  painter->drawRect(r);
}

/*! \internal
  
  Returns the polygon of a single bar with \a key and \a value. The Polygon is open at the bottom
  and shifted according to the bar stacking (see \ref moveAbove).
*/
QPolygonF QCPBars::getBarPolygon(double key, double value) const
{
  QPolygonF result;
  double baseValue = getBaseValue(key, value >= 0); // calculates base value dependant on stacking
  result << coordsToPixels(key-mWidth*0.5, baseValue);
  result << coordsToPixels(key-mWidth*0.5, baseValue+value);
  result << coordsToPixels(key+mWidth*0.5, baseValue+value);
  result << coordsToPixels(key+mWidth*0.5, baseValue);
  return result;
}

/*! \internal
  
  This function is called to find at which value to start drawing the base of a bar at \a key, when
  it is stacked ontop of another QCPBars (e.g. with \ref moveAbove).
  
  positive and negative bars are separated per stack (positive are stacked above 0-value upwards,
  negative are stacked below 0-value downwards). This can be indicated with \a positive. So if the
  bar for which we need the base value is negative, set \a positive to false.
*/
double QCPBars::getBaseValue(double key, bool positive) const
{
  if (mBarBelow)
  {
    double max = 0;
    // find bars of mBarBelow that are approximately at key and find largest one:
    QCPBarDataMap::const_iterator it = mBarBelow->mData->lowerBound(key-mWidth*0.1);
    QCPBarDataMap::const_iterator itEnd = mBarBelow->mData->upperBound(key+mWidth*0.1);
    while (it != itEnd)
    {
      if ((positive && it.value().value > max) ||
          (!positive && it.value().value < max))
        max = it.value().value;
      ++it;
    }
    // recurse down the bar-stack to find the total height:
    return max + mBarBelow->getBaseValue(key, positive);
  } else
    return 0;
}

/*! \internal

  Connects \a below and \a above to each other via their mBarAbove/mBarBelow properties.
  The bar(s) currently below lower and upper will become disconnected to lower/upper.
  
  If lower is zero, upper will be disconnected at the bottom.
  If upper is zero, lower will be disconnected at the top.
*/
void QCPBars::connectBars(QCPBars *lower, QCPBars *upper)
{
  if (!lower && !upper) return;
  
  if (!lower) // disconnect upper at bottom
  {
    // disconnect old bar below upper:
    if (upper->mBarBelow && upper->mBarBelow->mBarAbove == upper)
      upper->mBarBelow->mBarAbove = 0;
    upper->mBarBelow = 0;
  } else if (!upper) // disconnect lower at top
  {
    // disconnect old bar above lower:
    if (lower->mBarAbove && lower->mBarAbove->mBarBelow == lower)
      lower->mBarAbove->mBarBelow = 0;
    lower->mBarAbove = 0;
  } else // connect lower and upper
  {
    // disconnect old bar above lower:
    if (lower->mBarAbove && lower->mBarAbove->mBarBelow == lower)
      lower->mBarAbove->mBarBelow = 0;
    // disconnect old bar below upper:
    if (upper->mBarBelow && upper->mBarBelow->mBarAbove == upper)
      upper->mBarBelow->mBarAbove = 0;
    lower->mBarAbove = upper;
    upper->mBarBelow = lower;
  }
}

/* inherits documentation from base class */
QCPRange QCPBars::getKeyRange(bool &validRange, SignDomain inSignDomain) const
{
  QCPRange range;
  bool haveLower = false;
  bool haveUpper = false;
  
  double current;
  double barWidthHalf = mWidth*0.5;
  QCPBarDataMap::const_iterator it = mData->constBegin();
  while (it != mData->constEnd())
  {
    current = it.value().key;
    if (inSignDomain == sdBoth || (inSignDomain == sdNegative && current+barWidthHalf < 0) || (inSignDomain == sdPositive && current-barWidthHalf > 0))
    {
      if (current-barWidthHalf < range.lower || !haveLower)
      {
        range.lower = current-barWidthHalf;
        haveLower = true;
      }
      if (current+barWidthHalf > range.upper || !haveUpper)
      {
        range.upper = current+barWidthHalf;
        haveUpper = true;
      }
    }
    ++it;
  }
  
  validRange = haveLower && haveUpper;
  return range;
}

/* inherits documentation from base class */
QCPRange QCPBars::getValueRange(bool &validRange, SignDomain inSignDomain) const
{
  QCPRange range;
  bool haveLower = true; // set to true, because 0 should always be visible in bar charts
  bool haveUpper = true; // set to true, because 0 should always be visible in bar charts
  
  double current;
  
  QCPBarDataMap::const_iterator it = mData->constBegin();
  while (it != mData->constEnd())
  {
    current = it.value().value + getBaseValue(it.value().key, it.value().value >= 0);
    if (inSignDomain == sdBoth || (inSignDomain == sdNegative && current < 0) || (inSignDomain == sdPositive && current > 0))
    {
      if (current < range.lower || !haveLower)
      {
        range.lower = current;
        haveLower = true;
      }
      if (current > range.upper || !haveUpper)
      {
        range.upper = current;
        haveUpper = true;
      }
    }
    ++it;
  }
  
  validRange = !qFuzzyCompare(range.lower+1.0, range.upper+1.0);
  return range;
}


// ================================================================================
// =================== QCPStatisticalBox
// ================================================================================
/*! \class QCPStatisticalBox
  \brief A plottable representing a single statistical box in a plot.

  To plot data, assign it with the individual parameter functions or use \ref setData to set all
  parameters at once. The individual funcions are:
  \li \ref setMinimum
  \li \ref setLowerQuartile
  \li \ref setMedian
  \li \ref setUpperQuartile
  \li \ref setMaximum
  
  Additionally you can define a list of outliers, drawn as circle datapoints:
  \li \ref setOutliers
  
  \section appearance Changing the appearance
  
  The appearance of the box itself is controlled via \ref setPen and \ref setBrush. You
  may change the width of the box with \ref setWidth in plot coordinates (not pixels).

  Analog functions exist for the minimum/maximum-whiskers: \ref setWhiskerPen, \ref
  setWhiskerBarPen, \ref setWhiskerWidth. The whisker width is the width of the bar at the top
  (maximum) or bottom (minimum).
  
  The median indicator line has its own pen, \ref setMedianPen.
  
  If the pens are changed, especially the median and whisker pen, make sure to set the capStyle to
  Qt::FlatCap. Else, e.g. the median line might exceed the quartile box by a few pixels due to the
  pen cap being not perfectly flat.
  
  The Outlier data points are drawn as circles. Their look can be controlled with \ref
  setOutlierPen and \ref setOutlierBrush. The size (diameter) can be set with \ref setOutlierSize
  in pixels.
  
  \section usage Usage
  
  Like all data representing objects in QCustomPlot, the QCPStatisticalBox is a plottable
  (QCPAbstractPlottable). So the plottable-interface of QCustomPlot applies
  (QCustomPlot::plottable, QCustomPlot::addPlottable, QCustomPlot::removePlottable, etc.)
  
  Usually, you first create an instance:
  \code
  QCPStatisticalBox *newBox = new QCPStatisticalBox(customPlot->xAxis, customPlot->yAxis);\endcode
  add it to the customPlot with QCustomPlot::addPlottable:
  \code
  customPlot->addPlottable(newBox);\endcode
  and then modify the properties of the newly created plottable, e.g.:
  \code
  newBox->setName("Measurement Series 1");
  newBox->setData(1, 3, 4, 5, 7);
  newBox->setOutliers(QVector<double>() << 0.5 << 0.64 << 7.2 << 7.42);\endcode
*/

/*!
  Constructs a statistical box which uses \a keyAxis as its key axis ("x") and \a valueAxis as its
  value axis ("y"). \a keyAxis and \a valueAxis must both reside in the same QCustomPlot.
  
  The constructed statistical box can be added to the plot with QCustomPlot::addPlottable,
  QCustomPlot then takes ownership of the statistical box.
*/
QCPStatisticalBox::QCPStatisticalBox(QCPAxis *keyAxis, QCPAxis *valueAxis) :
  QCPAbstractPlottable(keyAxis, valueAxis),
  mKey(0),
  mMinimum(0),
  mLowerQuartile(0),
  mMedian(0),
  mUpperQuartile(0),
  mMaximum(0)
{
  QPen whiskerPen;
  whiskerPen.setStyle(Qt::DashLine);
  whiskerPen.setCapStyle(Qt::FlatCap);
  setWhiskerPen(whiskerPen);
  setWhiskerWidth(0.2);
  
  QPen medianPen;
  medianPen.setWidthF(3);
  medianPen.setCapStyle(Qt::FlatCap);
  setMedianPen(medianPen);
  
  setBrush(Qt::NoBrush);
  setWidth(0.5);
  
  QPen outlierPen;
  outlierPen.setColor(Qt::blue);
  setOutlierPen(outlierPen);
  setOutlierBrush(Qt::NoBrush);
  setOutlierSize(5);
}

QCPStatisticalBox::~QCPStatisticalBox()
{
}

/*!
  Sets the key coordinate of the statistical box.
*/
void QCPStatisticalBox::setKey(double key)
{
  mKey = key;
}

/*!
  Sets the parameter "minimum" of the statistical box plot. This is the position of the lower
  whisker, typically the minimum measurement of the sample that's not considered an outlier.
  
  \see setMaximum, setWhiskerPen, setWhiskerBarPen, setWhiskerWidth
*/
void QCPStatisticalBox::setMinimum(double value)
{
  mMinimum = value;
}

/*!
  Sets the parameter "lower Quartile" of the statistical box plot. This is the lower end of the
  box. The lower and the upper quartiles are the two statistical quartiles around the median of the
  sample, they contain 50% of the sample data.
  
  \see setUpperQuartile, setPen, setBrush, setWidth
*/
void QCPStatisticalBox::setLowerQuartile(double value)
{
  mLowerQuartile = value;
}

/*!
  Sets the parameter "median" of the statistical box plot. This is the value of the median mark
  inside the quartile box. The median separates the sample data in half (50% of the sample data is
  below/above the median).
  
  \see setMedianPen
*/
void QCPStatisticalBox::setMedian(double value)
{
  mMedian = value;
}

/*!
  Sets the parameter "upper Quartile" of the statistical box plot. This is the upper end of the
  box. The lower and the upper quartiles are the two statistical quartiles around the median of the
  sample, they contain 50% of the sample data.
  
  \see setLowerQuartile, setPen, setBrush, setWidth
*/
void QCPStatisticalBox::setUpperQuartile(double value)
{
  mUpperQuartile = value;
}

/*!
  Sets the parameter "maximum" of the statistical box plot. This is the position of the upper
  whisker, typically the maximum measurement of the sample that's not considered an outlier.
  
  \see setMinimum, setWhiskerPen, setWhiskerBarPen, setWhiskerWidth
*/
void QCPStatisticalBox::setMaximum(double value)
{
  mMaximum = value;
}

/*!
  Sets a vector of outlier values that will be drawn as circles. Any data points in the sample that
  are not within the whiskers (\ref setMinimum, \ref setMaximum) should be considered outliers and
  displayed as such.
  
  \see setOutlierPen, setOutlierBrush, setOutlierSize
*/
void QCPStatisticalBox::setOutliers(const QVector<double> &values)
{
  mOutliers = values;
}

/*!
  Sets all parameters of the statistical box plot at once.
  
  \see setKey, setMinimum, setLowerQuartile, setMedian, setUpperQuartile, setMaximum
*/
void QCPStatisticalBox::setData(double key, double minimum, double lowerQuartile, double median, double upperQuartile, double maximum)
{
  setKey(key);
  setMinimum(minimum);
  setLowerQuartile(lowerQuartile);
  setMedian(median);
  setUpperQuartile(upperQuartile);
  setMaximum(maximum);
}

/*!
  Sets the width of the box in key coordinates.
  
  \see setWhiskerWidth
*/
void QCPStatisticalBox::setWidth(double width)
{
  mWidth = width;
}

/*!
  Sets the width of the whiskers (\ref setMinimum, \ref setMaximum) in key coordinates.
  
  \see setWidth
*/
void QCPStatisticalBox::setWhiskerWidth(double width)
{
  mWhiskerWidth = width;
}

/*!
  Sets the pen used for drawing the whisker backbone (That's the line parallel to the value axis).
  
  Make sure to set the \a pen capStyle to Qt::FlatCap to prevent the backbone from reaching a few
  pixels past the bars, when using a non-zero pen width.
  
  \see setWhiskerBarPen
*/
void QCPStatisticalBox::setWhiskerPen(const QPen &pen)
{
  mWhiskerPen = pen;
}

/*!
  Sets the pen used for drawing the whisker bars (Those are the lines parallel to the key axis at
  each end of the backbone).
  
  \see setWhiskerPen
*/
void QCPStatisticalBox::setWhiskerBarPen(const QPen &pen)
{
  mWhiskerBarPen = pen;
}

/*!
  Sets the pen used for drawing the median indicator line inside the statistical box.
  
  Make sure to set the \a pen capStyle to Qt::FlatCap to prevent the median line from reaching a
  few pixels outside the box, when using a non-zero pen width.
*/
void QCPStatisticalBox::setMedianPen(const QPen &pen)
{
  mMedianPen = pen;
}

/*!
  Sets the pixel size (diameter) of the circles that represent the outliers.
  
  \see setOutlierPen, setOutlierBrush, setOutliers
*/
void QCPStatisticalBox::setOutlierSize(double pixels)
{
  mOutlierSize = pixels;
}

/*!
  Sets the pen used to draw the outlier circles.
  
  \see setOutlierSize, setOutlierBrush, setOutliers
*/
void QCPStatisticalBox::setOutlierPen(const QPen &pen)
{
  mOutlierPen = pen;
}

/*!
  Sets the brush used to fill the outlier circles.
  
  \see setOutlierSize, setOutlierPen, setOutliers
*/
void QCPStatisticalBox::setOutlierBrush(const QBrush &brush)
{
  mOutlierBrush = brush;
}

/* inherits documentation from base class */
void QCPStatisticalBox::clearData()
{
  setOutliers(QVector<double>());
  setKey(0);
  setMinimum(0);
  setLowerQuartile(0);
  setMedian(0);
  setUpperQuartile(0);
  setMaximum(0);
}

/* inherits documentation from base class */
void QCPStatisticalBox::draw(QPainter *painter) const
{
  if (!mVisible) return;
  painter->setClipRect(mKeyAxis->axisRect().united(mValueAxis->axisRect()));
  
  painter->setRenderHint(QPainter::Antialiasing, mParentPlot->antialiasedElements().testFlag(QCustomPlot::aeGraphs));
  drawQuartileBox(painter);
  drawMedian(painter);
  drawWhiskers(painter);
  drawOutliers(painter);
}

/* inherits documentation from base class */
void QCPStatisticalBox::drawLegendIcon(QPainter *painter, const QRect &rect) const
{
  // draw filled rect:
  painter->setRenderHint(QPainter::Antialiasing, mParentPlot->antialiasedElements().testFlag(QCustomPlot::aeGraphs));
  painter->setPen(mPen);
  painter->setBrush(mBrush);
  QRect r = QRect(0, 0, rect.width()*0.67, rect.height()*0.67);
  r.moveCenter(rect.center());
  painter->drawRect(r);
}

/*! \internal
  
  Draws the quartile box.
*/
void QCPStatisticalBox::drawQuartileBox(QPainter *painter) const
{
  QRectF box;
  box.setTopLeft(coordsToPixels(mKey-mWidth*0.5, mUpperQuartile));
  box.setBottomRight(coordsToPixels(mKey+mWidth*0.5, mLowerQuartile));
  painter->setPen(mPen);
  painter->setBrush(mBrush);
  painter->drawRect(box);
}

/*! \internal
  
  Draws the median line inside the quartile box.
*/
void QCPStatisticalBox::drawMedian(QPainter *painter) const
{
  QLineF medianLine;
  medianLine.setP1(coordsToPixels(mKey-mWidth*0.5, mMedian));
  medianLine.setP2(coordsToPixels(mKey+mWidth*0.5, mMedian));
  painter->setPen(mMedianPen);
  painter->drawLine(medianLine);
}

/*! \internal
  
  Draws both whisker backbones and bars.
*/
void QCPStatisticalBox::drawWhiskers(QPainter *painter) const
{
  QLineF backboneMin, backboneMax, barMin, barMax;
  backboneMax.setPoints(coordsToPixels(mKey, mUpperQuartile), coordsToPixels(mKey, mMaximum));
  backboneMin.setPoints(coordsToPixels(mKey, mLowerQuartile), coordsToPixels(mKey, mMinimum));
  barMax.setPoints(coordsToPixels(mKey-mWhiskerWidth*0.5, mMaximum), coordsToPixels(mKey+mWhiskerWidth*0.5, mMaximum));
  barMin.setPoints(coordsToPixels(mKey-mWhiskerWidth*0.5, mMinimum), coordsToPixels(mKey+mWhiskerWidth*0.5, mMinimum));
  painter->setPen(mWhiskerPen);
  painter->drawLine(backboneMin);
  painter->drawLine(backboneMax);
  painter->setPen(mWhiskerBarPen);
  painter->drawLine(barMin);
  painter->drawLine(barMax);
}

/*! \internal
  
  Draws the outlier circles.
*/
void QCPStatisticalBox::drawOutliers(QPainter *painter) const
{
  painter->setPen(mOutlierPen);
  painter->setBrush(mOutlierBrush);
  for (int i=0; i<mOutliers.size(); ++i)
    painter->drawEllipse(coordsToPixels(mKey, mOutliers.at(i)), mOutlierSize*0.5, mOutlierSize*0.5);
}

/* inherits documentation from base class */
QCPRange QCPStatisticalBox::getKeyRange(bool &validRange, SignDomain inSignDomain) const
{
  validRange = mWidth > 0;
  if (inSignDomain == sdBoth)
  {
    return QCPRange(mKey-mWidth*0.5, mKey+mWidth*0.5);
  } else if (inSignDomain == sdNegative)
  {
    if (mKey+mWidth*0.5 < 0)
      return QCPRange(mKey-mWidth*0.5, mKey+mWidth*0.5);
    else if (mKey < 0)
      return QCPRange(mKey-mWidth*0.5, mKey);
    else
    {
      validRange = false;
      return QCPRange();
    }
  } else if (inSignDomain == sdPositive)
  {
    if (mKey-mWidth*0.5 > 0)
      return QCPRange(mKey-mWidth*0.5, mKey+mWidth*0.5);
    else if (mKey > 0)
      return QCPRange(mKey, mKey+mWidth*0.5);
    else
    {
      validRange = false;
      return QCPRange();
    }
  }
  validRange = false;
  return QCPRange();
}

/* inherits documentation from base class */
QCPRange QCPStatisticalBox::getValueRange(bool &validRange, SignDomain inSignDomain) const
{
  if (inSignDomain == sdBoth)
  {
    double lower = qMin(mMinimum, qMin(mMedian, mLowerQuartile));
    double upper = qMax(mMaximum, qMax(mMedian, mUpperQuartile));
    for (int i=0; i<mOutliers.size(); ++i)
    {
      if (mOutliers.at(i) < lower)
        lower = mOutliers.at(i);
      if (mOutliers.at(i) > upper)
        upper = mOutliers.at(i);
    }
    validRange = upper > lower;
    return QCPRange(lower, upper);
  } else
  {
    QVector<double> values; // values that must be considered (i.e. all outliers and the five box-parameters)
    values.reserve(mOutliers.size() + 5);
    values << mMaximum << mUpperQuartile << mMedian << mLowerQuartile << mMinimum;
    values << mOutliers;
    // go through values and find the ones in legal range:
    bool haveUpper = false;
    bool haveLower = false;
    double upper = 0;
    double lower = 0;
    for (int i=0; i<values.size(); ++i)
    {
      if ((inSignDomain == sdNegative && values.at(i) < 0) ||
          (inSignDomain == sdPositive && values.at(i) > 0))
      {
        if (values.at(i) > upper || !haveUpper)
        {
          upper = values.at(i);
          haveUpper = true;
        }
        if (values.at(i) < lower || !haveLower)
        {
          lower = values.at(i);
          haveLower = true;
        }
      }
    }
    // return the bounds if we found some sensible values:
    if (haveLower && haveUpper && !qFuzzyCompare(upper+1.0, lower+1.0))
    {
      validRange = true;
      return QCPRange(lower, upper);
    } else
    {
      validRange = false;
      return QCPRange();
    }
  }
}














