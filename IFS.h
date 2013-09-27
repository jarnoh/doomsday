#ifndef IFS_h
#define IFS_h
#include <stdio.h>


namespace IFS
{
#ifndef M_PI
#define M_PI 3.1415926539f
#endif
#define EPS (1e-6f)

#define variation_random (-1)

#define NVARS		7	// variaatioiden lukumäärä.. 7 on normaali.
#define NXFORMS		6	// montako transformia voi olla systeemissä yhtä aikaa max.
						// enempi -> monimutkaisemmat kuvat.. ei välttämättä hienompia


/*
Kuten nimi sanoo, tämä on värikartta. System2 omistaa ColorMap:n, jonka
perusteella systeemin pisteet väritetään. ColorMap:iksi pitää ladata joku
ennaltamääritelty setti. Muistaakseni niitä on jotain 83. (kts. ColorMap.cpp)
*/
class ColorMap
{
public:
	float	data[256][3];

	// returns the number of the preset loaded
	int		loadRandomPreset(float hue_rotation);
	void	loadPreset(int n, float hue_rotation);

	static void	rgb2hsv(float *rgb, float *hsv);
	static void	hsv2rgb(float *hsv, float *rgb);
};


/*
Transformi joka kertoo miten pisteiden paikka muuttuu iteraatiolla. (c[3][2])
Transformilla on todennäköisyys (density), joka kertoo miten usein xformia
käytetään.

color osoittaa pisteen tulevan värin värikartalta.

var on transformiin käytettyjen variaatioiden (linear, sinusoidal, spherical,
jne.) intensiteetit.
*/
struct Xform2
{
	float	var[NVARS];   // normalized interp coefs between variations
	float	c[3][2];      // the coefs
	float	density;      // prob is this function is chosen. 0 - 1
	float	color;        // color coord for this function. 0 - 1
};


/*
Systeemi sisältää useita transformeja, joilla on todennäköisyydet kuinka usein
mitäkin käytetään. Sekä colormap, eli värikartta, joka kertoo pisteiden värit.

wiggle_amount ja wiggle_speed kertoo kuinka paljon systeemi värisee -- eli
transformien matriiseja muunnellaan hieman, kun niitä käytetään.

alpha kertoo pisteen läpinäkyvyyden.

radius kertoo pisteiden koon. Tietenkin jos rendataan vain pisteitä, (kuten
nyt) tällä ei ole väliä.

scale ja center[2] kertovat koko systeemin transformaation. Ne syötetään
3D-rajapinnalle, jotta laitteisto hoitaa pisteiden transformoinnin.

time kertoon ajan jolloin systeemi on aktiivinen.

randomize() generoi uuden satunnaisen systeemin.

renderFlame() renderöi systeemin sellaisenaan.

renderFlameLinear() renderöi systeemin käyttämättä flamen variaatioita
iteraatioissa. Systeemin renderointi on paljon nopeampaa käyttämällä tätä,
koska variaatiot käyttävät kalliita matematiikkarutiineja. (sin, cos, sqrt)

Loput metodit on systeemien lukemista ja kirjoittamista varten. Käytä
FS2-formaattia, koska siinä on kaikki data. FLA on Apophysiksen formaatti.
*/
class System2
{
public:
	Xform2			xforms[NXFORMS];	// transforms to choose from

	ColorMap		colormap;			// the colormap of this system
	int				colormap_index;		// index of colormap preset. -1 if the map has no preset.
	float			hue_rotation;		// hue rotation amount
	float			wiggle_amount,wiggle_speed; // wiggle effect

	unsigned int	point_quantity;		// how many point to render. tip: use 2 - 20 points per pixel
	float			alpha;				// alpha of dots, used in blend
	float			radius;				// the "sprite" radius
	float			scale, center[2];	// scaling and center
	float			time;				// time of system.. needed only when used with animation
	
	void		randomize();			// make this system random system 
	void		renderFlame();
	void		renderFlameLinear();

	void		appendFLA(FILE *f);					// appends this system to a FLA-file
	bool		saveFLA(char *fn);					// saves this system to a FLA-file. rewrites the file
	bool		loadFLA(char *fn, int index=0);		// loads a system from FLA-file
	void		appendFS2(FILE *f);					// appends this system to a FS2-file
	bool		saveFS2(char *fn);						// saves this system to a FS2-file. rewrites the file
	bool		loadFS2(char *fn, int index=0);		// loads a system from FS2-file
	System2();
};


/*
Animointi hoituu tällä luokalla.

systems-jono pitää sisällään animaation systeemit.
SYSTEEMIEN TULEE OLLA KRONOLOGISESSA JÄRJESTYKSESSÄ!

time on nykyinen aika.

getEndTime() etsii animaation pituuden lukemalla systems-jonon viimeisen
systeemin ajan.

sort() käytä tätä sortataksesi systeemit.

interpolate() interpoloi systeemien välillä. Älä käytä tätä, tämän pitäisi
olla private :)

renderFlame() rendaa flamen normaalisti interpoloiden automaagisesti animaation
time-muuttujan mukaan. Käytä tätä!

renderFlameLinear() rendaa flamen käyttämättä variaatioita -> nopeempi.
(kts. System2:n kommentointi) Käytä tätä jos tulee tehojen kanssa ongelmia!

Muut metodin onkin sitten animaation managerointia ja levyltä lukemista.
*/
class Animation2
{
public:
	System2			*systems;
	int				system_quantity;
	float			time;

	float	getEndTime() { return systems[system_quantity-1].time; }
	void	sort();
	void	interpolate(System2 *result);
	void	renderFlame();
	void	renderFlameLinear();
	void	renderIFS();

	void	deleteSystem(int n);
	void    insertSystem(int n, const System2 *sys);
	bool	loadFLA(char *fn);
	bool	loadFS2(char *fn);
	bool	saveFS2(char *fn);
	Animation2();
};


/******************************************************************************
Lopuksi kaikkea turhaa.. älä välitä näistä..
******************************************************************************/


struct Xform3
{
	float	var[NVARS];		// normalized interp coefs between variations
	float	c[4][3];		// the coefs
	float	q[4];			// new-style quaternions
	float	density;		// prob is this function is chosen. 0 - 1
	float	color;			// color coord for this function. 0 - 1
};


class System3
{
public:
	Xform3			xforms[NXFORMS];	// transforms to choose from

	ColorMap		colormap;			// the colormap of this system
	int				colormap_index;		// index of colormap preset. -1 if the map has no preset.
	float			hue_rotation;		// hue rotation amount
	float			wiggle_amount,wiggle_speed; // wiggle effect

	unsigned int	point_quantity;		// how many point to render. tip: use 2 - 20 points per pixel
	float			alpha;				// alpha of dots, used in blend
	float			radius;				// the "sprite" radius
	float			time;				// time of system.. needed only when used with animation

	float			translate[3];
	float			rotate[4];			// quaternion
	float			scale;

	void		randomize();			// make this system random system 
	void		renderFlameLinear();

	void		appendFS3(FILE *f);					// appends this system to a FS3-file
	bool		saveFS3(char *fn);					// saves this system to a FS3-file. rewrites the file
	bool		loadFS3(char *fn, int index=0);		// loads a system from FS3-file
	System3();
};


class Animation3
{
public:
	System3			*systems;
	int				system_quantity;
	float			time;

	float	getEndTime() { return systems[system_quantity-1].time; }
	void	sort();
	void	interpolate(System3 *result);
	void	renderFlameLinear();
	void	deleteSystem(int n);
	void    insertSystem(int n, const System3 *sys);
	bool	loadFS3(char *fn);
	bool	saveFS3(char *fn);
	Animation3();
};


// maths:
extern void tform2quat(float *q, const float a[4][3]);
extern void quat2tform(const float *q, float m[4][3]);
extern void mult_matrix(float s1[2][2], float s2[2][2], float d[2][2]);
extern float det_matrix(float s[2][2]);
extern void flip_matrix(float m[2][2], int h);
extern void transpose_matrix(float m[2][2]);
extern void choose_evector(float m[3][2], float r, float v[2]);
extern void interpolate_angle(float t, float s, float *v1, float *v2, float *v3, int tie, int cross);
extern void interpolate_complex(float t, float s, float r1[2], float r2[2], float r3[2],
	int flip, int tie, int cross);
extern void interpolate_matrix(float t, float m1[3][2], float m2[3][2], float m3[3][2]);
// buffer parse:
extern float readFloat(char *buf, char *value, float def);
extern int readInt(char *buf, char *value, int def);


}
#endif

