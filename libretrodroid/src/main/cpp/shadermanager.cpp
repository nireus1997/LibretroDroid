/*
 *     Copyright (C) 2019  Filippo Scognamiglio
 *
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "shadermanager.h"

namespace libretrodroid {

const std::string ShaderManager::defaultShaderVertex =
    "attribute vec4 vPosition;\n"
    "attribute vec2 vCoordinate;\n"
    "uniform mediump float vFlipY;\n"
    "uniform mediump float screenDensity;\n"
    "uniform lowp sampler2D texture;\n"
    "uniform mat4 vViewModel;\n"
    "uniform vec2 textureSize;\n"
    "\n"
    "varying mediump float screenMaskStrength;\n"
    "varying vec2 coords;\n"
    "varying vec2 screenCoords;\n"
    "void main() {\n"
    "  coords = vec2(vCoordinate.x, mix(vCoordinate.y, 1.0 - vCoordinate.y, vFlipY));\n"
    "  screenCoords = coords * textureSize;\n"
    "  screenMaskStrength = smoothstep(2.0, 6.0, screenDensity);\n"
    "  gl_Position = vViewModel * vPosition;\n"
    "}\n";

const std::string ShaderManager::defaultShaderFragment =
    "precision mediump float;\n"
    "uniform lowp sampler2D texture;\n"
    "varying vec2 coords;\n"
    "void main() {\n"
    "  vec4 tex = texture2D(texture, coords);"
    "  gl_FragColor = vec4(tex.rgb, 1.0);\n"
    "}\n";

const std::string ShaderManager::crtShaderFragment =
    "#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
    "#define HIGHP highp\n"
    "#else\n"
    "#define HIGHP mediump\n"
    "precision mediump float;\n"
    "#endif\n"
    "\n"
    "uniform HIGHP vec2 textureSize;\n"
    "\n"
    "uniform lowp sampler2D texture;\n"
    "varying HIGHP vec2 coords;\n"
    "varying HIGHP vec2 screenCoords;\n"
    "varying mediump float screenMaskStrength;\n"
    "\n"
    "#define INTENSITY 0.30\n"
    "#define BRIGHTBOOST 0.30\n"
    "\n"
    "void main() {\n"
    "  lowp vec3 texel = texture2D(texture, coords).rgb;\n"
    "  lowp vec3 pixelHigh = ((1.0 + BRIGHTBOOST) - (0.2 * texel)) * texel;\n"
    "  lowp vec3 pixelLow  = ((1.0 - INTENSITY) + (0.1 * texel)) * texel;\n"
    "\n"
    "  HIGHP vec2 coords = fract(screenCoords) * 2.0 - vec2(1.0);\n"
    "\n"
    "  lowp float mask = 1.0 - abs(coords.y);\n"
    "\n"
    "  gl_FragColor = vec4(mix(texel, mix(pixelLow, pixelHigh, mask), screenMaskStrength), 1.0);\n"
    "}\n";

const std::string ShaderManager::lcdShaderFragment =
    "#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
    "#define HIGHP highp\n"
    "#else\n"
    "#define HIGHP mediump\n"
    "precision mediump float;\n"
    "#endif\n"
    "\n"
    "uniform HIGHP vec2 textureSize;\n"
    "uniform lowp sampler2D texture;\n"
    "uniform mediump float screenDensity;\n"
    "\n"
    "varying HIGHP vec2 coords;\n"
    "varying HIGHP vec2 screenCoords;\n"
    "varying mediump float screenMaskStrength;\n"
    "\n"
    "#define INTENSITY 0.25\n"
    "#define BRIGHTBOOST 0.25\n"
    "\n"
    "void main() {\n"
    "  mediump vec2 threshold = vec2(1.0 / screenDensity);\n"
    "  mediump vec2 x = fract(screenCoords);\n"
    "  x = 0.5 * (smoothstep(vec2(0.0), threshold, x) + smoothstep(vec2(1.0) - threshold, vec2(1.0), x));\n"
    "  mediump vec2 sharpCoords = (floor(screenCoords) + x) / textureSize;\n"
    "\n"
    "  lowp vec3 texel = texture2D(texture, sharpCoords).rgb;\n"
    "  lowp vec3 pixelHigh = ((1.0 + BRIGHTBOOST) - (0.2 * texel)) * texel;\n"
    "  lowp vec3 pixelLow  = ((1.0 - INTENSITY) + (0.1 * texel)) * texel;\n"
    "\n"
    "  HIGHP vec2 coords = fract(screenCoords) * 2.0 - vec2(1.0);\n"
    "  coords = coords * coords;\n"
    "\n"
    "  lowp float mask = 1.0 - coords.x - coords.y;\n"
    "\n"
    "  gl_FragColor = vec4(mix(texel, mix(pixelLow, pixelHigh, mask), screenMaskStrength), 1.0);\n"
    "}\n";

const std::string ShaderManager::defaultSharpFragment =
    "#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
    "#define HIGHP highp\n"
    "#else\n"
    "#define HIGHP mediump\n"
    "precision mediump float;\n"
    "#endif\n"
    "\n"
    "precision mediump float;\n"
    "uniform lowp sampler2D texture;\n"
    "uniform HIGHP vec2 textureSize;\n"
    "uniform mediump float screenDensity;\n"
    "\n"
    "varying vec2 coords;\n"
    "varying vec2 screenCoords;\n"
    "\n"
    "void main() {\n"
    "  mediump vec2 threshold = vec2(1.0 / screenDensity);\n"
    "  mediump vec2 x = fract(screenCoords);\n"
    "  x = 0.5 * (smoothstep(vec2(0.0), threshold, x) + smoothstep(vec2(1.0) - threshold, vec2(1.0), x));\n"
    "  mediump vec2 sharpCoords = (floor(screenCoords) + x) / textureSize;\n"
    "\n"
    "  vec4 tex = texture2D(texture, sharpCoords);\n"
    "  gl_FragColor = vec4(tex.rgb, 1.0);\n"
    "}\n";

const std::unordered_map<std::string, std::string> ShaderManager::cutUpscaleParams = {
    { "USE_DYNAMIC_SHARPNESS", "1" },
    { "USE_SHARPENING_BIAS", "1" },
    { "DYNAMIC_SHARPNESS_MIN", "0.10" },
    { "DYNAMIC_SHARPNESS_MAX", "0.30" },
    { "STATIC_SHARPNESS", "0.5" },
    { "USE_FAST_LUMA", "1" },
    { "TRIANGULATION_THRESHOLD", "4.0" },
};

const std::string ShaderManager::cutUpscaleVertex =
    "#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
    "#define HIGHP highp\n"
    "#else\n"
    "#define HIGHP mediump\n"
    "precision mediump float;\n"
    "#endif\n"
    "\n"
    "attribute vec4 vPosition;\n"
    "attribute vec2 vCoordinate;\n"
    "uniform mediump float vFlipY;\n"
    "uniform mediump float screenDensity;\n"
    "uniform lowp sampler2D texture;\n"
    "uniform mat4 vViewModel;\n"
    "uniform HIGHP vec2 textureSize;\n"
    "\n"
    "varying HIGHP vec2 screenCoords;\n"
    "varying HIGHP vec2 c1;\n"
    "varying HIGHP vec2 c2;\n"
    "varying HIGHP vec2 c3;\n"
    "varying HIGHP vec2 c4;\n"
    "\n"
    "void main() {\n"
    "  HIGHP vec2 coords = vec2(vCoordinate.x, mix(vCoordinate.y, 1.0 - vCoordinate.y, vFlipY)) * 1.0001;\n"
    "  screenCoords = coords * textureSize - vec2(0.5);\n"
    "  c1 = (screenCoords) / textureSize;\n"
    "  c2 = (screenCoords + vec2(1.0, 0.0)) / textureSize;\n"
    "  c3 = (screenCoords + vec2(1.0, 1.0)) / textureSize;\n"
    "  c4 = (screenCoords + vec2(0.0, 1.0)) / textureSize;\n"
    "  gl_Position = vViewModel * vPosition;\n"
    "}\n";

const std::string ShaderManager::cutUpscaleFragment =
    "#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
    "#define HIGHP highp\n"
    "#else\n"
    "#define HIGHP mediump\n"
    "precision mediump float;\n"
    "#endif\n"
    "\n"
    "precision mediump float;\n"
    "uniform lowp sampler2D texture;\n"
    "uniform HIGHP vec2 textureSize;\n"
    "uniform mediump float screenDensity;\n"
    "\n"
    "varying HIGHP vec2 screenCoords;\n"
    "varying HIGHP vec2 c1;\n"
    "varying HIGHP vec2 c2;\n"
    "varying HIGHP vec2 c3;\n"
    "varying HIGHP vec2 c4;\n"
    "\n"
    "#if USE_FAST_LUMA\n"
    "lowp float luma(lowp vec3 v) {\n"
    "  return v.g;\n"
    "}\n"
    "#else\n"
    "lowp float luma(lowp vec3 v) {\n"
    "  return dot(v, vec3(0.299, 0.587, 0.114));\n"
    "}\n"
    "#endif\n"
    "lowp float linearStep(lowp float edge0, lowp float edge1, lowp float t) {\n"
    "  return clamp((t - edge0) / (edge1 - edge0), 0.0, 1.0);\n"
    "}\n"
    "lowp float sharpSmooth(lowp float t, lowp float sharpness) {\n"
    "  return linearStep(sharpness, 1.0 - sharpness, t);\n"
    "}\n"
    "lowp vec3 quadBilinear(lowp vec3 a, lowp vec3 b, lowp vec3 c, lowp vec3 d, lowp vec2 p, lowp float sharpness) {\n"
    "  lowp float x = sharpSmooth(p.x, sharpness);\n"
    "  lowp float y = sharpSmooth(p.y, sharpness);\n"
    "  return mix(mix(a, b, x), mix(c, d, x), y);\n"
    "}\n"
    "// Fast computation of barycentric coordinates only in the sub-triangle 1 2 4\n"
    "lowp vec3 fastBarycentric(lowp vec2 p, lowp float sharpness) {\n"
    "  lowp float l0 = sharpSmooth(1.0 - p.x - p.y, sharpness);\n"
    "  lowp float l1 = sharpSmooth(p.x, sharpness);\n"
    "  return vec3(l0, l1, 1.0 - l0 - l1);\n"
    "}\n"
    "\n"
    "lowp vec3 triangleInterpolate(lowp vec3 t1, lowp vec3 t2, lowp vec3 t3, lowp vec3 t4, lowp vec2 c, lowp float sharpness) {\n"
    "  // Alter colors and coordinates to compute the other triangle.\n"
    "  bool altTriangle = 1.0 - c.x < c.y;\n"
    "  lowp vec3 cornerColor = altTriangle ? t3 : t1;\n"
    "  lowp vec2 triangleCoords = altTriangle ? vec2(1.0 - c.y, 1.0 - c.x) : c;\n"
    "  lowp vec3 weights = fastBarycentric(triangleCoords, sharpness);\n"
    "  return weights.x * cornerColor + weights.y * t2 + weights.z * t4;\n"
    "}\n"
    "\n"
    "void main() {\n"
    "  lowp vec3 t1 = texture2D(texture, c1).rgb;\n"
    "  lowp vec3 t2 = texture2D(texture, c2).rgb;\n"
    "  lowp vec3 t3 = texture2D(texture, c3).rgb;\n"
    "  lowp vec3 t4 = texture2D(texture, c4).rgb;\n"
    "\n"
    "  lowp float l1 = luma(t1);\n"
    "  lowp float l2 = luma(t2);\n"
    "  lowp float l3 = luma(t3);\n"
    "  lowp float l4 = luma(t4);\n"
    "\n"
    "#if USE_DYNAMIC_SHARPNESS\n"
    "  lowp float lmax = max(max(l1, l2), max(l3, l4));\n"
    "  lowp float lmin = min(min(l1, l2), min(l3, l4));\n"
    "  lowp float contrast = (lmax - lmin) / (lmax + lmin + 0.05);\n"
    "#if USE_SHARPENING_BIAS\n"
    "  contrast = sqrt(contrast);\n"
    "#endif\n"
    "  lowp float sharpness = mix(DYNAMIC_SHARPNESS_MIN, DYNAMIC_SHARPNESS_MAX, contrast);\n"
    "#else\n"
    "  const lowp float sharpness = STATIC_SHARPNESS;\n"
    "#endif\n"
    "\n"
    "  lowp vec2 pxCoords = fract(screenCoords);\n"
    "\n"
    "  lowp float diagonal1Strength = abs(l1 - l3);\n"
    "  lowp float diagonal2Strength = abs(l2 - l4);\n"
    "\n"
    "  // Alter colors and coordinates to compute the other triangulation.\n"
    "  bool altTriangulation = diagonal1Strength < diagonal2Strength;\n"
    "\n"
    "  lowp vec3 cd = triangleInterpolate(\n"
    "    altTriangulation ? t2 : t1,\n"
    "    altTriangulation ? t3 : t2,\n"
    "    altTriangulation ? t4 : t3,\n"
    "    altTriangulation ? t1 : t4,\n"
    "    altTriangulation ? vec2(pxCoords.y, 1.0 - pxCoords.x) : pxCoords,\n"
    "    sharpness\n"
    "  );\n"
    "\n"
    "  lowp float minDiagonal = min(diagonal1Strength, diagonal2Strength);\n"
    "  lowp float maxDiagonal = max(diagonal1Strength, diagonal2Strength);\n"
    "  bool diagonal = minDiagonal * TRIANGULATION_THRESHOLD + 0.05 < maxDiagonal;\n"
    "\n"
    "  lowp vec3 final = diagonal ? cd : quadBilinear(t1, t2, t4, t3, pxCoords, sharpness);\n"
    "\n"
    "  gl_FragColor = vec4(final, 1.0);\n"
    "}\n";

const std::unordered_map<std::string, std::string> ShaderManager::cut2UpscaleParams = {
    { "USE_DYNAMIC_BLEND", "1" },
    { "BLEND_MIN_CONTRAST_EDGE", "0.25" },
    { "BLEND_MAX_CONTRAST_EDGE", "0.75" },
    { "BLEND_MIN_SHARPNESS", "0.0" },
    { "BLEND_MAX_SHARPNESS", "1.0" },
    { "STATIC_BLEND_SHARPNESS", "1.0" },
    { "EDGE_USE_FAST_LUMA", "0" },
    { "EDGE_MIN_VALUE", "0.03" },
    { "EDGE_MIN_CONTRAST", "2.0" },
    { "LUMA_ADJUST_GAMMA", "0" },
    { "SPLIT_DEMO_VIEW", "0" },
};

const std::string ShaderManager::cut2UpscalePass0Vertex =
    "#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
    "#define HIGHP highp\n"
    "#else\n"
    "#define HIGHP mediump\n"
    "precision mediump float;\n"
    "#endif\n"
    "\n"
    "attribute vec4 vPosition;\n"
    "attribute vec2 vCoordinate;\n"
    "uniform mediump float screenDensity;\n"
    "uniform lowp sampler2D texture;\n"
    "uniform mat4 vViewModel;\n"
    "uniform HIGHP vec2 textureSize;\n"
    "uniform mediump float vFlipY;\n"
    "\n"
    "varying HIGHP vec2 c01;\n"
    "varying HIGHP vec2 c02;\n"
    "varying HIGHP vec2 c04;\n"
    "varying HIGHP vec2 c05;\n"
    "varying HIGHP vec2 c06;\n"
    "varying HIGHP vec2 c07;\n"
    "varying HIGHP vec2 c08;\n"
    "varying HIGHP vec2 c09;\n"
    "varying HIGHP vec2 c10;\n"
    "varying HIGHP vec2 c11;\n"
    "varying HIGHP vec2 c13;\n"
    "varying HIGHP vec2 c14;\n"
    "\n"
    "void main() {\n"
    "  HIGHP vec2 coords = vec2(vCoordinate.x, mix(vCoordinate.y, 1.0 - vCoordinate.y, vFlipY)) * 1.0001;\n"
    "  HIGHP vec2 screenCoords = coords * textureSize - vec2(0.5);\n"
    "  c01 = (screenCoords + vec2(+0.0, -1.0)) / textureSize;\n"
    "  c02 = (screenCoords + vec2(+1.0, -1.0)) / textureSize;\n"
    "  c04 = (screenCoords + vec2(-1.0, +0.0)) / textureSize;\n"
    "  c05 = (screenCoords + vec2(+0.0, +0.0)) / textureSize;\n"
    "  c06 = (screenCoords + vec2(+1.0, +0.0)) / textureSize;\n"
    "  c07 = (screenCoords + vec2(+2.0, +0.0)) / textureSize;\n"
    "  c08 = (screenCoords + vec2(-1.0, +1.0)) / textureSize;\n"
    "  c09 = (screenCoords + vec2(+0.0, +1.0)) / textureSize;\n"
    "  c10 = (screenCoords + vec2(+1.0, +1.0)) / textureSize;\n"
    "  c11 = (screenCoords + vec2(+2.0, +1.0)) / textureSize;\n"
    "  c13 = (screenCoords + vec2(+0.0, +2.0)) / textureSize;\n"
    "  c14 = (screenCoords + vec2(+1.0, +2.0)) / textureSize;\n"
    "  gl_Position = vViewModel * vPosition;\n"
    "}";

const std::string ShaderManager::cut2UpscalePass0Fragment =
    "#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
    "#define HIGHP highp\n"
    "#else\n"
    "#define HIGHP mediump\n"
    "precision mediump float;\n"
    "#endif\n"
    "\n"
    "uniform lowp sampler2D texture;\n"
    "\n"
    "varying HIGHP vec2 c01;\n"
    "varying HIGHP vec2 c02;\n"
    "varying HIGHP vec2 c04;\n"
    "varying HIGHP vec2 c05;\n"
    "varying HIGHP vec2 c06;\n"
    "varying HIGHP vec2 c07;\n"
    "varying HIGHP vec2 c08;\n"
    "varying HIGHP vec2 c09;\n"
    "varying HIGHP vec2 c10;\n"
    "varying HIGHP vec2 c11;\n"
    "varying HIGHP vec2 c13;\n"
    "varying HIGHP vec2 c14;\n"
    "\n"
    "struct Pattern {\n"
    "  lowp float type;\n"
    "  bvec3 flip;\n"
    "  bvec2 cuts;\n"
    "};\n"
    "\n"
    "lowp float luma(lowp vec3 v) {\n"
    "#if EDGE_USE_FAST_LUMA\n"
    "  lowp float result = v.g;\n"
    "#else\n"
    "  lowp float result = dot(v, vec3(0.299, 0.587, 0.114));\n"
    "#endif\n"
    "#if LUMA_ADJUST_GAMMA\n"
    "  result = sqrt(result);\n"
    "#endif\n"
    "  return result;\n"
    "}\n"
    "\n"
    "lowp float maxOf(lowp float a, lowp float b, lowp float c, lowp float d) {\n"
    "  return max(max(a, b), max(c, d));\n"
    "}\n"
    "\n"
    "lowp float minOf(lowp float a, lowp float b, lowp float c, lowp float d) {\n"
    "  return min(min(a, b), min(c, d));\n"
    "}\n"
    "\n"
    "bvec2 hasDiagonal(lowp float a, lowp float b, lowp float c, lowp float d, lowp float e, lowp float f) {\n"
    "  lowp float dab = distance(a, b);\n"
    "  lowp float dac = distance(a, c);\n"
    "  lowp float dbc = distance(b, c);\n"
    "  lowp float dbd = distance(b, d);\n"
    "  lowp float dcd = distance(c, d);\n"
    "  lowp float dce = distance(c, e);\n"
    "  lowp float ddf = distance(d, f);\n"
    "  lowp float ded = distance(e, d);\n"
    "  lowp float def = distance(e, f);\n"
    "\n"
    "  lowp float leftInnerContrast = maxOf(dac, dce, def, dbd);\n"
    "  lowp float leftOuterContrast = minOf(dab, dcd, ddf, ded);\n"
    "  bool leftCut = max(EDGE_MIN_CONTRAST * leftInnerContrast, EDGE_MIN_VALUE) < leftOuterContrast;\n"
    "\n"
    "  lowp float rightInnerContrast = maxOf(dab, dbd, ddf, dce);\n"
    "  lowp float rightOuterContrast = minOf(dac, dcd, def, dbc);\n"
    "  bool rightCut = max(EDGE_MIN_CONTRAST * rightInnerContrast, EDGE_MIN_VALUE) < rightOuterContrast;\n"
    "\n"
    "  return bvec2(leftCut || rightCut, leftCut);\n"
    "}\n"
    "\n"
    "bool hasDiagonal(lowp float a, lowp float b, lowp float c, lowp float d) {\n"
    "  lowp float diff1 = distance(a, d);\n"
    "  lowp float diff2 = max(\n"
    "    min(distance(a, b), distance(b, d)),\n"
    "    min(distance(a, c), distance(c, d))\n"
    "  );\n"
    "  return max(EDGE_MIN_CONTRAST * diff1, EDGE_MIN_VALUE) < diff2;\n"
    "}\n"
    "\n"
    "lowp float pack(bool a, bool b, bool c) {\n"
    "  return dot(vec3(float(a), float(b), float(c)), vec3(4.0, 16.0, 64.0)) / 255.0;\n"
    "}\n"
    "\n"
    "void main() {\n"
    "  lowp vec3 t01 = texture2D(texture, c01).rgb;\n"
    "  lowp vec3 t02 = texture2D(texture, c02).rgb;\n"
    "  lowp vec3 t04 = texture2D(texture, c04).rgb;\n"
    "  lowp vec3 t05 = texture2D(texture, c05).rgb;\n"
    "  lowp vec3 t06 = texture2D(texture, c06).rgb;\n"
    "  lowp vec3 t07 = texture2D(texture, c07).rgb;\n"
    "  lowp vec3 t08 = texture2D(texture, c08).rgb;\n"
    "  lowp vec3 t09 = texture2D(texture, c09).rgb;\n"
    "  lowp vec3 t10 = texture2D(texture, c10).rgb;\n"
    "  lowp vec3 t11 = texture2D(texture, c11).rgb;\n"
    "  lowp vec3 t13 = texture2D(texture, c13).rgb;\n"
    "  lowp vec3 t14 = texture2D(texture, c14).rgb;\n"
    "\n"
    "  lowp float l01 = luma(t01);\n"
    "  lowp float l02 = luma(t02);\n"
    "  lowp float l04 = luma(t04);\n"
    "  lowp float l05 = luma(t05);\n"
    "  lowp float l06 = luma(t06);\n"
    "  lowp float l07 = luma(t07);\n"
    "  lowp float l08 = luma(t08);\n"
    "  lowp float l09 = luma(t09);\n"
    "  lowp float l10 = luma(t10);\n"
    "  lowp float l11 = luma(t11);\n"
    "  lowp float l13 = luma(t13);\n"
    "  lowp float l14 = luma(t14);\n"
    "\n"
    "  // Main diagonals\n"
    "  bool d05_10 = hasDiagonal(l05, l06, l09, l10);\n"
    "  bool d06_09 = hasDiagonal(l06, l05, l10, l09);\n"
    "\n"
    "  // Saddle fix\n"
    "  if (d05_10 && d06_09) {\n"
    "    lowp float diff1 = distance(l06, l01) + distance(l11, l06) + distance(l09, l04) + distance(l14, l09);\n"
    "    lowp float diff2 = distance(l05, l02) + distance(l08, l05) + distance(l10, l07) + distance(l13, l10);\n"
    "    d05_10 = diff1 < diff2;\n"
    "    d06_09 = diff2 < diff1;\n"
    "  }\n"
    "\n"
    "  // Vertical diagonals\n"
    "  bvec2 d01_10 = hasDiagonal(l10, l09, l06, l05, l02, l01);\n"
    "  bvec2 d02_09 = hasDiagonal(l09, l10, l05, l06, l01, l02);\n"
    "  bvec2 d05_14 = hasDiagonal(l05, l06, l09, l10, l13, l14);\n"
    "  bvec2 d06_13 = hasDiagonal(l06, l05, l10, l09, l14, l13);\n"
    "\n"
    "  // Horizontal diagonals\n"
    "  bvec2 d04_10 = hasDiagonal(l10, l06, l09, l05, l08, l04);\n"
    "  bvec2 d06_08 = hasDiagonal(l06, l10, l05, l09, l04, l08);\n"
    "  bvec2 d05_11 = hasDiagonal(l05, l09, l06, l10, l07, l11);\n"
    "  bvec2 d07_09 = hasDiagonal(l09, l05, l10, l06, l11, l07);\n"
    "\n"
    "  bvec4 type5 = bvec4(d02_09.x && d06_08.x, d01_10.x && d05_11.x, d06_13.x && d07_09.x, d05_14.x && d04_10.x);\n"
    "  bvec4 type4 = bvec4(d05_11.x && d06_08.x, d04_10.x && d07_09.x, d05_14.x && d02_09.x, d01_10.x && d06_13.x);\n"
    "  bvec4 type3 = bvec4(d05_11.x && d04_10.x, d06_08.x && d07_09.x, d01_10.x && d05_14.x, d02_09.x && d06_13.x);\n"
    "  bvec4 type2_v = bvec4(d01_10.x, d02_09.x, d05_14.x, d06_13.x);\n"
    "  bvec4 type2_h = bvec4(d04_10.x, d06_08.x, d05_11.x, d07_09.x);\n"
    "  bvec2 type1 = bvec2(d05_10, d06_09);\n"
    "\n"
    "  bool bottomCut = any(bvec4(all(d05_11), all(d07_09), all(d05_14), all(d06_13)));\n"
    "  bool topCut = any(bvec4(all(d01_10), all(d02_09), all(d04_10), all(d06_08)));\n"
    "\n"
    "  lowp vec4 final = vec4(0.0, 0.0, 0.0, 1.0);\n"
    "\n"
    "  Pattern pattern;\n"
    "\n"
    "  if (any(type5)) {\n"
    "    pattern.type = 0.55;\n"
    "    pattern.flip = bvec3(type5.z, type5.x, type5.y);\n"
    "    pattern.cuts = bvec2(false, false);\n"
    "  } else if (any(type4)) {\n"
    "    pattern.type = 0.45;\n"
    "    pattern.flip = bvec3(type4.w, type4.y, type4.x || type4.y);\n"
    "    pattern.cuts = bvec2(bottomCut, topCut);\n"
    "  } else if (any(type3)) {\n"
    "    pattern.type = 0.35;\n"
    "    pattern.flip = bvec3(type3.w, type3.y, type3.x || type3.y);\n"
    "    pattern.cuts = bvec2(bottomCut, !topCut);\n"
    "  } else if (any(type2_v)) {\n"
    "    pattern.type = 0.25;\n"
    "    pattern.flip = bvec3(type2_v.x || type2_v.w, type2_v.y || type2_v.x, false);\n"
    "    pattern.cuts = bvec2(bottomCut || topCut, false);\n"
    "  } else if (any(type2_h)) {\n"
    "    pattern.type = 0.25;\n"
    "    pattern.flip = bvec3(type2_h.y || type2_h.x, type2_h.w || type2_h.x, true);\n"
    "    pattern.cuts = bvec2(bottomCut || topCut, false);\n"
    "  } else if (any(type1)) {\n"
    "    pattern.type = 0.15;\n"
    "    pattern.flip = bvec3(type1.y, false, false);\n"
    "    pattern.cuts = bvec2(false, false);\n"
    "  }\n"
    "\n"
    "  gl_FragColor = vec4(\n"
    "    pattern.type,\n"
    "    pack(pattern.flip.x, pattern.flip.y, pattern.flip.z),\n"
    "    pack(pattern.cuts.x, pattern.cuts.y, false),\n"
    "    1.0\n"
    "  );\n"
    "}";

const std::string ShaderManager::cut2UpscalePass1Vertex =
    "#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
    "#define HIGHP highp\n"
    "#else\n"
    "#define HIGHP mediump\n"
    "precision mediump float;\n"
    "#endif\n"
    "\n"
    "attribute vec4 vPosition;\n"
    "attribute vec2 vCoordinate;\n"
    "uniform mediump float screenDensity;\n"
    "uniform lowp sampler2D texture;\n"
    "uniform mat4 vViewModel;\n"
    "uniform HIGHP vec2 textureSize;\n"
    "uniform mediump float vFlipY;\n"
    "\n"
    "varying HIGHP vec2 screenCoords;\n"
    "varying HIGHP vec2 passCoords;\n"
    "varying HIGHP vec2 c05;\n"
    "varying HIGHP vec2 c06;\n"
    "varying HIGHP vec2 c09;\n"
    "varying HIGHP vec2 c10;\n"
    "\n"
    "void main() {\n"
    "  HIGHP vec2 coords = vec2(vCoordinate.x, mix(vCoordinate.y, 1.0 - vCoordinate.y, vFlipY)) * 1.0001;\n"
    "  screenCoords = coords * textureSize - vec2(0.5);\n"
    "  c05 = (screenCoords + vec2(+0.0, +0.0)) / textureSize;\n"
    "  c06 = (screenCoords + vec2(+1.0, +0.0)) / textureSize;\n"
    "  c09 = (screenCoords + vec2(+0.0, +1.0)) / textureSize;\n"
    "  c10 = (screenCoords + vec2(+1.0, +1.0)) / textureSize;\n"
    "  passCoords = vec2(c05.x, mix(c05.y, 1.0 - c05.y, vFlipY));\n"
    "  gl_Position = vViewModel * vPosition;\n"
    "}";

const std::string ShaderManager::cut2UpscalePass1Fragment =
    "#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
    "#define HIGHP highp\n"
    "#else\n"
    "#define HIGHP mediump\n"
    "precision mediump float;\n"
    "#endif\n"
    "#define EPSILON 0.02\n"
    "\n"
    "precision mediump float;\n"
    "uniform lowp sampler2D texture;\n"
    "uniform lowp sampler2D previousPass;\n"
    "\n"
    "varying HIGHP vec2 screenCoords;\n"
    "varying HIGHP vec2 coords;\n"
    "varying HIGHP vec2 passCoords;\n"
    "varying HIGHP vec2 c05;\n"
    "varying HIGHP vec2 c06;\n"
    "varying HIGHP vec2 c09;\n"
    "varying HIGHP vec2 c10;\n"
    "\n"
    "lowp float luma(lowp vec3 v) {\n"
    "  return v.g;\n"
    "}\n"
    "\n"
    "struct Pixels {\n"
    "  lowp vec3 p0;\n"
    "  lowp vec3 p1;\n"
    "  lowp vec3 p2;\n"
    "  lowp vec3 p3;\n"
    "};\n"
    "\n"
    "struct Pattern {\n"
    "  Pixels pixels;\n"
    "  bool triangle;\n"
    "  lowp vec2 coords;\n"
    "};\n"
    "\n"
    "lowp vec3 triangle(lowp vec2 pxCoords) {\n"
    "  lowp vec3 ws = vec3(0.0);\n"
    "  ws.x = pxCoords.y - pxCoords.x;\n"
    "  ws.y = 1.0 - ws.x;\n"
    "  ws.z = (pxCoords.y - ws.x) / (ws.y + EPSILON);\n"
    "  return ws;\n"
    "}\n"
    "\n"
    "lowp vec3 quad(lowp vec2 pxCoords) {\n"
    "  return vec3(pxCoords.x, pxCoords.x, pxCoords.y);\n"
    "}\n"
    "\n"
    "lowp float linearStep(lowp float edge0, lowp float edge1, lowp float t) {\n"
    "  return clamp((t - edge0) / (edge1 - edge0 + EPSILON), 0.0, 1.0);\n"
    "}\n"
    "\n"
    "lowp float sharpSmooth(lowp float t, lowp float sharpness) {\n"
    "  return linearStep(sharpness, 1.0 - sharpness, t);\n"
    "}\n"
    "\n"
    "lowp float sharpness(lowp float l1, lowp float l2) {\n"
    "#if USE_DYNAMIC_BLEND\n"
    "  lowp float lumaDiff = abs(l1 - l2);\n"
    "  lowp float contrast = linearStep(BLEND_MIN_CONTRAST_EDGE, BLEND_MAX_CONTRAST_EDGE, lumaDiff);\n"
    "  lowp float result = mix(BLEND_MIN_SHARPNESS * 0.5, BLEND_MAX_SHARPNESS * 0.5, contrast);\n"
    "#else\n"
    "  lowp float result = STATIC_BLEND_SHARPNESS * 0.5;\n"
    "#endif\n"
    "  return result;\n"
    "}\n"
    "\n"
    "lowp vec3 blend(lowp vec3 a, lowp vec3 b, lowp float t) {\n"
    "  return mix(a, b, sharpSmooth(t, sharpness(luma(a), luma(b))));\n"
    "}\n"
    "\n"
    "lowp vec3 unpack(lowp float values) {\n"
    "  return vec3(floor(mod(values / 4.0, 4.0)), floor(mod(values / 16.0, 4.0)), floor(mod(values / 64.0, 4.0)));\n"
    "}\n"
    "\n"
    "Pattern pattern0(Pixels pixels, lowp vec3 ab, lowp vec3 cd, lowp vec2 pxCoords) {\n"
    "  return Pattern(pixels, false, pxCoords);\n"
    "}\n"
    "\n"
    "Pattern pattern1(Pixels pixels, lowp vec3 ab, lowp vec3 cd, lowp vec2 pxCoords) {\n"
    "  Pattern result;\n"
    "  if (pxCoords.y > pxCoords.x) {\n"
    "    result.pixels = Pixels(pixels.p0, pixels.p2, pixels.p2, pixels.p3);\n"
    "    result.triangle = true;\n"
    "    result.coords = vec2(pxCoords.x, pxCoords.y);\n"
    "  } else {\n"
    "    result.pixels = Pixels(pixels.p0, pixels.p1, pixels.p1, pixels.p3);\n"
    "    result.triangle = true;\n"
    "    result.coords = vec2(pxCoords.y, pxCoords.x);\n"
    "  }\n"
    "  return result;\n"
    "}\n"
    "\n"
    "Pattern pattern2(Pixels pixels, lowp vec3 ab, lowp vec3 cd, lowp vec2 pxCoords) {\n"
    "  Pattern result;\n"
    "  if (pxCoords.y > 2.0 * pxCoords.x) {\n"
    "    result.pixels = Pixels(pixels.p0, pixels.p2, pixels.p2, cd);\n"
    "    result.triangle = true;\n"
    "    result.coords = vec2(pxCoords.x * 2.0, pxCoords.y);\n"
    "  } else {\n"
    "    result.pixels = Pixels(pixels.p0, pixels.p1, cd, pixels.p3);\n"
    "    result.triangle = false;\n"
    "    result.coords = vec2((pxCoords.x - 0.5 * pxCoords.y) / (1.0 - 0.5 * pxCoords.y + EPSILON), pxCoords.y);\n"
    "  }\n"
    "  return result;\n"
    "}\n"
    "\n"
    "Pattern pattern3(Pixels pixels, lowp vec3 ab, lowp vec3 cd, lowp vec2 pxCoords) {\n"
    "  Pattern result;\n"
    "  if (pxCoords.y > 2.0 * pxCoords.x) {\n"
    "    result.pixels = Pixels(pixels.p0, pixels.p2, pixels.p2, cd);\n"
    "    result.triangle = true;\n"
    "    result.coords = vec2(pxCoords.x * 2.0, pxCoords.y);\n"
    "  } else if (pxCoords.y < 2.0 * pxCoords.x - 1.0) {\n"
    "    result.pixels = Pixels(pixels.p3, pixels.p1, pixels.p1, ab);\n"
    "    result.triangle = true;\n"
    "    result.coords = vec2((1.0 - pxCoords.x) * 2.0, 1.0 - pxCoords.y);\n"
    "  } else {\n"
    "    result.pixels = Pixels(pixels.p0, ab, cd, pixels.p3);\n"
    "    result.triangle = false;\n"
    "    result.coords = vec2(2.0 * (pxCoords.x - 0.5 * pxCoords.y), pxCoords.y);\n"
    "  }\n"
    "  return result;\n"
    "}\n"
    "\n"
    "Pattern pattern4(Pixels pixels, lowp vec3 ab, lowp vec3 cd, lowp vec2 pxCoords) {\n"
    "  Pattern result;\n"
    "  if (pxCoords.x < 0.5) {\n"
    "    result.pixels = Pixels(pixels.p0, ab, pixels.p2, cd);\n"
    "    result.triangle = false;\n"
    "    result.coords = vec2(pxCoords.x * 2.0, pxCoords.y);\n"
    "  } else {\n"
    "    result.pixels = Pixels(ab, pixels.p1, cd, pixels.p3);\n"
    "    result.triangle = false;\n"
    "    result.coords = vec2(2.0 * (pxCoords.x - 0.5), pxCoords.y);\n"
    "  }\n"
    "  return result;\n"
    "}\n"
    "\n"
    "Pattern pattern5(Pixels pixels, lowp vec3 ab, lowp vec3 cd, lowp vec2 pxCoords) {\n"
    "  Pattern result;\n"
    "  if (pxCoords.y > pxCoords.x + 0.5) {\n"
    "    result.pixels = Pixels(pixels.p0, pixels.p2, pixels.p2, pixels.p3);\n"
    "    result.triangle = true;\n"
    "    result.coords = vec2(2.0 * pxCoords.x, 2.0 * (pxCoords.y - 0.5));\n"
    "  } else if (pxCoords.y > pxCoords.x) {\n"
    "    result.pixels = Pixels(pixels.p0, pixels.p0, pixels.p3, pixels.p3);\n"
    "    result.triangle = true;\n"
    "    result.coords = vec2(pxCoords.x, pxCoords.y);\n"
    "  } else {\n"
    "    result.pixels = Pixels(pixels.p0, pixels.p1, pixels.p1, pixels.p3);\n"
    "    result.triangle = true;\n"
    "    result.coords = vec2(pxCoords.y, pxCoords.x);\n"
    "  }\n"
    "  return result;\n"
    "}\n"
    "\n"
    "void main() {\n"
    "  lowp vec3 t05 = texture2D(texture, c05).rgb;\n"
    "  lowp vec3 t06 = texture2D(texture, c06).rgb;\n"
    "  lowp vec3 t09 = texture2D(texture, c09).rgb;\n"
    "  lowp vec3 t10 = texture2D(texture, c10).rgb;\n"
    "\n"
    "  Pixels pixels = Pixels(t05, t06, t09, t10);\n"
    "\n"
    "  lowp vec3 flagsTexture = texture2D(previousPass, passCoords).xyz;\n"
    "\n"
    "  int patternType = int(flagsTexture.x * 10.0);\n"
    "  lowp vec3 transform = unpack(floor(flagsTexture.y * 255.0 + 0.5));\n"
    "  lowp vec3 patternFlags = unpack(floor(flagsTexture.z * 255.0 + 0.5));\n"
    "\n"
    "  lowp vec2 pxCoords = fract(screenCoords);\n"
    "\n"
    "  if (transform.x > 0.0) {\n"
    "    pixels = Pixels(pixels.p1, pixels.p0, pixels.p3, pixels.p2);\n"
    "    pxCoords.x = 1.0 - pxCoords.x;\n"
    "  }\n"
    "\n"
    "  if (transform.y > 0.0) {\n"
    "    pixels = Pixels(pixels.p2, pixels.p3, pixels.p0, pixels.p1);\n"
    "    pxCoords.y = 1.0 - pxCoords.y;\n"
    "  }\n"
    "\n"
    "  if (transform.z > 0.0) {\n"
    "    pixels = Pixels(pixels.p0, pixels.p2, pixels.p1, pixels.p3);\n"
    "    pxCoords = pxCoords.yx;\n"
    "  }\n"
    "\n"
    "  lowp vec3 ab = patternFlags.y > 0.5 ? pixels.p0 : pixels.p1;\n"
    "  lowp vec3 cd = patternFlags.x > 0.5 ? pixels.p2 : pixels.p3;\n"
    "\n"
    "  Pattern pattern;\n"
    "\n"
    "  if (patternType == 0) {\n"
    "    pattern = pattern0(pixels, ab, cd, pxCoords);\n"
    "  } else if (patternType == 1) {\n"
    "    pattern = pattern1(pixels, ab, cd, pxCoords);\n"
    "  } else if (patternType == 2) {\n"
    "    pattern = pattern2(pixels, ab, cd, pxCoords);\n"
    "  } else if (patternType == 3) {\n"
    "    pattern = pattern3(pixels, ab, cd, pxCoords);\n"
    "  } else if (patternType == 4) {\n"
    "    pattern = pattern4(pixels, ab, cd, pxCoords);\n"
    "  } else {\n"
    "    pattern = pattern5(pixels, ab, cd, pxCoords);\n"
    "  }\n"
    "\n"
    "  lowp vec3 weights = pattern.triangle ? triangle(pattern.coords) : quad(pattern.coords);\n"
    "\n"
    "  lowp vec3 final = blend(\n"
    "    blend(pattern.pixels.p0, pattern.pixels.p1, weights.x),\n"
    "    blend(pattern.pixels.p2, pattern.pixels.p3, weights.y),\n"
    "    weights.z\n"
    "  );\n"
    "\n"
    "#if SPLIT_DEMO_VIEW\n"
    "  lowp float splitSize = 0.001;\n"
    "  final = mix(t05, final, step(0.5, coords.x));\n"
    "  final *= step(coords.x, 0.5 - splitSize) + step(0.5 + splitSize, coords.x);\n"
    "#endif\n"
    "\n"
    "  gl_FragColor = vec4(final, 1.0);\n"
    "}";

ShaderManager::Chain ShaderManager::getShader(const ShaderManager::Config& config) {
    switch (config.type) {
    case Type::SHADER_DEFAULT: {
        return { { { defaultShaderVertex, defaultShaderFragment, true, 1.0 } }, true };
    }

    case Type::SHADER_CRT: {
        return { { { defaultShaderVertex, crtShaderFragment, true, 1.0 } } , true };
    }

    case Type::SHADER_LCD: {
        return { { {defaultShaderVertex, lcdShaderFragment, true, 1.0 } }, true };
    }

    case Type::SHADER_SHARP: {
        return { { { defaultShaderVertex, defaultSharpFragment, true, 1.0 } }, true };
    }

    case Type::SHADER_UPSCALE_CUT: {
        std::string defines = buildDefines(cutUpscaleParams, config.params);
        return { { {
            defines + cutUpscaleVertex,
            defines + cutUpscaleFragment,
            false,
            1.0
        } } , false };
    }

    case Type::SHADER_UPSCALE_CUT2: {
        std::string defines = buildDefines(cut2UpscaleParams, config.params);
        return {
            {
                {
                    defines + cut2UpscalePass0Vertex,
                    defines + cut2UpscalePass0Fragment,
                    false,
                    1.0
                },
                {
                    defines + cut2UpscalePass1Vertex,
                    defines + cut2UpscalePass1Fragment,
                    false,
                    1.0
                }
            },
            false
        };
    }
}
}

std::string ShaderManager::buildDefines(
    std::unordered_map<std::string, std::string> baseParams,
    std::unordered_map<std::string, std::string> customParams
) {
    customParams.insert(baseParams.begin(), baseParams.end());

    std::string result;
    std::for_each(customParams.begin(), customParams.end(), [&result] (auto param) {
        result += "#define " + param.first + " " + param.second + "\n";
    });

    return result + "\n";
}

bool ShaderManager::Chain::operator==(const ShaderManager::Chain &other) const {
    return this->passes == other.passes && this->linearTexture == other.linearTexture;
}

bool ShaderManager::Chain::operator!=(const ShaderManager::Chain &other) const {
    return !(*this == other);
}

bool ShaderManager::Pass::operator==(const ShaderManager::Pass &other) const {
    return this->linear == other.linear && this->scale == other.scale &&
           this->vertex == other.vertex && this->fragment == other.fragment;
}
} //namespace libretrodroid
