import math


def rotate(v, angle):
    return [
        v[0] * math.cos(math.radians(angle)) - v[1] * math.sin(math.radians(angle)),
        v[0] * math.sin(math.radians(angle)) + v[1] * math.cos(math.radians(angle)),
    ]


v_start = [0.0, 0.5]
vertices = []
vertices.append((None, [0.0, 0.0]))
for i in range(10):
    inner = 1.0 if i % 2 == 0 else 1.0 / 2.5
    v = [v_start[0] * inner, v_start[1] * inner]
    vertices.append((inner, v))
    v_start = rotate(v_start, 360 / 10)

print("FB_CONSTANT float3 STAR_VERTICES[11] = {")
for i, v in enumerate(vertices):
    print(f"  float3({v[1][0]:.03}f, {v[1][1]:.03}f, 0.0f),")
print("};")

print("FB_CONSTANT uint16_t STAR_INDICES[3 * 10] = {")
for i in range(10):
    print(f"  0, {i+1}, {(i+1)%10+1}, //")
print("};")
