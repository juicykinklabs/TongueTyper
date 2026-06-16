#!/usr/bin/env python3
"""
Upscale 120x120 PNGs to 240x240 BMPs.

- Input:  folder of 120x120 PNG files
- Output: 240x240 BMP files, 24-bit RGB (no color profile, no compression)
          Alpha channel is composited onto a white background before export.
- Upscale filter: nearest-neighbor (NEAREST) preserves hard pixel edges;
  change to Image.LANCZOS for smooth/anti-aliased upscaling.

Usage:
    python upscale_to_bmp.py <input_folder> [output_folder]

    output_folder defaults to <input_folder>/output_bmp/
"""

import sys
import os
from pathlib import Path

try:
    from PIL import Image
except ImportError:
    sys.exit(
        "Pillow is not installed. Run:  pip install Pillow\n"
        "then try again."
    )

# ── configuration ────────────────────────────────────────────────────────────
INPUT_SIZE  = (120, 120)   # expected source dimensions (set to None to skip check)
OUTPUT_SIZE = (240, 240)   # target dimensions
RESAMPLE    = Image.Resampling.NEAREST  # NEAREST = pixel-perfect; LANCZOS = smooth
# ─────────────────────────────────────────────────────────────────────────────


def upscale_png_to_bmp(input_folder: Path, output_folder: Path) -> None:
    output_folder.mkdir(parents=True, exist_ok=True)

    png_files = sorted(input_folder.glob("*.png"))
    if not png_files:
        print(f"No PNG files found in: {input_folder}")
        return

    ok = skipped = errors = 0

    for src in png_files:
        try:
            with Image.open(src) as img:
                # Optional size sanity-check
                if INPUT_SIZE and img.size != INPUT_SIZE:
                    print(f"  SKIP  {src.name}  (size {img.size}, expected {INPUT_SIZE})")
                    skipped += 1
                    continue

                # Strip all embedded metadata / color profiles before conversion
                img_clean = Image.new(img.mode, img.size)
                img_clean.putdata(list(img.getdata()))

                # Upscale
                img_up = img_clean.resize(OUTPUT_SIZE, resample=RESAMPLE)

                # Composite onto a white background, then convert to 24-bit RGB.
                # This flattens any alpha channel so the BMP is plain BI_RGB
                # (3 bytes/pixel, no transparency, no BI_BITFIELDS header).
                white = Image.new("RGB", OUTPUT_SIZE, (255, 255, 255))
                if img_up.mode == "RGBA":
                    white.paste(img_up, mask=img_up.split()[3])  # use alpha as mask
                elif img_up.mode == "RGB":
                    white.paste(img_up)
                else:
                    rgba = img_up.convert("RGBA")
                    white.paste(rgba, mask=rgba.split()[3])
                img_out = white  # guaranteed RGB, no alpha

                dst = output_folder / (src.stem + ".bmp")

                # Save as BMP — Pillow writes uncompressed BI_RGB for RGB images;
                # no ICC profile is embedded.
                img_out.save(dst, format="BMP")

                print(f"  OK    {src.name}  →  {dst.name}")
                ok += 1

        except Exception as exc:
            print(f"  ERROR {src.name}: {exc}")
            errors += 1

    print(
        f"\nDone. {ok} converted, {skipped} skipped (wrong size), {errors} errors."
        f"\nOutput folder: {output_folder}"
    )


def main():
    if len(sys.argv) < 2:
        print(__doc__)
        sys.exit(1)

    input_folder  = Path(sys.argv[1]).expanduser().resolve()
    output_folder = (
        Path(sys.argv[2]).expanduser().resolve()
        if len(sys.argv) >= 3
        else input_folder / "output_bmp"
    )

    if not input_folder.is_dir():
        sys.exit(f"Input folder not found: {input_folder}")

    print(f"Input  folder : {input_folder}")
    print(f"Output folder : {output_folder}")
    print(f"Resample      : {RESAMPLE.name}")
    print()

    upscale_png_to_bmp(input_folder, output_folder)


if __name__ == "__main__":
    main()