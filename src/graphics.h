#pragma once
#include <stdint.h>
#include <string>


typedef struct {
	unsigned short type; 	//specifies file type
	unsigned int size; 		//size (in bytes9 of file
	unsigned int reserved; 	//reserved, must be 0
	unsigned int offset; 	//specifies offset (in bytes) of the bitmapFileHeader to the bitmap's bits
} BitmapFileHeader;

typedef struct {

	unsigned int size; 				//specifies no. of bytes required by the struct
	int width; 						//width (in px)
	int height; 					//height (in px)
	unsigned short planes;			//specifies no. of colour planes, must be 1
	unsigned short bits; 			//no. of bits per pixel
	unsigned int compression; 		//type of compression
	unsigned int imageSize; 		//size of image in bytes
	int xResolution; 				//pixels per meter in X-axis
	int YResolution; 				//pixels per meter in Y-axis
	unsigned int nColours; 			//number of colours used by the bitmap
	unsigned int importantColours; 	//number of important colours in the bitmap
} BitmapInfoHeader;

class Bitmap {
	BitmapInfoHeader bitmapinfoheader;
	uint8_t *bitmapdata;
public:
	Bitmap() {};
	BitmapInfoHeader getBitmapInfoHeader() const;
	uint8_t* getBitmapData();
	int load(const char* filepath);
	void draw(int x, int y);
	void draw_rot(int gx, int gy, double rotation_degrees);
};

class Pixmap {
	int height;
	int width;
	uint32_t *map;
public:
	Pixmap() {};
	int getHeight();
	int getWidth();
	uint32_t* getMap();
	int read(const char *map[]);
	void draw(int gx, int gy, double rotation_degrees);
};

//GAME DATA

typedef struct {
	Pixmap asteroid_dest1;
	Pixmap asteroid_dest2;
	Pixmap asteroid_dest3;

	Pixmap cursor;
	Pixmap crosshair;
	Pixmap blue_laser;
	Pixmap red_laser;

	Pixmap n_colon;
	Pixmap n_zero;
	Pixmap n_one;
	Pixmap n_two;
	Pixmap n_three;
	Pixmap n_four;
	Pixmap n_five;
	Pixmap n_six;
	Pixmap n_seven;
	Pixmap n_eight;
	Pixmap n_nine;
	Pixmap n_one_large;
	Pixmap n_two_large;
	Pixmap n_three_large;
}pixmap_data;

typedef struct {
	Bitmap menubackground;
	Bitmap hsbackground;
	Bitmap playbutton;
	Bitmap optionsbutton;
	Bitmap quitbutton;
	Bitmap options;
	Bitmap boxticked;
	Bitmap game_background;
	Bitmap pause_message;
	Bitmap splash;
	Bitmap death_screen;
	Bitmap death_screen_highscore;
	Bitmap score_header;
	Bitmap hp_header;
	Bitmap hp_header_low;
	Bitmap fps_header;
	Bitmap teleport_ready_header;
	Bitmap teleport_not_ready_header;
	Bitmap medium_score;
	Bitmap large_score;
	Bitmap alien_score;
	Bitmap alien_ship;
	Bitmap large_asteroid;
	Bitmap medium_asteroid;
	Bitmap pix_ship_blue;
	Bitmap pix_ship_blue_bt;
	Bitmap pix_ship_blue_st;
	Bitmap pix_ship_blue_pt;
	Bitmap p_arrow;
	Bitmap slidemarker;
}bitmap_data;


int load_bitmaps(bitmap_data *bmp);
void load_xpms(pixmap_data *pix);
