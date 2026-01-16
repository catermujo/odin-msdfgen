#pragma once

#define PTR_TO_CONST_REF(type, ptr) (*((const type*)ptr))
#define PTR_TO_REF(type, ptr) (*((type*)ptr))

#define PTR_TO_CONST_BITMAP(type, num, ptr) (*((const BitmapConstRef<type, num>*)ptr))
#define PTR_TO_BITMAP(type, num, ptr) (*((const BitmapRef<type, num>*)ptr))

#define VEC2_CAST(vec) Vector2{ vec.x, vec.y }
#define RANGE_CAST(range) Range{ range.lower, range.upper }