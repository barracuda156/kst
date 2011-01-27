/***************************************************************************
                       asciiplugin.h
                             -------------------
    begin                : Fri Oct 17 2003
    copyright            : (C) 2003 The University of Toronto
     email                : netterfield@astro.utoronto.ca
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ASCII_PLUGIN_H
#define ASCII_PLUGIN_H


#include "asciisource.h"
#include "dataplugin.h"

#include "ui_asciiconfig.h"


class AsciiPlugin : public QObject, public Kst::DataSourcePluginInterface
{
    Q_OBJECT
    Q_INTERFACES(Kst::DataSourcePluginInterface)

  public:
    virtual ~AsciiPlugin() {}

    virtual QString pluginName() const;
    virtual QString pluginDescription() const;

    virtual bool hasConfigWidget() const { return true; }

    virtual Kst::DataSource *create(Kst::ObjectStore *store,
                                  QSettings *cfg,
                                  const QString &filename,
                                  const QString &type,
                                  const QDomElement &element) const;

    virtual QStringList matrixList(QSettings *cfg,
                                  const QString& filename,
                                  const QString& type,
                                  QString *typeSuggestion,
                                  bool *complete) const;

    virtual QStringList fieldList(QSettings *cfg,
                                  const QString& filename,
                                  const QString& type,
                                  QString *typeSuggestion,
                                  bool *complete) const;

    virtual QStringList scalarList(QSettings *cfg,
                                  const QString& filename,
                                  const QString& type,
                                  QString *typeSuggestion,
                                  bool *complete) const;

    virtual QStringList stringList(QSettings *cfg,
                                  const QString& filename,
                                  const QString& type,
                                  QString *typeSuggestion,
                                  bool *complete) const;

    virtual int understands(QSettings *cfg, const QString& filename) const;

    virtual bool supportsTime(QSettings *cfg, const QString& filename) const;

    virtual QStringList provides() const;

    virtual Kst::DataSourceConfigWidget *configWidget(QSettings *cfg, const QString& filename) const;
};


class ConfigWidgetAsciiInternal : public QWidget, public Ui_AsciiConfig
{
  Q_OBJECT

  public:
    ConfigWidgetAsciiInternal(QWidget *parent);

    AsciiSourceConfig config();
    void setConfig(const AsciiSourceConfig&);

  private Q_SLOTS:
    void columnLayoutChanged(int);

  private:
    const int _index_offset;

};

#endif
// vim: ts=2 sw=2 et
