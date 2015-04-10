/*
 * File:   tga.cpp
 * Author: juan.garibotti
 *
 * Created on 30 de mayo de 2014, 17:55
 */

#include "tga.hpp"

#include <array>
#include <cassert>
#include <cstring>
#include <fstream>
#include <memory>
#include <vector>
#include "TLS/Tools.hpp"

namespace
{
  struct Header
  {
    std::array<unsigned char, 1> m_idLength;
    std::array<unsigned char, 1> m_colorMapType;
    std::array<unsigned char, 1> m_imageType;
    std::array<unsigned char, 5> m_colorMapSpec;
    std::array<unsigned char,10> m_imageSpec;
  };
  static_assert(sizeof(Header) == 18, "Header has padding.");

  struct ImageData
  {
    std::unique_ptr<unsigned char[]> m_imageID;
    std::unique_ptr<unsigned char[]> m_colorMap;
    std::unique_ptr<unsigned char[]> m_imageData;
  };

  struct ExtensionArea
  {
    std::array<unsigned char,  2> m_extensionSize;
    std::array<unsigned char, 41> m_authorName;
    std::array<unsigned char,324> m_comment;
    std::array<unsigned char, 12> m_timestamp;
    std::array<unsigned char, 41> m_jobID;
    std::array<unsigned char,  6> m_jobTime;
    std::array<unsigned char, 41> m_softwareID;
    std::array<unsigned char,  3> m_softwareVersion;
    std::array<unsigned char,  4> m_keyColor;
    std::array<unsigned char,  4> m_pixelAspectRatio;
    std::array<unsigned char,  4> m_gammaValue;
    std::array<unsigned char,  4> m_colorCorrectionOffset;
    std::array<unsigned char,  4> m_thumbnailOffset;
    std::array<unsigned char,  4> m_scanLineOffset;
    std::array<unsigned char,  1> m_atributes;
  };
  static_assert( sizeof(ExtensionArea) == 495, "ExtensionArea has padding." );

  struct Footer
  {
    std::array<unsigned char, 4> m_extensionOffset;
    std::array<unsigned char, 4> m_developerOffset;
    std::array<unsigned char,16> m_signature;
    std::array<unsigned char, 1> m_dot;
    std::array<unsigned char, 1> m_null;

    static std::array<unsigned char,16> const k_signature;
  };
  static_assert( sizeof(Footer) == 26, "Footer has padding." );

  std::array<unsigned char,16> const Footer::k_signature =
  {
    'T','R','U','E','V','I','S','I','O','N','-','X','F','I','L','E'
  };

  std::array<GLenum, 4> const k_imageFormats =
  {
    GL_RED,
    GL_RG,
    GL_BGR,
    GL_BGRA
  };

  struct ColorMap
  {
    enum
    {
      k_included = 1
    };
  };

  struct ImageType
  {
    enum
    {
      k_noData = 0,
      k_ColorMapped = 1,
      k_TrueColor = 2,
      k_BlackAndWhite = 3,
      k_runLengthEncoded = 8
    };
  };

  struct ImageOrigin
  {
    enum
    {
      k_right = 0x10,
      k_top = 0x20
    };
  };

  static Header ReadHeader( std::ifstream & io_imageFile )
  {
    Header header;
    io_imageFile.seekg(0, std::ios::beg);
    io_imageFile.read(reinterpret_cast<char*>(&header), static_cast<std::streamsize>(sizeof(Header)));

    if (!io_imageFile.good())
    {
      throw 1;
    }

    return header;
  }

  static ImageData ReadImageData( std::ifstream & io_imageFile, Header const& i_header )
  {
    ImageData imageData;

    io_imageFile.seekg(static_cast<std::ios::off_type>(sizeof(Header)), std::ios::beg);

    if (i_header.m_idLength[0] > 0)
    {
      imageData.m_imageID = std::unique_ptr<unsigned char[]>{ new unsigned char [ i_header.m_idLength[0] ] };
      io_imageFile.read(reinterpret_cast<char*>(imageData.m_imageID.get()), i_header.m_idLength[0]);

      if (!io_imageFile.good())
      {
        throw 2;
      }
    }

    if (i_header.m_colorMapType[0] == ColorMap::k_included)
    {
      int const colorEntrySize = tls::BitsToBytes(i_header.m_colorMapSpec[4]);
      int const colorMapSize = tls::UCharArrayLEToInt(&i_header.m_colorMapSpec[2], 2) * colorEntrySize;

      imageData.m_colorMap = std::unique_ptr<unsigned char[]>{ new unsigned char [ static_cast<unsigned int>(colorMapSize) ] };
      io_imageFile.read( reinterpret_cast<char*>( imageData.m_colorMap.get() ), colorMapSize );

      if ( !io_imageFile.good() )
      {
        throw 3;
      }
    }

    if ( i_header.m_imageType[0] != ImageType::k_noData )
    {
      int const width = tls::UCharArrayLEToInt(&i_header.m_imageSpec[4], 2);
      int const height = tls::UCharArrayLEToInt(&i_header.m_imageSpec[6], 2);
      int const pixelDepth = tls::BitsToBytes(i_header.m_imageSpec[8]);
      int const imageSize = width * height * pixelDepth;

      imageData.m_imageData = std::unique_ptr< unsigned char[]>{ new unsigned char [ static_cast<unsigned int>(imageSize) ] };
      io_imageFile.read( reinterpret_cast<char*>( imageData.m_imageData.get() ), imageSize );

      if ( !io_imageFile.good() )
      {
        throw 4;
      }

      if (i_header.m_imageSpec[9] & ImageOrigin::k_right)
      {
        // Mirror data vertically
        auto buffer = std::unique_ptr< unsigned char[]>{ new unsigned char [ static_cast<unsigned int>(imageSize) ] };
        for ( int row = 0; row < height; ++row )
        {
          for ( int column = 0; column < width; ++column )
          {
            std::size_t const sourcePosition = static_cast<std::size_t>((row*width+column)*pixelDepth);
            std::size_t const targetPosition = static_cast<std::size_t>( imageSize )-sourcePosition-static_cast<std::size_t>(pixelDepth);
            std::memcpy( &buffer[targetPosition], &imageData.m_imageData[sourcePosition], static_cast<std::size_t>(pixelDepth) );
          }
        }
        std::swap( imageData.m_imageData, buffer );
      }
      if (i_header.m_imageSpec[9] & ImageOrigin::k_top)
      {
        // Mirror data horizontally
        auto buffer = std::unique_ptr<unsigned char[]>{ new unsigned char [ static_cast<unsigned int>(imageSize) ] };
        for ( int row = 0; row < height; ++row )
        {
          std::size_t const sourcePosition = static_cast<std::size_t>( (row*width)*pixelDepth);
          std::size_t const targetPosition = static_cast<std::size_t>( imageSize )-sourcePosition-static_cast<std::size_t>(pixelDepth*width);
          std::memcpy( &buffer[targetPosition], &imageData.m_imageData[sourcePosition], static_cast<std::size_t>(pixelDepth*width) );
        }
        std::swap( imageData.m_imageData, buffer );
      }
    }

    return imageData;
  }

  static Footer ReadFooter( std::ifstream & io_imageFile )
  {
    Footer footer;
    io_imageFile.seekg( -static_cast<std::ios::off_type>(sizeof(Footer)), std::ios::end );
    io_imageFile.read( reinterpret_cast<char*>(&footer), static_cast<std::streamsize>(sizeof( Footer )) );

    if ( !io_imageFile.good() )
    {
      throw 0;
    }

    return footer;
  }

  glt::Image ImageFromColorMapped(Header const& i_header, ImageData const& i_imageData)
  {
    auto const width = tls::UCharArrayLEToInt(&i_header.m_imageSpec[4], 2);
    auto const height = tls::UCharArrayLEToInt(&i_header.m_imageSpec[6], 2);
    auto const colorEntrySize = tls::BitsToBytes(i_header.m_colorMapSpec[4]);
    auto imageRaw = std::make_unique<unsigned char[]>(static_cast<std::size_t>(width * height * colorEntrySize));

    for(auto index = 0; index < width * height; ++index)
    {
      auto colorIndex = i_imageData.m_imageData[static_cast<std::size_t>(index)];
      std::memcpy(&imageRaw[static_cast<std::size_t>(index*colorEntrySize)],
          &i_imageData.m_colorMap[static_cast<std::size_t>(colorIndex*colorEntrySize)],
          static_cast<std::size_t>(colorEntrySize));
    }

    return { {width,
             height,
             k_imageFormats[static_cast<std::size_t>(colorEntrySize-1)]},
             std::move(imageRaw) };
  }
}

namespace tga
{
  glt::Image MakeImage(std::string const& i_filename)
  {
    std::ifstream imageFile (i_filename, std::ifstream::binary);
    if (!imageFile.good())
    {
      throw -1;
    }

    // Read footer and determine kind of tga file
    Footer const footer = ReadFooter(imageFile);
    if (footer.m_signature == Footer::k_signature)
    {
      // Read extension and developer areas
    }

    auto const header = ReadHeader(imageFile );
    auto imageData = ReadImageData(imageFile, header);

    if (header.m_colorMapType[0] == ColorMap::k_included)
    {
      return ImageFromColorMapped(header, imageData);
    }

    return {{tls::UCharArrayLEToInt(&header.m_imageSpec[4], 2),
             tls::UCharArrayLEToInt(&header.m_imageSpec[6], 2),
             k_imageFormats[static_cast<std::size_t>(tls::BitsToBytes(header.m_imageSpec[8])-1)]},
             std::move(imageData.m_imageData) };
  }
}