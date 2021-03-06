/***************************************************************************
                          qgsrectangle.cpp  -  description
                             -------------------
    begin                : Sat Jun 22 2002
    copyright            : (C) 2002 by Gary E.Sherman
    email                : sherman at mrcc.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <algorithm>
#include <cmath>
#include <limits>
#include <QRectF>
#include <QString>
#include <QTextStream>
#include <QTransform>
#include <QRegExp>
#include <qnumeric.h>

#include "qgspoint.h"
#include "qgsrectangle.h"
#include "qgslogger.h"

QgsRectangle::QgsRectangle( double newxmin, double newymin, double newxmax, double newymax )
    : xmin( newxmin )
    , ymin( newymin )
    , xmax( newxmax )
    , ymax( newymax )
{
  normalize();
}

QgsRectangle::QgsRectangle( const QgsPoint& p1, const QgsPoint& p2 )
{
  set( p1, p2 );
}

QgsRectangle::QgsRectangle( QRectF const & qRectF )
{
  xmin = qRectF.topLeft().x();
  ymin = qRectF.topLeft().y();
  xmax = qRectF.bottomRight().x();
  ymax = qRectF.bottomRight().y();
}

QgsRectangle::QgsRectangle( const QgsRectangle &r )
{
  xmin = r.xMinimum();
  ymin = r.yMinimum();
  xmax = r.xMaximum();
  ymax = r.yMaximum();
}


void QgsRectangle::set( const QgsPoint& p1, const QgsPoint& p2 )
{
  xmin = p1.x();
  xmax = p2.x();
  ymin = p1.y();
  ymax = p2.y();
  normalize();
}

void QgsRectangle::set( double xmin_, double ymin_, double xmax_, double ymax_ )
{
  xmin = xmin_;
  ymin = ymin_;
  xmax = xmax_;
  ymax = ymax_;
  normalize();
}

void QgsRectangle::normalize()
{
  if ( isNull() )
    return;

  if ( xmin > xmax )
  {
    std::swap( xmin, xmax );
  }
  if ( ymin > ymax )
  {
    std::swap( ymin, ymax );
  }
} // QgsRectangle::normalize()


void QgsRectangle::setMinimal()
{
  xmin = std::numeric_limits<double>::max();
  ymin = std::numeric_limits<double>::max();
  xmax = -std::numeric_limits<double>::max();
  ymax = -std::numeric_limits<double>::max();
}

void QgsRectangle::scale( double scaleFactor, const QgsPoint * cp )
{
  // scale from the center
  double centerX, centerY;
  if ( cp )
  {
    centerX = cp->x();
    centerY = cp->y();
  }
  else
  {
    centerX = xmin + width() / 2;
    centerY = ymin + height() / 2;
  }
  scale( scaleFactor, centerX, centerY );
}

void QgsRectangle::scale( double scaleFactor, double centerX, double centerY )
{
  double newWidth = width() * scaleFactor;
  double newHeight = height() * scaleFactor;
  xmin = centerX - newWidth / 2.0;
  xmax = centerX + newWidth / 2.0;
  ymin = centerY - newHeight / 2.0;
  ymax = centerY + newHeight / 2.0;
}

void QgsRectangle::grow( double delta )
{
  xmin -= delta;
  xmax += delta;
  ymin -= delta;
  ymax += delta;
}

void QgsRectangle::include( const QgsPoint &p )
{
  if ( p.x() < xMinimum() )
    setXMinimum( p.x() );
  else if ( p.x() > xMaximum() )
    setXMaximum( p.x() );
  if ( p.y() < yMinimum() )
    setYMinimum( p.y() );
  if ( p.y() > yMaximum() )
    setYMaximum( p.y() );
}

QgsRectangle QgsRectangle::buffer( double width )
{
  return QgsRectangle( xmin - width, ymin - width, xmax + width, ymax + width );
}

QgsRectangle QgsRectangle::intersect( const QgsRectangle * rect ) const
{
  QgsRectangle intersection = QgsRectangle();
  //If they don't actually intersect an empty QgsRectangle should be returned
  if ( !rect || !intersects( *rect ) )
  {
    return intersection;
  }

  intersection.setXMinimum( xmin > rect->xMinimum() ? xmin : rect->xMinimum() );
  intersection.setXMaximum( xmax < rect->xMaximum() ? xmax : rect->xMaximum() );
  intersection.setYMinimum( ymin > rect->yMinimum() ? ymin : rect->yMinimum() );
  intersection.setYMaximum( ymax < rect->yMaximum() ? ymax : rect->yMaximum() );
  return intersection;
}

bool QgsRectangle::intersects( const QgsRectangle& rect ) const
{
  double x1 = ( xmin > rect.xmin ? xmin : rect.xmin );
  double x2 = ( xmax < rect.xmax ? xmax : rect.xmax );
  if ( x1 > x2 )
    return false;
  double y1 = ( ymin > rect.ymin ? ymin : rect.ymin );
  double y2 = ( ymax < rect.ymax ? ymax : rect.ymax );
  if ( y1 > y2 )
    return false;
  return true;
}

bool QgsRectangle::contains( const QgsRectangle& rect ) const
{
  return ( rect.xmin >= xmin && rect.xmax <= xmax && rect.ymin >= ymin && rect.ymax <= ymax );
}

bool QgsRectangle::contains( const QgsPoint &p ) const
{
  return xmin <= p.x() && p.x() <= xmax &&
         ymin <= p.y() && p.y() <= ymax;
}

void QgsRectangle::combineExtentWith( const QgsRectangle &rect )
{
  if ( isNull() )
    *this = rect;
  else
  {
    xmin = (( xmin < rect.xMinimum() ) ? xmin : rect.xMinimum() );
    xmax = (( xmax > rect.xMaximum() ) ? xmax : rect.xMaximum() );

    ymin = (( ymin < rect.yMinimum() ) ? ymin : rect.yMinimum() );
    ymax = (( ymax > rect.yMaximum() ) ? ymax : rect.yMaximum() );
  }

}

void QgsRectangle::combineExtentWith( double x, double y )
{
  if ( isNull() )
    *this = QgsRectangle( x, y, x, y );
  else
  {
    xmin = (( xmin < x ) ? xmin : x );
    xmax = (( xmax > x ) ? xmax : x );

    ymin = (( ymin < y ) ? ymin : y );
    ymax = (( ymax > y ) ? ymax : y );
  }
}

bool QgsRectangle::isEmpty() const
{
  return xmax <= xmin || ymax <= ymin;
}

bool QgsRectangle::isNull() const
{
  // rectangle created QgsRectangle() or with rect.setMinimal() ?
  return ( qgsDoubleNear( xmin, 0.0 ) && qgsDoubleNear( xmax, 0.0 ) && qgsDoubleNear( ymin, 0.0 ) && qgsDoubleNear( ymax, 0.0 ) ) ||
         ( qgsDoubleNear( xmin, std::numeric_limits<double>::max() ) && qgsDoubleNear( ymin, std::numeric_limits<double>::max() ) &&
           qgsDoubleNear( xmax, -std::numeric_limits<double>::max() ) && qgsDoubleNear( ymax, -std::numeric_limits<double>::max() ) );
}

QString QgsRectangle::asWktCoordinates() const
{
  QString rep =
    qgsDoubleToString( xmin ) + ' ' + qgsDoubleToString( ymin ) + ", " +
    qgsDoubleToString( xmax ) + ' ' + qgsDoubleToString( ymax );

  return rep;
}

QString QgsRectangle::asWktPolygon() const
{
  QString rep =
    QStringLiteral( "POLYGON((" ) +
    qgsDoubleToString( xmin ) + ' ' + qgsDoubleToString( ymin ) + ", " +
    qgsDoubleToString( xmax ) + ' ' + qgsDoubleToString( ymin ) + ", " +
    qgsDoubleToString( xmax ) + ' ' + qgsDoubleToString( ymax ) + ", " +
    qgsDoubleToString( xmin ) + ' ' + qgsDoubleToString( ymax ) + ", " +
    qgsDoubleToString( xmin ) + ' ' + qgsDoubleToString( ymin ) +
    QStringLiteral( "))" );

  return rep;
}

//! returns a QRectF with same coordinates.
QRectF QgsRectangle::toRectF() const
{
  return QRectF( static_cast< qreal >( xmin ), static_cast< qreal >( ymin ), static_cast< qreal >( xmax - xmin ), static_cast< qreal >( ymax - ymin ) );
}

// Return a string representation of the rectangle with automatic or high precision
QString QgsRectangle::toString( bool automaticPrecision ) const
{
  if ( automaticPrecision )
  {
    int precision = 0;
    if (( width() < 1 || height() < 1 ) && ( width() > 0 && height() > 0 ) )
    {
      precision = static_cast<int>( ceil( -1.0 * log10( qMin( width(), height() ) ) ) ) + 1;
      // sanity check
      if ( precision > 20 )
        precision = 20;
    }
    return toString( precision );
  }
  else
    return toString( 16 );
}

// overloaded version of above fn to allow precision to be set
// Return a string representation of the rectangle with high precision
QString QgsRectangle::toString( int precision ) const
{
  QString rep;
  if ( isEmpty() )
    rep = QStringLiteral( "Empty" );
  else
    rep = QStringLiteral( "%1,%2 : %3,%4" )
          .arg( xmin, 0, 'f', precision )
          .arg( ymin, 0, 'f', precision )
          .arg( xmax, 0, 'f', precision )
          .arg( ymax, 0, 'f', precision );

  QgsDebugMsgLevel( QString( "Extents : %1" ).arg( rep ), 4 );

  return rep;
}


// Return the rectangle as a set of polygon coordinates
QString QgsRectangle::asPolygon() const
{
//   QString rep = tmp.sprintf("%16f %16f,%16f %16f,%16f %16f,%16f %16f,%16f %16f",
//     xmin, ymin, xmin, ymax, xmax, ymax, xmax, ymin, xmin, ymin);
  QString rep;

  QTextStream foo( &rep );

  foo.setRealNumberPrecision( 8 );
  foo.setRealNumberNotation( QTextStream::FixedNotation );
  // NOTE: a polygon isn't a polygon unless its closed. In the case of
  //       a rectangle, that means 5 points (last == first)
  foo
  << xmin << ' ' << ymin << ", "
  << xmin << ' ' << ymax << ", "
  << xmax << ' ' << ymax << ", "
  << xmax << ' ' << ymin << ", "
  << xmin << ' ' << ymin;

  return rep;

} // QgsRectangle::asPolygon() const


bool QgsRectangle::operator==( const QgsRectangle & r1 ) const
{
  return qgsDoubleNear( r1.xMaximum(), xMaximum() ) &&
         qgsDoubleNear( r1.xMinimum(), xMinimum() ) &&
         qgsDoubleNear( r1.yMaximum(), yMaximum() ) &&
         qgsDoubleNear( r1.yMinimum(), yMinimum() );
}


bool QgsRectangle::operator!=( const QgsRectangle & r1 ) const
{
  return ( ! operator==( r1 ) );
}


QgsRectangle & QgsRectangle::operator=( const QgsRectangle & r )
{
  if ( &r != this )
  {
    xmax = r.xMaximum();
    xmin = r.xMinimum();
    ymax = r.yMaximum();
    ymin = r.yMinimum();
  }

  return *this;
}


void QgsRectangle::unionRect( const QgsRectangle& r )
{
  if ( r.xMinimum() < xMinimum() )
    setXMinimum( r.xMinimum() );
  if ( r.xMaximum() > xMaximum() )
    setXMaximum( r.xMaximum() );
  if ( r.yMinimum() < yMinimum() )
    setYMinimum( r.yMinimum() );
  if ( r.yMaximum() > yMaximum() )
    setYMaximum( r.yMaximum() );
}

bool QgsRectangle::isFinite() const
{
  if ( qIsInf( xmin ) || qIsInf( ymin ) || qIsInf( xmax ) || qIsInf( ymax ) )
  {
    return false;
  }
  if ( qIsNaN( xmin ) || qIsNaN( ymin ) || qIsNaN( xmax ) || qIsNaN( ymax ) )
  {
    return false;
  }
  return true;
}

void QgsRectangle::invert()
{
  double tmp;
  tmp = xmin;
  xmin = ymin;
  ymin = tmp;
  tmp = xmax;
  xmax = ymax;
  ymax = tmp;
}

QDataStream& operator<<( QDataStream& out, const QgsRectangle& rectangle )
{
  out << rectangle.xMinimum() << rectangle.yMinimum() << rectangle.xMaximum() << rectangle.yMaximum();
  return out;
}

QDataStream& operator>>( QDataStream& in, QgsRectangle& rectangle )
{
  double xmin, ymin, xmax, ymax;
  in >> xmin >> ymin >> xmax >> ymax;
  rectangle.setXMinimum( xmin );
  rectangle.setYMinimum( ymin );
  rectangle.setXMaximum( xmax );
  rectangle.setYMaximum( ymax );
  return in;
}
