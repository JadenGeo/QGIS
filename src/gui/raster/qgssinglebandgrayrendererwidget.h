/***************************************************************************
                         qgssinglebandgrayrendererwidget.h
                         ---------------------------------
    begin                : March 2012
    copyright            : (C) 2012 by Marco Hugentobler
    email                : marco at sourcepole dot ch
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QGSSINGLEBANDGRAYRENDERERWIDGET_H
#define QGSSINGLEBANDGRAYRENDERERWIDGET_H

#include "qgsrasterrendererwidget.h"
#include "ui_qgssinglebandgrayrendererwidgetbase.h"
#include "qgis_gui.h"

class QgsRasterMinMaxWidget;

/** \ingroup gui
 * \class QgsSingleBandGrayRendererWidget
 */
class GUI_EXPORT QgsSingleBandGrayRendererWidget: public QgsRasterRendererWidget, private Ui::QgsSingleBandGrayRendererWidgetBase
{
    Q_OBJECT
  public:
    QgsSingleBandGrayRendererWidget( QgsRasterLayer* layer, const QgsRectangle &extent = QgsRectangle() );

    static QgsRasterRendererWidget* create( QgsRasterLayer* layer, const QgsRectangle &extent ) { return new QgsSingleBandGrayRendererWidget( layer, extent ); }

    QgsRasterRenderer* renderer() override;
    void setMapCanvas( QgsMapCanvas* canvas ) override;

    void setFromRenderer( const QgsRasterRenderer* r );

    QString min( int index = 0 ) override { Q_UNUSED( index ); return mMinLineEdit->text(); }
    QString max( int index = 0 ) override { Q_UNUSED( index ); return mMaxLineEdit->text(); }
    void setMin( const QString& value, int index = 0 ) override;
    void setMax( const QString& value, int index = 0 ) override;
    int selectedBand( int index = 0 ) override { Q_UNUSED( index ); return mGrayBandComboBox->currentIndex() + 1; }
    void doComputations() override;
    QgsRasterMinMaxWidget* minMaxWidget() override { return mMinMaxWidget; }

  public slots:
    //! called when new min/max values are loaded
    void loadMinMax( int bandNo, double min, double max );

  private slots:
    void on_mGrayBandComboBox_currentIndexChanged( int index );
    void on_mMinLineEdit_textChanged( const QString & );
    void on_mMaxLineEdit_textChanged( const QString & );

  private:
    QgsRasterMinMaxWidget * mMinMaxWidget = nullptr;
    bool mDisableMinMaxWidgetRefresh;

    void minMaxModified();
};

#endif // QGSSINGLEBANDGRAYRENDERERWIDGET_H
