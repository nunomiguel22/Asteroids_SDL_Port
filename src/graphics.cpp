#include "graphics.h"
#include "renderer.h"
#include "mvector.h"
#include "pixmaps.h"

#pragma warning(disable:4996)
#define COLOR_IGNORED 0xFFAEC9 //Alpha channel not possible in original minix project

using namespace std;

BitmapInfoHeader Bitmap::getBitmapInfoHeader() const { return infoheader; }
uint8_t* Bitmap::getBitmapData() { return bitmapdata; }

int Bitmap::load(const char* filepath) {
	
	//allocating size
	bitmapdata = (uint8_t*)malloc(sizeof(Bitmap) - sizeof(infoheader));

	//open file in binary mode
	FILE *filePtr;
	filePtr = fopen(filepath, "rb");

	if (filePtr == NULL)
		return 1;

	//read the bitmap's file header
	BitmapFileHeader fileheader;
	fread(&fileheader, 2, 1, filePtr);

	//verify correct file type
	if (fileheader.type != 0x4D42)
	{
		fclose(filePtr);
		return 2;
	}

	if (fread(&fileheader.size, 4, 1, filePtr) != 1)
		return 3;

	if (fread(&fileheader.reserved, 4, 1, filePtr) != 1)
		return 3;

	if (fread(&fileheader.offset, 4, 1, filePtr) != 1)
		return 3;

	//read bitmap's info header
	fread(&infoheader, sizeof(BitmapInfoHeader), 1, filePtr);

	int padding = 0;
	int pixelbytes = infoheader.bits / 8;
	int bytes_per_line = infoheader.width * pixelbytes;

	/* Check for padding size at the end of each line */
	for (int i = 0; i < 3; ++i)
		if ((bytes_per_line + padding) % 4 != 0)
			++padding;
		else break;

	//move file pointer to beginning of bitmap data
	fseek(filePtr, fileheader.offset, SEEK_SET);

	//alocate memory to image data
	uint8_t* bitmapImage = (uint8_t*)malloc(infoheader.imageSize);

	//verify allocation
	if (!bitmapImage)
	{
		free(bitmapImage);
		fclose(filePtr);
		return 1;
	}

	//read image data
	fread(bitmapImage, infoheader.imageSize, 1, filePtr);

	//making sure data was read
	if (bitmapImage == NULL)
	{
		fclose(filePtr);
		return 1;
	}

	//close file and return image data
	fclose(filePtr);

	if (padding) {
		uint8_t *bmp = (uint8_t*)malloc(bytes_per_line * infoheader.height);

		for (int i = 0; i < infoheader.height; ++i)
			for (int j = 0; j < bytes_per_line; ++j)
				bmp[(i * bytes_per_line) + j] = bitmapImage[(i * (bytes_per_line + padding)) + j];
		bitmapdata = bmp;
		free(bitmapImage);
	}
	else bitmapdata = bitmapImage;

	return 0;
}

void Bitmap::draw(int x, int y) {
	/*
		Draws a static bitmap in given location
	*/

	if (bitmapdata == NULL)
		return;

	if (x + infoheader.width < 0 || x > 1023 || y + infoheader.height < 0 || y > 767)
		return;

	for (int i = infoheader.height; i != 0; --i)
		for (int j = 0; j < infoheader.width; ++j) {
			uint32_t color = (bitmapdata[(i * infoheader.width * 3) + (j * 3 + 2)] << 16);
			color |= (bitmapdata[(i * infoheader.width * 3) + (j * 3 + 1)] << 8);
			color |= (bitmapdata[(i * infoheader.width * 3) + (j * 3)]);
			if (color && color != COLOR_IGNORED && i < infoheader.height - 1)
				if (draw_pixel(x + j, y + infoheader.height - i - 1, color))
					continue;
		}
}

void Bitmap::draw_transform(int gx, int gy, double rotation_degrees, double scale) {

	/*
		*Allows scaling bitmaps
		*Rotation by assigning pixels from source
		*Interpolation too slow...
	*/
	
	if (bitmapdata == NULL)
		return;

	int height = infoheader.height;
	int width = infoheader.width;
	int heightpadding = 0;;
	int widthpadding = 0;
	int center_x = (width / 2);
	int center_y = (height / 2) + 1;
	int upperbound = center_y / 2;

	/* Check if out of bounds */

	if (gx + width < 0 || gx > hres - 1 || gy + height < 0 || gy > vres - 1)
		return;

	/* Create extra drawing space for rotation*/
	if (width > height)
		heightpadding = width - height;
	else widthpadding = height - width;

	for (int i = height + heightpadding; i != -upperbound; --i)
		for (int j = -center_x; j < width + widthpadding; ++j) {

			/* Convert point to cartesian*/
			int x = j - center_x;
			int y = center_y - i;

			/* Create and rotate vector, center is (0, 0) no need to convert */
			mvector2d point(x, y);
			point.rotate(270 - rotation_degrees);

			/* Convert back to graphics coordinates*/
			x = (int)(round(point.getX() + center_x));
			y = (int)(round(center_y - point.getY()));

			/* Check bounds and get color*/
			uint32_t color;
			if (x < 0 || x > width - 1 || y < 1 || y > height - 2)
				continue;
			else {
				color = (bitmapdata[(y * width * 3) + (x * 3 + 2)] << 16);
				color |= (bitmapdata[(y * width * 3) + (x * 3 + 1)] << 8);
				color |= (bitmapdata[(y * width * 3) + (x * 3)]);
			}
			
			/* Draw to pixelbuffer ignoring black and another color*/
			if (color && color != COLOR_IGNORED) 
				if (draw_pixel((int)(gx + ((j-center_x) * scale) ) , (int)(gy + ((i - center_y) * scale)), color))
					continue;
		}
}

void Bitmap::draw_colorswap(int x, int y, uint32_t originalcolor, uint32_t newcolor) {
	
	/*
		Swaps a color in the bitmap and draws to pixelbuffer
	*/

	if (bitmapdata == NULL)
		return;

	if (x + infoheader.width < 0 || x > 1023 || y + infoheader.height < 0 || y > 767)
		return;

	for (int i = infoheader.height; i != 0; --i)
		for (int j = 0; j < infoheader.width; ++j) {
			uint32_t color = (bitmapdata[(i * infoheader.width * 3) + (j * 3 + 2)] << 16);
			color |= (bitmapdata[(i * infoheader.width * 3) + (j * 3 + 1)] << 8);
			color |= (bitmapdata[(i * infoheader.width * 3) + (j * 3)]);
			if (color && color != COLOR_IGNORED && i < infoheader.height - 1)
				if (color == originalcolor)
					color = newcolor;
				if (draw_pixel(x + j, y + infoheader.height - i - 1, color))
					continue;
		}

}

void Bitmap::verticalflip() {
	/*
		Flips a bitmap vertically
	*/

	int pixelbytes = infoheader.bits / 8;
	int bytes_per_line = infoheader.width * pixelbytes;
	uint8_t *bmp = (uint8_t*)malloc(infoheader.imageSize);
	memset(bmp, 0, infoheader.imageSize);
	uint8_t *bmpdata = (uint8_t*)getBitmapData();

	for (int i = 0; i < infoheader.height; ++i)
		for (int j = 0; j < bytes_per_line; ++j)
			bmp[((infoheader.height - 1 - i) * bytes_per_line) + j] = bmpdata[(i * bytes_per_line) + j];

	free(bitmapdata);
	bitmapdata = bmp;
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

void Pixmap::draw(int gx, int gy) {

	/* Phasing out pixmaps, rotation no longer needed*/

	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++) {
			
			uint32_t color = map[(i * width) + j];
			if (color && color != COLOR_IGNORED) {
				if (draw_pixel(gx + j, gy + i, color))
					continue;
			}
		}
}


int load_bitmaps(bitmap_data *bmp) {

	/* Repeat until sucessfully loaded*/

	while (bmp->menubackground.load("Assets/textures/MenuBackground.bmp"))
		bmp->menubackground.load("Assets/textures/MenuBackground.bmp");

	while (bmp->hsbackground.load("Assets/textures/highscoresbackground.bmp"))
		bmp->hsbackground.load("Assets/textures/highscoresbackground.bmp");

	while (bmp->playbutton.load("Assets/textures/playbutton.bmp"))
		bmp->playbutton.load("Assets/textures/playbutton.bmp");

	while (bmp->optionsbutton.load("Assets/textures/optionsbutton.bmp"))
		bmp->optionsbutton.load("Assets/textures/optionsbutton.bmp");

	while (bmp->quitbutton.load("Assets/textures/quitbutton.bmp"))
		bmp->quitbutton.load("Assets/textures/quitbutton.bmp");

	while (bmp->options.load("Assets/textures/options.bmp"))
		bmp->options.load("Assets/textures/options.bmp");

	while (bmp->death_screen.load("Assets/textures/deathscreen.bmp"))
		bmp->death_screen.load("Assets/textures/deathscreen.bmp");

	while (bmp->death_screen_highscore.load("Assets/textures/deathscreenhighscore.bmp"))
		bmp->death_screen_highscore.load("Assets/textures/deathscreenhighscore.bmp");

	while (bmp->boxticked.load("Assets/textures/boxticked.bmp"))
		bmp->boxticked.load("Assets/textures/boxticked.bmp");

	while (bmp->game_background.load("Assets/textures/gamebackground.bmp"))
		bmp->game_background.load("Assets/textures/gamebackground.bmp");

	while (bmp->pause_message.load("Assets/textures/pausemessage.bmp"))
		bmp->pause_message.load("Assets/textures/pausemessage.bmp");

	while (bmp->splash.load("Assets/textures/splash.bmp"))
		bmp->splash.load("Assets/textures/splash.bmp");

	while (bmp->score_header.load("Assets/textures/score_header.bmp"))
		bmp->score_header.load("Assets/textures/score_header.bmp");

	while (bmp->hp_header.load("Assets/textures/hp_header.bmp"))
		bmp->hp_header.load("Assets/textures/hp_header.bmp");

	while (bmp->hp_header_low.load("Assets/textures/hp_header_low.bmp"))
		bmp->hp_header_low.load("Assets/textures/hp_header_low.bmp");

	while (bmp->fps_header.load("Assets/textures/fps_header.bmp"))
		bmp->fps_header.load("Assets/textures/fps_header.bmp");

	while (bmp->alien_ship.load("Assets/textures/alienship.bmp"))
		bmp->alien_ship.load("Assets/textures/alienship.bmp");

	while (bmp->teleport_ready_header.load("Assets/textures/jmpheaderrdy.bmp"))
		bmp->teleport_ready_header.load("Assets/textures/jmpheaderrdy.bmp");

	while (bmp->teleport_not_ready_header.load("Assets/textures/jmpheadernotrdy.bmp"))
		bmp->teleport_not_ready_header.load("Assets/textures/jmpheadernotrdy.bmp");

	while (bmp->medium_score.load("Assets/textures/mediumscore.bmp"))
		bmp->medium_score.load("Assets/textures/mediumscore.bmp");

	while (bmp->large_score.load("Assets/textures/largescore.bmp"))
		bmp->large_score.load("Assets/textures/largescore.bmp");

	while (bmp->alien_score.load("Assets/textures/alienscore.bmp"))
		bmp->alien_score.load("Assets/textures/alienscore.bmp");

	while (bmp->large_asteroid.load("Assets/textures/LargeAsteroid.bmp"))
		bmp->large_asteroid.load("Assets/textures/LargeAsteroid.bmp");

	while (bmp->medium_asteroid.load("Assets/textures/MediumAsteroid.bmp"))
		bmp->medium_asteroid.load("Assets/textures/MediumAsteroid.bmp");

	while (bmp->pix_ship_blue.load("Assets/textures/pix_ship_blue.bmp"))
		bmp->pix_ship_blue.load("Assets/textures/pix_ship_blue.bmp");

	while (bmp->pix_ship_blue_bt.load("Assets/textures/pix_ship_blue_bt.bmp"))
		bmp->pix_ship_blue_bt.load("Assets/textures/pix_ship_blue_bt.bmp");

	while (bmp->pix_ship_blue_pt.load("Assets/textures/pix_ship_blue_pt.bmp"))
		bmp->pix_ship_blue_pt.load("Assets/textures/pix_ship_blue_pt.bmp");

	while (bmp->pix_ship_blue_st.load("Assets/textures/pix_ship_blue_st.bmp"))
		bmp->pix_ship_blue_st.load("Assets/textures/pix_ship_blue_st.bmp");

	while (bmp->p_arrow.load("Assets/textures/parrow.bmp"))
		bmp->p_arrow.load("Assets/textures/parrow.bmp");

	while (bmp->slidemarker.load("Assets/textures/slidemarker.bmp"))
		bmp->slidemarker.load("Assets/textures/slidemarker.bmp");
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
