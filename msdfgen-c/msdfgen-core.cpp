#include "msdfgen-core.h"
#include "utils.h"

#include <msdfgen/msdfgen.h>

using namespace msdfgen;

const msdfgen_GeneratorConfig msdfgen_GeneratorConfig_default =
    msdfgen_GeneratorConfig{true};
const msdfgen_ErrorCorrectionConfig msdfgen_ErrorCorrectionConfig_default =
    msdfgen_ErrorCorrectionConfig{
        msdfgen_EDGE_PRIORITY, msdfgen_CHECK_DISTANCE_AT_EDGE,
        1.11111111111111111, 1.11111111111111111, (unsigned char *)0};
const msdfgen_MSDFGeneratorConfig msdfgen_MSDFGeneratorConfig_default =
    msdfgen_MSDFGeneratorConfig{true, msdfgen_ErrorCorrectionConfig_default};

void msdfgen_generateSDF(const msdfgen_BitmapRef *output_r,
                         const msdfgen_Shape *shape,
                         const msdfgen_SDFTransformation *transformation,
                         const msdfgen_GeneratorConfig *config) {
    generateSDF(PTR_TO_BITMAP(float, 1, output_r),
                PTR_TO_CONST_REF(Shape, shape),
                PTR_TO_CONST_REF(SDFTransformation, transformation),
                PTR_TO_CONST_REF(GeneratorConfig, config));
}

void msdfgen_generatePSDF(const msdfgen_BitmapRef *output_r,
                          const msdfgen_Shape *shape,
                          const msdfgen_SDFTransformation *transformation,
                          const msdfgen_GeneratorConfig *config) {
    generatePSDF(PTR_TO_BITMAP(float, 1, output_r),
                 PTR_TO_CONST_REF(Shape, shape),
                 PTR_TO_CONST_REF(SDFTransformation, transformation),
                 PTR_TO_CONST_REF(GeneratorConfig, config));
}

void msdfgen_generateMSDF(const msdfgen_BitmapRef *output_rgb,
                          const msdfgen_Shape *shape,
                          const msdfgen_SDFTransformation *transformation,
                          const msdfgen_MSDFGeneratorConfig *config) {
    generateMSDF(PTR_TO_BITMAP(float, 3, output_rgb),
                 PTR_TO_CONST_REF(Shape, shape),
                 PTR_TO_CONST_REF(SDFTransformation, transformation),
                 PTR_TO_CONST_REF(MSDFGeneratorConfig, config));
}

void msdfgen_generateMTSDF(const msdfgen_BitmapRef *output_rgba,
                           const msdfgen_Shape *shape,
                           const msdfgen_SDFTransformation *transformation,
                           const msdfgen_MSDFGeneratorConfig *config) {
    generateMTSDF(PTR_TO_BITMAP(float, 4, output_rgba),
                  PTR_TO_CONST_REF(Shape, shape),
                  PTR_TO_CONST_REF(SDFTransformation, transformation),
                  PTR_TO_CONST_REF(MSDFGeneratorConfig, config));
}

void msdfgen_generateSDF_legacy(const msdfgen_BitmapRef *output_r,
                                const msdfgen_Shape *shape, msdfgen_Range range,
                                msdfgen_Vector2 scale,
                                msdfgen_Vector2 translate) {
    generateSDF_legacy(PTR_TO_BITMAP(float, 1, output_r),
                       PTR_TO_CONST_REF(Shape, shape), RANGE_CAST(range),
                       VEC2_CAST(scale), VEC2_CAST(translate));
}

void msdfgen_generatePSDF_legacy(const msdfgen_BitmapRef *output_r,
                                 const msdfgen_Shape *shape,
                                 msdfgen_Range range, msdfgen_Vector2 scale,
                                 msdfgen_Vector2 translate) {
    generatePSDF_legacy(PTR_TO_BITMAP(float, 1, output_r),
                        PTR_TO_CONST_REF(Shape, shape), RANGE_CAST(range),
                        VEC2_CAST(scale), VEC2_CAST(translate));
}

void msdfgen_generatePseudoSDF_legacy(const msdfgen_BitmapRef *output_r,
                                      const msdfgen_Shape *shape,
                                      msdfgen_Range range,
                                      msdfgen_Vector2 scale,
                                      msdfgen_Vector2 translate) {
    generatePseudoSDF_legacy(PTR_TO_BITMAP(float, 1, output_r),
                             PTR_TO_CONST_REF(Shape, shape), RANGE_CAST(range),
                             VEC2_CAST(scale), VEC2_CAST(translate));
}

void msdfgen_generateMSDF_legacy(
    const msdfgen_BitmapRef *output_rgb, const msdfgen_Shape *shape,
    msdfgen_Range range, msdfgen_Vector2 scale, msdfgen_Vector2 translate,
    const msdfgen_ErrorCorrectionConfig *errorCorrectionConfig) {
    generateMSDF_legacy(
        PTR_TO_BITMAP(float, 3, output_rgb), PTR_TO_CONST_REF(Shape, shape),
        RANGE_CAST(range), VEC2_CAST(scale), VEC2_CAST(translate),
        PTR_TO_CONST_REF(ErrorCorrectionConfig, errorCorrectionConfig));
}

void msdfgen_generateMTSDF_legacy(
    const msdfgen_BitmapRef *output_rgba, const msdfgen_Shape *shape,
    msdfgen_Range range, msdfgen_Vector2 scale, msdfgen_Vector2 translate,
    const msdfgen_ErrorCorrectionConfig *errorCorrectionConfig) {
    generateMTSDF_legacy(
        PTR_TO_BITMAP(float, 4, output_rgba), PTR_TO_CONST_REF(Shape, shape),
        RANGE_CAST(range), VEC2_CAST(scale), VEC2_CAST(translate),
        PTR_TO_CONST_REF(ErrorCorrectionConfig, errorCorrectionConfig));
}

msdfgen_Shape *msdfgen_createShape() {
    Shape *shape = new Shape();
    return (msdfgen_Shape *)shape;
}

void msdfgen_destroyShape(msdfgen_Shape *shape) { delete (Shape *)shape; }

void msdfgen_normalizeShape(msdfgen_Shape *shape) {
    ((Shape *)shape)->normalize();
}

void msdfgen_setShapeInverseYAxis(msdfgen_Shape *shape, bool inverseYAxis) {
    ((Shape *)shape)->inverseYAxis = inverseYAxis;
}

msdfgen_ShapeBounds msdfgen_getShapeBounds(const msdfgen_Shape *shape,
                                           double border, double miterLimit,
                                           int polarity) {
    Shape::Bounds bounds =
        ((Shape *)shape)->getBounds(border, miterLimit, polarity);
    return msdfgen_ShapeBounds{bounds.l, bounds.b, bounds.r, bounds.t};
}

void msdfgen_shapeOrientContours(msdfgen_Shape *shape) {
    ((Shape *)shape)->orientContours();
}

void msdfgen_edgeColoringSimple(msdfgen_Shape *shape, double angleThreshold,
                                unsigned long long seed) {
    edgeColoringSimple(PTR_TO_REF(Shape, shape), angleThreshold, seed);
}

void msdfgen_edgeColoringInkTrap(msdfgen_Shape *shape, double angleThreshold,
                                 unsigned long long seed) {
    edgeColoringInkTrap(PTR_TO_REF(Shape, shape), angleThreshold, seed);
}

void msdfgen_edgeColoringByDistance(msdfgen_Shape *shape, double angleThreshold,
                                    unsigned long long seed) {
    edgeColoringByDistance(PTR_TO_REF(Shape, shape), angleThreshold, seed);
}
