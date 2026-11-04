# Batched ray tests and indexed raster spans

These instructions use a descriptor in VM memory and read their source data directly from a Steg `file`. The file may be an in-memory vector. Descriptor entries are 32-bit words in VM memory order. Write floating-point entries through a `float @` view. Offsets are in bytes.

## RAYCAST_SEGMENTS

```text
RAYCAST_SEGMENTS result, descriptor, source
```

```steg
uint32 hit = raycast_segments(descriptor, source);
```

The Steg parameters are `uint32 @` and `file`. The result is the zero-based index of the nearest segment, or `4294967295` when there is no hit. Equal distances preserve source order. The ray is `origin + t * direction`. The segment is `start + u * delta`.

| Word | Value |
| --- | --- |
| 0–3 | Source offset, segment count, byte stride, flags |
| 4–7 | Origin X/Y, direction X/Y as floats |
| 8–10 | Exclusive minimum and maximum t, determinant tolerance as floats |
| 11–12 | Output t and u as floats, written on hit |

Each segment starts with X, Y, delta X and delta Y as four little-endian float32 values. Flag 1 instead reads two little-endian words, each with biased signed 16-bit coordinates (low X, high Y, subtract 32768 from each). The first word contains the start and the second the delta. Flag 2 enables both face orientations. By default the determinant must be below minus the tolerance.

## RASTER_INDEXED_SPAN

```text
RASTER_INDEXED_SPAN descriptor, source, framebuffer
```

```steg
raster_indexed_span(descriptor, source, fb);
```

The Steg parameters are `uint32 @`, `file` and `framebuffer`. The instruction writes RGBA pixels directly to the framebuffer. Framebuffer coordinates are expressed as byte offsets in its backing buffer.

| Word | Value |
| --- | --- |
| 0–3 | Texture offset, palette offset, texture width and height |
| 4–6 | First destination byte offset, signed destination byte step, sample count |
| 7–10 | Initial U/V and increments dU/dV as floats |
| 11–13 | Flags, RGBA fallback word, horizontal pixel repeat from 1 to 4 |
| 14–15 | Start row and signed anchor row for flag 2 |
| 16 | Output number of destination pixels written, including repetitions |

The source texture is column-major and stores little-endian 16-bit indices in the range 0–256. The palette contains 257 four-byte RGBA entries. Index 0 uses the fallback word (red in the high byte) or leaves the destination untouched when flag 4 is set.

By default sampling truncates coordinates toward zero and clamps them to the texture bounds. Flag 1 wraps coordinates. Flag 2 enables anchored vertical runs from the first texture column. It requires positive dV and uses the phase `(row - anchor) * dV + V` and a wrapped, floored texture row. This mode preserves the original column renderer's rounding at sample boundaries.

Flag 8 enables projective sampling and takes priority over flag 2. In this mode the descriptor has three additional words.

| Word | Value |
| --- | --- |
| 17–18 | Horizon row and distance factor as floats |
| 19 | Base shade from 0 to 60 |

Each row samples `(U + dU * d, V + dV * d)`, with `d = factor / (row - horizon)`. A denominator whose magnitude is below 0.5 is replaced by 0.5. The palette offset addresses overlapping banks with a 1024-byte stride and 32 shade levels. The bank is `clamp(baseShade - 80 / (1 + min(trunc(abs(d)/16), 127)), 0, 31)`, using integer division.
