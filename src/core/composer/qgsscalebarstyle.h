/***************************************************************************
                            qgsscalebarstyle.h
                            ------------------
    begin                : June 2008
    copyright            : (C) 2008 by Marco Hugentobler
    email                : marco.hugentobler@karto.baug.ethz.ch
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QGSSCALEBARSTYLE_H
#define QGSSCALEBARSTYLE_H

#include <QIcon>
#include <QRectF>

#include "qgis_core.h"

class QgsComposerScaleBar;
class QPainter;

/** \ingroup core
 * Abstraction of composer scale bar style. Subclasses draw themselves, have the
possibility to implement custom labeling and calculate corresponding box size.
*/
class CORE_EXPORT QgsScaleBarStyle
{
  public:
    QgsScaleBarStyle( const QgsComposerScaleBar* bar );
    virtual ~QgsScaleBarStyle() = default;

    /** Draws the style
     @param p painter object
     @param xOffset offset to account for centered labeling*/
    virtual void draw( QPainter* p, double xOffset = 0 ) const = 0; //to do by every subclass
    virtual void drawLabels( QPainter* p ) const; //default implementation provided
    virtual QRectF calculateBoxSize() const; //default implementation provided

    /**
     * Get a name for this style.
     * Needs to be remiplmeented by subclasses.
     */
    virtual QString name() const = 0;

  private:
    QgsScaleBarStyle(); //default constructor forbidden

  protected:
    const QgsComposerScaleBar* mScaleBar = nullptr;
};

#endif
