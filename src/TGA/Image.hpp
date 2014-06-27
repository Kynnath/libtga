/*
 * File:   Image.hpp
 * Author: juan.garibotti
 *
 * Created on 30 de mayo de 2014, 12:34
 */

#ifndef TGA_IMAGE_HPP
#define	TGA_IMAGE_HPP

#include <memory>

namespace tga
{
    enum class PixelFormat
    {
        e_ARGB32,
        e_RGB24,
        e_ABW16,
        e_BW8
    };

    class Image
    {
        std::unique_ptr<unsigned char[]> m_data;
        PixelFormat m_pixelFormat;
        int m_width;
        int m_height;

        public:
            Image( int const& i_width, int const& i_height, PixelFormat const& i_pixelFormat, std::unique_ptr<unsigned char[]> c_data );
            unsigned char const* Data() const;
            int const& GetWidth() const;
            int const& GetHeight() const;
            PixelFormat const& GetPixelFormat() const;
            void FlipAlpha();
    };
}

#endif	/* TGA_IMAGE_HPP */

