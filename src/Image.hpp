/*
 * File:   Image.hpp
 * Author: juan.garibotti
 *
 * Created on 30 de mayo de 2014, 12:34
 */

#ifndef IMAGE_HPP
#define	IMAGE_HPP

#include <memory>

namespace tga
{
    enum class PixelFormat
    {
        e_grayscale8
    };

    class Image
    {
        std::unique_ptr<unsigned char[]> m_data;
        PixelFormat m_pixelFormat;
        int m_width;
        int m_height;

        public:
            unsigned char const* Data() const;
            int const& GetWidth() const;
            int const& GetHeight() const;
            PixelFormat const& GetPixelFormat() const;
    };
}

#endif	/* IMAGE_HPP */

