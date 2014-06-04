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

    struct ImageType
    {
        enum
        {
            k_noData = 0,
            k_uncompressedColorMapped = 1,
            k_uncompressedTrueColor = 2,
            k_uncompressedBlackAndWhite = 3,
            k_runLengthEncodedColorMapped = 9,
            k_runLengthEncodedTrueColor = 10,
            k_runLengthEncodedBlackAndWhite = 11
        };
    };

    int BitsToBytes( unsigned char const& i_bits );
    int UCharArrayLEToInt( unsigned char const*const i_char, int const& i_arrayLength );

    int BitsToBytes( unsigned char const& i_bits )
    {
        return ( i_bits%8 != 0 )? i_bits/8 + 1 : i_bits/8;
    }

    int UCharArrayLEToInt( unsigned char const*const i_char, int const& i_arrayLength )
    {
        assert( i_arrayLength > 0 && i_arrayLength <= 4 );

        int accumulator = 0;
        for( auto index = 0; index < i_arrayLength; ++index )
        {
            accumulator += ( i_char[index] << (8*index) );
        }
        return accumulator;
    }

    Image MakeImage( std::string const& i_filename )
    {
        std::ifstream imageFile ( i_filename, std::ifstream::binary );

        // Read footer and determine kind of tga file
        Footer footer;
        imageFile.seekg( static_cast<std::ios::off_type>(sizeof(Footer)), std::ios::end );
        imageFile.read( reinterpret_cast<char*>(&footer), static_cast<std::streamsize>(sizeof( Footer )) );
        if ( !imageFile.good() )
        {
            throw 0;
        }

        if ( footer.m_signature == k_signature )
        {
            // TGA v2
            imageFile.seekg( 0, std::ios::beg );
            Header header;
            imageFile.read( reinterpret_cast<char*>(&header), static_cast<std::streamsize>(sizeof( Header )) );

            if ( !imageFile.good() )
            {
                throw 1;
            }

            ImageData imageData;
            if ( header.m_idLength[0] > 0 )
            {
                imageData.m_imageID = std::unique_ptr<char[]>{ new char [ header.m_idLength[0] ] };
                imageFile.read( imageData.m_imageID.get(), header.m_idLength[0] );

                if ( !imageFile.good() )
                {
                    throw 2;
                }
            }

            if ( header.m_colorMapType[0] == 1 )
            {
                int const colorEntrySize = BitsToBytes( header.m_colorMapSpec[4] );
                int const colorMapSize = UCharArrayLEToInt( &header.m_colorMapSpec[2], 2 ) * colorEntrySize;

                imageData.m_colorMap = std::unique_ptr<char[]>{ new char [ static_cast<unsigned int>(colorMapSize) ] };
                imageFile.read( imageData.m_colorMap.get(), colorMapSize );

                if ( !imageFile.good() )
                {
                    throw 3;
                }
            }

            if ( header.m_imageType[0] != 0 )
            {
                int const width = UCharArrayLEToInt( &header.m_imageSpec[4], 2 );
                int const height = UCharArrayLEToInt( &header.m_imageSpec[6], 2 );

            }


        }
        else
        {
            // TGA v1
        }






        Image image;
        return image;
    }
}