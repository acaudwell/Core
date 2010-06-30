/*
    Copyright (c) 2009 Andrew Caudwell (acaudwell@gmail.com)
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. The name of the author may not be used to endorse or promote products
       derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
    IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
    OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
    NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
    THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    Portions of this code are derivived from FTGL (FTTextureFont.cpp)

    FTGL - OpenGL font library

    Copyright (c) 2001-2004 Henry Maddocks <ftgl@opengl.geek.nz>
    Copyright (c) 2008 Sam Hocevar <sam@zoy.org>

    Permission is hereby granted, free of charge, to any person obtaining
    a copy of this software and associated documentation files (the
    "Software"), to deal in the Software without restriction, including
    without limitation the rights to use, copy, modify, merge, publish,
    distribute, sublicense, and/or sell copies of the Software, and to
    permit persons to whom the Software is furnished to do so, subject to
    the following conditions:
 
    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.
 
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef FX_FONT_H
#define FX_FONT_H

#include "display.h"
#include "vectors.h"
#include "logger.h"

#include "resource.h"

#include <string>
#include <vector>
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "ftgl/FTUnicode.h"

class FXFontException : public ResourceException {
public:
    FXFontException(const std::string & font_file) : ResourceException(font_file) {}
};


class FXGlyphPage;
class FXGlyphSet;

class FXGlyph {
    unsigned int chr;
    GLuint call_list;
    
    vec2f dims;
    vec2f corner;
    vec2f advance;
       
public:
    FXGlyphPage* page;
    FXGlyphSet* set;
    FT_BitmapGlyph glyph_bitmap;

    FXGlyph(FXGlyphSet* set, unsigned int chr);

    void compile(FXGlyphPage* page, const vec4f& texcoords);

    const vec2f& getAdvance() const { return advance; };
    const vec2f& getCorner() const { return corner; };
    const vec2f& getDimensions() const { return dims; }
    inline void draw() const { glCallList(call_list); };
};

class FXGlyphPage {
    GLubyte* texture_data;
    bool needs_update;
    int page_width;
    int page_height;
public:
    FXGlyphPage(int page_width, int page_height);
    ~FXGlyphPage();

    bool addGlyph(FXGlyph* glyph);
    
    void updateTexture();

    GLuint textureid;
    
    int cursor_x, cursor_y;
    int max_glyph_height;
};

class FXGlyphSet {
    FT_Library freetype;
    FT_Face ftface;
    std::string fontfile;
    int size;
    int dpi;

    bool pre_caching;
        
    std::vector<FXGlyphPage*> pages;
    
    std::map<unsigned int, FXGlyph*> glyphs;
    
    void init();
    FXGlyph* getGlyph(unsigned int chr);
public:
    FXGlyphSet(FT_Library freetype, const std::string& fontfile, int size, int dpi);
    ~FXGlyphSet();

    void precache(const std::string& chars);
    
    FT_Face getFTFace() const { return ftface; }
    
    const std::string& getFontFile() const { return fontfile; }
    
    float getWidth(const std::string& text);

    float getMaxWidth() const;
    float getMaxHeight() const;
    
    int getSize() const { return size; };

    void draw(const std::string& text);
};

class FXFont {

    FXGlyphSet* glyphset;

    std::string fontfile;
    bool shadow;
    bool round;

    float shadow_strength;
    vec2f shadow_offset;

    bool align_right, align_top;

    void render(float x, float y, const std::string& text) const;
    void init();
public:
    FXFont();
    FXFont(FXGlyphSet* glyphset);

    bool initialized() const { return (glyphset!=0); } 
    
    void print(float x, float y, const char *str, ...) const;
    void draw(float x, float y, const std::string& text) const;

    float getWidth(const std::string& text) const;

    void alignTop(bool top);
    void alignRight(bool right);

    void roundCoordinates(bool round);

    const std::string& getFontFile() const;

    float getMaxWidth() const;
    float getMaxHeight() const;

    int getFontSize() const;

    bool dropShadow() const { return shadow; };
    
    void dropShadow(bool shadow);
    void shadowStrength(float s);
    void shadowOffset(float x, float y);
};

class FXLabel {
    GLuint call_list;
public:
    FXLabel();
    FXLabel(const FXFont& font, const std::string& text);
    ~FXLabel();
    
    void setText(const FXFont& font, const std::string& text);
    inline void draw() const { glCallList(call_list); };   
};

typedef std::map<int,FXGlyphSet*> fontSizeMap;

class FXFontManager {

    std::string font_dir;

    std::map<std::string, fontSizeMap*> fonts;
    FT_Library library;
public:
    FXFontManager();

    void setDir(std::string font_dir);
    void init();
    void destroy();
    void purge();

    FXFont grab(std::string font_file, int size, int dpi = 72);
};

extern FXFontManager fontmanager;

#endif
