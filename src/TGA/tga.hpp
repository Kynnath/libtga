/*
 * File:   tga.hpp
 * Author: juan.garibotti
 *
 * Created on 30 de mayo de 2014, 17:55
 */

#ifndef TGA_HPP
#define	TGA_HPP

#include <string>
#include "GLT/Image.hpp"

namespace tga
{
  glt::Image MakeImage(std::string const& i_filename);
}

#endif	/* TGA_HPP */

