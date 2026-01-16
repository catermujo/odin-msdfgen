package demo

import msdfgen "../"

main :: proc() {
    ft := msdfgen.initializeFreetype()
    assert(ft != nil)
    defer msdfgen.deinitializeFreetype(ft)

    font := msdfgen.loadFont(ft, "LiberationMono.ttf")
    assert(font != nil)
    defer msdfgen.destroyFont(font)

    shape := msdfgen.createShape()
    defer msdfgen.destroyShape(shape)

    assert(msdfgen.loadGlyph(shape, font, 'A', .em_normalized))
    msdfgen.normalizeShape(shape)

    msdfgen.edgeColoringSimple(shape, 3)

    msdf: [32 * 32 * 3]f32
    msdf_ref: msdfgen.BitmapRef(f32, 3) = {raw_data(&msdf), 32, 32}

    msdfgen.generateMSDF(msdf_ref, shape, {{{32.0, 32.0}, {0.125, 0.125}}, msdfgen.sym_dist(0.125)})

    msdfgen.savePng_rgb(msdf_ref, "output.png")
}
