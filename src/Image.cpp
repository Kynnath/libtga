/*
 * File:   Image.cpp
 * Author: juan.garibotti
 *
 * Created on 30 de mayo de 2014, 12:34
 */

#include "Image.hpp"

namespace tga
{
    Image::Image( int const& i_width, int const& i_height, PixelFormat const& i_pixelFormat, std::unique_ptr<char[]> c_data )
        : m_data { std::move( c_data ) }
        , m_pixelFormat { i_pixelFormat }
        , m_width { i_width }
        , m_height { i_height }
    {}

    char const* Image::Data() const
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
}