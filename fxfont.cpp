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

#include "fxfont.h"

FXFontManager fontmanager;

//FxGlyph

FXGlyph::FXGlyph(FXGlyphSet* set, unsigned int chr) {

    this->set = set;
    this->chr = chr;
    this->ftglyph = 0;

    FT_Face ft_face = set->getFTFace();

    FT_UInt index = FT_Get_Char_Index( ft_face, chr );

    //debugLog("FXGlyph %x %d %d", chr, chr, index);

    if(FT_Load_Glyph( ft_face, index, set->getFlags()))
    throw FXFontException(ft_face->family_name);

    if(FT_Get_Glyph( ft_face->glyph, &ftglyph ))
        throw FXFontException(ft_face->family_name);

    FT_Glyph_Metrics *metrics = &ft_face->glyph->metrics;
    height = glm::ceil(metrics->height / 64.0);

    FT_Glyph_To_Bitmap( &ftglyph, FT_RENDER_MODE_NORMAL, 0, 1 );

    glyph_bitmap = (FT_BitmapGlyph)ftglyph;

    dims    = vec2( glyph_bitmap->bitmap.width, glyph_bitmap->bitmap.rows) + vec2(2.0f, 2.0f);
    corner  = vec2( glyph_bitmap->left, -glyph_bitmap->top) + vec2(0.5, -0.5);
    advance = vec2( ft_face->glyph->advance.x >> 6, ft_face->glyph->advance.y >> 6);

    vertex_positions[0] = vec2(0.0f, 0.0f);
    vertex_positions[1] = vec2(dims.x, 0.0f);
    vertex_positions[2] = dims;
    vertex_positions[3] = vec2(0.0f, dims.y);

    //call_list = 0;
    page = 0;
}

FXGlyph::~FXGlyph() {
    if(ftglyph!=0) FT_Done_Glyph(ftglyph);
}

void FXGlyph::setPage(FXGlyphPage* page, const vec4& texcoords) {
    this->page = page;
    this->texcoords = texcoords;

    vertex_texcoords[0] = vec2(texcoords.x, texcoords.y);
    vertex_texcoords[1] = vec2(texcoords.z, texcoords.y);
    vertex_texcoords[2] = vec2(texcoords.z, texcoords.w);
    vertex_texcoords[3] = vec2(texcoords.x, texcoords.w);
}

void FXGlyph::drawToVBO(quadbuf& buffer, const vec2& pos, const vec4& colour) const {
    buffer.add(page->texture->textureid, pos + corner, dims, colour, texcoords);
}

void FXGlyph::draw(const vec2& pos) const {
    for(int i=0;i<4;i++) {
        vec2 pos_offset = vertex_positions[i] + pos + corner;

        glTexCoord2fv(glm::value_ptr(vertex_texcoords[i]));
        glVertex2fv(glm::value_ptr(pos_offset));

    }
}

//FXGlyphPage


FXGlyphPage::FXGlyphPage(int page_width, int page_height) {
    this->page_width  = page_width;
    this->page_height = page_height;

    texture_data = new GLubyte[ page_width * page_height ];
    memset(texture_data, 0, page_width * page_height);

    needs_update = false;
    texture = 0;

    max_glyph_height = cursor_x = cursor_y = 1;
}

FXGlyphPage::~FXGlyphPage() {
    delete[] texture_data;
}

bool FXGlyphPage::addGlyph(FXGlyph* glyph) {

    FT_BitmapGlyph bitmap = glyph->glyph_bitmap;

    int corner_x = cursor_x;
    int corner_y = cursor_y;

    int padding = 3;

    if(bitmap->bitmap.rows > max_glyph_height) max_glyph_height = bitmap->bitmap.rows;

    if(corner_x + bitmap->bitmap.width + padding > page_width) {
        corner_x = 1;
        corner_y += max_glyph_height + padding;

        //bitmap is bigger than the full dimension
        if(corner_x + bitmap->bitmap.width + padding > page_width) return false;
    }

    if(corner_y + bitmap->bitmap.rows + padding > page_height) return false;

    needs_update = true;

    for(int j=0; j < bitmap->bitmap.rows;j++) {
        for(int i=0; i < bitmap->bitmap.width; i++) {
            texture_data[(corner_x+i+(j+corner_y)*page_width)] = bitmap->bitmap.buffer[i + bitmap->bitmap.width*j];
        }
    }

    //fprintf(stderr, "corner_x = %d, corner_y = %d\n", corner_x, corner_y);

    vec4 texcoords = vec4( (((float)corner_x)-0.5f) / (float) page_width,
                             (((float)corner_y)-0.5f) / (float) page_height,
                             (((float)corner_x+bitmap->bitmap.width)+1.5f) / (float) page_width,
                             (((float)corner_y+bitmap->bitmap.rows)+1.5f) / (float) page_height );

    glyph->setPage(this, texcoords);

    //glyph->compile(this, texcoords);

    // move cursor for next character
    cursor_x = corner_x + bitmap->bitmap.width + padding;
    cursor_y = corner_y;

    return true;
}

void FXGlyphPage::updateTexture() {
    if(!needs_update) return;

    if(!texture) {
        texture = texturemanager.create(page_width, page_height, false, GL_CLAMP_TO_EDGE, GL_ALPHA, texture_data);
    } else {
        texture->reload();
    }

    needs_update = false;
}

//FXGlyphSet

FXGlyphSet::FXGlyphSet(FT_Library freetype, const std::string& fontfile, int size, int dpi, FT_Int32 ft_flags) {
    this->freetype = freetype;
    this->fontfile = fontfile;
    this->size     = size;
    this->dpi      = dpi;
    this->ft_flags  = ft_flags;
    this->ft_face   = 0;

    this->tab_width  = 4.0f;
    this->max_height = 0;

    init();
}

FXGlyphSet::~FXGlyphSet() {
    if(ft_face!=0) FT_Done_Face(ft_face);

    for(std::vector<FXGlyphPage*>::iterator it = pages.begin(); it != pages.end(); it++) {
        delete (*it);
    }
    pages.clear();

    for(auto it: glyphs) {
        delete it.second;
    }
    glyphs.clear();
}

void FXGlyphSet::init() {

    if(FT_New_Face(freetype, fontfile.c_str(), 0, &ft_face)) {
        throw FXFontException(fontfile);
    }
   
    int ft_font_size = 64 * size;

    FT_Set_Char_Size( ft_face, ft_font_size, ft_font_size, dpi, dpi );

    double em_size = 1.0 * ft_face->units_per_EM;

    unit_scale = vec2( ft_face->size->metrics.x_ppem / em_size, ft_face->size->metrics.y_ppem / em_size);
    
    precache("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ;:'\",<.>/?-_=+!@#$%^&*()\\ ");
}

void FXGlyphSet::precache(const std::string& chars) {

    FTUnicodeStringItr<unsigned char> precache_glyphs((const unsigned char*)chars.c_str());

    unsigned int chr;

    //add to bitmap without updating textures until the end
    pre_caching = true;

    while (*precache_glyphs) {
        chr  = *precache_glyphs++;
        getGlyph(chr);
    }

    pre_caching = false;

    for(std::vector<FXGlyphPage*>::iterator it = pages.begin(); it != pages.end(); it++) {
        (*it)->updateTexture();
    }

}

FXGlyph* FXGlyphSet::getGlyph(unsigned int chr) {
    std::map<unsigned int, FXGlyph*>::iterator it;

    if((it = glyphs.find(chr)) != glyphs.end()) return it->second;

    //if new

    FXGlyph* glyph = new FXGlyph(this, chr);

    // paint glyph to next page it will fit on

    FXGlyphPage* page = 0;

    if(!pages.empty()) page = pages.back();

    //page is full, create new page
    if(page == 0 || !page->addGlyph(glyph)) {

        //allocate page using maximum allowed texture size
        GLint max_texture_size;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
        max_texture_size = std::min( 512, max_texture_size );

        page = new FXGlyphPage(max_texture_size, max_texture_size);

        pages.push_back(page);

        if(!page->addGlyph(glyph)) {
            throw FXFontException(glyph->set->getFTFace()->family_name);
        }
    }

    max_height = glm::max( glyph->getHeight(), max_height );

    //update the texture unless this is the precaching process
    if(!pre_caching) page->updateTexture();

    glyphs[chr] = glyph;

    return glyph;
}

float FXGlyphSet::getMaxWidth() const {
    return ft_face->size->metrics.max_advance / 64.0f;
}

float FXGlyphSet::getMaxHeight() const {
    return max_height;
}

float FXGlyphSet::getAscender() const {
    return getFTFace()->ascender * unit_scale.y;
}

float FXGlyphSet::getDescender() const {
    return getFTFace()->descender * unit_scale.y;
}

float FXGlyphSet::getWidth(const std::string& text) {

    FTUnicodeStringItr<unsigned char> unicode_text((const unsigned char*)text.c_str());

    float width = 0.0;
    unsigned int chr;

    while (*unicode_text) {
        chr = *unicode_text++;

        FXGlyph* glyph = getGlyph(chr);
        width += glyph->getAdvance().x;
    }

    return width;
}

void FXGlyphSet::drawToVBO(vec2& cursor, const std::string& text, const vec4& colour) {
    FTUnicodeStringItr<unsigned char> unicode_text((const unsigned char*)text.c_str());

    unsigned int chr;

    while (*unicode_text) {
        chr = *unicode_text++;

        FXGlyph* glyph = getGlyph(chr);
        glyph->drawToVBO(fontmanager.font_vbo, cursor, colour);
        cursor += glyph->getAdvance();
    }
}

void FXGlyphSet::draw(const std::string& text) {

    FTUnicodeStringItr<unsigned char> unicode_text((const unsigned char*)text.c_str());

    GLuint textureid = -1;

    unsigned int chr;

    vec2 pos;

    std::vector<FXGlyph*> glyphs;
    glyphs.reserve(text.size());

    // iterate over glyphs before drawing to avoid
    // encountering a new glyph while inside the GL draw call

    while (*unicode_text) {
        chr = *unicode_text++;

        if(chr == '\t') {
             FXGlyph* glyph = getGlyph('M');
             pos += glyph->getAdvance() * tab_width;
             continue;
        }
        FXGlyph* glyph = getGlyph(chr);

        glyphs.push_back(glyph);
    }

    for(auto glyph : glyphs) {
        if(glyph->page->texture->textureid != textureid) {
            if(textureid != -1) glEnd();
            textureid = glyph->page->texture->textureid;
            glBindTexture(GL_TEXTURE_2D, textureid);
            glBegin(GL_QUADS);
        }

        glyph->draw(pos);

        pos += glyph->getAdvance();
    }

    if(textureid != -1) glEnd();
}

void FXGlyphSet::drawPages() {

    glPushMatrix();

    for(std::vector<FXGlyphPage*>::iterator it = pages.begin(); it != pages.end(); it++) {
        FXGlyphPage* page = *it;
        page->texture->bind();

        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f);
            glVertex2f(0.0f, 0.0f);

            glTexCoord2f(1.0f, 0.0f);
            glVertex2f(page->texture->w, 0.0f);

            glTexCoord2f(1.0f, 1.0f);
            glVertex2f(page->texture->w, page->texture->h);

            glTexCoord2f(0.0f, 1.0f);
            glVertex2f(0.0f, page->texture->h);
        glEnd();

        glTranslatef(page->texture->w, 0.0f, 0.0f);
    }

    glPopMatrix();
}

//FXFont

FXFont::FXFont() {
    glyphset = 0;
}

FXFont::FXFont(FXGlyphSet* glyphset) {
    this->glyphset = glyphset;
    init();
}

void FXFont::init() {
    shadow          = false;
    shadow_strength = 0.7;
    shadow_offset   = vec2(1.0, 1.0);
    round           = false;
    align_right     = false;
    align_top       = true;

    colour          = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    shadow_colour   = vec4(0.0f, 0.0f, 0.0f, shadow_strength);
}

void FXFont::roundCoordinates(bool round) {
    this->round = round;
}

void FXFont::alignRight(bool align_right) {
    this->align_right = align_right;
}

void FXFont::alignTop(bool align_top) {
    this->align_top = align_top;
}

void FXFont::shadowStrength(float s) {
    shadow_strength = s;
    shadow_colour.w = colour.w * shadow_strength;
}

void FXFont::setColour(const vec4& colour) {
    this->colour = colour;
    shadow_colour.w = colour.w * shadow_strength;
}

void FXFont::setAlpha(float alpha) {
    colour.w = alpha;
    shadow_colour.w = shadow_strength * alpha;
}

void FXFont::shadowOffset(float x, float y) {
    shadow_offset = vec2(x,y);
}

void FXFont::dropShadow(bool shadow) {
    this->shadow = shadow;
}

const std::string& FXFont::getFontFile() const {
    return glyphset->getFontFile();
}

float FXFont::getMaxWidth() const {
    return glyphset->getMaxWidth();
}

float FXFont::getMaxHeight() const {
    return glyphset->getMaxHeight();
}

int FXFont::getFontSize() const {
    return glyphset->getSize();
}

float FXFont::getWidth(const std::string& text) const {
    return glyphset->getWidth(text);
}

float FXFont::getHeight() const {
    return glm::ceil(getAscender() + getDescender());
}

float FXFont::getAscender() const {
    return glyphset->getAscender();
}

float FXFont::getDescender() const {
    return glyphset->getDescender();
}

void FXFont::render(float x, float y, const std::string& text, const vec4& colour) const{

    if(fontmanager.use_vbo) {
        vec2 cursor_start(x,y);
        glyphset->drawToVBO(cursor_start, text, colour);
        return;
    }

    glColor4fv(glm::value_ptr(colour));

    glPushMatrix();

       glTranslatef(x,y,0.0f);

       glyphset->draw(text);

    glPopMatrix();
}

void FXFont::print(float x, float y, const char *str, ...) const{
    char buf[4096];

    va_list vl;

    va_start (vl, str);
    vsnprintf (buf, 4096, str, vl);
    va_end (vl);

    std::string text = std::string(buf);

    draw(x, y, text);
}

void FXFont::draw(float x, float y, const std::string& text) const {

    if(align_right) {
        x -= getWidth(text);
    }

    if(align_top) {
        y += getHeight();
    }

    if(round) {
        x = roundf(x);
        y = roundf(y);
    }

    //buffered fonts need to do shadow in a shader pass
    if(shadow && !fontmanager.use_vbo) {
        render(x + shadow_offset.x, y + shadow_offset.y, text, shadow_colour);
    }

    render(x, y, text, colour);
}

void FXFont::drawGlyphes() {
    if(glyphset) glyphset->drawPages();
}

// FXFontManager
FXFontManager::FXFontManager() {
    library = 0;
}

void FXFontManager::init() {
    if(FT_Init_FreeType( &library ))
        throw FXFontException("Failed to init FreeType");
    use_vbo = false;
}

void FXFontManager::unload() {
    font_vbo.unload();
}

void FXFontManager::reload() {
}

void FXFontManager::startBuffer() {
    font_vbo.reset();
    use_vbo = true;
}

void FXFontManager::commitBuffer() {
    font_vbo.update();
    use_vbo = false;
}

void FXFontManager::drawBuffer() {
    font_vbo.draw();
}

void FXFontManager::destroy() {
    if(library != 0) FT_Done_FreeType(library);
}

void FXFontManager::setDir(std::string font_dir) {
    this->font_dir = font_dir;
}

void FXFontManager::purge() {

    for(std::map<std::string,fontSizeMap*>::iterator it = fonts.begin(); it!=fonts.end();it++) {
        fontSizeMap* sizemap = it->second;

        for(fontSizeMap::iterator ft_it = sizemap->begin(); ft_it != sizemap->end(); ft_it++) {
            delete ft_it->second;
        }
        delete sizemap;
    }

    fonts.clear();
}

FXFont FXFontManager::grab(std::string font_file, int size, int dpi, FT_Int32 ft_flags) {

    if(font_dir.size()>0 && !(font_file.size() > 2 && font_file[1] == ':') && !(font_file.size() > 1 && font_file[0] == '/')) {
        font_file = font_dir + font_file;
    }

    //sprintf(buf, "%s:%i", font_file.c_str(), size);
    //std::string font_key = std::string(buf);

    fontSizeMap* sizemap = fonts[font_file];

    if(!sizemap) {
        sizemap = fonts[font_file] = new fontSizeMap;
    }

    fontSizeMap::iterator ft_it = sizemap->find(size);

    FXGlyphSet* glyphset;

    if(ft_it == sizemap->end()) {
        glyphset = new FXGlyphSet(library, font_file.c_str(), size, dpi, ft_flags);
        sizemap->insert(std::pair<int,FXGlyphSet*>(size,glyphset));
    } else {
        glyphset = ft_it->second;
    }

    return FXFont(glyphset);
}

