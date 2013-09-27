#include "plattis.h"

extern float scroller_flash;

extern int zoomer_blend;

extern void scroll_render(float t);
extern int zoomerit[15][9];
extern int nopeat_zoomerit[11][9];

extern void mosaic_text();

extern float ifs_z;

extern float zoomer_bias;
extern float zoomer_biasr;
extern float zoomer_biasg;
extern float zoomer_biasb;
extern float mosaic_text_alpha;

extern int slides_texture;
extern int ifs_number;
int part=0;

#include "tech04.h"
#include "track16b.h"
#include "track17b.h"
#include "track19b.h"

int *midi_tech04=track_tech04;
float tech04_keytime[256];
float tech04_keyvelocity[256];

int *midi_track16=track_tech16;
float track16_keytime[256];
float track16_keyvelocity[256];

int *midi_track17=track_tech17;
float track17_keytime[256];
float track17_keyvelocity[256];

int *midi_track19=track_tech19;
float track19_keytime[256];
float track19_keyvelocity[256];

extern void zoomer_set2(int layer, float speed, float rotate, float hue, float pos=0.0, float posrot=0.0);

float dt;

void midi_tracks(float t)
{
	if(*midi_tech04!=-1)
	{
		while(*midi_tech04!=-1 && *midi_tech04/960.f<=t) midi_tech04+=3;
		tech04_keytime[midi_tech04[-2]]=midi_tech04[-3]/960.f;
		tech04_keyvelocity[midi_tech04[-2]]=midi_tech04[-1]/127.f;
	}
	if(*midi_track16!=-1)
	{
		while(*midi_track16!=-1 && *midi_track16/960.f<=t) midi_track16+=3;
		track16_keytime[midi_tech04[-2]]=midi_track16[-3]/960.f;
		track16_keyvelocity[midi_tech04[-2]]=midi_track16[-1]/127.f;
	}
	
	if(*midi_track17!=-1)
	{
		while(*midi_track17!=-1 && *midi_track17/960.f<=t) midi_track17+=3;
		track17_keytime[midi_track17[-2]]=midi_track17[-3]/960.f;
		track17_keyvelocity[midi_track17[-2]]=midi_track17[-1]/127.f;
	}

	if(*midi_track19!=-1)
	{
		while(*midi_track19!=-1 && *midi_track19/960.f<=t) midi_track19+=3;
		track19_keytime[midi_track17[-2]]=midi_track19[-3]/960.f;
		track19_keyvelocity[midi_track17[-2]]=midi_track19[-1]/127.f;
	}
}

void demo_init()
{
	ifs_init();
	zoomer_init();
	mosaic_init();

	slides_init();

	part=3;
}

float syncPoint=0;

#define BLUR_BEGIN 	SetSecondaryRenderTarget();
#define BLUR_END 	RenderSecondary(main_back, main_depth);

#define FADEIN(start,speed) CLAMP((dt-start)*(1.0f/speed))
#define FADEOUT(start,speed) CLAMP((start-dt)*(1.0f/speed))


int current_preset=-1;
void hieno_preset_nopeat(int a, float sf=1.0, float rf=1.0)
{
	current_preset=a;
	int count=sizeof(nopeat_zoomerit)/sizeof(nopeat_zoomerit[0]);

	zoomer_none();
	int *z=&nopeat_zoomerit[a%count][0];
	zoomer_blend=z[0];
	zoomer_set(z[1], z[3]*sf/4.f, z[4]*rf/4.f, z[5]/4.f, dt*.15);
	zoomer_set(z[2], z[6]*sf/4.f, z[7]*rf/4.f, z[8]/4.f, dt*.15);
}

void hieno_preset(int a, float sf=1.0, float rf=1.0)
{
	angles[253]=a;
	current_preset=a;
	int count=sizeof(zoomerit)/sizeof(zoomerit[0]);

	zoomer_none();
	int *z=&zoomerit[a%count][0];
	zoomer_blend=z[0];
	zoomer_set(z[1], z[3]*sf/4.f, z[4]*rf/4.f, z[5]/4.f);
	zoomer_set(z[2], z[6]*sf/4.f, z[7]*rf/4.f, z[8]/4.f);
}

void hieno_preset_timely(int a, float sf=1.0, float rf=1.0)
{
	current_preset=a;
	int count=sizeof(zoomerit)/sizeof(zoomerit[0]);

	zoomer_none();
	int *z=&zoomerit[a%count][0];
	zoomer_blend=z[0];
	zoomer_set2(z[1], z[3]*sf/4.f, z[4]*rf/4.f, z[5]/4.f);
	zoomer_set2(z[2], z[6]*sf/4.f, z[7]*rf/4.f, z[8]/4.f);
}

void nopea_preset(int a, float sf=1.0, float rf=1.0)
{
	current_preset=a;
	int count=sizeof(nopeat_zoomerit)/sizeof(nopeat_zoomerit[0]);

	zoomer_none();
	int *z=&nopeat_zoomerit[a%count][0];
	zoomer_blend=z[0];
	zoomer_set(z[1], z[3]*sf/4.f, z[4]*rf/4.f, z[5]/4.f);
	zoomer_set(z[2], z[6]*sf/4.f, z[7]*rf/4.f, z[8]/4.f);
}

#define FLASH(start,speed) (dt>=start?1-FADEIN(start,speed):0)

void xdemo_render(float t)
{
	float dt=t;
	int m=angles[254]+angles[255]+angles[250];
	static int t156=-1;
	if(t156!=m || t<syncPoint) 
	{
		t156=m;
		syncPoint=t;dt=0;
/*		zoomer_none();
		zoomer_set(angles[254], angles[0*8+0], angles[0*8+0], angles[0*8+0]);
		zoomer_set(angles[255], angles[1*8+0], angles[1*8+0], angles[1*8+0]);
*/		hieno_preset(angles[255]);
	}
	BLUR_BEGIN
	motionblur=0;
	radialblur=0;
#if 0
	float b0=(FLASH(0,1));
	float b1=(FLASH(3,1));
	float b2=(FLASH(6,1));
	float b3=(FLASH(9,1));

	zoomer_bias=b0+b1+b2+b3;
#endif
	zoomer_render(t);
	BLUR_END
}


void demo_render(float t)
{
//	t+=295;
//	t+=150;

//	t=t+50;
//	t=t+60+52.941;
//	t+=30;	

//	t+=132;

//	t+=75;
/*
 0 pidempi
 1 ok
 2 ok
 3 ok
 4 ok
 5 ok
 6 ok lisää blurria
 7 hidastus
 8 hidastus anim
 8 flash
 9 bugaa

*/
	midi_tracks(t);

	zoomer_biasr=0;
	zoomer_biasg=0;
	zoomer_biasb=0;

/*
	zoomer_biasr=CLAMP(1+4*(tech04_keytime[38]-t))*tech04_keyvelocity[38];
	zoomer_biasg=CLAMP(1+4*(tech04_keytime[43]-t))*tech04_keyvelocity[43];
	zoomer_biasb=CLAMP(1+4*(tech04_keytime[44]-t))*tech04_keyvelocity[44];
*/

	dt=t-syncPoint;
/**
	BLUR_BEGIN
	motionblur=.9;//25;
	radialblur=.3;
	ifs_number=angles[254];
//	fade=FADEIN(0,5)*FADEOUT(9,1);
	ifs_z=FADEIN(0,8)*1.1;
	ifs_render(dt*.5);
	BLUR_END


	return;
**/



#if 1

	if(t<16.5)
	{
		static int t17=0;
		if(!t17++) 
		{
			syncPoint=t;dt=0;
			hieno_preset(12);
		}
		fade=FADEIN(0,20)*FADEOUT(16,3);

		BLUR_BEGIN
		motionblur=.7;
		radialblur=0;
//		animSet[11].speed=pow(dt+6,1.1f)*.2;

		zoomer_render(t);
		BLUR_END
	} else
	if(t<24.5-.5f)
	{
		static int t24=0;
		if(!t24++) 
		{
			syncPoint=t;dt=0;
		}
		slides_texture=0;
		fade=FADEIN(.5f,4)*FADEOUT(6,1);
		slides_render(t);
	} else
	if(t<32-.5f)
	{
		static int t32=0;
		if(!t32++) 
		{
			syncPoint=t;dt=0;
		}
		slides_texture=1;
		fade=FADEIN(.5f,4)*FADEOUT(6,1);
		slides_render(t);
	} else
	if(t<41)//0.492 )
	{
		static int t41=0;
		if(!t41++) 
		{
			syncPoint=t;dt=0;
		}
		BLUR_BEGIN
		motionblur=.7;//25;
		radialblur=.3;
		ifs_number=2;
		fade=FADEIN(0,5)*FADEOUT(8,1);
		ifs_z=FADEIN(0,8)*0.6;
		ifs_render(dt*.65);
		BLUR_END
/*
		static int t40=0;
		if(!t40++) 
		{
			syncPoint=t;dt=0;
			zoomer_none();
			zoomer_set(6, 11, 14.3, 3);
		}
		fade=FADEIN(0,5)*FADEOUT(9,1);
//		BLUR_BEGIN
		motionblur=0;
		radialblur=0;
		zoomer_render(t);
//		BLUR_END
*/
	} else
	if(t<48)
	{
		static int t48=0;
		if(!t48++) 
		{
			syncPoint=t;dt=0;
		}
		BLUR_BEGIN
		motionblur=.5;//25;
		radialblur=.9;
		ifs_number=4;
		fade=FADEIN(0,4)*FADEOUT(6.7,1);
		ifs_z=FADEIN(0,8)*.951-.5f;
		ifs_render(dt*.66+.4f);
		BLUR_END
/*
		static int t53=0;
		if(!t53++) 
		{
			syncPoint=t;dt=0;
			zoomer_none();
			zoomer_set(6, -11, 0, 3);
//			zoomer_set(4, 11, 14.3, 3);
//			zoomer_set(13, 6, 6.3, 3);
		}
		animSet[6].huet=20;
		fade=FADEIN(0,5)*FADEOUT(7,1);
		BLUR_BEGIN
		motionblur=.40;
		radialblur=0;
		zoomer_render(t);
		BLUR_END*/
	} else
	if(t<56)
	{
		static int t56=0;
		if(!t56++) 
		{
			syncPoint=t;dt=0;
		}
//		BLUR_BEGIN
		motionblur=0;//25;
		radialblur=0;
		ifs_number=9;
		ifs_z=FADEIN(0,8)*1.4;
		fade=.8f*FADEIN(0,4)*FADEOUT(7,1);
		ifs_render(dt*1.96);
//		BLUR_END
	} else
	if(t<64)
	{
		static int t64=0;
		if(!t64++) 
		{
			syncPoint=t;dt=0;
		}
		BLUR_BEGIN
		motionblur=.85;//25;
		radialblur=.50;
		ifs_number=1;
		ifs_z=FADEIN(0,8)*.6;
		fade=FADEIN(0,4)*FADEOUT(7,1);
		ifs_render(dt*.3+.2f);
		BLUR_END
	} else
	/*
	if(t<64)
	{
		BLUR_BEGIN
		motionblur=.25;//25;
		radialblur=0;
		ifs_number=2;
		ifs_render(t);
		BLUR_END
	} else*/
//	if(t<72)
	if(t<79.9)
	{
		// ok
		static int t80=0;
		if(!t80++) 
		{
			syncPoint=t;dt=0;
		}
		BLUR_BEGIN
		motionblur=.75;//25;
		radialblur=.80;
		ifs_number=5;
		ifs_z=1;//FADEIN(0,8)*.6;
		fade=FADEIN(0,8)*FADEOUT(14.9,1);
		ifs_render(dt*.45);
		BLUR_END
	} else
	if(t<98.73)
	{
#if 0
		// nopeet 80-114s
		static int t114=0;
		if(!t114++) 
		{
			syncPoint=t;dt=0;
			current_preset=-1;
			zoomer_none();
		}

		static float poo;
		float poop=0;
		poop += CLAMP(1+4*(track16_keytime[38]-t))*track16_keyvelocity[38]*.2f;
		poop += CLAMP(1+4*(track16_keytime[43]-t))*track16_keyvelocity[43]*.4f;
		poop += CLAMP(1+4*(track16_keytime[36]-t))*track16_keyvelocity[36]*.6f;
		poop += CLAMP(1+4*(track17_keytime[38]-t))*track17_keyvelocity[38]*.2f;
		poop += CLAMP(1+4*(track17_keytime[43]-t))*track17_keyvelocity[43]*.3f;
		poop += CLAMP(1+4*(track17_keytime[36]-t))*track17_keyvelocity[36]*.3f;

		poo+=poop;

		poop += CLAMP(1+4*(track19_keytime[38]-t))*track19_keyvelocity[38]*.2f;
		poop += CLAMP(1+4*(track19_keytime[37]-t))*track19_keyvelocity[37]*.3f;
		poop += CLAMP(1+4*(track19_keytime[36]-t))*track19_keyvelocity[36]*.4f;

	/*
		static float poop=0;
		poop += CLAMP(1+4*(track17_keytime[38]-t))*track17_keyvelocity[38]*.2f;
		poop += CLAMP(1+4*(track17_keytime[43]-t))*track17_keyvelocity[43]*.1f;
		poop += CLAMP(1+4*(track17_keytime[36]-t))*track17_keyvelocity[36]*.7f;

		poop += CLAMP(1+4*(track19_keytime[38]-t))*track19_keyvelocity[38]*.2f;
		poop += CLAMP(1+4*(track19_keytime[37]-t))*track19_keyvelocity[37]*.1f;
		poop += CLAMP(1+4*(track19_keytime[36]-t))*track19_keyvelocity[36]*.4f;


		zoomer_bias=CLAMP(1+2*(track16_keytime[44]-t));//*track16_keyvelocity[36];
*/
		int m=(int)(dt*.25f+poop);
		if(current_preset!=m) 
		{
//			syncPoint=t;dt=0;
			nopea_preset(m);
			motionblur=CLAMP((cos(t*5.3)+1)*.75)*.89;
			radialblur=CLAMP((sin(t)+1)*.75)*.5;
		}

		fade=FADEOUT(17,1);
		BLUR_BEGIN
		zoomer_render(t);
		BLUR_END
#endif

		// nopeet 80-114s
		static int t114=0;
		if(!t114++) 
		{
			syncPoint=t;dt=0;
			current_preset=-1;
			zoomer_none();
		}

		static float poo;
		float poop=0;
		poop += CLAMP(1+4*(track16_keytime[38]-t))*track16_keyvelocity[38]*.2f;
		poop += CLAMP(1+4*(track16_keytime[43]-t))*track16_keyvelocity[43]*.3f;
		poop += CLAMP(1+4*(track17_keytime[36]-t))*track17_keyvelocity[36]*.7f;

		poo+=poop;

		poop += CLAMP(1+4*(track19_keytime[38]-t))*track19_keyvelocity[38]*.2f;
		poop += CLAMP(1+4*(track19_keytime[37]-t))*track19_keyvelocity[37]*.3f;
		poop += CLAMP(1+4*(track19_keytime[36]-t))*track19_keyvelocity[36]*.4f;

		zoomer_bias=CLAMP(1+4*(track17_keytime[36]-t))*track17_keyvelocity[36];

		int m=(int)(dt*.25f+poo*2); 
//		int m=(int)(dt*.25f+poo);
		if(current_preset!=m) 
		{
//			syncPoint=t;dt=0;
			nopea_preset(m, 1);
			motionblur=CLAMP((sin(t*3.3)+1)*.75)*.89;
			radialblur=CLAMP((cos(t)+1)*.75)*.5;
		}

		fade=FADEOUT(17,1);
		BLUR_BEGIN
		zoomer_render((dt+poop)*.9);
		BLUR_END




	} else
	if(t<104.6)
	{
		static int t104=0;
		if(!t104++) 
		{
			syncPoint=t;dt=0;
			zoomer_none();
			zoomer_set(7, 18, 0, 0, 0, 0);
		}
		zoomer_bias=FLASH(0,1);
		fade=FADEOUT(6,1);
		zoomer_render(dt);
/*
		scroller_flash=FLASH(0,1);
		scroll_render(dt);
		*/
	} else
	if(t<114.6)
	{
		// nopeet 80-114s
		static int t114=0;
		if(!t114++) 
		{
			syncPoint=t;dt=0;
			current_preset=-1;
			zoomer_none();
		}

		static float poo;
		float poop=0;
		poop += CLAMP(1+4*(track16_keytime[38]-t))*track16_keyvelocity[38]*.3f;
		poop += CLAMP(1+4*(track16_keytime[43]-t))*track16_keyvelocity[43]*.1f;
		poop += CLAMP(1+4*(track16_keytime[36]-t))*track16_keyvelocity[36]*.3f;

		poo+=poop;

		poop += CLAMP(1+4*(track19_keytime[38]-t))*track19_keyvelocity[38]*.2f;
		poop += CLAMP(1+4*(track19_keytime[37]-t))*track19_keyvelocity[37]*.1f;
		poop += CLAMP(1+4*(track19_keytime[36]-t))*track19_keyvelocity[36]*.4f;

		zoomer_bias=CLAMP(1+4*(track17_keytime[36]-t))*track17_keyvelocity[36];

		int m=(int)(dt*.25f+poo*2); 
//		int m=(int)(dt*.25f+poo);
		if(current_preset!=m) 
		{
//			syncPoint=t;dt=0;
			nopea_preset(m, -1);
			motionblur=CLAMP((sin(t*5.3)+1)*.75)*.89;
			radialblur=CLAMP((cos(t)+1)*.75)*.5;
		}

		fade=FADEOUT(17,1);
		BLUR_BEGIN
		zoomer_render((dt+poop)*.9);
		BLUR_END
	} else
	if(t<132.4)
	{
		static int t130=0;
		if(!t130++) 
		{
			syncPoint=t;dt=0;
		}
		dt=dt*.85+0.15f;
		if(dt<0) dt=0;
		fade=0;
		fade+=FADEIN(0,3)*FADEOUT(0+3.f,1.5f);
		fade+=FADEIN(4,3)*FADEOUT(4+3.f,1.5f);
		fade+=FADEIN(8,3)*FADEOUT(8+3.f,1.5f);
		fade+=FADEIN(12,3)*FADEOUT(12+3.f,1.5f);
/*		fade+=FADEIN(0,3)*FADEOUT(0+3.f,.5f);
		fade+=FADEIN(4,3)*FADEOUT(4+3.f,.5f);
		fade+=FADEIN(8,3)*FADEOUT(8+3.f,.5f);
		fade+=FADEIN(12,3)*FADEOUT(12+3.f,.5f);
*/
		slides_texture=(int)(2+(dt/4)+.25f);
		slides_render(t);
	} else
	if(t<156.5)
	{
		static int t156=0;
		if(!t156++) 
		{
			syncPoint=t;dt=0;
		}

		fade=FADEOUT(21,3);
		mosaic_text_alpha=fade*FADEIN(8,4);

		motionblur=FADEIN(4,6)*fade*.7;
		radialblur=.65*FADEIN(4,8);

		BLUR_BEGIN
		mosaic_render(dt);
		BLUR_END
		mosaic_text();
	} else
#endif
#if 0
	if(t<9180)
	{
		static int t180=0;
		if(!t180++) 
		{
			current_preset=-1;
			syncPoint=t;dt=0;
			zoomer_none();
			hieno_preset(0);
		}

		int m=(int)(dt/15.);
		if(current_preset!=m) 
		{
			//syncPoint=t;dt=0;
			zoomer_none();
			hieno_preset(m);
			motionblur=(cos(t*5.3)+1)*.25;
			radialblur=(sin(t*2.2)+1)*.25;
		}


		BLUR_BEGIN

		fade=1;
		float b0=(FLASH(0,1));
		float b1=(FLASH(3,1));
		float b2=(FLASH(6,1));
		float b3=(FLASH(9,1));
/*
		zoomer_bias=b0+b1+b2+b3;

		zoomer_biasr=CLAMP(1+4*(tech04_keytime[38]-t));
		zoomer_biasg=CLAMP(1+4*(tech04_keytime[43]-t));
		zoomer_biasb=CLAMP(1+4*(tech04_keytime[44]-t));
*/
		zoomer_render(t);
		BLUR_END
	} els
#endif

/*

*/

	if(t<164.490)
	{
		static int t196=0;
		if(!t196++) 
		{
			current_preset=-1;
			syncPoint=t;dt=0;
			hieno_preset(2);
			motionblur=.5; radialblur=.3;
		}

		BLUR_BEGIN
		fade=FADEOUT(15,1);
		zoomer_bias=FLASH(0,1);
		zoomer_render(dt+8);
		BLUR_END
	} else
	if(t<180.5)
	{
		static int t180=0;
		if(!t180++) 
		{
			current_preset=-1;
			syncPoint=t;dt=0;
			hieno_preset(1);
			motionblur=0; radialblur=0;
		}

		BLUR_BEGIN
		fade=FADEOUT(15,1);
		zoomer_bias=FLASH(0,1);
		zoomer_render(dt+0);
		BLUR_END
	} else
	if(t<196.45)
	{
		static int t164=0;
		if(!t164++) 
		{
			current_preset=-1;
			syncPoint=t;dt=0;
			hieno_preset(0);
			motionblur=.8; radialblur=.6;
		}
		BLUR_BEGIN

		zoomer_bias=FLASH(0,1);
		fade=FADEOUT(15,2);
		zoomer_render(dt+26);
		BLUR_END
	} else
	if(t<204.47)
	{
		static int t204=0;
		if(!t204++) 
		{
			current_preset=-1;
			syncPoint=t;dt=0;
			hieno_preset(3);
			motionblur=0; radialblur=0;
		}

		BLUR_BEGIN
		fade=1;
		zoomer_bias=FLASH(0,1);
		zoomer_render(dt+30);
		BLUR_END
	} else
	if(t<212.474)
	{
		static int t212=0;
		if(!t212++) 
		{
			current_preset=-1;
			syncPoint=t;dt=0;
			hieno_preset(4);
			motionblur=.5; radialblur=.5;
		}

		BLUR_BEGIN
		fade=1;
		zoomer_bias=FLASH(0,1);
		zoomer_render(dt+20);
		BLUR_END
	} else
	if(t<228.472)
	{
		static int t228=0;
		if(!t228++) 
		{
			current_preset=-1;
			syncPoint=t;dt=0;
			hieno_preset(5);
			motionblur=.8; radialblur=.2;
		}

		BLUR_BEGIN
		fade=1;
		zoomer_bias=FLASH(0,1);
		zoomer_render(dt+5);
		BLUR_END
	} else
	if(t<244.482)
	{
		// vaihda 2
		static int t164=0;
		if(!t164++) 
		{
			current_preset=-1;
			syncPoint=t;dt=0;
			hieno_preset(10);  //krava
			motionblur=0; radialblur=.5;
		}

		BLUR_BEGIN
		fade=1;
		zoomer_bias=FLASH(0,1);
		zoomer_render(dt+0);
		BLUR_END
	} else
	if(t<252.482)
	{
		static int t164=0;
		if(!t164++) 
		{
			current_preset=-1;
			syncPoint=t;dt=0;
			hieno_preset(8);
			motionblur=.5; radialblur=.8;
		}

		BLUR_BEGIN
		zoomer_bias=FLASH(0,1);
		fade=1;
		zoomer_render(dt+22);
		BLUR_END
	} else
/*	if(t<252.482)
	{
		static int t164=0;
		if(!t164++) 
		{
			current_preset=-1;
			syncPoint=t;dt=0;
			hieno_preset(9);
			motionblur=.7; radialblur=.7;
		}

		BLUR_BEGIN
		fade=1;
		zoomer_bias=FLASH(0,1);
		zoomer_render(dt+25);
		BLUR_END
	} else*/
	if(t<271.464)
	{
		// vaihda 1
		static int t244=0;
		if(!t244++) 
		{
			current_preset=-1;
			syncPoint=t;dt=0;
			hieno_preset(6);
			motionblur=.8; radialblur=.5;
		}

		BLUR_BEGIN
		zoomer_render(dt+26);
		zoomer_bias=FLASH(0,1);
		BLUR_END
	} else
	if(t<295.357)
	{
		static int t164=0;
		if(!t164++) 
		{
			current_preset=-1;
			syncPoint=t;dt=0;
			hieno_preset(11);
			motionblur=.1; radialblur=.8;
		}

		BLUR_BEGIN
		fade=1;
		zoomer_bias=FLASH(0,1);
		zoomer_render(dt+68+10);
		BLUR_END
	} else
	if(t<308.357)
	{
		static int t164=0;
		if(!t164++) 
		{
			current_preset=-1;
			syncPoint=t;dt=0;
//			hieno_preset(7);
//			motionblur=0.1; radialblur=.8;
			hieno_preset(9);
			motionblur=.7; radialblur=.7;
		}

		BLUR_BEGIN
		fade=FADEOUT(10,3);
		zoomer_bias=FLASH(0,1);
		zoomer_render(dt+48);
		BLUR_END
	} else
	if(t<60000)
	{
		static int t24=0;
		if(!t24++) 
		{
			syncPoint=t;dt=0;
		}
		slides_texture=6;
		fade=FADEIN(0,8)*FADEOUT(16,30);
		slides_render(t);
	} else
	;
}