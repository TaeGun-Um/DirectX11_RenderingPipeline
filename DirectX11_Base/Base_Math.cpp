#include "PrecompileHeader.h"
#include "Base_Math.h"

const float Base_Math::PIE = 3.141592653589793238462643383279502884197169399375105820974944f;
const float Base_Math::PIE2 = PIE * 2.0f;
const float Base_Math::DegToRad = Base_Math::PIE / 180;
const float Base_Math::RadToDeg = 180 / Base_Math::PIE;

const float4 float4::LEFT = { -1.0f, 0.0f, 0.0f, 1.0f };
const float4 float4::RIGHT = { 1.0f, 0.0f, 0.0f, 1.0f };
const float4 float4::UP = { 0.0f, 1.0f, 0.0f, 1.0f };
const float4 float4::DOWN = { 0.0f, -1.0f, 0.0f, 1.0f };
const float4 float4::FORWARD = { 0.0f, 0.0f, 1.0f, 1.0f };
const float4 float4::BACK = { 0.0f, 0.0f, -1.0f, 1.0f };
const float4 float4::ONE = { 1.0f, 1.0f, 1.0f, 1.0f };
const float4 float4::ZERO = { 0.0f, 0.0f, 0.0f, 1.0f };
const float4 float4::ZERONULL = { 0.0f, 0.0f, 0.0f, 0.0f };
const float4 float4::RED = { 1.0f, 0.0f, 0.0f, 1.0f };
const float4 float4::BLUE = { 0.0f, 0.0f, 1.0f, 1.0f };
const float4 float4::GREEN = { 0.0f, 1.0f, 0.0f, 1.0f };
const float4 float4::WHITE = { 1.0f, 1.0f, 1.0f, 1.0f };
const float4 float4::BLACK = { 0.0f, 0.0f, 0.0f, 1.0f };