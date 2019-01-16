#include "graphics.h"
#include "vcard.h"
#include "mvector.h"
#include "pixmaps.h"

#pragma warning(disable:4996)
#define COLOR_IGNORED 0xFFAEC9

using namespace std;

BitmapInfoHeader Bitmap::getBitmapInfoHeader() const { return bitmapinfoheader; }
uint8_t* Bitmap::getBitmapData() { return bitmapdata; }

int Bitmap::load(const char* filepath) {
	
	//allocating size
	bitmapdata = (uint8_t*)malloc(sizeof(Bitmap) - sizeof(bitmapinfoheader));

	//open file in binary mode
	FILE *filePtr;
	filePtr = fopen(filepath, "rb");

	if (filePtr == NULL)
		return 1;

	//read the bitmap's file header
	BitmapFileHeader bitmapFileHeader;
	fread(&bitmapFileHeader, 2, 1, filePtr);

	//verify correct file type
	if (bitmapFileHeader.type != 0x4D42)
	{
		fclose(filePtr);
		return 1;
	}

	int rd;

	do
	{
		if ((rd = fread(&bitmapFileHeader.size, 4, 1, filePtr)) != 1)
			break;

		if ((rd = fread(&bitmapFileHeader.reserved, 4, 1, filePtr)) != 1)
			break;

		if ((rd = fread(&bitmapFileHeader.offset, 4, 1, filePtr)) != 1)
			break;
	} while (0);

	if (rd != 1)
	{
		fprintf(stderr, "Error reading file\n");
		exit(-1);
	}

	//read bitmap's info header
	fread(&bitmapinfoheader, sizeof(BitmapInfoHeader), 1, filePtr);

	//move file pointer to beginning of bitmap data
	fseek(filePtr, bitmapFileHeader.offset, SEEK_SET);

	//alocate memory to image data
	uint8_t* bitmapImage = (uint8_t*)malloc(bitmapinfoheader.imageSize);

	//verify allocation
	if (!bitmapImage)
	{
		free(bitmapImage);
		fclose(filePtr);
		return 1;
	}

	//read image data
	fread(bitmapImage, bitmapinfoheader.imageSize, 1, filePtr);

	//making sure data was read
	if (bitmapImage == NULL)
	{
		fclose(filePtr);
		return 1;
	}

	//close file and return image data
	fclose(filePtr);

	bitmapdata = bitmapImage;

	return 0;
}

void Bitmap::draw(int x, int y) {

	if (bitmapdata == NULL)
		return;

	if (x + bitmapinfoheader.width < 0 || x > 1023 || y + bitmapinfoheader.height < 0 || y > 767)
		return;

	for (int i = bitmapinfoheader.height; i != 0; i--)
		for (int j = 0; j < bitmapinfoheader.width; j++) {
			uint32_t color = (bitmapdata[(i * bitmapinfoheader.width * 3) + (j * 3 + 2)] << 16);
			color += (bitmapdata[(i * bitmapinfoheader.width * 3) + (j * 3 + 1)] << 8);
			color += (bitmapdata[(i * bitmapinfoheader.width * 3) + (j * 3)]);
			if (color && color != COLOR_IGNORED && i < bitmapinfoheader.height - 1)
				if (draw_pixel(x + j, y + bitmapinfoheader.height - i - 1, color))
					continue;
		}
}

void Bitmap::draw_rot(int gx, int gy, double rotation_degrees) {

	if (bitmapdata == NULL)
		return;
	int height = bitmapinfoheader.height;
	int width = bitmapinfoheader.width;

	if (gx + width < 0 || gx > 1023 || gy + height < 0 || gy > 767)
		return;

	int center_x = (width / 2) - 1;
	int center_y = (height / 2) - 1;

	for (int i = height; i != 0; i--)
		for (int j = 0; j < width; j++) {
			uint32_t color = (bitmapdata[(i * width * 3) + (j * 3 + 2)] << 16);
			color += (bitmapdata[(i * width * 3) + (j * 3 + 1)] << 8);
			color += (bitmapdata[(i * width * 3) + (j * 3)]);

			if (color && color != COLOR_IGNORED && i < height - 1) {
				mvector2d center(center_x, center_y);
				mvector2d point(j, i);
				mvector2d pivot = center - point;
				pivot.rotate(90 - rotation_degrees);
				pivot += center;
				int x = (int)(round(pivot.getX() + gx - center_x));
				int y = (int)(round(pivot.getY() + gy - center_y));
				
				if (draw_pixel(x, y, color)) {
				continue;
				}
			}
		}
}

int Pixmap::getHeight() { return height; }
int Pixmap::getWidth() { return width; }
uint32_t* Pixmap::getMap() { return map; }

int Pixmap::read(const char *pixmap[]) {
	int colors;

	/* read width, height, colors */

	if (sscanf(pixmap[0], "%d %d %d", &width, &height, &colors) != 3) {
		printf("read_xpm: incorrect width, height, colors\n");
		return 1;
	}

	unsigned int size = width * height;
	map = (uint32_t*)malloc(size * 4);
	uint32_t *color = (uint32_t*) malloc(colors);
	char *symbol = (char*)malloc(colors); 

	for (int i = 0; i < colors; i++) {
		sscanf(pixmap[i + 1], "%c %x", &symbol[i], &color[i]);
	}

	char *line = (char *)malloc(width);

	for (int i = 0; i < height; i++) {
		memcpy(line, pixmap[i + colors + 1], width);
		for (int j = 0; j < width; j++) {
			char sym = line[j];
			int index = 0;
			for (int k = 0; k < colors; k++) {
				if (sym == symbol[k]) {
					index = k;
					break;
				}
			}
			uint32_t colr = color[index];
			map[(i * width) + j] = colr;
		}
	}
	return 0;
}

void Pixmap::draw(int gx, int gy, double rotation_degrees) {

	int center_x = (width / 2) - 1;
	int center_y = (height / 2) - 1;
	int x = gx;
	int y = gy;
	double degrees = 180 - rotation_degrees;

	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++) {
			
			uint32_t color = map[(i * width) + j];

			if (color && color != COLOR_IGNORED) {
				mvector2d center(center_x, center_y);
				mvector2d point(j, i);
				mvector2d pivot = center - point;
				pivot.rotate(degrees);
				pivot += center;
				x = (int)(round(pivot.getX() + gx - center_x));
				y = (int)(round(pivot.getY() + gy - center_y));

				if (draw_pixel(x, y, color))
					continue;
			}
		}
}


int load_bitmaps(bitmap_data *bmp) {

	while (bmp->menubackground.load("textures/MenuBackground.bmp"))
		bmp->menubackground.load("textures/MenuBackground.bmp");
	while (bmp->hsbackground.load("textures/highscoresbackground.bmp"))
		bmp->hsbackground.load("textures/highscoresbackground.bmp");
	while (bmp->playbutton.load("textures/playbutton.bmp"))
		bmp->playbutton.load("textures/playbutton.bmp");
	while (bmp->optionsbutton.load("textures/optionsbutton.bmp"))
		bmp->optionsbutton.load("textures/optionsbutton.bmp");
	while (bmp->quitbutton.load("textures/quitbutton.bmp"))
		bmp->quitbutton.load("textures/quitbutton.bmp");
	while (bmp->options.load("textures/options.bmp"))
		bmp->options.load("textures/options.bmp");
	while (bmp->death_screen.load("textures/deathscreen.bmp"))
		bmp->death_screen.load("textures/deathscreen.bmp");
	while (bmp->death_screen_highscore.load("textures/deathscreenhighscore.bmp"))
		bmp->death_screen_highscore.load("textures/deathscreenhighscore.bmp");
	while (bmp->boxticked.load("textures/boxticked.bmp"))
		bmp->boxticked.load("textures/boxticked.bmp");
	while (bmp->game_background.load("textures/gamebackground.bmp"))
		bmp->game_background.load("textures/gamebackground.bmp");
	while (bmp->pause_message.load("textures/pausemessage.bmp"))
		bmp->pause_message.load("textures/pausemessage.bmp");
	while (bmp->splash.load("textures/splash.bmp"))
		bmp->splash.load("textures/splash.bmp");
	while (bmp->score_header.load("textures/score_header.bmp"))
		bmp->score_header.load("textures/score_header.bmp");
	while (bmp->hp_header.load("textures/hp_header.bmp"))
		bmp->hp_header.load("textures/hp_header.bmp");
	while (bmp->hp_header_low.load("textures/hp_header_low.bmp"))
		bmp->hp_header_low.load("textures/hp_header_low.bmp");
	while (bmp->fps_header.load("textures/fps_header.bmp"))
		bmp->fps_header.load("textures/fps_header.bmp");
	while (bmp->alien_ship.load("textures/alienship.bmp"))
		bmp->alien_ship.load("textures/alienship.bmp");
	while (bmp->teleport_ready_header.load("textures/jmpheaderrdy.bmp"))
		bmp->teleport_ready_header.load("textures/jmpheaderrdy.bmp");
	while (bmp->teleport_not_ready_header.load("textures/jmpheadernotrdy.bmp"))
		bmp->teleport_not_ready_header.load("textures/jmpheadernotrdy.bmp");
	while (bmp->medium_score.load("textures/mediumscore.bmp"))
		bmp->medium_score.load("textures/mediumscore.bmp");
	while (bmp->large_score.load("textures/largescore.bmp"))
		bmp->large_score.load("textures/largescore.bmp");
	while (bmp->alien_score.load("textures/alienscore.bmp"))
		bmp->alien_score.load("textures/alienscore.bmp");
	while (bmp->large_asteroid.load("textures/LargeAsteroid.bmp"))
		bmp->large_asteroid.load("textures/LargeAsteroid.bmp");
	while (bmp->medium_asteroid.load("textures/MediumAsteroid.bmp"))
		bmp->medium_asteroid.load("textures/MediumAsteroid.bmp");
	while (bmp->pix_ship_blue.load("textures/pix_ship_blue.bmp"))
		bmp->pix_ship_blue.load("textures/pix_ship_blue.bmp");
	while (bmp->pix_ship_blue_bt.load("textures/pix_ship_blue_bt.bmp"))
		bmp->pix_ship_blue_bt.load("textures/pix_ship_blue_bt.bmp");
	while (bmp->pix_ship_blue_pt.load("textures/pix_ship_blue_pt.bmp"))
		bmp->pix_ship_blue_pt.load("textures/pix_ship_blue_pt.bmp");
	while (bmp->pix_ship_blue_st.load("textures/pix_ship_blue_st.bmp"))
		bmp->pix_ship_blue_st.load("textures/pix_ship_blue_st.bmp");
	while (bmp->p_arrow.load("textures/parrow.bmp"))
		bmp->p_arrow.load("textures/parrow.bmp");
	while (bmp->slidemarker.load("textures/slidemarker.bmp"))
		bmp->slidemarker.load("textures/slidemarker.bmp");
	return 0;
}

void load_xpms(pixmap_data *pix) {

	pix->asteroid_dest1.read(pix_asteroid_destroyed_1);
	pix->asteroid_dest2.read(pix_asteroid_destroyed_2);
	pix->asteroid_dest3.read(pix_asteroid_destroyed_3);

	pix->cursor.read(pix_menu_cursor);
	pix->crosshair.read(pix_crosshair);

	pix->blue_laser.read(pix_laser);
	pix->red_laser.read(pix_laser_red);

	pix->n_colon.read(colon);
	pix->n_zero.read(zero);
	pix->n_one.read(one);
	pix->n_two.read(two);
	pix->n_three.read(three);
	pix->n_four.read(four);
	pix->n_five.read(five);
	pix->n_six.read(six);
	pix->n_seven.read(seven);
	pix->n_eight.read(eight);
	pix->n_nine.read(nine);
	pix->n_one_large.read(one_large);
	pix->n_two_large.read(two_large);
	pix->n_three_large.read(three_large);
}
