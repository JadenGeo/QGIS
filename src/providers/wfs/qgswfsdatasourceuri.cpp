/***************************************************************************
    qgswfsdatasourceuri.cpp
    ---------------------
    begin                : February 2016
    copyright            : (C) 2016 by Even Rouault
    email                : even.rouault at spatialys.com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "QtGlobal"

#include "qgswfsconstants.h"
#include "qgswfsdatasourceuri.h"
#include "qgsmessagelog.h"

QgsWFSDataSourceURI::QgsWFSDataSourceURI( const QString& uri )
    : mURI( uri )
{
  // Compatibility with QGIS < 2.16 layer URI of the format
  // http://example.com/?SERVICE=WFS&VERSION=1.0.0&REQUEST=GetFeature&TYPENAME=x&SRSNAME=y&username=foo&password=
  if ( !mURI.hasParam( QgsWFSConstants::URI_PARAM_URL ) )
  {
    QUrl url( uri );
    // Transform all param keys to lowercase
    typedef QPair<QString, QString> queryItem;
    QList<queryItem> items( url.queryItems() );
    Q_FOREACH ( const queryItem& item, items )
    {
      url.removeQueryItem( item.first );
      url.addQueryItem( item.first.toLower(), item.second );
    }

    QString srsname = url.queryItemValue( QgsWFSConstants::URI_PARAM_SRSNAME );
    QString bbox = url.queryItemValue( QgsWFSConstants::URI_PARAM_BBOX );
    QString typeName = url.queryItemValue( QgsWFSConstants::URI_PARAM_TYPENAME );
    QString version = url.queryItemValue( QgsWFSConstants::URI_PARAM_VERSION );
    QString filter = url.queryItemValue( QgsWFSConstants::URI_PARAM_FILTER );
    mAuth.mAuthCfg = url.queryItemValue( QgsWFSConstants::URI_PARAM_AUTHCFG );
    // NOTE: A defined authcfg overrides any older username/password auth
    //       Only check for older auth if it is undefined
    if ( mAuth.mAuthCfg.isEmpty() )
    {
      mAuth.mUserName = url.queryItemValue( QgsWFSConstants::URI_PARAM_USERNAME );
      // In QgsDataSourceURI, the "username" param is named "user", check it
      if ( mAuth.mUserName.isEmpty() )
      {
        mAuth.mUserName = url.queryItemValue( QgsWFSConstants::URI_PARAM_USER );
      }
      mAuth.mPassword = url.queryItemValue( QgsWFSConstants::URI_PARAM_PASSWORD );
    }

    // Now remove all stuff that is not the core URL
    url.removeQueryItem( QStringLiteral( "SERVICE" ) );
    url.removeQueryItem( QStringLiteral( "VERSION" ) );
    url.removeQueryItem( QStringLiteral( "TYPENAME" ) );
    url.removeQueryItem( QStringLiteral( "REQUEST" ) );
    url.removeQueryItem( QStringLiteral( "BBOX" ) );
    url.removeQueryItem( QStringLiteral( "SRSNAME" ) );
    url.removeQueryItem( QStringLiteral( "FILTER" ) );
    url.removeQueryItem( QgsWFSConstants::URI_PARAM_USERNAME );
    url.removeQueryItem( QgsWFSConstants::URI_PARAM_PASSWORD );
    url.removeQueryItem( QgsWFSConstants::URI_PARAM_AUTHCFG );

    mURI = QgsDataSourceUri();
    mURI.setParam( QgsWFSConstants::URI_PARAM_URL, url.toEncoded() );
    setTypeName( typeName );
    setSRSName( srsname );
    setVersion( version );

    //if the xml comes from the dialog, it needs to be a string to pass the validity test
    if ( filter.startsWith( '\'' ) && filter.endsWith( '\'' ) && filter.size() > 1 )
    {
      filter.chop( 1 );
      filter.remove( 0, 1 );
    }

    setFilter( filter );
    if ( !bbox.isEmpty() )
      mURI.setParam( QgsWFSConstants::URI_PARAM_RESTRICT_TO_REQUEST_BBOX, QStringLiteral( "1" ) );
  }
  else
  {
    mAuth.mUserName = mURI.username();
    mAuth.mPassword = mURI.password();
    mAuth.mAuthCfg = mURI.authConfigId();
  }
}

const QString QgsWFSDataSourceURI::uri( bool expandAuthConfig ) const
{
  QgsDataSourceUri theURI( mURI );
  // Add authcfg param back into the uri (must be non-empty value)
  if ( ! mAuth.mAuthCfg.isEmpty() )
  {
    theURI.setAuthConfigId( mAuth.mAuthCfg );
  }
  else
  {
    // Add any older username/password auth params back in (allow empty values)
    if ( ! mAuth.mUserName.isNull() )
    {
      theURI.setUsername( mAuth.mUserName );
    }
    if ( ! mAuth.mPassword.isNull() )
    {
      theURI.setPassword( mAuth.mPassword );
    }
  }
  // NOTE: avoid expanding authcfg here; it is handled during network access
  return theURI.uri( expandAuthConfig );
}


QUrl QgsWFSDataSourceURI::baseURL( bool bIncludeServiceWFS ) const
{
  QUrl url( mURI.param( QgsWFSConstants::URI_PARAM_URL ) );
  if ( bIncludeServiceWFS )
  {
    url.addQueryItem( QStringLiteral( "SERVICE" ), QStringLiteral( "WFS" ) );
  }
  return url;
}

QString QgsWFSDataSourceURI::version() const
{
  if ( !mURI.hasParam( QgsWFSConstants::URI_PARAM_VERSION ) )
    return QgsWFSConstants::VERSION_AUTO;
  return mURI.param( QgsWFSConstants::URI_PARAM_VERSION );
}

int QgsWFSDataSourceURI::maxNumFeatures() const
{
  if ( !mURI.hasParam( QgsWFSConstants::URI_PARAM_MAXNUMFEATURES ) )
    return 0;
  return mURI.param( QgsWFSConstants::URI_PARAM_MAXNUMFEATURES ).toInt();
}

void QgsWFSDataSourceURI::setMaxNumFeatures( int maxNumFeatures )
{
  mURI.removeParam( QgsWFSConstants::URI_PARAM_MAXNUMFEATURES );
  mURI.setParam( QgsWFSConstants::URI_PARAM_MAXNUMFEATURES , QString( maxNumFeatures ) );
}

void QgsWFSDataSourceURI::setTypeName( const QString& typeName )
{
  mURI.removeParam( QgsWFSConstants::URI_PARAM_TYPENAME );
  mURI.setParam( QgsWFSConstants::URI_PARAM_TYPENAME, typeName );
}

QString QgsWFSDataSourceURI::typeName() const
{
  return mURI.param( QgsWFSConstants::URI_PARAM_TYPENAME );
}

void QgsWFSDataSourceURI::setSRSName( const QString& crsString )
{
  mURI.removeParam( QgsWFSConstants::URI_PARAM_SRSNAME );
  if ( !crsString.isEmpty() )
    mURI.setParam( QgsWFSConstants::URI_PARAM_SRSNAME, crsString );
}

void QgsWFSDataSourceURI::setVersion( const QString& versionString )
{
  mURI.removeParam( QgsWFSConstants::URI_PARAM_VERSION );
  if ( !versionString.isEmpty() )
    mURI.setParam( QgsWFSConstants::URI_PARAM_VERSION, versionString );
}

QString QgsWFSDataSourceURI::SRSName() const
{
  return mURI.param( QgsWFSConstants::URI_PARAM_SRSNAME );
}

QString QgsWFSDataSourceURI::filter() const
{
  return mURI.param( QgsWFSConstants::URI_PARAM_FILTER );
}

void QgsWFSDataSourceURI::setFilter( const QString& filter )
{
  mURI.removeParam( QgsWFSConstants::URI_PARAM_FILTER );
  if ( !filter.isEmpty() )
  {
    mURI.setParam( QgsWFSConstants::URI_PARAM_FILTER, filter );
  }
}

QString QgsWFSDataSourceURI::sql() const
{
  return mURI.sql();
}

void QgsWFSDataSourceURI::setSql( const QString& sql )
{
  mURI.setSql( sql );
}

bool QgsWFSDataSourceURI::isRestrictedToRequestBBOX() const
{
  if ( mURI.hasParam( QgsWFSConstants::URI_PARAM_RESTRICT_TO_REQUEST_BBOX ) &&
       mURI.param( QgsWFSConstants::URI_PARAM_RESTRICT_TO_REQUEST_BBOX ).toInt() == 1 )
    return true;

  // accept previously used version with typo
  if ( mURI.hasParam( QStringLiteral( "retrictToRequestBBOX" ) ) && mURI.param( QStringLiteral( "retrictToRequestBBOX" ) ).toInt() == 1 )
    return true;

  return false;
}

bool QgsWFSDataSourceURI::ignoreAxisOrientation() const
{
  return mURI.hasParam( QgsWFSConstants::URI_PARAM_IGNOREAXISORIENTATION );
}

bool QgsWFSDataSourceURI::invertAxisOrientation() const
{
  return mURI.hasParam( QgsWFSConstants::URI_PARAM_INVERTAXISORIENTATION );
}

bool QgsWFSDataSourceURI::validateSqlFunctions() const
{
  return mURI.hasParam( QgsWFSConstants::URI_PARAM_VALIDATESQLFUNCTIONS );
}

bool QgsWFSDataSourceURI::hideDownloadProgressDialog() const
{
  return mURI.hasParam( QgsWFSConstants::URI_PARAM_HIDEDOWNLOADPROGRESSDIALOG );
}

QString QgsWFSDataSourceURI::build( const QString& baseUri,
                                    const QString& typeName,
                                    const QString& crsString,
                                    const QString& sql,
                                    bool restrictToCurrentViewExtent )
{
  QgsWFSDataSourceURI uri( baseUri );
  uri.setTypeName( typeName );
  uri.setSRSName( crsString );
  uri.setSql( sql );
  if ( restrictToCurrentViewExtent )
    uri.mURI.setParam( QgsWFSConstants::URI_PARAM_RESTRICT_TO_REQUEST_BBOX, QStringLiteral( "1" ) );
  return uri.uri();
}
