//
// C++ Implementation: CommandLineParser
//
// Description: 
//
//
// Author: Barth Netterfield <netterfield@physics.utoronto.ca>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "commandlineparser.h"
#include "plotitem.h"

#include <iostream>
#include <QCoreApplication>
#include "kst_i18n.h"

namespace Kst {

CommandLineParser::CommandLineParser():
      _doAve(false), _doSkip(false), _doConsecutivePlots(true), _useBargraph(false), 
      _useLines(true), _usePoints(false), _sampleRate(1.0), _numFrames(-1), _startFrame(0),
      _skip(0), _plotName(), _errorField(), _fileName(), _xField() {
  
  Q_ASSERT(QCoreApplication::instance());
  _arguments = QCoreApplication::instance()->arguments();
  _arguments.takeFirst(); //appname

  _fileNames.clear();
}


CommandLineParser::~CommandLineParser() {
}

void CommandLineParser::usage(QString Message) {
  //FIXME: proper printing and exiting!
  std::cerr << Message.latin1();
exit(0);
  std::cerr <<
  i18n("KST Command Line Usage\n"
"Load a kst file:\n"
"kst [OPTIONS] kstfile\n"
"\n"
"[OPTIONS] will override the datasource parameters for all data sources in the kst file:\n"
"      -F  <datasource>\n"
"      -f  <startframe>\n"
"      -n  <numframes>\n"
"      -s  <frames per sample>\n"
"      -a                     (apply averaging filter: requires -s)\n"
"Read a data file:\n"
"kst datasource OPTIONS [datasource OPTIONS []]\n"
"\n"
"OPTIONS are read and interpreted in order. Except for data object options, all are applied to all future data objects, unless later overridden.\n"
"File Options:\n"
"      -f <startframe>          default: 0.  'end' counts from end.\n"
"      -n <numframes>           default: to end of file ('eof')\n"
"      -s <frames per sample>   default: 0 (read every sample)\n"
"      -a                       apply averaging filter: requires -s\n"
"Position:\n"
"      -P <plot name>:          Place curves in one plot.\n"
"      -A                       Place future curves in individual plots.\n"
"Appearance\n"
"      -d:                      use points\n"
"      -l:                      use lines (default)\n"
"      -b:                      use bargraph\n"
"Data Object Modifiers\n"
"      -x <field>:              Create vector and use as X vector for curves.\n"
"      -e <field>:              Create vector and use as Y-error vector for curves.\n"
"      -r <rate>:               sample rate (spectra & spectograms).\n"
"Data Objects:\n"
"      -y <field>               plot an XY curve of field.\n"
"      -p <field>               plot the spectrum of field.\n"
"      -h <field>               plot a histogram of field.\n"
"      -z <field>               plot an image of matrix field.\n"
"Examples:\n"
"Data sources and fields\n"
"Plot all data in column 2 from data.dat.\n"
"       kst data.dat -y 2\n"
"\n"
"Same as above, except only read 20 lines, starting at line 10.\n"
"       kst data.dat -f 10 -n 20 -y 2\n"
"\n"
"...also read col 1. One plot per curve.\n"
"       kst data.dat -f 10 -n 20 -y 1 -y 2\n"
"\n"
"...read col 1 from data2.dat and col 1 from data.dat\n"
"       kst data.dat -f 10 -n 20 -y 2 data2.dat -y 1\n"
"\n"
"...same as above, except read 40 lines starting at 30 in data2.dat\n"
"       kst data.dat -f 10 -n 20 -y 2 data2.dat -f 30 -n 40 -y 1\n"
"Specify the X vector and error bars.\n"
"Plot x = col 1 and Y = col 2 and error flags = col 3 from data.dat\n"
"       kst data.dat -x 1 -e 3 -y 2\n"
"\n"
"Get the X vector from data1.dat, and the Y vector from data2.dat.\n"
"       kst data1.dat -x 1 data2.dat -y 1\n"
"Placement:\n"
"Plot column 2 and column 3 in plot P1 and column 4 in plot P2\n"
"       kst data.dat -P P1 -y 2 -y 3 -P P2 -y 4\n"
      ).latin1();
  exit(0);
}

void CommandLineParser::_setIntArg(int *arg, QString Message) {
  QString param;
  bool ok = true;
  
  if (_arguments.count()> 0) {
    param = _arguments.takeFirst();
    *arg = param.toInt(&ok);
  } else {
    ok=false;
  }
  if (!ok) usage(Message);

}

void CommandLineParser::_setDoubleArg(double *arg, QString Message) {
  QString param;
  bool ok = true;
  
  if (_arguments.count()> 0) {
    param = _arguments.takeFirst();
    *arg = param.toDouble(&ok);
  } else {
    ok=false;
  }
  if (!ok) usage(Message);
}

void CommandLineParser::_setStringArg(QString &arg, QString Message) {
  bool ok = true;

  if (_arguments.count()> 0) {
    arg = _arguments.takeFirst();
  } else {
    ok=false;
  }
  if (!ok) usage(Message);

}

bool CommandLineParser::processCommandLine() {
  QString arg, param;
  bool ok=true;
  bool first_plot = true;
  PlotItem *plotItem = 0;

  while (1) {
    if (_arguments.count()<1) break;

    arg = _arguments.takeFirst();
    ok = true;
    if ((arg == "--help")||(arg == "-help") || (arg=="-h")) {
      usage();
    } else if (arg == "-f") {
      _setIntArg(&_startFrame, i18n("Usage: -f <startframe>\n"));
    } else if (arg == "-n") {
      _setIntArg(&_numFrames, i18n("Usage: -f <numframes>\n"));
    } else if (arg == "-s") {
      _setIntArg(&_skip, i18n("Usage: -s <frames per sample>\n"));
    } else if (arg == "-a") {
      _doAve = true;
    } else if (arg == "-P") {
      QString plot_name;
      _setStringArg(plot_name,i18n("Usage: -P <plotname>\n"));

      CreatePlotForCurve *cmd = new CreatePlotForCurve(true,true);
      cmd->createItem();
      if (!first_plot) {
        plotItem->update();
      }
      plotItem = static_cast<PlotItem*>(cmd->item());
      plotItem->setName(plot_name);
      first_plot=false;
      _doConsecutivePlots=false;
    } else if (arg == "-A") {
      _doConsecutivePlots = true;
    } else if (arg == "-d") {
      _useBargraph=false;
      _useLines = false;
      _usePoints = true;
    } else if (arg == "-l") {
      _useBargraph=false;
      _useLines = true;
      _usePoints = false;
    } else if (arg == "-b") {
      _useBargraph=true;
      _useLines = false;
      _usePoints = false;
    } else if (arg == "-x") {
      _setStringArg(_xField,i18n("Usage: -x <xfieldname>\n"));
      //FIXME: now create the XVector
    } else if (arg == "-e") {
      _setStringArg(_errorField,i18n("Usage: -P <errorfieldname>\n"));
      //FIXME: now create the error field!
    } else if (arg == "-r") {
      _setDoubleArg(&_sampleRate,i18n("Usage: -r <samplerate>\n"));
    } else if (arg == "-y") {
      QString field;
      _setStringArg(field,i18n("Usage: -y <fieldname>\n"));
      //FIXME: Create the YVector, and the curve
    } else if (arg == "-p") {
      QString field;
      _setStringArg(field,i18n("Usage: -p <fieldname>\n"));
      //FIXME: Create the Vector, and the psd
    } else if (arg == "-h") {
      QString field;
      _setStringArg(field,i18n("Usage: -h <fieldname>\n"));
      //FIXME: Create the Vector, and the histogram
    } else if (arg == "-z") {
      QString field;
      _setStringArg(field,i18n("Usage: -z <fieldname>\n"));
      //FIXME: Create the matrix, and the image
    }
  }
  return (true);
}

}
