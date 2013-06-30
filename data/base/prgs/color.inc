// Author:   Benjamin Pickhardt
// Project:  LinWarrior 3d
// Home:     hackcraft.de

// -----------------------
// Color space conversions


vec3 RGB2YUV(vec3 color)
{
	float CR = color.r;
	float CG = color.g;
	float CB = color.b;

	float CY = 0.299*CR + 0.587*CG + 0.114*CB;
	float CU = (CB-CY)*0.565;
	float CV = (CR-CY)*0.713;

	vec3 c = vec3(CY, CU, CV);

	return c;
}


vec3 YUV2RGB(vec3 color)
{
	float CY = color.r;
	float CU = color.g;
	float CV = color.b;

	float CR = CY + 1.403*CV;
	float CG = CY - 0.344*CU - 0.714*CV;
	float CB = CY + 1.770*CU;

	vec3 c = vec3(CR, CG, CB);

	return c;
}


