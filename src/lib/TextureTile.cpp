//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

#include "TextureTile.h"

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtGui/QFont>
#include <QtGui/QPainter>
#include <QtGui/QPen>

#include <cmath>

#include "global.h"
#include "MarbleDirs.h"
#include "TileLoaderHelper.h"


static uint **jumpTableFromQImage32( QImage &img )
{
    const int  height = img.height();
    const int  bpl    = img.bytesPerLine() / 4;
    uint      *data = (QRgb*)(img.scanLine(0));
    uint     **jumpTable = new uint*[height];

    for ( int y = 0; y < height; ++y ) {
	jumpTable[ y ] = data;
	data += bpl;
    }

    return jumpTable;
}


static uchar **jumpTableFromQImage8( QImage &img )
{
    const int  height = img.height();
    const int  bpl    = img.bytesPerLine();
    uchar     *data = img.bits();
    uchar    **jumpTable = new uchar*[height];

    for ( int y = 0; y < height; ++y ) {
	jumpTable[ y ] = data;
	data += bpl;
    }

    return jumpTable;
}

TextureTile::TextureTile( TileId const& id )
    : QObject(),
      jumpTable8(0),
      jumpTable32(0),
      m_id(id),
      m_rawtile( QImage() ),
      m_depth(0),
      m_used(false)
{
}


TextureTile::~TextureTile()
{
    switch ( m_depth ) {
    case 32:
	delete [] jumpTable32;
	break;
    case 8:
	delete [] jumpTable8;
	break;
    default:
	qDebug() << "Color m_depth(" << m_depth << ") of a tile could not be retrieved. Exiting.";
//	exit(-1);
    }

//    qDebug() << "Tile deleted: " << m_id;
//    delete m_rawtile;
}

void TextureTile::loadRawTile( GeoSceneTexture *textureLayer, int level, int x, int y)
{
//   qDebug() << "TextureTile::loadRawTile";
  
  m_used = true; // Needed to avoid frequent deletion of tiles
  
    // qDebug() << "Entered loadTile( int, int, int) of Tile" << m_id;
    // m_used = true; // Needed to avoid frequent deletion of tiles

  QString  absfilename;

  // qDebug() << "Requested tile level" << level;

  // If the tile level offers the requested tile then load it.
  // Otherwise cycle from the requested tilelevel down to one where
  // the requested area is covered.  Then scale the area to create a
  // replacement for the tile that has been requested.

  for ( int i = level; i > -1; --i ) {

      double origx1 = (double)(x) / (double)( TileLoaderHelper::levelToRow( level ) );
      double origy1 = (double)(y) / (double)( TileLoaderHelper::levelToColumn( level ) );
      double testx1 = origx1 * (double)( TileLoaderHelper::levelToRow( i ) ) ;
      double testy1 = origy1 * (double)( TileLoaderHelper::levelToColumn( i ) );

      QString relfilename = TileLoaderHelper::relativeTileFileName( textureLayer, i,
                                                                    (int)(testx1), (int)(testy1) );
      absfilename = MarbleDirs::path( relfilename );

      if ( QFile::exists( absfilename ) ) {
          // qDebug() << "The image filename does exist: " << absfilename ;

          QImage temptile( absfilename );

          if ( !temptile.isNull() ) {
              //         qDebug() << "Image has been successfully loaded.";

              if ( level != i ) { 
                  // qDebug() << "About to start cropping an existing image.";
                  QSize tilesize = temptile.size();
                  double origx2 = (double)(x + 1) / (double)( TileLoaderHelper::levelToRow( level ) );
                  double origy2 = (double)(y + 1) / (double)( TileLoaderHelper::levelToColumn( level ) );
                  double testx2 = origx2 * (double)( TileLoaderHelper::levelToRow( i ) );
                  double testy2 = origy2 * (double)( TileLoaderHelper::levelToColumn( i ) );

                  QPoint topleft( (int)( ( testx1 - (int)(testx1) ) * temptile.width() ),
                                  (int)( ( testy1 - (int)(testy1) ) * temptile.height() ) );
                  QPoint bottomright( (int)( ( testx2 - (int)(testx1) ) * temptile.width() ) - 1,
                                      (int)( ( testy2 - (int)(testy1) ) * temptile.height() ) - 1 );

                  // This should not create any memory leaks as
                  // 'copy' and 'scaled' return a value (on the
                  // stack) which gets deep copied always into the
                  // same place for m_rawtile on the heap:
                  temptile = temptile.copy( QRect( topleft, bottomright ) );
                  temptile = temptile.scaled( tilesize ); // TODO: use correct size
                  //          qDebug() << "Finished scaling up the Temporary Tile.";
              }

              m_rawtile = temptile;

              break;
          } // !tempfile.isNull()
          //      else {
          //         qDebug() << "Image load failed for: " + 
          //           absfilename.toLocal8Bit();
          //      }
      }
      else {
          //      qDebug() << "emit downloadTile(" << relfilename << ");";
          emit downloadTile( relfilename, m_id.toString() );
      }
  }
  
//   qDebug() << "TextureTile::loadRawTile end";

  m_depth = m_rawtile.depth();
  
//   qDebug() << "m_depth =" << m_depth;
}

void TextureTile::loadTile( bool requestTileUpdate )
{
  //    qDebug() << "Entered loadTile( int, int, int) of Tile" << m_id;

  if ( m_rawtile.isNull() ) {
    qDebug() << "An essential tile is missing. Please rerun the application.";
    exit(-1);
  }

  switch ( m_depth ) {
      case 32:
          if ( jumpTable32 ) delete [] jumpTable32;
          jumpTable32 = jumpTableFromQImage32( m_rawtile );
          break;
      case 8:
          if ( jumpTable8 ) delete [] jumpTable8;
          jumpTable8 = jumpTableFromQImage8( m_rawtile );
          break;
      default:
          qDebug() << QString("Color m_depth %1 of tile could not be retrieved. Exiting.").arg(m_depth);
          exit( -1 );
  }

  if ( requestTileUpdate ) {
    // qDebug() << "TileUpdate available";
    emit tileUpdateDone();
  }
}

#include "TextureTile.moc"
