// Copyright (c) 2005 - 2018 Settlers Freaks (sf-team at siedler25.org)
//
// This file is part of Return To The Roots.
//
// Return To The Roots is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Return To The Roots is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Return To The Roots. If not, see <http://www.gnu.org/licenses/>.

#include "rttrDefines.h" // IWYU pragma: keep
/*#include "Shader.h"
#include <stdexcept>

Shader::Shader(GLenum type) : type_(type), shaderObject_(glCreateShader(type))
{
   if(!shaderObject_)
   {
       throw std::runtime_error("Could not create %s shader.", shader_to_string(type).c_str());
   }
}

Shader::~Shader()
{
   if(shaderObject_)
   {
       glDeleteShader(shaderObject_);
   }
}

void Shader::compile(const char* source)
{
   glShaderSource(shaderObject_, 1, &source, nullptr);

   glCompileShader(shaderObject_);
   GLint compiled;
   glGetShaderiv(shaderObject_, GL_COMPILE_STATUS, &compiled);
   if(!compiled)
   {
       GLint infoLen = 0;
       glGetShaderiv(shaderObject_, GL_INFO_LOG_LENGTH, &infoLen);
       if(infoLen > 1)
       {
           std::unique_ptr<char[]> infoLog(new char[infoLen]);
           glGetShaderInfoLog(shaderObject_, infoLen, NULL, infoLog.get());
           throw wexception(
               "Error compiling %s shader:\n%s", shader_to_string(type_).c_str(), infoLog.get());
       }
   }
}
}*/
