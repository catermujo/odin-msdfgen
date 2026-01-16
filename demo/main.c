#include "../bindings/msdfgen-core.h"
#include "../bindings/msdfgen-ext.h"

#include <assert.h>

int main() {
    msdfgen_FreetypeHandle* ft = msdfgen_initializeFreetype();
    assert(ft);

    msdfgen_FontHandle* font = msdfgen_loadFont(ft, "LiberationMono.ttf");
    assert(font);

    msdfgen_Shape* shape = msdfgen_createShape();
            
    if (msdfgen_loadGlyph(shape, font, 'A', msdfgen_FONT_SCALING_EM_NORMALIZED, NULL)) {
        msdfgen_normalizeShape(shape);
        msdfgen_edgeColoringSimple(shape, 3.0, 0);

        float msdf[32 * 32 * 3];
        msdfgen_BitmapRef msdf_ref = { msdf, 32, 32 };
        
        const msdfgen_Vector2 scale = { 32.0, 32.0 };
        const msdfgen_Vector2 translation = { 0.125, 0.125 };
        const msdfgen_SDFTransformation transform = { (msdfgen_Projection){scale, translation}, msdfgen_symmetricalDistanceMapping(0.125) };
        msdfgen_generateMSDF(&msdf_ref, shape, &transform, &msdfgen_MSDFGeneratorConfig_default);
        msdfgen_savePng_rgb(&msdf_ref, "output.png");
    }

    msdfgen_destroyShape(shape);
    msdfgen_destroyFont(font);
    msdfgen_deinitializeFreetype(ft);

	return 0;
}