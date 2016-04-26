#ifndef CONSTANTS_H_
#define CONSTANTS_H_

// the length of frames to be computed as one step for trajectory similarity
const int step = 6;

// Variance threshold is used to segment trajectories of hands by variance
const float var_threshold = 12;

// difference between variances of trajectories
const float delta_var = 8;
const float delta_mean = 30;
const float delta_angle = 4;

// difference between displacement of coordinates of x and y of points of trajectories
const float delta_dis_d = 0.1;
// difference between coordinates of x and y of points of trajectories
const float delta_dis = 15;


// colors
const int colors[50][3] = 
{
/*{233,56,157},
{85,209,62},
{227,99,222},
{153,202,45},
{155,122,239},*/

{94,173,83},
{199,99,172},
{183,149,45},
{109,138,196},
{208,85,72},

{66,158,43},
{200,122,221},
{194,188,52},
{88,129,230},
{238,143,32},
{90,153,228},
{231,77,27},
{74,197,215},
{228,72,59},
{86,206,118},
{222,53,115},
{139,195,91},
{167,78,166},
{228,176,49},
{104,100,177},
{137,159,52},
{230,105,183},
{71,125,37},
{173,143,225},
{181,150,53},
{139,156,211},
{188,81,28},
{84,167,207},
{221,54,84},
{62,197,166},
{186,62,133},
{71,161,102},
{225,128,175},
{65,120,64},
{204,147,206},
{108,115,35},
{156,88,138},
{147,110,25},
{65,116,152},
{217,131,54},
{125,103,147},
{224,120,79},
{41,124,114},
{182,65,58},
{132,98,42},
{225,113,138},
{161,93,47},
{170,74,100},
{230,109,105},
{170,83,74}};

#endif /*CONSTANTS_H_*/