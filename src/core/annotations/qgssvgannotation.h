/***************************************************************************
                              qgssvgannotation.h
                              ------------------------
  begin                : November, 2012
  copyright            : (C) 2012 by Marco Hugentobler
  email                : marco dot hugentobler at sourcepole dot ch
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QGSSVGANNOTATION_H
#define QGSSVGANNOTATION_H

#include "qgsannotation.h"
#include <QSvgRenderer>
#include "qgis_core.h"

/**
 * \class QgsSvgAnnotation
 * \ingroup core
 * An annotation which renders the contents of an SVG file.
 * \note added in QGIS 3.0
 */
class CORE_EXPORT QgsSvgAnnotation: public QgsAnnotation
{
    Q_OBJECT

  public:

    /**
     * Constructor for QgsSvgAnnotation.
     */
    QgsSvgAnnotation( QObject* parent = nullptr );

    virtual void writeXml( QDomElement& elem, QDomDocument & doc ) const override;
    virtual void readXml( const QDomElement& itemElem, const QDomDocument& doc ) override;

    /**
     * Sets the file path for the source SVG file.
     * @see filePath()
     */
    void setFilePath( const QString& file );

    /**
     * Returns the file path for the source SVG file.
     * @see setFilePath()
     */
    QString filePath() const { return mFilePath; }

    /**
     * Returns a new QgsSvgAnnotation object.
     */
    static QgsSvgAnnotation* create() { return new QgsSvgAnnotation(); }

  protected:

    void renderAnnotation( QgsRenderContext& context, QSizeF size ) const override;

  private:
    mutable QSvgRenderer mSvgRenderer; // QSvgRenderer::render methods are not const?
    QString mFilePath;
};

#endif // QGSSVGANNOTATION_H
