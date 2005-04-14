// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// lookat
//
// Copyright 2003  Braden McDaniel
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# include <iostream>
# include <SDL.h>
# include <openvrml/browser.h>
# include <openvrml/gl/viewer.h>

extern "C" Uint32 update_timer_callback(Uint32 interval, void * param);

namespace {

    class sdl_error : public std::runtime_error {
    public:
        explicit sdl_error(const std::string & message);
        virtual ~sdl_error() throw ();
    };

    class sdl_viewer : public openvrml::gl::viewer {
        friend Uint32 update_timer_callback(Uint32 interval, void * param);

        static const Uint32 video_mode_flags;

        SDL_TimerID update_timer_id;
        bool mouse_button_down;

    public:
        static const int redraw_event_code = 1;
        static const int update_event_code = 2;

        explicit sdl_viewer(const std::string & title) throw (sdl_error);
        virtual ~sdl_viewer() throw ();

        void run();

        //
        // Window system specific methods
        //
        virtual void post_redraw();
        virtual void set_cursor(cursor_style c);
        virtual void swap_buffers();
        virtual void set_timer(double);
    };
}

int main(int argc, char * argv[])
{
    using std::exception;
    using std::cerr;
    using std::cout;
    using std::endl;
    using std::string;
    using std::vector;

    try {
        using openvrml::browser;

        string inputUrl;
        string inputName;

        const char * const usage = " file.wrl\n";

        for (int i = 1; i < argc; ++i) {
            if (*argv[i] == '-') {
                if (strcmp(argv[i], "-url") == 0) {
                    inputUrl = argv[++i];
                } else {
                    cerr << "Error: unrecognized option " << argv[i] << '\n';
                    cerr << "Usage: " << argv[0] << usage << endl;
                    exit(EXIT_FAILURE);
                }
            } else if (inputName.empty()) {
                inputName = argv[i];
            } else {
                cerr << "Usage: " << argv[0] << usage << endl;
                exit(EXIT_FAILURE);
            }
        }

        if (inputName.empty()) {
            if (inputUrl.empty()) {
                inputName = inputUrl;
            } else {
                inputName = inputUrl = "-"; // Read stdin
            }
        }

        if (inputUrl.empty()) { inputUrl = inputName; }

        sdl_viewer v(inputUrl);
        browser b(cout, cerr);
        b.viewer(&v);

        vector<string> uri(1, inputUrl);
        vector<string> parameter;
        b.load_url(uri, parameter);

        v.run();
    } catch (std::exception & ex) {
        cerr << ex.what() << endl;
    }
}

namespace {

    sdl_error::sdl_error(const std::string & message):
        std::runtime_error(message)
    {}

    sdl_error::~sdl_error() throw ()
    {}

    const Uint32 sdl_viewer::video_mode_flags(SDL_OPENGL | SDL_RESIZABLE);

    sdl_viewer::sdl_viewer(const std::string & title) throw (sdl_error):
        update_timer_id(0),
        mouse_button_down(false)
    {
        static const size_t initial_width = 640;
        static const size_t initial_height = 480;

        if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) < 0) {
            throw sdl_error(SDL_GetError());
        }
        if (!SDL_SetVideoMode(initial_width,
                              initial_height,
                              0,
                              sdl_viewer::video_mode_flags)) {
            throw sdl_error(SDL_GetError());
        }
        this->resize(initial_width, initial_height);

        static const char * const icon = 0;
        SDL_WM_SetCaption(title.c_str(), icon);
    }

    sdl_viewer::~sdl_viewer() throw ()
    {
        SDL_Quit();
    }

    void sdl_viewer::run()
    {
        this->update();
        bool done = false;
        bool need_update = false;
        bool need_redraw = false;
        do {
            SDL_Event event;
            sdl_viewer::event_info viewer_event_info;
            SDL_WaitEvent(0);
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                case SDL_VIDEOEXPOSE:
                    this->post_redraw();
                    break;
                case SDL_VIDEORESIZE:
                    SDL_SetVideoMode(event.resize.w,
                                     event.resize.h,
                                     0,
                                     sdl_viewer::video_mode_flags);
                    this->resize(event.resize.w, event.resize.h);
                    this->post_redraw();
                    break;
                case SDL_KEYDOWN:
                    viewer_event_info.event = sdl_viewer::event_key_down;
                    switch (event.key.keysym.sym) {
                    case SDLK_HOME:
                        viewer_event_info.what = sdl_viewer::key_home;
                        break;
                    case SDLK_LEFT:
                        viewer_event_info.what = sdl_viewer::key_left;
                        break;
                    case SDLK_UP:
                        viewer_event_info.what = sdl_viewer::key_up;
                        break;
                    case SDLK_RIGHT:
                        viewer_event_info.what = sdl_viewer::key_right;
                        break;
                    case SDLK_DOWN:
                        viewer_event_info.what = sdl_viewer::key_down;
                        break;
                    case SDLK_PAGEDOWN:
                        viewer_event_info.what = sdl_viewer::key_page_down;
                        break;
                    case SDLK_PAGEUP:
                        viewer_event_info.what = sdl_viewer::key_page_up;
                        break;
                    default:
                        break;
                    }
                    this->input(&viewer_event_info);
                    break;
                case SDL_MOUSEBUTTONDOWN: case SDL_MOUSEBUTTONUP:
                    viewer_event_info.event = event.button.state == SDL_PRESSED
                                            ? sdl_viewer::event_mouse_click
                                            : sdl_viewer::event_mouse_release;
                    viewer_event_info.what = event.button.button - 1;
                    viewer_event_info.x = event.button.x;
                    viewer_event_info.y = event.button.y;
                    this->input(&viewer_event_info);
                    break;
                case SDL_MOUSEMOTION:
                    if (!event.motion.state) {
                        viewer_event_info.event = sdl_viewer::event_mouse_move;
                        viewer_event_info.x = event.motion.x;
                        viewer_event_info.y = event.motion.y;
                        this->input(&viewer_event_info);
                    } else {
                        for (Uint8 button = SDL_BUTTON_LEFT;
                             button < 4;
                             ++button) {
                            if (event.motion.state & SDL_BUTTON(button)) {
                                viewer_event_info.event =
                                    sdl_viewer::event_mouse_drag;
                                viewer_event_info.what = button - 1;
                                viewer_event_info.x = event.motion.x;
                                viewer_event_info.y = event.motion.y;
                                this->input(&viewer_event_info);
                            }
                        }
                    }
                    break;
                case SDL_QUIT:
                    done = true;
                    break;
                case SDL_USEREVENT:
                    switch (event.user.code) {
                    case redraw_event_code:
                        need_redraw = true;
                        break;
                    case update_event_code:
                        need_update = true;
                        break;
                    }
                    break;
                default:
                    break;
                }
            }
            if (need_update) {
                this->update();
                need_update = false;
            }
            if (need_redraw) {
                this->redraw();
                need_redraw = false;
            }
        } while (!done);
    }

    void sdl_viewer::post_redraw()
    {
        SDL_Event redraw_event;
        redraw_event.type = SDL_USEREVENT;
        redraw_event.user.code = sdl_viewer::redraw_event_code;
        redraw_event.user.data1 = 0;
        redraw_event.user.data2 = 0;
        SDL_PushEvent(&redraw_event);
    }

    void sdl_viewer::set_cursor(const cursor_style c)
    {}

    void sdl_viewer::swap_buffers()
    {
        SDL_GL_SwapBuffers();
    }

    Uint32 update_timer_callback(const Uint32 interval, void * const param)
    {
        sdl_viewer & v = *static_cast<sdl_viewer *>(param);
        SDL_RemoveTimer(v.update_timer_id);
        v.update_timer_id = 0;
        SDL_Event update_event;
        update_event.type = SDL_USEREVENT;
        update_event.user.code = sdl_viewer::update_event_code;
        update_event.user.data1 = 0;
        update_event.user.data2 = 0;
        SDL_PushEvent(&update_event);
        return 0;
    }

    void sdl_viewer::set_timer(const double t)
    {
        if (!this->update_timer_id) {
            const Uint32 interval = Uint32(1000.0 * t + 20); // milliseconds.
            this->update_timer_id =
                SDL_AddTimer(interval, update_timer_callback, this);
        }
    }
}
