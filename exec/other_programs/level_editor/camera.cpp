#include "camera.h"

StaticDArray<SDL_Rect,3>    camera::Rects;
bool                        camera::Scroll = false;
SDL_Rect                    camera::Cam = {0, 0, WIDTH, HEIGHT};

using namespace camera;

void camera::define(const SDL_Rect& src) {
    // reference to the camera rect
    StaticDArray<int,6> vals = {
        WIDTH, HEIGHT,
        src.w, src.h,
        WIDTH, HEIGHT
    };

    auto itrCam = Rects.begin();

    if (src.w > WIDTH || src.h > HEIGHT)
        Scroll = true;

    // define the rectangle of the camera
    for (int x = 0; x < 6; x += 2) {
        itrCam->x = 0;
        itrCam->y = 0;
        itrCam->w = vals[x];
        itrCam->h = vals[x+1];
        ++itrCam;
    }
}

void camera::scrollx(int speed) {
    if (Scroll) {
        Cam.x += speed;
        keepbound();
    }
}

void camera::keepbound() {
    // camera is to the left of the level bounds
    if (Cam.x < 0)
        Cam.x = 0;

    // if the camera's to the right of the level, push it back
    const SDL_Rect& level = Rects[1];

    if ((Cam.x + Cam.w) > level.w)
        Cam.x = level.w - Cam.w;
}
