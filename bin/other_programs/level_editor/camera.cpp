#include "camera.h"

StaticDArray<SDL_Rect,3>    camera::rects;
bool                        camera::scroll = false;
SDL_Rect                    camera::cam = {0, 0, WIDTH, HEIGHT};

using namespace camera;

void camera::define(const SDL_Rect& src) {
    if (src.w > WIDTH || src.h > HEIGHT)
        scroll = true;

    // reference to the camera rect
    StaticDArray<int,6> vals = {
        WIDTH, HEIGHT,
        src.w, src.h,
        WIDTH, HEIGHT
    };

    // define the rectangle of the camera
    auto itrCam = rects.begin();
    for (int x = 0; x < 6; x += 2) {
        itrCam->x = 0;
        itrCam->y = 0;
        itrCam->w = vals[x];
        itrCam->h = vals[x+1];
        ++itrCam;
    }
}

void camera::scrollx(int speed) {
    if (scroll) {
        cam.x += speed;
        keepbound();
    }
}

void camera::keepbound() {
    // camera is to the left of the level bounds
    if (cam.x < 0)
        cam.x = 0;

    // if the camera's to the right of the level, push it back
    const SDL_Rect& level = rects[1];

    if ((cam.x + cam.w) > level.w)
        cam.x = level.w - cam.w;
}
