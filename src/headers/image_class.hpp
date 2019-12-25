#ifndef IMAGE_CLASS_HPP
#define IMAGE_CLASS_HPP

#include <iostream>
#include <cstring>
#include <utility>
#include "private/image_class_defines.hpp"
#include "private/image_class_texturepacks.hpp"

#include <SDL.h>
#include <SDL_image.h>

// whether to use the struct or not per blit
enum SDL_USEFLIPOBJ {
	SDL_USEFLIPOBJ_NO	= 0,
	SDL_USEFLIPOBJ_YES	= 1
};

enum SDL_PIVOTPOINT {
	SDL_PIVOTPOINT_ZERO = 0,
	SDL_PIVOTPOINT_CENTER,
	SDL_PIVOTPOINT_TOPLEFT,
	SDL_PIVOTPOINT_TOPRIGHT,
	SDL_PIVOTPOINT_BOTTOMLEFT,
	SDL_PIVOTPOINT_BOTTOMRIGHT
};

struct CSDL_FlipObj {
	double angle;
	SDL_Point* center;
	SDL_RendererFlip flip;
	SDL_USEFLIPOBJ use;
	
	CSDL_FlipObj() : angle(0), center(nullptr),
		flip(SDL_FLIP_NONE), use(SDL_USEFLIPOBJ_NO) {}
};

class Image {
	// sets all member data to their default values
	void set_vals();

	// allocates memory for a character array
	size_t alloc_str(const char* str);

	// frees memory associated with a character array
	void dealloc_str();

	// the defacto move constructor: sets all the variables here at the expense of _src
	void move(Image&& src);

	// the defacto copy constructor: creates an identical copy of _src
	void copy(const Image& src);


	public:

	// object pointing toward the screen to blit onto
	SDL_Renderer* renderer;


	// constructors and destructor //

	// constructs a basic Image object with default values
	Image();

	// constructs an Image object and also loads a file and sets the renderer
	Image(std::string file, SDL_Renderer* renderer);

	// constructs a copy of the original object
	Image(const Image& _src) {this->copy(_src);}

	// copy operator
	Image& operator=(const Image& _rhs) {
		this->copy(_rhs);
		return *this;
	}

	// move constructor
	Image(Image&& _src) {this->move(std::move(_src));}

	// move operator
	Image& operator=(Image&& _rhs) {
		this->move( std::move(_rhs) );
		return *this;
	}

	// destroys the object, freeing all memory found on the heap
	virtual ~Image() {unload();}


	// functions that allocate memory //

	// opens the selected file into memory; returns true on success
	bool open(std::string file);

	// opens the selected file into memory; also sets a colorkey; returns true on success
	bool open(std::string file, uint32_t colorkey);

	// registers a shared texture pack with this instance of the class
	bool use_pack(TexturePack* pack);


	// functions that manipulate/free memory //

	// destroys the texture and clears all member variables. Called by the destructor
	void unload();


	// draw functions //

	// blit to the screen: a short version of the next function
	void blit(const SDL_Rect* _vpt = nullptr) {blit(0, 0, _vpt);}

	// blit to the screen: accepts an XY coordinate and an optional rectangle acting as a viewport
	virtual void blit(int x, int y, const SDL_Rect* vpt = nullptr);


	// set functions //

	// sets the width and height of the image when blitted
	void set_blit_size(int _width, int _height) {
		DestRect.w = _width;
		DestRect.h = _height;
	}

	// resets the size of the blitting rectangle
	void reset_blit_rect() {
		DestRect.w = ImageRect.w;
		DestRect.h = ImageRect.h;
	}

	// sets the size of the source image rectangle
	void set_clip_size(int _width, int _height) {
		ClipRect.w = _width;
		ClipRect.h = _height;
	}

	// sets the size and offsets of the clipping rectangle
	void set_clip_rect(int width, int height, const SDL_Point* xy);

	// resets the clipping rectangle
	void reset_clip_rect() {
		ClipRect = ImageRect;
	}

	// sets a flag to either use or not use image flipping and rotation
	void use_flip(SDL_USEFLIPOBJ _flag) {
		FlipFlags.use = _flag;
	}

	// sets the type of flip being used
	void flip(SDL_RendererFlip _type) {
		FlipFlags.flip = _type;
	}

	// sets the rotation angle
	void set_angle(double _angle) {
		FlipFlags.angle = _angle;
	}

	// sets the rotation point
	void set_point_xy(int _x, int _y) {
		Center.x = _x;
		Center.y = _y;
	}

	// sets the rotation point according to a preset
	void point_preset(SDL_PIVOTPOINT preset);

	// sets the internal blendmode
	int set_blend_mode(SDL_BlendMode blendmode);

	// sets the current alpha value
	int set_alpha(uint8_t alpha);

	// sets the current color modulation: set it to white to disable it (white = #ffffff00 [alpha not factored in here])
	int set_color_mod(uint32_t color32bit);


	// get functions //

	// returns the blit destination and size rectangle [READONLY]
	const SDL_Rect& get_dest_rect() const {return DestRect;}

	// returns the source image clipping rectangle [READONLY]
	const SDL_Rect& get_clip_rect() const {return ClipRect;}

	// returns a rectangle with the image's true dimensions [READONLY]
	const SDL_Rect& get_file_rect() const {return ImageRect;}

	// get temporary rectangle
	SDL_Rect& get_temp_rect() {return TempRect;}

	// get the underlying texture [READONLY]
	const SDL_Texture* get_texture() const {return Texture;}

	/* get_blend_mode returns the currently active blend mode for the active texture
	   get_alpha returns the alpha value of the currently active texture. If the return value is
	   -1 that means there is an error. If the operation is a success, and if a pointer to an integer
	   is provided, the pointer is also given the alpha value.
	*/
	SDL_BlendMode get_blend_mode() const PURE;
	int get_alpha(uint8_t* ptr = nullptr);

	/* get_angle returns the number of degrees the texture is rotated
	   get_point returns the XY coordinate the rotation pivots on
	   horz_direction returns 1 if the image is flipped horizontally or zero if it isn't
	*/
	double get_angle() const {return FlipFlags.angle;}
	SDL_Point get_point() const {return Center;}
	int horz_direction() const {
		//return (FlipFlags.flip & SDL_FLIP_HORIZONTAL) ? 1 : 0;
		return (FlipFlags.flip & SDL_FLIP_HORIZONTAL) != 0;
	}

	/* returns the current color modulation value, or -1 if there is an error.
	   In addition, if a pointer is given, it will be given the color value upon success.
	*/
	int get_color_mod(uint8_t* r, uint8_t* g, uint8_t* b);


	// error/debug functions //

	// returns a string detailing the last error that has occurred
	static const char* get_error() {return ErrorString.c_str();}

	// returns a file path to the currently loaded image
	const char* get_file() const {return (File != nullptr) ? File : "undefined";}

	// returns true if there is an error
	bool error() const {return Error;}

	// returns true if the texture is loaded with an image
	bool loaded() const {return Loaded;}

	// prints out a variety of debug information, but DEBUG must be defined first
	void dprint() const;


	protected:

	// texture
	SDL_Texture* Texture;

	// texture pack, used so that multiple instances of this class can share one texture
	TexturePack* Pack;

	// rectangles that define the image
	SDL_Rect DestRect;
	SDL_Rect ClipRect;
	SDL_Rect ImageRect;
	SDL_Rect TempRect;

	// various configuration variables for the image
	uint32_t	ColorkeyValue;	// specific color, if any, that doesn't get drawn for this image
	uint8_t		BlendMode;	// a number for the active blend mode of this image
	CSDL_FlipObj	FlipFlags;	// flipping and rotation
	SDL_Point	Center;		// where the rotation pivots from

	// miscellaneous class information
	char*	File;     // file this texture is based on
	bool	Colorkey; // whether or not the colorkey is set for this image
	bool	Error;    // whether or not there is an error
	bool	Loaded;   // whether or not a texture is loaded

	// error string, shared across all instances of this class and derived classes
	static std::string ErrorString;
};

#endif /* IMAGE_CLASS_HPP */
