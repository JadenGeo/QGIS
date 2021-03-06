/***************************************************************************
 *   Copyright (C) 2003 by Tim Sutton                                      *
 *   Copyright (C) 2004 by Gary Sherman                                    *
 *   tim@linfiniti.com                                                     *
 *                                                                         *
 *   This is a plugin generated from the QGIS plugin template              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef QGSDELIMITEDTEXTSOURCESELECT_H
#define QGSDELIMITEDTEXTSOURCESELECT_H

#include "ui_qgsdelimitedtextsourceselectbase.h"

#include <QTextStream>
#include "qgshelp.h"
#include "qgisgui.h"

class QButtonGroup;
class QgisInterface;
class QgsDelimitedTextFile;

/**
 * \class QgsDelimitedTextSourceSelect
 */
class QgsDelimitedTextSourceSelect : public QDialog, private Ui::QgsDelimitedTextSourceSelectBase
{
    Q_OBJECT

  public:
    QgsDelimitedTextSourceSelect( QWidget * parent, Qt::WindowFlags fl = QgisGui::ModalDialogFlags, bool embedded = false );
    ~QgsDelimitedTextSourceSelect();

    QStringList splitLine( QString line );

  private:
    bool loadDelimitedFileDefinition();
    void updateFieldLists();
    void getOpenFileName();
    QString selectedChars();
    void setSelectedChars( const QString& delimiters );
    void loadSettings( const QString& subkey = QString(), bool loadGeomSettings = true );
    void saveSettings( const QString& subkey = QString(), bool saveGeomSettings = true );
    void loadSettingsForFile( const QString& filename );
    void saveSettingsForFile( const QString& filename );
    bool trySetXYField( QStringList &fields, QList<bool> &isValidNumber, const QString& xname, const QString& yname );

  private:
    QgsDelimitedTextFile *mFile = nullptr;
    int mExampleRowCount;
    int mBadRowCount;
    QString mPluginKey;
    QString mLastFileType;
    QButtonGroup *bgFileFormat = nullptr;
    QButtonGroup *bgGeomType = nullptr;

  private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_buttonBox_helpRequested()
    {
      QgsHelp::openHelp( QStringLiteral( "working_with_vector/supported_data.html#delimited-text-files" ) );
    }
    void on_btnBrowseForFile_clicked();

  public slots:
    void updateFileName();
    void updateFieldsAndEnable();
    void enableAccept();
    bool validate();

  signals:
    void addVectorLayer( const QString&, const QString&, const QString& );
};

#endif // QGSDELIMITEDTEXTSOURCESELECT_H
