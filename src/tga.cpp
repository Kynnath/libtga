/*
 * File:   tga.cpp
 * Author: juan.garibotti
 *
 * Created on 30 de mayo de 2014, 17:55
 */

#include "tga.hpp"

#include <array>
#include <cassert>
#include <fstream>
#include <memory>
#include <vector>

namespace tga
{
    struct Header
    {
        std::array<unsigned char, 1> m_idLength;
        std::array<unsigned char, 1> m_colorMapType;
        std::array<unsigned char, 1> m_imageType;
        std::array<unsigned char, 5> m_colorMapSpec;
        std::array<unsigned char,10> m_imageSpec;
    };
    static_assert( sizeof(Header) == 18, "Header has padding." );

    struct ImageData
    {
        std::unique_ptr<char[]> m_imageID;
        std::unique_ptr<char[]> m_colorMap;
        std::unique_ptr<char[]> m_imageData;
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
    };
    static_assert( sizeof(Footer) == 26, "Footer has padding." );

    std::array<unsigned char,16> const k_signature =
    {
        'T','R','U','E','V','I','S','I','O','N','-','X','F','I','L','E'
    };

    class ImageType
    {
        enum Flags
        {
            k_noData = 0,
            k_ColorMapped = 1,
            k_TrueColor = 2,
            k_BlackAndWhite = 3,
            k_runLengthEncoded = 8
        };
    };

    class ImageOrigin
    {
        enum Direction
        {
            k_right = 1,
            k_top = 2
        };
    };

    int BitsToBytes( int const& i_bits );
    int UCharArrayLEToInt( unsigned char const*const i_char, int const& i_arrayLength );
    Header ReadHeader( std::ifstream & i_imageFile );
    ImageData ReadImageData( std::ifstream & i_imageFile, Header const& i_header );
    Footer ReadFooter( std::ifstream & i_imageFile );

    int BitsToBytes( int const& i_bits )
    {
        return ( ( i_bits+7 ) / 8 );
    }

    int UCharArrayLEToInt( unsigned char const*const i_char, int const& i_arrayLength )
    {
        assert( i_arrayLength > 0 && i_arrayLength <= 4 );

        int result = 0;
        for( auto index = 0; index < i_arrayLength; ++index )
        {
            result |= ( i_char[index] << (8*index) );
        }
        return result;
    }

    Header ReadHeader( std::ifstream & i_imageFile )
    {
        Header header;
        i_imageFile.seekg( 0, std::ios::beg );
        i_imageFile.read( reinterpret_cast<char*>(&header), static_cast<std::streamsize>(sizeof( Header )) );

        if ( !i_imageFile.good() )
        {
            throw 1;
        }

        return header;
    }

    ImageData ReadImageData( std::ifstream & i_imageFile, Header const& i_header )
    {
        ImageData imageData;

        i_imageFile.seekg( static_cast<std::ios::off_type>(sizeof(Footer)), std::ios::beg );

        if ( i_header.m_idLength[0] > 0 )
        {
            imageData.m_imageID = std::unique_ptr<char[]>{ new char [ i_header.m_idLength[0] ] };
            i_imageFile.read( imageData.m_imageID.get(), i_header.m_idLength[0] );

            if ( !i_imageFile.good() )
            {
                throw 2;
            }
        }

        if ( i_header.m_colorMapType[0] == 1 )
        {
            int const colorEntrySize = BitsToBytes( i_header.m_colorMapSpec[4] );
            int const colorMapSize = UCharArrayLEToInt( &i_header.m_colorMapSpec[2], 2 ) * colorEntrySize;

            imageData.m_colorMap = std::unique_ptr<char[]>{ new char [ static_cast<unsigned int>(colorMapSize) ] };
            i_imageFile.read( imageData.m_colorMap.get(), colorMapSize );

            if ( !i_imageFile.good() )
            {
                throw 3;
            }
        }

        if ( i_header.m_imageType[0] != 0 )
        {
            int const width = UCharArrayLEToInt( &i_header.m_imageSpec[4], 2 );
            int const height = UCharArrayLEToInt( &i_header.m_imageSpec[6], 2 );
            int const pixelDepth = BitsToBytes( i_header.m_imageSpec[8] );
            int const imageSize = width * height * pixelDepth;

            imageData.m_imageData = std::unique_ptr<char[]>{ new char [ static_cast<unsigned int>(imageSize) ] };
            i_imageFile.read( imageData.m_imageData.get(), imageSize );

            if ( i_imageFile.good() )
            {
                throw 4;
            }
        }

        return imageData;
    }

    Footer ReadFooter( std::ifstream & i_imageFile )
    {
        Footer footer;
        i_imageFile.seekg( -static_cast<std::ios::off_type>(sizeof(Footer)), std::ios::end );
        i_imageFile.read( reinterpret_cast<char*>(&footer), static_cast<std::streamsize>(sizeof( Footer )) );

        if ( !i_imageFile.good() )
        {
            throw 0;
        }

        return footer;
    }

    Image MakeImage( std::string const& i_filename )
    {
        std::ifstream imageFile ( i_filename, std::ifstream::binary );
        if ( !imageFile.good() )
        {
            throw -1;
        }

        // Read footer and determine kind of tga file
        Footer const footer = ReadFooter( imageFile );
        if ( footer.m_signature == k_signature )
        {

        }

        Header const header = ReadHeader( imageFile );

        ImageData const imageData = ReadImageData( imageFile, header );



        Image image;
        return image;
    }
}