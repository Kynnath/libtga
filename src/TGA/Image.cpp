/*
 * File:   Image.cpp
 * Author: juan.garibotti
 *
 * Created on 30 de mayo de 2014, 12:34
 */

#include "Image.hpp"

#include <cstring>

namespace tga
{
    Image::Image( int const& i_width, int const& i_height, PixelFormat const& i_pixelFormat, std::unique_ptr<unsigned char[]> c_data )
        : m_data { std::move( c_data ) }
        , m_pixelFormat { i_pixelFormat }
        , m_width { i_width }
        , m_height { i_height }
    {}

    unsigned char const* Image::Data() const
    {
        return m_data.get();
    }

    int const& Image::GetWidth() const
    {
        return m_width;
    }

    int const& Image::GetHeight() const
    {
        return m_height;
    }

    PixelFormat const& Image::GetPixelFormat() const
    {
        return m_pixelFormat;
    }

    void Image::FlipAlpha()
    {
        unsigned int const imageSize = static_cast<unsigned int>( m_height * m_width );
        unsigned int const step = ( m_pixelFormat == PixelFormat::e_ABW16 )? 2:4;
        for ( unsigned int pixel = 0; pixel < imageSize; ++pixel )
        {
            auto alpha = m_data[ pixel*step ];
            std::memmove( &m_data[ pixel*step ], &m_data[ (pixel*step)+1 ], step-1 );
            m_data[ (pixel*step) + (step-1) ] = alpha;
        }
    }
}