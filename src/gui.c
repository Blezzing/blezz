#include<xcb/xcb.h>
#include<xcb/xcb_ewmh.h>
#include<xcb/xcb_icccm.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>

#include"gui.h"
#include"data.h"
#include"argpass.h"
#include"errors.h"

int windowHeight = 1;
int windowWidth = 0;
int windowX = 0;
int windowY = 0;

int numberOfLinesToPrint;
char** linesToPrint = NULL;

xcb_connection_t* connection = NULL;
int screenNumber = 0;
xcb_screen_t* screen = NULL;
xcb_drawable_t window = 0;

int foregroundColor = 0xffffff;
int backgroundColor = 0x000000;

xcb_gc_t fontGC;
xcb_gc_t fillGC;

static void testCookie(xcb_void_cookie_t cookie, xcb_connection_t *connection, char *errMessage ) {
    xcb_generic_error_t *error = xcb_request_check (connection, cookie);
    if (error) {
        xcb_disconnect (connection);
        guiError(errMessage);
    }
}

int grabKeyboard(int iters) {
    int i = 0;
    while(1) {
        if ( xcb_connection_has_error ( connection ) ) {
            guiError("Error in connection while grabbing keyboard");
        }
        xcb_grab_keyboard_cookie_t cc = xcb_grab_keyboard(connection, 1, window, XCB_CURRENT_TIME, XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC);
        xcb_grab_keyboard_reply_t *r = xcb_grab_keyboard_reply(connection, cc, NULL);
        if ( r ) {
            if ( r->status == XCB_GRAB_STATUS_SUCCESS ) {
                free ( r );
                return 0;
            }
            free ( r );
        }
        if ( (++i) > iters ) {
            break;
        }
        usleep ( 1000 );
    }
    return 1;
}

void releaseKeyboard() {
    xcb_ungrab_keyboard ( connection, XCB_CURRENT_TIME );
}

void fontGCInit() {
    //get font
    xcb_font_t font = xcb_generate_id (connection);
    xcb_void_cookie_t fontCookie = xcb_open_font_checked(connection,font,strlen(arguments.font),arguments.font);
    testCookie(fontCookie,connection,"can't open font");

    //get graphics content
    fontGC = xcb_generate_id (connection);
    uint32_t mask = XCB_GC_FOREGROUND | XCB_GC_BACKGROUND | XCB_GC_FONT;
    uint32_t value_list[3] = { foregroundColor, backgroundColor, font };
    xcb_create_gc(connection, fontGC, window, mask, value_list);

    //close font
    fontCookie = xcb_close_font_checked(connection, font);
    testCookie(fontCookie, connection, "can't close font");
}

void drawText(int16_t  x1, int16_t y1, const char *label ) {
    fontGCInit(arguments.font);
    xcb_void_cookie_t textCookie = xcb_image_text_8_checked(connection,strlen(label),window,fontGC,x1,y1,label);
    testCookie(textCookie,connection,"can't paste text");
    xcb_void_cookie_t gcCookie = xcb_free_gc(connection,fontGC);
    testCookie(gcCookie,connection,"can't free gc");
}

void updateWindowLocation() {
    windowX = screen->height_in_pixels / 2;
    windowY = screen->width_in_pixels / 2 - windowWidth / 2;

    uint32_t values[] = { windowY, windowX };
    xcb_configure_window (connection, window, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, values);
}

void updateData() {
    if (linesToPrint == NULL) {
        linesToPrint = allocForDirToStrings();
    }
    dirToStrings(linesToPrint,&numberOfLinesToPrint);
}

void updateWindowFlags() {
    //This code is based on conversation from https://lists.freedesktop.org/archives/xcb/2010-December/006718.html it is not pretty, but it works. Most of the time..
    xcb_intern_atom_cookie_t cookie1 = xcb_intern_atom(connection, 0, strlen("_NET_WM_WINDOW_TYPE"),"_NET_WM_WINDOW_TYPE");
    xcb_intern_atom_reply_t* reply = xcb_intern_atom_reply(connection, cookie1, 0);
    xcb_intern_atom_cookie_t cookie2 = xcb_intern_atom(connection, 0, strlen("_NET_WM_WINDOW_TYPE_DOCK"), "_NET_WM_WINDOW_TYPE_DOCK");
    xcb_intern_atom_reply_t* reply2 = xcb_intern_atom_reply(connection, cookie2, 0);
    xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, reply->atom, XCB_ATOM_ATOM, 32, 1, &(reply2->atom));
    //xcb_aux_sync(connection); //this is a to maybe get rid of the random borders...
}

void connectionInit() {
    connection = xcb_connect(NULL,&screenNumber);
}

void screenInit() {
    screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;
}

void fillGCInit() {
    fillGC = xcb_generate_id(connection);
    uint32_t mask = XCB_GC_FOREGROUND | XCB_GC_BACKGROUND | XCB_GC_GRAPHICS_EXPOSURES;
    uint32_t values[3] = {backgroundColor,backgroundColor,0};
    xcb_create_gc(connection,fillGC,window,mask,values);
}

void windowInit() {
    uint32_t values[3];
    uint32_t mask = 0;

    window = xcb_generate_id(connection);
    mask = XCB_CW_BACK_PIXEL| XCB_CW_EVENT_MASK;
    values[0] = backgroundColor;
    values[1] = XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_EXPOSURE;
    xcb_void_cookie_t windowCookie = xcb_create_window_checked(connection,screen->root_depth,window,screen->root,windowX,windowY,windowWidth,windowHeight,0,XCB_WINDOW_CLASS_INPUT_OUTPUT,screen->root_visual,mask,values);
    testCookie(windowCookie,connection,"can't create window");
}

void mapWindow() {
    xcb_void_cookie_t mapCookie = xcb_map_window_checked (connection, window);
    testCookie(mapCookie,connection,"can't map window");
}

void updateWindowSize() {
    windowHeight = (numberOfLinesToPrint * 20 + 16);
    xcb_configure_window (connection, window, XCB_CONFIG_WINDOW_HEIGHT, &windowHeight);
}

void clearWindow(){
    xcb_rectangle_t rect = {0, 0, windowWidth, windowHeight};
    xcb_poly_fill_rectangle(connection, window, fillGC, 1, &rect);
}

void drawAllText() {
    updateWindowSize();
    clearWindow();

    for (int i = 0; i < numberOfLinesToPrint; i++) {
        drawText(18,20*(i+1), linesToPrint[i]);
    }
}

//i hate myself for doing this.... it have to be revisited later.
char getCharfromKeycode(int code){
    switch (code){
        //row1
        case 10: return '1';
        case 11: return '2';
        case 12: return '3';
        case 13: return '4';
        case 14: return '5';
        case 15: return '6';
        case 16: return '7';
        case 17: return '8';
        case 18: return '9';
        case 19: return '0';
        case 20: return '+';
        //row2
        case 24: return 'q';
        case 25: return 'w';
        case 26: return 'e';
        case 27: return 'r';
        case 28: return 't';
        case 29: return 'y';
        case 30: return 'u';
        case 31: return 'i';
        case 32: return 'o';
        case 33: return 'p';
        //row3
        case 38: return 'a';
        case 39: return 's';
        case 40: return 'd';
        case 41: return 'f';
        case 42: return 'g';
        case 43: return 'h';
        case 44: return 'j';
        case 45: return 'k';
        case 46: return 'l';
        //row4
        case 52: return 'z';
        case 53: return 'x';
        case 54: return 'c';
        case 55: return 'v';
        case 56: return 'b';
        case 57: return 'n';
        case 58: return 'm';
        case 59: return ',';
        case 60: return '.';
        case 61: return '-';
        //whatever
        default: return '\0';
    }
}

int handleEvent(xcb_generic_event_t* event) {
    int shouldFinishAfter = 0;
    switch(event->response_type & ~0x80) { //why this mask?...
        case XCB_EXPOSE: {
            drawAllText();
            break;
        }
        case XCB_KEY_PRESS: {
            xcb_keycode_t keycode = ((xcb_key_press_event_t *)event)->detail;
            if (keycode == 9) {
                shouldFinishAfter = 1;
                break;
            }

            char character = getCharfromKeycode(keycode);
            int selectionResult = selectElement(character);

            if (selectionResult == ELEMENT_SELECTION_OVER) {
                shouldFinishAfter = 1; 
                break;
            }
            
            if (selectionResult == ELEMENT_SELECTION_FALSE) {
                break;
            }
            
            updateData();
            drawAllText();
            break;
        }
    } 
    free(event);

    return shouldFinishAfter;
}

void guiStart() {
    windowWidth = arguments.windowWidth;
    connectionInit();
    screenInit();
    windowInit();
    fontGCInit();
    fillGCInit();
    xcb_flush(connection);

    clearWindow();
    mapWindow();
    xcb_flush(connection);

    updateWindowFlags();
    updateWindowLocation();
    xcb_flush(connection);

    updateData();
}

void guiEnd() {
    //give back control
    releaseKeyboard();

    //disconnect
    xcb_disconnect(connection);
    xcb_flush(connection);

    //i'm sorry. This is needed to avoid hanging interface after levelup+another key pressed at the same time from root menu..
    connection = xcb_connect(NULL,0);
    xcb_disconnect(connection);
    xcb_flush(connection);
}

void guiEventLoop() {
    guiStart();

    int finished = grabKeyboard(10);   //return 1 if failure
    xcb_generic_event_t* event;  
    while(!finished && (event = xcb_wait_for_event(connection))) {
        finished = handleEvent(event); //return 1 if an expected exit condition is met
    }

    guiEnd();
}