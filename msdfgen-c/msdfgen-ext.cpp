// #include <ft2build.h>
// #include FT_FREETYPE_H
#include "msdfgen-ext.h"
#include "utils.h"

#include <msdfgen/msdfgen-ext.h>

using namespace msdfgen;

const int msdfgen_SVG_IMPORT_FAILURE = SVG_IMPORT_FAILURE;
const int msdfgen_SVG_IMPORT_SUCCESS_FLAG = SVG_IMPORT_SUCCESS_FLAG;
const int msdfgen_SVG_IMPORT_PARTIAL_FAILURE_FLAG =
    SVG_IMPORT_PARTIAL_FAILURE_FLAG;
const int msdfgen_SVG_IMPORT_INCOMPLETE_FLAG = SVG_IMPORT_INCOMPLETE_FLAG;
const int msdfgen_SVG_IMPORT_UNSUPPORTED_FEATURE_FLAG =
    SVG_IMPORT_UNSUPPORTED_FEATURE_FLAG;
const int msdfgen_SVG_IMPORT_TRANSFORMATION_IGNORED_FLAG =
    SVG_IMPORT_TRANSFORMATION_IGNORED_FLAG;

bool msdfgen_resolveShapeGeometry(msdfgen_Shape *shape) {
    return false; // resolveShapeGeometry(PTR_TO_REF(Shape, shape));
}

bool msdfgen_savePng_r8(const msdfgen_BitmapConstRefByte *bitmap_r,
                        const char *filename) {
    return savePng(PTR_TO_CONST_BITMAP(unsigned char, 1, bitmap_r), filename);
}

bool msdfgen_savePng_rgb8(const msdfgen_BitmapConstRefByte *bitmap_rgb,
                          const char *filename) {
    return savePng(PTR_TO_CONST_BITMAP(unsigned char, 3, bitmap_rgb), filename);
}

bool msdfgen_savePng_rgba8(const msdfgen_BitmapConstRefByte *bitmap_rgba,
                           const char *filename) {
    return savePng(PTR_TO_CONST_BITMAP(unsigned char, 4, bitmap_rgba),
                   filename);
}

bool msdfgen_savePng_r(const msdfgen_BitmapConstRef *bitmap_r,
                       const char *filename) {
    return savePng(PTR_TO_CONST_BITMAP(float, 1, bitmap_r), filename);
}

bool msdfgen_savePng_rgb(const msdfgen_BitmapConstRef *bitmap_rgb,
                         const char *filename) {
    return savePng(PTR_TO_CONST_BITMAP(float, 3, bitmap_rgb), filename);
}

bool msdfgen_savePng_rgba(const msdfgen_BitmapConstRef *bitmap_rgba,
                          const char *filename) {
    return savePng(PTR_TO_CONST_BITMAP(float, 4, bitmap_rgba), filename);
}

bool msdfgen_buildShapeFromSvgPath(msdfgen_Shape *shape, const char *pathDef,
                                   double endpointSnapRange) {
    return buildShapeFromSvgPath(PTR_TO_REF(Shape, shape), pathDef,
                                 endpointSnapRange);
}

bool msdfgen_loadSvgShape(msdfgen_Shape *output, const char *filename,
                          int pathIndex, msdfgen_Vector2 *dimensions) {
    return loadSvgShape(PTR_TO_REF(Shape, output), filename, pathIndex,
                        (Vector2 *)dimensions);
}

int msdfgen_loadSvgShape_skia(msdfgen_Shape *output,
                              msdfgen_ShapeBounds *viewBox,
                              const char *filename) {
    return loadSvgShape(PTR_TO_REF(Shape, output),
                        PTR_TO_REF(Shape::Bounds, viewBox), filename);
}

msdfgen_FreetypeHandle *msdfgen_initializeFreetype() {
    return (msdfgen_FreetypeHandle *)initializeFreetype();
}

void msdfgen_deinitializeFreetype(msdfgen_FreetypeHandle *library) {
    deinitializeFreetype((FreetypeHandle *)library);
}

msdfgen_FontHandle *msdfgen_loadFont(msdfgen_FreetypeHandle *library,
                                     const char *filename) {
    return (msdfgen_FontHandle *)loadFont((FreetypeHandle *)library, filename);
}

msdfgen_FontHandle *msdfgen_loadFontData(msdfgen_FreetypeHandle *library,
                                         const unsigned char *data,
                                         int length) {
    return (msdfgen_FontHandle *)loadFontData((FreetypeHandle *)library, data,
                                              length);
}
msdfgen_FontHandle *msdfgen_adoptFreetypeFont(FT_Face face) {
	return (msdfgen_FontHandle *)adoptFreetypeFont(face);
}

void msdfgen_destroyFont(msdfgen_FontHandle *font) {
    destroyFont((FontHandle *)font);
}

bool msdfgen_getFontMetrics(msdfgen_FontMetrics *metrics,
                            msdfgen_FontHandle *font,
                            msdfgen_FontCoordinateScaling coordinateScaling) {
    return getFontMetrics(PTR_TO_REF(FontMetrics, metrics), (FontHandle *)font,
                          (FontCoordinateScaling)coordinateScaling);
}

bool msdfgen_getFontWhitespaceWidth(
    double *spaceAdvance, double *tabAdvance, msdfgen_FontHandle *font,
    msdfgen_FontCoordinateScaling coordinateScaling) {
    return getFontWhitespaceWidth(*spaceAdvance, *tabAdvance,
                                  (FontHandle *)font,
                                  (FontCoordinateScaling)coordinateScaling);
}

bool msdfgen_getGlyphCount(unsigned *output, msdfgen_FontHandle *font) {
    return getGlyphCount(*output, (FontHandle *)font);
}

msdfgen_GlyphIndex *msdfgen_createGlyphIndex() {
    GlyphIndex *shape = new GlyphIndex();
    return (msdfgen_GlyphIndex *)shape;
}
void msdfgen_destroyGlyphIndex(msdfgen_GlyphIndex *glyphIndex) { delete ((GlyphIndex *)glyphIndex); }

bool msdfgen_getGlyphIndex(msdfgen_GlyphIndex *glyphIndex,
                           msdfgen_FontHandle *font,
                           msdfgen_unicode_t unicode) {
    return getGlyphIndex(PTR_TO_REF(GlyphIndex, glyphIndex), (FontHandle *)font,
                         unicode);
}
unsigned msdfgen_getGlyphIndexIndex(msdfgen_GlyphIndex *glyphIndex) {
    return ((GlyphIndex*)glyphIndex)->getIndex();
}

bool msdfgen_loadGlyph(msdfgen_Shape *output, msdfgen_FontHandle *font,
                       msdfgen_GlyphIndex *glyphIndex,
                       msdfgen_FontCoordinateScaling coordinateScaling,
                       double *outAdvance) {
    return loadGlyph(PTR_TO_REF(Shape, output), (FontHandle *)font, PTR_TO_REF(GlyphIndex, glyphIndex),
                     (FontCoordinateScaling)coordinateScaling, outAdvance);
}

bool msdfgen_loadGlyph_unicode(msdfgen_Shape *output, msdfgen_FontHandle *font,
                               msdfgen_unicode_t unicode,
                               msdfgen_FontCoordinateScaling coordinateScaling,
                               double *outAdvance) {
    return loadGlyph(PTR_TO_REF(Shape, output), (FontHandle *)font, unicode,
                     (FontCoordinateScaling)coordinateScaling, outAdvance);
}

bool msdfgen_getKerning(double *output, msdfgen_FontHandle *font,
                        msdfgen_GlyphIndex *glyphIndex1,
                        msdfgen_GlyphIndex *glyphIndex2,
                        msdfgen_FontCoordinateScaling coordinateScaling) {
    return getKerning(*output, (FontHandle *)font, PTR_TO_REF(GlyphIndex, glyphIndex1), PTR_TO_REF(GlyphIndex, glyphIndex2),
                      (FontCoordinateScaling)coordinateScaling);
}

bool msdfgen_getKerning_unicode(
    double *output, msdfgen_FontHandle *font, msdfgen_unicode_t unicode1,
    msdfgen_unicode_t unicode2,
    msdfgen_FontCoordinateScaling coordinateScaling) {
    return getKerning(*output, (FontHandle *)font, unicode1, unicode2,
                      (FontCoordinateScaling)coordinateScaling);
}
