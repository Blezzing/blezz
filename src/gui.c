#include<xcb/xcb.h>
#include<xcb/xcb_keysyms.h>
#include<xkbcommon/xkbcommon-keysyms.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>

#include"gui.h"
#include"data.h"
#include"argpass.h"
#include"errors.h"
#include"keys.h"

//Known state of the window
int windowH = 100;
int windowW = 100;
int windowX = 0;
int windowY = 0;

//Magic number, should be changed when changing font rendering method.
int lineHeight = 19;

int numberOfLinesToPrint;
char** linesToPrint = NULL;

//X handles
xcb_connection_t* connection = NULL;
xcb_screen_t* screen = NULL;
xcb_key_symbols_t *ksyms = NULL;
xcb_drawable_t window = 0;
xcb_gc_t fontGC = 0;
xcb_gc_t fillGC = 0;

void testCookie(xcb_void_cookie_t cookie, xcb_connection_t *connection, char *errMessage ) {
    xcb_generic_error_t *error = xcb_request_check (connection, cookie);
    if (error) {
        xcb_disconnect (connection);
        guiError(errMessage);
    }
}

void grabKeyboard(int iters) {
    int i = 0;
    while(1) {
        if (xcb_connection_has_error(connection)) {
            guiError("Error in connection while grabbing keyboard");
        }

        xcb_grab_keyboard_cookie_t cookie = xcb_grab_keyboard(connection, 1, window, XCB_CURRENT_TIME, XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC);
        xcb_grab_keyboard_reply_t* reply = xcb_grab_keyboard_reply(connection, cookie, NULL);

        if (reply) {
            if (reply->status == XCB_GRAB_STATUS_SUCCESS) {
                free (reply);
                return;
            }
            free (reply);
        }

        if ((++i)>iters) {
            break;
        }
        usleep ( 10000 );
    }
    guiError("failed grabbing a keyboard.");
}

void releaseKeyboard() {
    xcb_ungrab_keyboard (connection, XCB_CURRENT_TIME);
}

void fontGCInit() {
    //get font
    xcb_font_t font = xcb_generate_id (connection);
    xcb_open_font(connection,font,strlen(arguments.font),arguments.font);

    //get graphics content
    fontGC = xcb_generate_id (connection);
    uint32_t mask = XCB_GC_FOREGROUND | XCB_GC_BACKGROUND | XCB_GC_FONT;
    uint32_t value_list[3] = { arguments.fgColor, arguments.bgColor, font };
    xcb_create_gc(connection, fontGC, window, mask, value_list);

    //close font
    xcb_close_font(connection, font);
}

void drawText(int16_t  x1, int16_t y1, const char *label ) {
    fontGCInit(arguments.font);
    xcb_image_text_8(connection,strlen(label),window,fontGC,x1,y1,label);
    xcb_void_cookie_t gcCookie = xcb_free_gc(connection,fontGC);
    testCookie(gcCookie,connection,"can't free gc");
}

uint32_t calcXPos(){
    switch (arguments.winXPos) {
        case (XLeft)  : return 0 + arguments.winYOffset; break;
        case (XMid)   : return screen->width_in_pixels / 2 - arguments.windowWidth / 2 + arguments.winYOffset; break;
        case (XRight) : return screen->width_in_pixels - arguments.windowWidth; break;
        default       : return 0;
    }
}

uint32_t calcYPos(){    
    switch (arguments.winYPos) {
        case (YTop) : return 0 + arguments.winXOffset; break;
        case (YMid) : return screen->height_in_pixels / 2 + arguments.winXOffset; break;
        case (YBot) : return screen->height_in_pixels + arguments.winXOffset; break;
        default     : return 0;
    }
}

uint32_t calcHeight(){
    return (numberOfLinesToPrint * 20) + arguments.topIndent + arguments.botIndent;
}

uint32_t calcWidth(){
    return arguments.windowWidth; //Possibilities for future.
}

//Answer is handled in event chain
void requestNewWindowGeometry() {
    uint32_t mask = XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y | XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT;
    uint32_t values[4] = {calcXPos(), calcYPos(), calcWidth(), calcHeight()};
    xcb_configure_window(connection, window, mask, values);
}

//Get data to print
void updateData() {
    if (linesToPrint == NULL) {
        linesToPrint = allocForDirToStrings();
    }
    dirToStrings(linesToPrint,&numberOfLinesToPrint);
}

void setFlagsForWM() {
    xcb_intern_atom_cookie_t cookie1 = xcb_intern_atom(connection, 0, strlen("_NET_WM_WINDOW_TYPE"),"_NET_WM_WINDOW_TYPE");
    xcb_intern_atom_cookie_t cookie2 = xcb_intern_atom(connection, 0, strlen("_NET_WM_WINDOW_TYPE_DOCK"), "_NET_WM_WINDOW_TYPE_DOCK");
    xcb_intern_atom_reply_t* reply1 = xcb_intern_atom_reply(connection, cookie1, 0);
    xcb_intern_atom_reply_t* reply2 = xcb_intern_atom_reply(connection, cookie2, 0);
    xcb_void_cookie_t cc1 = xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, reply1->atom, XCB_ATOM_ATOM, 32, 1, &(reply2->atom));
    xcb_void_cookie_t cc2 = xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, XCB_ATOM_WM_CLASS, XCB_ATOM_STRING, 8, strlen("blezz\0Blezz\0"), "blezz\0Blezz\0");
    xcb_void_cookie_t cc3 = xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, strlen("Blezz"), "Blezz");
    testCookie(cc1, connection, "failed setting _NET_WM_WINDOW_TYPE");
    testCookie(cc2, connection, "failed setting WM_CLASS");
    testCookie(cc3, connection, "failed setting WM_NAME");
}

void connectionInit() {
    int screenNumber = 0;
    connection = xcb_connect(NULL,&screenNumber);
}

void screenInit() {
    screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;
}

void ksymsInit() {
    ksyms = xcb_key_symbols_alloc(connection);
}

void fillGCInit() {
    fillGC = xcb_generate_id(connection);
    uint32_t mask = XCB_GC_FOREGROUND | XCB_GC_BACKGROUND | XCB_GC_GRAPHICS_EXPOSURES;
    uint32_t values[3] = {arguments.bgColor, arguments.bgColor, 0};
    xcb_create_gc(connection, fillGC, window, mask, values);
}

void windowInit() {
    window = xcb_generate_id(connection);
    uint32_t mask = XCB_CW_BACK_PIXEL| XCB_CW_EVENT_MASK;
    uint32_t values[2] = { arguments.bgColor, XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_STRUCTURE_NOTIFY };
    xcb_void_cookie_t windowCookie = xcb_create_window_checked(connection,screen->root_depth,window,screen->root,windowX,windowY,windowW,windowH,0,XCB_WINDOW_CLASS_INPUT_OUTPUT,screen->root_visual,mask,values);
    testCookie(windowCookie,connection,"can't create window");
}

void dimensionsInit(){
    //Set the global values to the initial state for initializing window.
    windowH = calcHeight();
    windowW = calcWidth();
    windowX = calcXPos();
    windowY = calcYPos();
}

void mapWindow() {
    xcb_void_cookie_t mapCookie = xcb_map_window_checked (connection, window);
    testCookie(mapCookie,connection,"can't map window");
}

void clearWindow(){
    xcb_rectangle_t rect = {0, 0, windowW, windowH};
    xcb_poly_fill_rectangle(connection, window, fillGC, 1, &rect);
}

void drawAllText() {
    for (int i = 0; i < numberOfLinesToPrint; i++) {
        drawText(arguments.leftIndent,lineHeight*(i+1)+arguments.topIndent, linesToPrint[i]);
    }
}

void renderWindow(){
    clearWindow();
    drawAllText();
}

int handleKeyPress(xcb_generic_event_t* event) {
    xcb_keycode_t keycode;
    uint16_t state;
    xcb_keysym_t keysym;

    state = ((xcb_key_press_event_t *)event)->state;
    keycode = ((xcb_key_press_event_t *)event)->detail;
    keysym = xcb_key_symbols_get_keysym(ksyms, keycode, state);

    if (keysym == XKB_KEY_Escape) { //If the keycode represents escape
        return 1;
    }

    char character = getCharfromKeysym(keysym);
    int selectionResult = selectElement(character);

    if (selectionResult == ELEMENT_SELECTION_OVER) {
        return 1;
    }

    if (selectionResult == ELEMENT_SELECTION_TRUE) {
        updateData();
        requestNewWindowGeometry();
        renderWindow();
        return 0;
    }

    return 0;
}

void handleConfigure(xcb_generic_event_t* event){
    //Set variable to newest known configuration.
    windowX = ((xcb_configure_notify_event_t*)event)->x;
    windowY = ((xcb_configure_notify_event_t*)event)->y;
    windowH = ((xcb_configure_notify_event_t*)event)->height;
    windowW = ((xcb_configure_notify_event_t*)event)->width;
}

int handleEvent(xcb_generic_event_t* event) {
    int doneFlag = 0;
    switch(event->response_type & ~0x80) { //why this mask?...
        case XCB_EXPOSE: {
            renderWindow();
            break;
        }
        case XCB_KEY_PRESS: {
            doneFlag = handleKeyPress(event);
            break;
        }
        case XCB_BUTTON_PRESS: {
            doneFlag = 1;
            break;
        }
        case XCB_CONFIGURE_NOTIFY: {
            handleConfigure(event);
            break;
        }
    } 
    free(event);

    return doneFlag;
}

void guiStart() {
    //Initialize X stuff
    connectionInit();
    screenInit();
    ksymsInit();

    //Fill with initial data..
    updateData();
    dimensionsInit();

    //Prepare window
    windowInit();

    //Let the wm know how we want to be treated
    setFlagsForWM();

    //Unsure about calling these inits here, font gc sometimes got lost after..
    fontGCInit();
    fillGCInit();

    //Now give us that shiny window!
    mapWindow();
    requestNewWindowGeometry();

    //Grab the keyboard or die trying
    grabKeyboard(1000);

    //Make sure we are heady to handle events by flushing
    xcb_flush(connection);
}

void guiEnd() {
    //Give back control
    releaseKeyboard();

    // Destroy ksyms
    xcb_key_symbols_free(ksyms);

    //Disconnect from X
    xcb_disconnect(connection);
    xcb_flush(connection);

    //I'm sorry. This is needed to avoid hanging interface after levelup+another key pressed at the same time from root menu..
    connection = xcb_connect(NULL,0);
    xcb_disconnect(connection);
    xcb_flush(connection);
}

void guiEventLoop() {
    //Get ready
    guiStart();

    int finished = 0;
    xcb_generic_event_t* event;  
    while(!finished && (event = xcb_wait_for_event(connection))) {
        finished = handleEvent(event); //return 1 if an expected exit condition is met
    }

    //Wrap stuff up
    guiEnd();
}