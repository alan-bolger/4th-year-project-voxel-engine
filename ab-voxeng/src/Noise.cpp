#include "Noise.h"

/// <summary>
/// Constructor for the Noise class.
/// </summary>
ab::Noise::Noise()
{
	initialise();
}

/// <summary>
/// Destructor for the Noise class.
/// </summary>
ab::Noise::~Noise()
{
	
}

/// <summary>
/// Initialise the Noise class.
/// </summary>
void ab::Noise::initialise()
{
	for (int i = 0; i < 512; i++)
	{
		perm[i] = p[i & 255]; 
	}
}

/// <summary>
/// Floor function (faster than the standard floor function!).
/// </summary>
/// <returns>The result of the floor function.</returns>
int ab::Noise::fastFloor(double t_x)
{
	return t_x > 0 ? (int)t_x : (int)t_x - 1;
}

/// <summary>
/// Dot product (2D vector).
/// </summary>
/// <returns>The result of the dot product.</returns>
double ab::Noise::dot(int t_g[], double t_x, double t_y)
{
	return t_g[0] * t_x + t_g[1] * t_y;
}

/// <summary>
/// Dot product (3D vector).
/// </summary>
/// <returns>The result of the dot product.</returns>
double ab::Noise::dot(int t_g[], double t_x, double t_y, double t_z)
{
	return t_g[0] * t_x + t_g[1] * t_y + t_g[2] * t_z;
}

/// <summary>
/// Dot product (4D vector).
/// </summary>
/// <returns>The result of the dot product.</returns>
double ab::Noise::dot(int t_g[], double t_x, double t_y, double t_z, double t_w)
{
	return t_g[0] * t_x + t_g[1] * t_y + t_g[2] * t_z + t_g[3] * t_w;
}

/// <summary>
/// Noise generation function. Returns a value between 0 and 1.
/// </summary>
/// <param name="t_xin">The range's lowest value.</param>
/// <param name="t_yin">The range's highest value.</param>
/// <returns>The resulting noise value.</returns>
double ab::Noise::noise(double t_xin, double t_yin)
{
	// Noise contributions from the three corners
	double n0;
	double n1;
	double n2;

	// Skew the input space to determine which simplex cell we're in
	const double F2 = 0.5 * (sqrt(3.0) - 1.0);

	// Hairy factor for 2D
	double s = (t_xin + t_yin) * F2;
	int i = fastFloor(t_xin + s);
	int j = fastFloor(t_yin + s);
	const double G2 = (3.0 - sqrt(3.0)) / 6.0;
	double t = ((double)i + j) * G2;

	// Unskew the cell origin back to (x, y) space
	double X0 = i - t;
	double Y0 = j - t;

	// The (x, y) distances from the cell origin
	double x0 = t_xin - X0;
	double y0 = t_yin - Y0;

	// For the 2D case, the simplex shape is an equilateral triangle
	// Determine which simplex we are in

	// Offsets for second (middle) corner of simplex in (i, j) coords
	int i1;
	int j1;

	// Lower triangle, (x, y) order: (0, 0)->(1, 0)->(1, 1)
	if (x0 > y0)
	{
		i1 = 1;
		j1 = 0;
	}
	// Upper triangle, (y, x) order: (0, 0)->(0, 1)->(1, 1)
	else
	{
		i1 = 0;
		j1 = 1;
	}

	// A step of (1, 0) in (i, j) means a step of (1 - c, -c) in (x, y)
	// A step of (0, 1) in (i, j) means a step of (-c, 1 - c) in (x, y) where c = (3 - sqrt(3)) / 6

	// Offsets for middle corner in (x, y) unskewed coords
	double x1 = x0 - i1 + G2;
	double y1 = y0 - j1 + G2;

	// Offsets for last corner in (x, y) unskewed coords
	double x2 = x0 - 1.0 + 2.0 * G2;
	double y2 = y0 - 1.0 + 2.0 * G2;

	// Work out the hashed gradient indices of the three simplex corners
	int ii = i & 255;
	int jj = j & 255;
	int gi0 = perm[ii + perm[jj]] % 12;
	int gi1 = perm[ii + i1 + perm[jj + j1]] % 12;
	int gi2 = perm[ii + 1 + perm[jj + 1]] % 12;

	// Calculate the contribution from the three corners
	double t0 = 0.5 - x0 * x0 - y0 * y0;

	if (t0 < 0)
	{
		n0 = 0.0;
	}
	else 
	{
		t0 *= t0;
		n0 = t0 * t0 * dot(grad3[gi0], x0, y0); // (x, y) of grad3 used for 2D gradient
	}

	double t1 = 0.5 - x1 * x1 - y1 * y1;

	if (t1 < 0)
	{
		n1 = 0.0;
	}
	else
	{
		t1 *= t1;
		n1 = t1 * t1 * dot(grad3[gi1], x1, y1);
	}

	double t2 = 0.5 - x2 * x2 - y2 * y2;

	if (t2 < 0)
	{
		n2 = 0.0;
	}
	else 
	{
		t2 *= t2;
		n2 = t2 * t2 * dot(grad3[gi2], x2, y2);
	}

	// Add contributions from each corner to get the final noise value
	// The result is scaled to return values in the interval [0, 1]
	float v = 70.0f * (n0 + n1 + n2);
	return normaliseToRange(v, -1, 1);	 
}

/// <summary>
/// Normalise value to a specified range.
/// </summary>
/// <param name="t_value">The value to normalise.</param>
/// <param name="t_min">The minimum number in the range.</param>
/// <param name="t_max">The maximum number in the range.</param>
/// <returns>The normalised value.</returns>
float ab::Noise::normaliseToRange(float t_value, float t_min, float t_max)
{
	return (t_value - t_min) / (t_max - t_min);
}
