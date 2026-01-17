#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif
	typedef struct {
		double x, y;
	} msdfgen_Vector2;

	typedef struct {
		double lower, upper;
	} msdfgen_Range;

	inline msdfgen_Range msdfgen_symmetricalRange(double symmetricalWidth) { 
		msdfgen_Range range = { -.5 * symmetricalWidth, .5 * symmetricalWidth };
		return range;
	}

	typedef struct {
		msdfgen_Vector2 scale, translate;
	} msdfgen_Projection;

	/// Linear transformation of signed distance values.
	typedef struct {
		double scale;
		double translate;
	} msdfgen_DistanceMapping;

	inline msdfgen_DistanceMapping msdfgen_DistanceMappingFromRange(msdfgen_Range range) { 
		msdfgen_DistanceMapping mapping = {1.0 / (range.upper - range.lower), -range.lower}; 
		return mapping;
	}
	inline msdfgen_DistanceMapping msdfgen_symmetricalDistanceMapping(double symmetricalWidth) { return msdfgen_DistanceMappingFromRange(msdfgen_symmetricalRange(symmetricalWidth)); }

	/**
	 * Full signed distance field transformation specifies both spatial transformation (Projection)
	 * as well as distance value transformation (DistanceMapping).
	 */
	typedef struct {
		msdfgen_Projection projection;
		msdfgen_DistanceMapping distanceMapping;
	} msdfgen_SDFTransformation;

	typedef struct {
		/// Specifies whether to use the version of the algorithm that supports overlapping contours with the same winding. May be set to false to improve performance when no such contours are present.
		bool overlapSupport;
	} msdfgen_GeneratorConfig;

	extern const msdfgen_GeneratorConfig msdfgen_GeneratorConfig_default;

	/// Mode of operation.
	typedef enum {
		/// Skips error correction pass.
		msdfgen_DISABLED,
		/// Corrects all discontinuities of the distance field regardless if edges are adversely affected.
		msdfgen_INDISCRIMINATE,
		/// Corrects artifacts at edges and other discontinuous distances only if it does not affect edges or corners.
		msdfgen_EDGE_PRIORITY,
		/// Only corrects artifacts at edges.
		msdfgen_EDGE_ONLY
	} msdfgen_Mode;

	typedef enum {
		/// Never computes exact shape distance.
		msdfgen_DO_NOT_CHECK_DISTANCE,
		/// Only computes exact shape distance at edges. Provides a good balance between speed and precision.
		msdfgen_CHECK_DISTANCE_AT_EDGE,
		/// Computes and compares the exact shape distance for each suspected artifact.
		msdfgen_ALWAYS_CHECK_DISTANCE
	} msdfgen_DistanceCheckMode;

	/// The configuration of the MSDF error correction pass.
	typedef struct {
		
		msdfgen_Mode mode;
		msdfgen_DistanceCheckMode distanceCheckMode;

		/// The minimum ratio between the actual and maximum expected distance delta to be considered an error.
		double minDeviationRatio;
		/// The minimum ratio between the pre-correction distance error and the post-correction distance error. Has no effect for DO_NOT_CHECK_DISTANCE.
		double minImproveRatio;
		/// An optional buffer to avoid dynamic allocation. Must have at least as many bytes as the MSDF has pixels.
		unsigned char* buffer;

	} msdfgen_ErrorCorrectionConfig;

	extern const msdfgen_ErrorCorrectionConfig msdfgen_ErrorCorrectionConfig_default;

	typedef struct {
		/// Specifies whether to use the version of the algorithm that supports overlapping contours with the same winding. May be set to false to improve performance when no such contours are present.
		bool overlapSupport;
		/// Configuration of the error correction pass.
		msdfgen_ErrorCorrectionConfig errorCorrection;
	} msdfgen_MSDFGeneratorConfig;

	extern const msdfgen_MSDFGeneratorConfig msdfgen_MSDFGeneratorConfig_default;

	typedef struct {
		float* pixels;
		int width, height;
	} msdfgen_BitmapRef;

	typedef struct {
		const float* pixels;
		int width, height;
	} msdfgen_BitmapConstRef;

	typedef struct {
		const unsigned char* pixels;
		int width, height;
	} msdfgen_BitmapConstRefByte;

	typedef struct msdfgen_Shape msdfgen_Shape;

	typedef struct {
		double l, b, r, t;
	} msdfgen_ShapeBounds;

	/// Generates a conventional single-channel signed distance field.
	void msdfgen_generateSDF(const msdfgen_BitmapRef* output_r, const msdfgen_Shape* shape, const msdfgen_SDFTransformation* transformation, const msdfgen_GeneratorConfig* config);
	
	/// Generates a single-channel signed perpendicular distance field.
	void msdfgen_generatePSDF(const msdfgen_BitmapRef* output_r, const msdfgen_Shape* shape, const msdfgen_SDFTransformation* transformation, const msdfgen_GeneratorConfig* config);
	
	/// Generates a multi-channel signed distance field. Edge colors must be assigned first! (See edgeColoringSimple)
	void msdfgen_generateMSDF(const msdfgen_BitmapRef* output_rgb, const msdfgen_Shape* shape, const msdfgen_SDFTransformation* transformation, const msdfgen_MSDFGeneratorConfig* config);
	
	/// Generates a multi-channel signed distance field with true distance in the alpha channel. Edge colors must be assigned first.
	void msdfgen_generateMTSDF(const msdfgen_BitmapRef* output_rgba, const msdfgen_Shape* shape, const msdfgen_SDFTransformation* transformation, const msdfgen_MSDFGeneratorConfig* config);

	// Original simpler versions of the previous functions, which work well under normal circumstances, but cannot deal with overlapping contours.
	void msdfgen_generateSDF_legacy(const msdfgen_BitmapRef* output_r, const msdfgen_Shape* shape, msdfgen_Range range, msdfgen_Vector2 scale, msdfgen_Vector2 translate);
	void msdfgen_generatePSDF_legacy(const msdfgen_BitmapRef* output_r, const msdfgen_Shape* shape, msdfgen_Range range, msdfgen_Vector2 scale, msdfgen_Vector2 translate);
	void msdfgen_generatePseudoSDF_legacy(const msdfgen_BitmapRef* output_r, const msdfgen_Shape* shape, msdfgen_Range range, msdfgen_Vector2 scale, msdfgen_Vector2 translate);
	void msdfgen_generateMSDF_legacy(const msdfgen_BitmapRef* output_rgb, const msdfgen_Shape* shape, msdfgen_Range range, msdfgen_Vector2 scale, msdfgen_Vector2 translate, const msdfgen_ErrorCorrectionConfig* errorCorrectionConfig);
	void msdfgen_generateMTSDF_legacy(const msdfgen_BitmapRef* output_rgba, const msdfgen_Shape* shape, msdfgen_Range range, msdfgen_Vector2 scale, msdfgen_Vector2 translate, const msdfgen_ErrorCorrectionConfig* errorCorrectionConfig);


	// Shape.h
	msdfgen_Shape* msdfgen_createShape();
	void msdfgen_destroyShape(msdfgen_Shape* shape);

	void msdfgen_normalizeShape(msdfgen_Shape* shape);
	void msdfgen_setShapeInverseYAxis(msdfgen_Shape* shape, bool inverseYAxis);
	msdfgen_ShapeBounds msdfgen_getShapeBounds(const msdfgen_Shape* shape, double border, double miterLimit, int polarity);
	void msdfgen_shapeOrientContours(msdfgen_Shape* shape);
	bool msdfgen_shapeValidate(msdfgen_Shape* shape);

    /// Adjusts the bounding box to fit the shape border's mitered corners.
    void msdfgen_shapeBoundMiters(msdfgen_Shape* shape, double* xMin, double* yMin, double* xMax, double* yMax, double border, double miterLimit, int polarity);


	// edge-coloring.h
	/** Assigns colors to edges of the shape in accordance to the multi-channel distance field technique.
	 *  May split some edges if necessary.
	 *  angleThreshold specifies the maximum angle (in radians) to be considered a corner, for example 3 (~172 degrees).
	 *  Values below 1/2 PI will be treated as the external angle.
	 */
	void msdfgen_edgeColoringSimple(msdfgen_Shape* shape, double angleThreshold, unsigned long long seed);

	/** The alternative "ink trap" coloring strategy is designed for better results with typefaces
	 *  that use ink traps as a design feature. It guarantees that even if all edges that are shorter than
	 *  both their neighboring edges are removed, the coloring remains consistent with the established rules.
	 */
	void msdfgen_edgeColoringInkTrap(msdfgen_Shape* shape, double angleThreshold, unsigned long long seed);

	/** The alternative coloring by distance tries to use different colors for edges that are close together.
	 *  This should theoretically be the best strategy on average. However, since it needs to compute the distance
	 *  between all pairs of edges, and perform a graph optimization task, it is much slower than the rest.
	 */
	void msdfgen_edgeColoringByDistance(msdfgen_Shape* shape, double angleThreshold, unsigned long long seed);

#ifdef __cplusplus
}
#endif
