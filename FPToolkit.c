// VERSION 103

/*
    FPToolkit.c : A simple set of graphical tools.
    Copyright (C) 2018  Ely

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License (version 3)
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/


/*
This version (100) fixed a memory leak in
Get_Image_From_File_X  (thanks to Casey Yamamura)

and also cleaned up the business of actually returning
an int if the function was declared to do so
(thanks to Arden Rasmussen).
*/

// version should work in repl...the key
// was to eliminate calls to XSetFont and
// open the display with "MAGIC" instad of "\0"


					    
/* Version 101 :					    

1. supplies simple error messages
   if you try to open a repl window in a linux
   environment or a vice-versa.

2. Provides a function to save the image to a bmp file
   int G_save_to_bmp_file (char *fname)

*/

					    
/*  Version 102 ;
  Fixes a clipping bug in
  Horizontal_Single_Pixel_Line_X
  that was causing filled circles
  that were drawn to the left
  or the right of the window 
  leaving a residue at the left
  or right boundaries.
*/

					    
/*  Version 103 :
  Support for reading an existing bmp file and displaying it :

  int get_dimensions_of_bmp_file (char filename[], int dimensions[2])
  // return 0 if failure, else return 1

  int G_display_bmp_file (char filename[], int xoffset, int yoffset)
  // return 0 if failure, else return 1  
*/


					    

#ifndef FPT876PBNM3521
#define FPT876PBNM3521


#include <stdio.h>
#include <math.h>
#include <stdlib.h> // drand48, atoi
#include <unistd.h> // usleep
#include <time.h> // for the get_time stuff
#include <sys/time.h> 
#include <string.h> // for strlen





//====================================================================
// X stuff :


#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/keysym.h> 
#include <X11/Xutil.h>// for XComposeStatus

int Set_Color_Rgb_X (int r, int g, int b) ;


typedef XImage *XImagePointer ;
static int Xx_Win_width ;
static int Xx_Win_height ;
static int Xx_Pix_width ;
static int Xx_Pix_height ;
static int Xx_Tmp_width ;
static int Xx_Tmp_height ;
static int XxDepth;
static Display*XxDisplay;
static Window XxRootWindow;
static int XxScreenNumber;
static Window XxWindow;
static Pixmap XxPixmap;
static Pixmap XxTmpPixmap;
static XID XxDrawable;
static GC XxWindowContext;
static GC XxPixmapContext;
static GC XxTmpContext;

//static char *XxFont = "10x20" ; // g++ does not like this
static char XxFont[100] = "10x20" ;  

static XFontStruct *XxFontInfo;
static int Current_Red_Int ;
static int Current_Green_Int ;
static int Current_Blue_Int ;
static unsigned long int Current_Color_Pixel ;
static unsigned long int Last_Clear_Buffer_Pixel ;




//////////////////////////////////////////////////////////////


static int Display_Code = 100 ; // default on linux systems

int G_choose_repl_display()
// for those who want do display on Repl.it
// make this call BEFORE G_init_graphics
{
  Display_Code = 101 ;
}

//////////////////////////////////////////////////////////////







int Clear_Buffer_X() 
{
   unsigned long int p ;
   XFillRectangle(XxDisplay, XxDrawable, XxPixmapContext, 
                                           0, 0, Xx_Pix_width, Xx_Pix_height);
   XFlush(XxDisplay);  
   Last_Clear_Buffer_Pixel = Current_Color_Pixel ;

   return 1 ;
}



int Resize_window_X_helper (int request[2])
// request[0] = desired_width
// request[1] = desired_height
{
    int ww,hh ;
    int maxwidth,maxheight ;
    
    ww = request[0] ;
    hh = request[1] ;

    if ((ww == Xx_Win_width) && (hh == Xx_Win_height)) { return 1 ; }

    maxwidth = XDisplayWidth(XxDisplay,XxScreenNumber) ;
    maxheight = XDisplayHeight(XxDisplay,XxScreenNumber) ;


    if (ww <= 0) {
      printf("Requested width of %d is NOT positive.\n",ww) ;
      printf("Request refused.\n") ;
      return 0 ;
    } else if (ww > maxwidth) {
      printf("Requested width of %d exceeds max allowable of %d.\n",
	     ww,maxwidth) ;

      printf("Xx display window width truncated\n") ;
      printf("BUT the back buffer IS as specified.\n") ;
      ww = maxwidth ;
    } 

    if (hh <= 0) {
      printf("Requested height of %d is NOT positive.\n",hh) ;
      printf("Request refused.\n") ;
      return 0 ;
    } else if (hh > maxheight) {
      printf("Requested height of %d exceeds max allowable of %d.\n",
	     hh,maxheight) ;

      printf("Xx display window width truncated\n") ;
      printf("BUT the back buffer IS as specified.\n") ;
      hh = maxheight ;
    } 


    if ((ww > Xx_Pix_width)|| (hh > Xx_Pix_height)) {
	  // at least one of the dimensions of the new window
	  // is larger than the current pixmap
          // printf("at least one dimension of new win is larger than pixmaxp\n") ;

	  // create a tmp pixmap and copy current pixmap to it
          Xx_Tmp_width = Xx_Pix_width ;
          Xx_Tmp_height = Xx_Pix_height ;
	  XxTmpPixmap = XCreatePixmap(XxDisplay, XxRootWindow,
                                    Xx_Tmp_width, Xx_Tmp_height, XxDepth);
          XxTmpContext = XCreateGC(XxDisplay, XxTmpPixmap, 0, 0);

          XCopyArea(XxDisplay, XxPixmap, XxTmpPixmap, XxTmpContext,
		    0, 0,
                    Xx_Pix_width, Xx_Pix_height,
                    0,0) ;


	  // free up the old pixmap
          XFreeGC(XxDisplay, XxPixmapContext);
	  XFreePixmap(XxDisplay, XxPixmap);

	  // construct new pixmap
	  // don't make either dimension smaller
          if (ww > Xx_Pix_width)  Xx_Pix_width = ww ;
          if (hh > Xx_Pix_height) Xx_Pix_height = hh ;

	  XxPixmap = XCreatePixmap(XxDisplay, XxRootWindow,
                                    Xx_Pix_width, Xx_Pix_height, XxDepth);
          XxPixmapContext = XCreateGC(XxDisplay, XxPixmap, 0, 0);

	  // clear it out.
	  // We use the same color as was last used to clear the buffer
	  // but make sure to restore the current color when done clearing.
          XSetForeground(XxDisplay,XxPixmapContext, Last_Clear_Buffer_Pixel) ;
          XFillRectangle(XxDisplay,XxPixmap, XxPixmapContext, 
                                           0, 0, Xx_Pix_width, Xx_Pix_height);

          XSetForeground(XxDisplay,XxPixmapContext, Current_Color_Pixel) ;



	  // copy the data back
          int yy ;
          if (Xx_Pix_height >= Xx_Tmp_height) {
            yy = Xx_Pix_height - Xx_Tmp_height ;
            XCopyArea(XxDisplay, XxTmpPixmap, XxPixmap, XxPixmapContext,
	              0,0,
                      Xx_Tmp_width, Xx_Tmp_height,
                      0,yy) ;
	  } else {
            yy = Xx_Pix_height - Xx_Tmp_height ;
            XCopyArea(XxDisplay, XxTmpPixmap, XxPixmap, XxPixmapContext,
	              0,-yy,
                      Xx_Tmp_width, Xx_Tmp_height,
                      0,0) ;
	  }

          XxDrawable = XxPixmap;
	  //          XSetFont(XxDisplay, XxPixmapContext, XxFontInfo->fid) ;
	  // probably not necessary and causes problems
	  // with repl and maybe windows installs
	  
	  
    } // end if ((ww > Xx_Pix_width)|| (hh > Xx_Pix_height)) 



    Xx_Win_width = ww ;
    Xx_Win_height = hh ;

    return 1 ;
}



int Resize_window_X (int request[2], int actual[2])
// request[0] = desired_width
// request[1] = desired_height
// what you actually got is 'returned' in the actual[2] array
{
  int s ;

  s =  Resize_window_X_helper (request) ;
  XResizeWindow(XxDisplay, XxWindow, Xx_Win_width, Xx_Win_height) ;
  actual[0] = Xx_Win_width ;
  actual[1] = Xx_Win_height ;

  return s ;
}



int Copy_Buffer_X()
{
   XCopyArea(XxDisplay, XxPixmap, XxWindow, XxWindowContext,
	     0, Xx_Pix_height - Xx_Win_height,
             Xx_Win_width, Xx_Win_height,
             0,0) ;
   return 1 ;
			
}








int Init_X (double Dswidth, double Dsheight)
{
    int swidth = (int)Dswidth ;
    int sheight = (int)Dsheight ;

    XGCValues vals;
    Colormap XxColormap;
    XColor XxColor;
    int i;
    
    //    XxDisplay = XOpenDisplay("\0");
    //    XxDisplay = XOpenDisplay("MAGIC"); // FOR REPL
    if (Display_Code == 101) {
      XxDisplay = XOpenDisplay("MAGIC"); // for repl
      if (XxDisplay == NULL) {
        printf("Unable to open repl display ... exiting.\n") ;
        exit(0) ;
      }      
    } else {
      XxDisplay = XOpenDisplay("\0");
        // default for typical Xwindows on Linux
      if (XxDisplay == NULL) {
        printf("Unable to open specified display ... exiting.\n") ;
        exit(0) ;
      }
    }

    


    
    
    
    XxRootWindow = DefaultRootWindow(XxDisplay);
    XxScreenNumber = DefaultScreen(XxDisplay); 
    XxDepth = DefaultDepth(XxDisplay, XxScreenNumber);
    if (XxDepth != 24) {
      printf("24 bit color not supported.\n") ;
      printf("Color function not likely to work.\n") ;
      return 0 ;
    }


    ///////////////////////////////////////////////////////////////////

    Xx_Pix_width = swidth ;
    Xx_Pix_height = sheight ;

    Xx_Win_width = swidth ;   // initial belief
    Xx_Win_height = sheight ; // initial belief

    int maxwidth,maxheight ;
    maxwidth = XDisplayWidth(XxDisplay,XxScreenNumber) ;
    maxheight = XDisplayHeight(XxDisplay,XxScreenNumber) ;

    if (swidth > maxwidth) {
      printf("Requested width of %d exceeds max allowable of %d.\n",
	     swidth,maxwidth) ;

      printf("Xx display window width truncated\n") ;
      printf("BUT the back buffer IS as specified.\n") ;
      Xx_Win_width = maxwidth ;
    } 


    if (sheight > maxheight) {
      printf("Requested height of %d exceeds max allowable of %d.\n",
	     sheight,maxheight) ;

      printf("Xx display window width truncated\n") ;
      printf("BUT the back buffer IS as specified.\n") ;
      Xx_Win_height = maxheight ;
    } 

    ///////////////////////////////////////////////////////////////////


    XxWindow = XCreateSimpleWindow(XxDisplay, XxRootWindow,
                        0, 0, Xx_Win_width, Xx_Win_height, 0, 0, 0);

    if (!XxWindow) return 0 ;



    XxPixmap = XCreatePixmap(XxDisplay, XxRootWindow,
                              Xx_Pix_width, Xx_Pix_height, XxDepth);

    if (!XxPixmap) return 0 ;

    XxDrawable = XxPixmap;


    XMapWindow(XxDisplay, XxWindow);
    XSelectInput(XxDisplay, XxWindow, ExposureMask |
					StructureNotifyMask |
					PointerMotionMask | 
					ButtonPressMask |
					KeyPressMask );


    vals.graphics_exposures = 0; // False
    XxWindowContext = XCreateGC(XxDisplay, XxWindow,
                                 GCGraphicsExposures, &vals);
    if (!XxWindowContext) return 0;



    XxPixmapContext = XCreateGC(XxDisplay, XxPixmap, 0, 0);
    if (!XxPixmapContext) return 0;


    XxColormap = DefaultColormap(XxDisplay, XxScreenNumber);


    XxFontInfo = XLoadQueryFont(XxDisplay, XxFont) ;
    //    XSetFont(XxDisplay, XxWindowContext, XxFontInfo->fid) ;

    // XSetFont(XxDisplay, XxPixmapContext, XxFontInfo->fid) ;
    // probably no need to really do this
    // and it creates a hassle for repl and other
    // environments

    
    XClearArea(XxDisplay, XxWindow, 0,0,0,0,True); 

    
    // most people expect a white piece of paper
    // with a black pencil
    Set_Color_Rgb_X (255,255,255) ; // white
    Clear_Buffer_X() ; // otherwise you can inherit garbage
    // from the parent window

    Copy_Buffer_X() ;


    XFlush(XxDisplay);  


    Set_Color_Rgb_X (0,0,0) ; // black pencil
    return 1 ;
}



int Close_Down_X()
{
    XDestroyWindow(XxDisplay, XxWindow);
    XFreeGC(XxDisplay, XxWindowContext);
    XFreeGC(XxDisplay, XxPixmapContext);
    XFreePixmap(XxDisplay, XxPixmap);
    XCloseDisplay(XxDisplay);

    return 1 ;    
}



int Copy_Buffer_And_Flush_X()
{
   Copy_Buffer_X() ;

   XFlush(XxDisplay) ;

   return 1 ;   
}



int Send_Expose_Event()
{
  XExposeEvent e ;
  e.type = Expose ;
  e.send_event = 1 ;
  e.display = XxDisplay ;
  e.window = PointerWindow ;
  e.count = 1 ;
  XSendEvent (XxDisplay, XxWindow, 0, ExposureMask, (XEvent *)&e) ;

  return 1 ;  
}



int Handle_Events_X(int *px, int *py)
{
    int DummyInt;
    Window DummyWindow;
    XEvent event;
    int x,y ;
    KeySym keysym ;
    char buffer[10] ;
    int bufsize = 10 ;
    XComposeStatus compose ;
    int ww,hh ;
    int retval ;
    int request[2],sig ;

    // printf("enter Handle_Events_X\n") ;


 MORE_EVENTS : 

    if ( !(XPending(XxDisplay)) ) {
      retval = -3000 ;
      goto LLL ;
    } 

    XNextEvent(XxDisplay, &event);
      // if the event queue is empty, XNextEvent flushes the
      // output buffer and blocks until an event is received
      // Since we don't want it to block when the queue is empty
      // we check beforehand with XPending to see if there
      // is anything in the queue

    // printf("event.type == %d\n",event.type) ;

    switch(event.type) {


    case Expose:
        // printf("Expose\n") ;

        Copy_Buffer_And_Flush_X() ;
             // this is new ... when the window is uncovered
	     // this will regenerate it from the buffer
	*px = 0 ; *py = 0 ;
        retval = -1 ; 
        goto LLL ;
	break;	


    case MotionNotify:
        // printf("MotionNotify\n") ;
        // while this gives coords relative to the upper left
        // of the window being displayed...so, for example,
        // suppose we target mouse movement around the center
        // of the window that is being displayed.  this will give
        // a certain (x,y). suppose we drag the window and 
        // then again target mouse movement near the center of
        // the moved window.  this will give the same (x,y).

	x = event.xmotion.x ;
	y = event.xmotion.y ;
	// this will also give "correct" values if the motion
	// of the mouse carries it off the window

        y = Xx_Pix_height-1 - y ;

	if(event.xmotion.state & Button3Mask) {
	    *px = x ;	    *py = y ;
	} else 
	if(event.xmotion.state & Button1Mask) {
	    *px = x ;	    *py = y ;
	} else {
	    goto MORE_EVENTS;
	}

        retval = -2 ; 
        goto LLL ;
	break;


    case ButtonPress:
        // printf("ButtonPress\n") ;
        // while this gives coords relative to the upper left
        // of the window being displayed...so, for example,
        // suppose we click the mouse at the center
        // of the window that is being displayed.  this will give
        // a certain (x,y). suppose we drag the window and 
        // then again click the mouse at the center of
        // the moved window.  this will give the same (x,y).
        x = event.xbutton.x ;
        y = event.xbutton.y ;
        y = Xx_Win_height-1 - y ;
        *px = x ; *py = y ;
        retval = -3 ; 
        goto LLL ;
	break;



    case ConfigureNotify:
        // printf("ConfigureNotify\n") ;
        // check for window resizing
	request[0] = event.xconfigure.width;
	request[1] = event.xconfigure.height;
        sig =  Resize_window_X_helper (request) ;
	*px = Xx_Win_width ; *py = Xx_Win_height ;
        retval = -4 ; 
        goto LLL ;
	break;		


    case KeyPress:
        // printf("KeyPress\n") ;
        XLookupString (&(event.xkey), buffer, bufsize, &keysym, &compose) ;
	*px = 0 ; *py = 0 ;
        retval = keysym ;
        goto LLL ;
	break;


    default:
      // printf("default\n") ;
	*px = 0 ; *py = 0 ;
        retval = -1000 ;
        goto LLL ;
	break;

    }




    // printf("none of the above\n") ;
    *px = 0 ; *py = 0 ;
    retval = -2000 ;
    goto LLL ;
    // This actually does happen
    //e.g when you move the window.
    // I think the reason the default doesn't pick
    // that case up is that such an action isn't
    // placed on the display event queue because
    // of the way Init_X has set things up with
    // XSelectInput(...


 LLL : ;

    // printf("exit Handle_Events_X\n") ;
    return retval ;
}







int Get_Events_X (int *d)
// d needs to point to 2 or more ints
// merely repackages Handle_Events_X
{
  int s,x,y ;

  s = Handle_Events_X (&x,&y) ;
  d[0] = x ; 
  d[1] = y ;
  return s ;
}


int Get_Events_DX (double *d)
// d needs to point to 2 or more doubles
// merely repackages Handle_Events_X
{
  int s,x,y ;

  s = Handle_Events_X (&x,&y) ;
  d[0] = x ; 
  d[1] = y ;
  return s ;
}



////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////







int Point_X (double Dx, double Dy) 
{
  int x,y ;
  x = (int)Dx ; 
  y = (int)Dy ;

  XDrawPoint(XxDisplay, XxDrawable, XxPixmapContext, 
               x, Xx_Pix_height - 1 - y) ;

  return 1 ;
}




int Safe_Point_X (double Dx, double Dy)
{
  int x = (int)Dx ;
  int y = (int)Dy ;

    if ((x < 0) || (y < 0) || (x >= Xx_Pix_width) || (y >= Xx_Pix_height)) {return 0 ;}
    XDrawPoint(XxDisplay, XxDrawable, XxPixmapContext,
               x,  Xx_Pix_height - 1 - y) ;
    return 1 ;
}





int Line_X (double Dxs, double Dys, double Dxe, double Dye)
{
  int xs = (int)Dxs ;
  int ys = (int)Dys ;
  int xe = (int)Dxe ;
  int ye = (int)Dye ;


    XDrawLine (XxDisplay, XxDrawable, XxPixmapContext,
               xs, Xx_Pix_height-1-ys,
               xe, Xx_Pix_height-1-ye);

  return 1 ;    
}






int Safe_Line_X (double Dxs, double Dys, double Dxe, double Dye)
{
  int ixs = (int)Dxs ;
  int iys = (int)Dys ;
  int ixe = (int)Dxe ;
  int iye = (int)Dye ;

  double xs, ys, xe, ye ; // doubles for accuracy in clipping 
  double t, xedge, yedge ;


  if (  (ixs >= 0 ) && (ixs < Xx_Pix_width)  
     && (ixe >= 0 ) && (ixe < Xx_Pix_width)  
     && (iys >= 0 ) && (iys < Xx_Pix_height)  
     && (iye >= 0 ) && (iye < Xx_Pix_height)  ) {

    XDrawLine (XxDisplay, XxDrawable, XxPixmapContext,
               ixs, Xx_Pix_height-1-iys,
               ixe, Xx_Pix_height-1-iye );

    return 1 ;

  }



  xs = ixs ; ys = iys ; xe = ixe ; ye = iye ;

// start to end 
// in       in      leave line alone...draw it all 
// in      out      replace end 
// out      in      replace start 
// out     out      don't draw the line 


// clip against all 4 sides of screen 



// first clip against y = 0 

 yedge = 0 ;
 if (ys >= yedge) {
     if (ye >= yedge) {
         // start is in, end is in , leave alone...next test 
     }
     else {
         // start is in, end is out, replace end...next test 
         t = (yedge - ys)/(ye - ys) ; 
             // ye cannot be ys .. no div by 0 worry 
         xe = xs + t*(xe - xs) ;
         ye = yedge ;
     }
 } else {
     if (ye >= yedge) {
         //start is out, end is in, replace start...next test 
         t = (yedge - ys)/(ye - ys) ; 
             // ye cannot be ys .. no div by 0 worry 
         xs = xs + t*(xe - xs) ;
         ys = yedge ;

     }
     else {
         // both are out...don't draw line at all 
         goto CLend ;
     }
 }

// second, clip against y = Xx_Pix_height - 1 

 yedge = Xx_Pix_height - 1 ;
 if (ys <= yedge) {
     if (ye <= yedge) {
         // start is in, end is in , leave alone...next test 
     }
     else {
         // start is in, end is out, replace end...next test 
         t = (yedge - ys)/(ye - ys) ; 
             // ye cannot be ys .. no div by 0 worry 
         xe = xs + t*(xe - xs) ;
         ye = yedge ;
     }
 } else {
     if (ye <= yedge) {
         //start is out, end is in, replace start...next test 
         t = (yedge - ys)/(ye - ys) ; 
             // ye cannot be ys .. no div by 0 worry 
         xs = xs + t*(xe - xs) ;
         ys = yedge ;

     }
     else {
         // both are out...don't draw line at all 
         goto CLend ;
     }
 }


// third, clip against x = 0 

 xedge = 0 ;
 if (xs >= xedge) {
     if (xe >= xedge) {
         // start is in, end is in , leave alone...next test 
     }
     else {
         // start is in, end is out, replace end...next test 
         t = (xedge - xs)/(xe - xs) ; 
             // xe cannot be xs .. no div by 0 worry 
         ye = ys + t*(ye - ys) ;
         xe = xedge ;
     }
 } else {
     if (xe >= xedge) {
         //start is out, end is in, replace start...next test 
         t = (xedge - xs)/(xe - xs) ; 
             // xe cannot be xs .. no div by 0 worry 
         ys = ys + t*(ye - ys) ;
         xs = xedge ;

     }
     else {
         // both are out...don't draw line at all 
         goto CLend ;
     }
 }

// finally, clip against x = Xx_Pix_width - 1 

 xedge = Xx_Pix_width - 1 ;
 if (xs <= xedge) {
     if (xe <= xedge) {
         // start is in, end is in , leave alone...next test 
     }
     else {
         // start is in, end is out, replace end...next test 
         t = (xedge - xs)/(xe - xs) ; 
             // xe cannot be xs .. no div by 0 worry 
         ye = ys + t*(ye - ys) ;
         xe = xedge ;
     }
 } else {
     if (xe <= xedge) {
         //start is out, end is in, replace start...next test 
         t = (xedge - xs)/(xe - xs) ; 
             // xe cannot be xs .. no div by 0 worry 
         ys = ys + t*(ye - ys) ;
         xs = xedge ;

     }
     else {
         // both are out...don't draw line at all 
         goto CLend ;
     }
 }


    XDrawLine (XxDisplay, XxDrawable, XxPixmapContext,
               (int)xs, (int)(Xx_Pix_height-1-ys),
               (int)xe, (int)(Xx_Pix_height-1-ye) );


 CLend : 

    return 1 ;

}





int Rectangle_X (double Dxlow, double Dylow, double Dwidth, double Dheight) 
{

  int xlow = (int)Dxlow ;
  int ylow = (int)Dylow ;
  int width = (int)Dwidth ;
  int height = (int)Dheight ; 

  XDrawRectangle(XxDisplay, XxDrawable, XxPixmapContext,
                   xlow,  Xx_Pix_height - ylow - height,
                   width,height);

  return 1 ;  
}




int Fill_Rectangle_X (double Dxlow, double Dylow, double Dwidth, double Dheight) 
{
  int xlow = (int)Dxlow ;
  int ylow = (int)Dylow ;
  int width = (int)Dwidth ;
  int height = (int)Dheight ; 

  XFillRectangle(XxDisplay, XxDrawable, XxPixmapContext,
                   xlow, Xx_Pix_height - ylow - height,
                   width, height);

  return 1 ;  
}



int Triangle_X (double Dx1, double Dy1, 
                double Dx2, double Dy2,
                double Dx3, double Dy3)
{
  int x1 = (int)Dx1 ;
  int y1 = (int)Dy1 ;
  int x2 = (int)Dx2 ;
  int y2 = (int)Dy2 ;
  int x3 = (int)Dx3 ;
  int y3 = (int)Dy3 ;

  XPoint Points[4];

  // you DO want the height-1  here though

  Points[0].x = (x1);
  Points[0].y = (Xx_Pix_height-1-y1);
  Points[1].x = (x2);
  Points[1].y = (Xx_Pix_height-1-y2);
  Points[2].x = (x3);
  Points[2].y = (Xx_Pix_height-1-y3);
  Points[3].x = Points[0].x;
  Points[3].y = Points[0].y;

  XDrawLines(XxDisplay, XxDrawable,XxPixmapContext,
                               Points, 4, CoordModeOrigin);

  return 1 ;  
}





int Fill_Triangle_X (double Dx1, double Dy1, 
                     double Dx2, double Dy2,
                     double Dx3, double Dy3)
{
  int x1 = (int)Dx1 ;
  int y1 = (int)Dy1 ;
  int x2 = (int)Dx2 ;
  int y2 = (int)Dy2 ;
  int x3 = (int)Dx3 ;
  int y3 = (int)Dy3 ;

  XPoint Points[3];

  Points[0].x = (x1);
  Points[0].y = (Xx_Pix_height-1-y1);
  Points[1].x = (x2);
  Points[1].y = (Xx_Pix_height-1-y2);
  Points[2].x = (x3);
  Points[2].y = (Xx_Pix_height-1-y3);


  XFillPolygon(XxDisplay, XxDrawable, XxPixmapContext,
                Points, 3, Convex, CoordModeOrigin);

  return 1 ;  
}





int Polygon_X (int *x, int *y, int npts)
{
   XPoint xpoint[1000] ;
   int k ;

   if (npts <= 0) return 0 ;

   if (npts > 1000) {
      printf("\nPolygon+X has been asked to deal with %d points.\n",
             npts) ;
      printf("Points past first 1000 ignored.\n") ;
      npts = 1000 ;
   }

   for (k = 0 ; k < npts ; k++) {
        xpoint[k].x = x[k] ; 
        xpoint[k].y = Xx_Pix_height -1 - y[k] ;
   }

   XDrawLines(XxDisplay,XxDrawable,XxPixmapContext,
                       xpoint,npts,  CoordModeOrigin);
   XDrawLine(XxDisplay,XxDrawable,XxPixmapContext,
                    xpoint[0].x, xpoint[0].y,
                         xpoint[npts-1].x, xpoint[npts-1].y ) ;

   return 1 ;
}



int Polygon_DX (double *x, double *y, double Dnpts)
{
  int npts = (int)Dnpts ;
   XPoint xpoint[1000] ;
   int k ;

   if (npts <= 0) return 0 ;

   if (npts > 1000) {
      printf("\nPolygon+X has been asked to deal with %d points.\n",
             npts) ;
      printf("Points past first 1000 ignored.\n") ;
      npts = 1000 ;
   }

   for (k = 0 ; k < npts ; k++) {
        xpoint[k].x = (int)x[k] ;
        xpoint[k].y = (int)(Xx_Pix_height -1 - y[k]) ;
   }

   XDrawLines(XxDisplay,XxDrawable,XxPixmapContext,
                       xpoint,npts,  CoordModeOrigin);
   XDrawLine(XxDisplay,XxDrawable,XxPixmapContext,
                    xpoint[0].x, xpoint[0].y,
                         xpoint[npts-1].x, xpoint[npts-1].y ) ;

   return 1 ;
}



int Fill_Polygon_X (int *x, int *y, int npts)
{
   XPoint xpoint[1000] ;
   int k ;

   if (npts <= 0) return 0 ;

   if (npts > 1000) {
      printf("\nFill_Polygon_X has been asked to deal with %d points.\n",
             npts) ;
      printf("Points past first 1000 ignored.\n") ;
      npts = 1000 ;
   }

   for (k = 0 ; k < npts ; k++) {
        xpoint[k].x = x[k] ; 
        xpoint[k].y = Xx_Pix_height -1 - y[k] ;
   }


   XFillPolygon(XxDisplay,XxDrawable,XxPixmapContext,
                xpoint,npts,Nonconvex,CoordModeOrigin);   

   return 1 ;

}



int Fill_Polygon_DX (double *x, double *y, double Dnpts)
{
  int npts = (int)Dnpts ;
   XPoint xpoint[1000] ;
   int k ;

   if (npts <= 0) return 0 ;

   if (npts > 1000) {
      printf("\nFill_Polygon_X has been asked to deal with %d points.\n",
             npts) ;
      printf("Points past first 1000 ignored.\n") ;
      npts = 1000 ;
   }

   for (k = 0 ; k < npts ; k++) {
        xpoint[k].x = (int)x[k] ; 
        xpoint[k].y = (int)(Xx_Pix_height -1 - y[k]) ;
   }


   XFillPolygon(XxDisplay,XxDrawable,XxPixmapContext,
                xpoint,npts,Nonconvex,CoordModeOrigin);   


   return 1 ;
}




int FLAWED_Horizontal_Single_Pixel_Line_X (double Dx0, double Dx1, double Dy)
{
  //  Line_X(Dx0,Dy,  Dx1,Dy) ; return 1 ; // this is ok
  //  But below is NOT...
  // consider drawing a horizontal line where both points are outside of the
  // window.  This code maps both ends to the boundary and so will draw at
  // least a single point when it shouldn't draw anything.
   int x0 = (int)Dx0 ;
   int x1 = (int)Dx1 ;
   int y = (int)Dy ;
   int t ;

   // protect against an offscreen line
   if (y < 0) return 0 ;
   if (y >= Xx_Pix_height) return 0 ;

   if (x0 < 0) x0 = 0 ;
   else if (x0 >= Xx_Pix_width) x0 = (Xx_Pix_width - 1) ;
   if (x1 < 0) x1 = 0 ;
   else if (x1 >= Xx_Pix_width)  x1 = (Xx_Pix_width - 1) ;
   // end protection code

   if (x1 < x0) { t = x1 ; x1 = x0 ; x0 = t ; }

   Line_X (x0,y, x1,y) ;
   
   return 1 ;
} 





int Horizontal_Single_Pixel_Line_X (double Dx0, double Dx1, double Dy)
{
   int x0 = (int)Dx0 ;
   int x1 = (int)Dx1 ;
   int y = (int)Dy ;
   int t ;

   // protect against an offscreen line
   if (y < 0) return 0 ;
   if (y >= Xx_Pix_height) return 0 ;

   
   if (x0 > x1) { t = x1 ; x1 = x0 ; x0 = t ; } // now we know  x0 <= x1


   
   if (x1 < 0) return 0 ;
   if (x0 >= Xx_Pix_width) return 0 ;
      // now we know that x1 >= 0  and  x0 < Xx_Pix_width


   
   if (x0 < 0) x0 = 0 ; // this makes x0 STILL less than or equal to x1 because of above.
   if (x1 >= Xx_Pix_width) x1 = Xx_Pix_width - 1 ; // still guarantees x0 <= x1
   
   // end protection code

   //   Line_X (x0,y, x1,y) ;
   int x ;
   for (x = x0 ; x <= x1 ; x++) {
     Safe_Point_X(x,y) ;
   }
   
   return 1 ;
} 






int Circle_X (double Da, double Db, double Dr)
{
 int a = (int)Da ;
 int b = (int)Db ;
 int r = (int)Dr ;

 int x,y,e,e1,e2 ;

 x = r ;
 y = 0 ;
 e = 0;

 while (x >= y) {

       Safe_Point_X( a+x,b+y) ;   Safe_Point_X( a-x,b+y) ;
       Safe_Point_X( a+x,b-y) ;   Safe_Point_X( a-x,b-y) ;

       Safe_Point_X( a+y,b+x) ;   Safe_Point_X( a-y,b+x) ;
       Safe_Point_X( a+y,b-x) ;   Safe_Point_X( a-y,b-x) ;

       e1 =  e + y + y + 1 ;
       e2 = e1 - x - x + 1 ;
       y  =  y + 1 ;

       if ( abs(e2) < abs(e1) ) {
              x = x - 1 ;
              e = e2 ;
       } else e = e1 ;

     } 

  return 1 ; 
} 







int Fill_Circle_X (double Da, double Db, double Dr)
{
 int a = (int)Da ;
 int b = (int)Db ;
 int r = (int)Dr ;

 int x,y,e,e1,e2 ;

 x = r ;
 y = 0 ;
 e = 0;

 while (x >= y) {

       Horizontal_Single_Pixel_Line_X (a-x, a+x, b+y) ;
       Horizontal_Single_Pixel_Line_X (a-x, a+x, b-y) ;

       Horizontal_Single_Pixel_Line_X (a-y, a+y, b+x) ;
       Horizontal_Single_Pixel_Line_X (a-y, a+y, b-x) ;

       e1 =  e + y + y + 1 ;
       e2 = e1 - x - x + 1 ;
       y  =  y + 1 ;

       if ( abs(e2) < abs(e1) ) {
              x = x - 1 ;
              e = e2 ;
       } else e = e1 ;

     } 

  return 1 ; 
} 





int Font_Pixel_Height_X ()
// Returns the height of the font in pixels. 
{
     return XxFontInfo->max_bounds.ascent + XxFontInfo->max_bounds.descent;
}



// the void * delcarations below allow for the passing of
// arrays of characters (usual usage) but also, for instance,
// an array of doubles that is composed of packed characters


int String_Pixel_Width_X (const void *s)
// Returns the length, in pixels, of the string s 
{
     int len;

     len = strlen((char *)s);

     return XTextWidth(XxFontInfo, (char *)s, len);
}





int Draw_String_X (const void *s, double Dx, double Dy)
// Draw the string s, with the lower left hand corner at (x,y)
{
  int x = (int)Dx ;
  int y = (int)Dy ;

     int len;

     len = strlen((const char *)s);

     XDrawString(XxDisplay,XxDrawable,XxPixmapContext,
                                         x,Xx_Pix_height-1-y,
                                         (char *)s, len);

  return 1 ;     
}




int Get_Current_Dimensions_X (int dimensions[2])
{
  dimensions[0] = Xx_Pix_width ;
  dimensions[1] = Xx_Pix_height ;
  return 1 ;  
}


int Get_Current_Dimensions_DX (double dimensions[2]) 
{
  dimensions[0] = Xx_Pix_width ;
  dimensions[1] = Xx_Pix_height ;
  return 1 ;  
}


int Change_Pen_Dimensions_X (double Dw, double Dh)
{
  printf("Change_Pen_Dimensions_X  not implemented\n") ;
  return 0 ;
}



int Draw_Text_X (
               int num_lines_of_text,
               const void *lines_of_text, // an array of pointers
               double startx, 
               double starty,
               double height,
               double x_over_y_ratio,
               double extra_space_between_letters_fraction,
               double extra_space_between_lines_fraction) 
{
  printf("Draw_Text_X  not implemented\n") ;
  return 0 ;
}



/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////


void fputintB (int x, FILE *fp)
{
   char *p ;
   int c[4],i ;

   p = (char *) &x ;

   for (i = 0 ; i < 4 ; i++) {
        c[i] = *p ;
        p++ ;
   }

   for (i = 3 ; i >= 0 ; i--) {
        fputc(c[i], fp) ;
   }

}




int fgetintB (FILE *fp)
{
   char *p ;
   int c[4],i ;
   int x ;
   char q ;

   for (i = 3 ; i >= 0 ; i--) {
        c[i] = fgetc(fp) ;
   }



   p = (char *) &x ;

   for (i = 0 ; i < 4 ; i++) {
        q = c[i] ;
        *p = q ;;
        p++ ;
   }

   return x ;
}







void  XImage_To_XWD_File (XImage *pxim,  FILE *fp)
{
  // assumes full color but uses full 32 bits per pixel
 int header_size, file_version, format, depth ;
 int xoffset, byte_order, bitmap_unit, bitmap_order, bitmap_pad ;
 int bits_per_pixel, bytes_per_line, visual_class ;
 int redmask, greenmask, bluemask ;
 int bitsperrgb, numentriesincolormap, numcolorstructures ;
 int windowwidth, windowheight, windowx, windowy, windowborderwidth ;
 int numbytestowrite ;
 int width,height ;

 header_size = 104 ;
 fputintB(header_size,fp) ;

 file_version = 7 ;
 fputintB(file_version,fp) ; 

 format = 2 ; // ZPixmap
 fputintB(format,fp) ;

 depth = 24 ;
 fputintB(depth,fp) ;

 width = pxim->width ;
 fputintB(width,fp) ;

 height = pxim->height ;
 fputintB(height,fp) ;

 xoffset = 0 ;
 fputintB(xoffset,fp) ;

 byte_order = 0 ;
 fputintB(byte_order,fp) ;

 bitmap_unit = 32 ;
 fputintB(bitmap_unit,fp) ;

 bitmap_order = 0 ;
 fputintB(bitmap_order,fp) ;

 bitmap_pad = 32  ;
 fputintB(bitmap_pad,fp) ;

 bits_per_pixel = 32 ;
 fputintB(bits_per_pixel,fp) ;

 bytes_per_line = width * 4 ;
 fputintB(bytes_per_line,fp) ;

 visual_class = 5 ; // more than one sample per pixel
 fputintB(visual_class,fp) ;

 redmask = 0x00ff0000 ;
 fputintB(redmask,fp) ;

 greenmask = 0x0000ff00 ;
 fputintB(greenmask,fp) ;

 bluemask = 0x000000ff ;
 fputintB(bluemask,fp) ;

 // bitsperrgb = 8 ; // hard to believe
 bitsperrgb = 24 ; 
 fputintB(bitsperrgb,fp) ;

 numentriesincolormap = 0 ; // not used :
 fputintB(numentriesincolormap,fp) ;

 numcolorstructures = 0 ;
 fputintB(numcolorstructures,fp) ;

 windowwidth = width ;
 fputintB(windowwidth,fp) ;

 windowheight = height ;
 fputintB(windowheight,fp) ;

 windowx = 0 ; // how crucial is this ?
 fputintB(windowx,fp) ;

 windowy = 0 ; // how crucial is this ?
 fputintB(windowy,fp) ;

 windowborderwidth = 0 ;
 fputintB(windowborderwidth,fp) ;


 // null terminated window name fleshed out to multiple of 4 bytes
 fputc('\0',fp) ;
 fputc('\0',fp) ;
 fputc('\0',fp) ;
 fputc('\0',fp) ;


 numbytestowrite =   bytes_per_line * height ;

 fwrite( pxim->data, numbytestowrite, 1, fp) ;

}











void XImage_From_XWD_File (XImage *pxim, FILE *fp)
{
  // assumes full color but uses full 32 bits per pixel

 int header_size, file_version, format, depth ;
 int xoffset, byte_order, bitmap_unit, bitmap_order, bitmap_pad ;
 int bits_per_pixel, bytes_per_line, visual_class ;
 int redmask, greenmask, bluemask ;
 int bitsperrgb, numentriesincolormap, numcolorstructures ;
 int windowwidth, windowheight, windowx, windowy, windowborderwidth ;
 int numbytestowrite ;

 int width,height ;

 int c, numbytestoread ;


 header_size = fgetintB(fp) ;
 file_version = fgetintB(fp) ;
 format = fgetintB(fp) ;
 depth = fgetintB(fp) ;
 width =  fgetintB(fp) ;
 height =  fgetintB(fp) ;
 xoffset = fgetintB(fp) ;
 byte_order = fgetintB(fp) ;
 bitmap_unit = fgetintB(fp) ;
 bitmap_order = fgetintB(fp) ;
 bitmap_pad = fgetintB(fp) ;
 bits_per_pixel = fgetintB(fp) ;
 bytes_per_line = fgetintB(fp) ;
 visual_class = fgetintB(fp) ;
 redmask = fgetintB(fp) ;
 greenmask = fgetintB(fp) ;
 bluemask = fgetintB(fp) ;
 bitsperrgb = fgetintB(fp) ;
 numentriesincolormap = fgetintB(fp) ;
 numcolorstructures = fgetintB(fp) ;
 windowwidth = fgetintB(fp) ;
 windowheight = fgetintB(fp) ;
 windowx = fgetintB(fp) ;
 windowy = fgetintB(fp) ;
 windowborderwidth = fgetintB(fp) ;

 // null terminated window name fleshed out to multiple of 4 bytes
 c = fgetc(fp) ; c = fgetc(fp) ; c = fgetc(fp) ; c = fgetc(fp) ;

 /* ++++++++++++++++++++++++ */
 
 pxim->width = width ;
 pxim->height = height ;
 pxim->depth = depth ;
 pxim->xoffset = xoffset ;
 pxim->format = format ;
 pxim->bitmap_unit = bitmap_unit ;
 pxim->bitmap_pad = bitmap_pad ;
 pxim->bytes_per_line = bytes_per_line ;
 pxim->bits_per_pixel = bits_per_pixel ;
 pxim->byte_order = byte_order ;
 pxim->bitmap_bit_order = bitmap_order ;



 /* on users head to have freed up any previous memory that pxim->data
    might have pointed to...trying to free it up here will cause faults
    if not already pointing to something sane */


 numbytestoread =   pxim->bytes_per_line * pxim->height  ;
 pxim->data = (char *)malloc(numbytestoread) ;
 if (pxim->data == NULL) {
      printf("ERROR: XImage_From_XWD_File : can't malloc space needed\n") ;
      printf("Program terminating\n\n") ;
      exit(1) ;
 }

 fread (pxim->data, numbytestoread, 1, fp) ;

 rewind(fp) ;

}





int Save_Image_To_File_X (const void *filename)
// return 1 if successful else 0
{
  FILE *fp ;
  XImage *pxim ;

  fp = fopen ((char *)filename,"w") ;
  if (fp == NULL) {
    printf("Save_Image_To_File_X cannot open file %s\n",(char *)filename) ;
    return 0 ;
  }

  //  pxim = XGetImage (XxDisplay, XxWindow,0,0, Xx_Pix_width, Xx_Pix_height) ;
  pxim = XGetImage (XxDisplay, XxDrawable, 0,0, Xx_Pix_width, Xx_Pix_height,
                      AllPlanes, ZPixmap) ;

  XImage_To_XWD_File (pxim,  fp) ;

  XDestroyImage(pxim) ; // lack of this was causing mem leaks when
  // many images were being saved for movies

  fclose(fp) ;

  return 1 ;
}





int Get_Image_From_File_X (const void *filename, double Dx, double Dy)
// Put lower left corner of file into the graphics window at (x,y).
// return 1 if successful else 0
{
  int x = (int)Dx ;
  int y = (int)Dy ;

  FILE *fp ;
  XImage xim[1] ;
  int srcx,srcy,destx,desty ;
  int image_width, image_height ;
  int transfer_width, transfer_height ;

  fp = fopen ((char *)filename,"r") ;
  if (fp == NULL) {
    printf("Get_Image_From_File_X cannot open file %s\n",(char *)filename) ;
    return 0 ;
  }

  XImage_From_XWD_File (&xim[0], fp) ;
  image_width = xim[0].width ;
  image_height = xim[0].height ;
  //  printf("%d %d\n",image_width,image_height) ;

  if (image_height <= Xx_Pix_height - y) {

    transfer_height = image_height ;
    srcx = 0 ;
    srcy = 0 ;
    destx = x ;
    desty = Xx_Pix_height - y - image_height ;
    if (image_width <= Xx_Pix_width - x) { transfer_width = image_width ; }
    else { transfer_width = Xx_Pix_width - x ;}

  } else {

    transfer_height = Xx_Pix_height - y ;
    srcx = 0 ;
    srcy = image_height - (Xx_Pix_height - y) ;
    destx = x ;
    desty = 0 ;
    if (image_width <= Xx_Pix_width - x) { transfer_width = image_width ; }
    else { transfer_width = Xx_Pix_width - x ;}

  }


  XPutImage (XxDisplay, XxDrawable, XxPixmapContext, &xim[0],
             srcx, srcy, destx, desty,
             transfer_width, transfer_height) ;
	     //	     Xx_Pix_width, Xx_Pix_height) ;


  fclose(fp) ;

  // fix memory leak :
  free (xim[0].data) ; // thanks to Casey Yamamura
  
  return 1 ;
}




int Get_Image_Dimensions_From_File_X (int d[2], void *filename)
// return 1 if successful else 0
// You do NOT need to have initialized the graphics environment 
// before you call this function
{
 FILE *fp ;

 fp = fopen ((char *)filename,"r") ;
 if (fp == NULL) {
   printf("Get_Image_Dimensions_From_File_X cannot open file %s\n",
                                                     (char *)filename) ;
   return 0 ;
 }

 // assumes full color but uses full 32 bits per pixel
 int header_size, file_version, format, depth ;
 int width,height ;

 header_size = fgetintB(fp) ;
 file_version = fgetintB(fp) ;
 format = fgetintB(fp) ;
 depth = fgetintB(fp) ;
 width =  fgetintB(fp) ;
 height =  fgetintB(fp) ;

 d[0] = width ;
 d[1] = height ;

 fclose(fp) ;

 return 1 ;
}





int Get_Pixel_X (double Dx, double Dy)
// return the 32 bit pixel value...assumes x,y are legal
// i.e. it is NOT safe
{
  int x = (int)Dx ;
  int y = (int)Dy ;

  XImage *pxim ;
  int p ;

  pxim = XGetImage (XxDisplay, XxDrawable, 0,0, Xx_Pix_width, Xx_Pix_height,
                      AllPlanes, ZPixmap) ;
  p = XGetPixel(pxim,x, Xx_Pix_height - 1 - y) ;

  XDestroyImage(pxim) ; // lack of this was causing mem leaks when
  // many images were being saved for movies

  return p ;
}



int Get_Pixel_SAFE_X (double Dx, double Dy, int pixel[1]) 
// return 1 if successful, else 0
{
  int x = (int)Dx ;
  int y = (int)Dy ;
  XImage *pxim ;

  if ((x < 0) || (x >= Xx_Pix_width) || (y < 0) || (y >= Xx_Pix_height))
    return 0 ;

  pxim = XGetImage (XxDisplay, XxDrawable, 0,0, Xx_Pix_width, Xx_Pix_height,
                      AllPlanes, ZPixmap) ;

  pixel[0] = XGetPixel(pxim,x, Xx_Pix_height - 1 - y) ;

  XDestroyImage(pxim) ; // lack of this was causing mem leaks when
  // many images were being saved for movies

  return 1 ;
}




/////////////////////////////////////////////////////////////////
// suppose there were just 4 colors, 0,1,2,3
// [0.00, 0.25) -> 0
// [0.25, 0.50) -> 1
// [0.50, 0.75) -> 2
// [0.75, 1.00] -> 3
// 0 -> middle of [0.00, 0.25)
// 1 -> middle of [0.25, 0.50)
// 2 -> middle of [0.50, 0.75)
// 3 -> middle of [0.75, 1.00)



int Set_Color_Rgb_X (int r, int g, int b)
{
  unsigned long int p ;

  if (r < 0) r = 0 ; else if (r > 255) r = 255 ;
  if (g < 0) g = 0 ; else if (g > 255) g = 255 ;
  if (b < 0) b = 0 ; else if (b > 255) b = 255 ;

  p = (r << 16) | (g  << 8) | (b) ;
  XSetForeground(XxDisplay, XxPixmapContext, p) ;

  // new
  Current_Red_Int   = r ;
  Current_Green_Int = g ;
  Current_Blue_Int  = b ;
  Current_Color_Pixel = p ;

  return 1 ;  
}



int Set_Color_Rgb_DX (double dr, double dg, double db)
{
  int r,g,b ;

  if (dr < 0.0) dr = 0.0 ; else if (dr > 1.0) dr = 1.0 ;
  if (dg < 0.0) dg = 0.0 ; else if (dg > 1.0) dg = 1.0 ;
  if (db < 0.0) db = 0.0 ; else if (db > 1.0) db = 1.0 ;

  r = (int)(256*dr) ;
  g = (int)(256*dg) ;
  b = (int)(256*db) ;

  Set_Color_Rgb_X (r,g,b) ;

  return 1 ;  
}




int Convert_Pixel_To_rgbI_X (int pixel, int rgbI[3]) 
// rgbI[] values in 0-255
// return 1 if successful, else 0
{
  rgbI[2] = pixel & 0xff ;
  rgbI[1] = (pixel >> 8) & 0xff ;
  rgbI[0] = (pixel >> 16) & 0xff ;
  return 1 ;
}


int Convert_rgbI_To_rgb_X (int rgbI[3], double rgb[3]) 
// convert rgbI[] in 0-255 to rgb[] in 0.0 - 1.0
// return 1 if successful, else 0
{
  double d ;
  d = 0.5/256.0 ;
  // see above discussion of middle of intervals for purpose of d
  rgb[0] = rgbI[0] / 256.0 + d ;
  rgb[1] = rgbI[1] / 256.0 + d ;
  rgb[2] = rgbI[2] / 256.0 + d ;

  return 1 ;  
}



int XImage_to_Display (XImage *pxim, double Dx, double Dy)
// Put lower left corner of file into the graphics window at (x,y).
// return 1 if successful else 0
{
  int x = (int)Dx ;
  int y = (int)Dy ;

  int image_width, image_height ;
  int srcx,srcy,destx,desty ;
  int transfer_width, transfer_height ;

  image_width = pxim->width ;
  image_height = pxim->height ;
  //  printf("%d %d\n",image_width,image_height) ;

  if (image_height <= Xx_Pix_height - y) {

    transfer_height = image_height ;
    srcx = 0 ;
    srcy = 0 ;
    destx = x ;
    desty = Xx_Pix_height - y - image_height ;
    if (image_width <= Xx_Pix_width - x) { transfer_width = image_width ; }
    else { transfer_width = Xx_Pix_width - x ;}

  } else {

    transfer_height = Xx_Pix_height - y ;
    srcx = 0 ;
    srcy = image_height - (Xx_Pix_height - y) ;
    destx = x ;
    desty = 0 ;
    if (image_width <= Xx_Pix_width - x) { transfer_width = image_width ; }
    else { transfer_width = Xx_Pix_width - x ;}

  }


  XPutImage (XxDisplay, XxDrawable, XxPixmapContext, pxim,
             srcx, srcy, destx, desty,
             transfer_width, transfer_height) ;
	     //	     Xx_Pix_width, Xx_Pix_height) ;

  return 1 ;
}



XImagePointer Get_ximage_of_display()
// should be used with caution...
// XImage pxim = Get_ximage_of_display() ;
// later on, should do
// XDestroyImage(pxim) 
// when done with ximage or could create memory leak
{
  XImage *pxim ;

  pxim = XGetImage (XxDisplay, XxDrawable, 0,0, Xx_Pix_width, Xx_Pix_height,
                      AllPlanes, ZPixmap) ;

  return pxim ;
}






//====================================================================
// G stuff :



/////////////////////////////////////////////////////////////////////
// the G_ routines are graphics related and should
// not be called unless G_init_graphics has been called first
/////////////////////////////////////////////////////////////////////
// Xx following are merely pointers
// which are initialized by G_Init
// Xx actual space for the pointers is declared here
// Xx header file has these pointers declared as "extern"
/////////////////////////////////////////////////////////////////////

int (* G_close) () ;
// terminate the graphics


int (* G_display_image) () ;
// make drawing visible if it already isn't
// in some environments, this might not do anything
// in others it might copy a buffer and/or do other
// synchronizing tasks


int (* Gi_events) (int *d) ;

int (* G_events) (double *d) ;



int (* G_resize_window) (int request[2], int actual[2]) ;
// request[0] = desired width
// request[1] = desired heigth
// actual[0] = width you actually got
// actual[1] = height you actuall got
// added jan 23, 2014



int (* G_change_pen_dimensions) (double w, double h) ;
// return 0 if illegal w,h specified, otherwise 1


int (* Gi_get_current_window_dimensions) (int *dimentsions) ;
// return 1 if successful
// needs to be passed an array of two ints


int (* G_get_current_window_dimensions) (double *dimentsions) ;
// return 1 if successful
// needs to be passed an array of two doubles


int (* Gi_rgb) (int r, int g, int b) ;
// assumes r,g,b are ints in [0,255]

int (* G_rgb) (double r, double g, double b) ;
// assumes r,g,b are doubles in [0, 1]


/////////////////////////////////////////////////////////////////////
// Xx next batch are based on the ability to repeatedly
//  plot a single pixel
/////////////////////////////////////////////////////////////////////

int (* G_pixel) (double x, double y) ;
// return 1 always
// This is not guaranteed to be safe


int (* G_point) (double x, double y) ;
// return 1 always
// This is SAFE.
// Draws a thick point whose dimensions are controlled by
// int G_change_pen_dimensions (double w, double h).

int (* G_circle) (double a, double b, double r) ;
// always return 1
// capable of drawing a circle with thick outline

int (* G_unclipped_line) (double ixs, double iys, double ixe, double iye) ;
// return 1 always
// This is SAFE.
// Capable of drawing a thick line.
// This is safe because it depends on G_point (which is SAFE)
// but because it is unclipped, this code could waste a great
// deal of time trying to plot lots of points outside the window.


int (* G_line) (double ixs, double iys, double ixe, double iye) ;
// return 0 if line clipped away entirely, else return 1
// This is SAFE.
// Capable of drawing a thick line.
// This also clips
// Note that the clipping occurs only for a thin, "perfect"
// line, not for a possibly thick line, but the safety feature
// of G_point that will actually plot a thick point, keeps the
// entire code safe.


int (* Gi_polygon) (int *x, int *y, int numpts) ; 
// return 1 always
// capable of drawing a polygon with thick outline

int (* G_polygon) (double *x, double *y, double numpts) ;
// provided as an alternative for passing arrays of doubles
// return 1 always
// capable of drawing a polygon with thick outline


int (* G_triangle) (double x0, double y0, double x1, double y1, double x2, double y2) ; 
// return value it inherits from G_polygon
// capable of drawing a triangle with thick outline


int (* G_rectangle) (double xleft, double yleft, double width, double height) ; 
// return value it inherits from G_polygon
// capable of drawing a rectangle with thick outline



/////////////////////////////////////////////////////////////////////
// This batch are based on the ability to repeatedly
// draw single pixel horizontal lines
/////////////////////////////////////////////////////////////////////


int (* G_single_pixel_horizontal_line) (double x0, double x1, double y) ;
// return 1 always
// This is not guaranteed to be safe


int (* G_clear) () ; 
// return 1 always


int (* G_fill_circle) (double a, double b, double r) ;
// always return 1


int (* G_unclipped_fill_polygon) (double *xx, double *yy, double n) ;
// return 0 if size needs to be truncated (unusual), else 1 
// This is SAFE


int (* Gi_fill_polygon) (int *xx, int *yy, int n) ;
// return 0 if size needs to be truncated (unusual), 
// or if clipping is used,  otherwise return 1
// This is SAFE
// AND it clips


int (* G_fill_polygon) (double *xx, double *yy, double n) ;
// provided as an alternative for passing arrays of doubles
// return 0 if size needs to be truncated (unusual), 
// or if clipping is used,  otherwise return 1
// This is SAFE
// AND it clips


int (* G_fill_triangle) (double x0, double y0, double x1, double y1, double x2, double y2) ; 
// return value it inherits from G_fill_polygon


int (* G_fill_rectangle) (double xleft, double yleft, double width, double height) ;
// return value it inherits from G_fill_polygon


int (* G_font_pixel_height) () ;
// return the font height in pixels






// the void * pointers below allow for passing in 
// either an array of characters (usual usage)
// but also, perhaps, an double array of packed 
// characters

int (* G_string_pixel_width) (const void *s) ;
// return the length in pixles of the string s

int (* G_draw_string) (const void *one_line_of_text, double LLx, double LLy) ;
// draw a single line of text beginning at (LLx,LLy) which specifies
// the coordinates of the lower left corner of the bounding box
// of the text


int (* G_draw_text) (
               int num_lines_of_text,
               const void  *lines_of_text, // an array of pointers
               double startx, 
               double starty,
               double height,
               double x_over_y_ratio,
               double extra_space_between_letters_fraction,
               double extra_space_between_lines_fraction) ;


int (* G_save_image_to_file) (const void *filename) ;
// return 1 if successful, else 0

int (* G_get_image_from_file) (const void *filename, double x, double y) ;
// return 1 if successful, else 0



int (* G_get_pixel) (double x, double y) ;
// return the 32 bit pixel value...assumes x,y are legal
// i.e. it is NOT safe

int (* G_get_pixel_SAFE) (double x, double y, int pixel[1]) ;
// return 1 if successful, else 0

int (* G_convert_pixel_to_rgbI) (int pixel, int rgbI[3]) ;
// rgbI[] values in 0-255
// return 1 if successful, else 0

int (* G_convert_rgbI_to_rgb) (int rgbI[3], double rgb[3]) ;
// convert rgbI[] in 0-255 to rgb[] in 0.0 - 1.0
// return 1 if successful, else 0



///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// Below are actual functions
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////


int  G_init_graphics (double w, double h)
// G_init_graphics has the task of connecting this interface
// with actual routines that can do the work in a
// specific environment, e.g. X11 or Windows or whatever
{
 int s ;

 G_close = Close_Down_X ;

 G_display_image = Copy_Buffer_And_Flush_X ;

 Gi_events = Get_Events_X ;

 G_events = Get_Events_DX ;

 G_resize_window = Resize_window_X ;

 G_change_pen_dimensions =  Change_Pen_Dimensions_X ;

 Gi_get_current_window_dimensions = Get_Current_Dimensions_X ;

 G_get_current_window_dimensions = Get_Current_Dimensions_DX ;

 Gi_rgb = Set_Color_Rgb_X ;

 G_rgb = Set_Color_Rgb_DX ;

 G_pixel = Point_X ;

 G_point = Safe_Point_X ;

 G_circle = Circle_X ;

 G_unclipped_line = Line_X ;

 // G_line = Line_X ;
 G_line = Safe_Line_X ;

 Gi_polygon = Polygon_X ; 

 G_polygon = Polygon_DX ;

 G_triangle = Triangle_X ; 

 G_rectangle = Rectangle_X ; 

 G_single_pixel_horizontal_line = Horizontal_Single_Pixel_Line_X ;

 G_clear = Clear_Buffer_X ;

 G_fill_circle =  Fill_Circle_X ;

 G_unclipped_fill_polygon =  Fill_Polygon_DX ; 

 Gi_fill_polygon = Fill_Polygon_X ; 

 G_fill_polygon = Fill_Polygon_DX ; 

 G_fill_triangle = Fill_Triangle_X ;

 G_fill_rectangle = Fill_Rectangle_X ;

 G_font_pixel_height = Font_Pixel_Height_X ;

 G_string_pixel_width = String_Pixel_Width_X ;

 G_draw_string = Draw_String_X ;

 G_draw_text = Draw_Text_X ;

 G_save_image_to_file = Save_Image_To_File_X ;

 G_get_image_from_file = Get_Image_From_File_X ;

 G_get_pixel = Get_Pixel_X ;

 G_get_pixel_SAFE = Get_Pixel_SAFE_X ;

 G_convert_pixel_to_rgbI = Convert_Pixel_To_rgbI_X ;

 G_convert_rgbI_to_rgb = Convert_rgbI_To_rgb_X ;

 s = Init_X(w,h) ;

 return s ;
}


///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////


// The following functions are built on top of G
// not directly on top of the underlying
// video system


int Gi_wait_click(int p[2])
{
  int sig ;

  G_display_image();  
  do {
    sig = Gi_events(p) ;
  }  while (sig != -3) ;

  return sig ;
}



int G_wait_click(double p[2])
{
  int sig ;
  int pi[2] ;

  sig = Gi_wait_click(pi) ;
  p[0] = pi[0] ;
  p[1] = pi[1] ;
  
  return sig ;
}



int G_wait_key()
{
  int p[2] ;
  int sig ;

  G_display_image();  
  do {
    sig = Gi_events(p) ;
  }  while (sig < 0) ;

  return sig ;
}




int G_no_wait_key()
// if key has been hit, return non-negative (ascii) value
// return negative if key has NOT been hit  (some other action or no action)
{
  int p[2] ;
  int sig ;
  sig = Gi_events(p) ;
  return sig ;
}










/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
// Added Aug 28, 2012



static int sector(double xcenter, double ycenter, double radius, 
                  double start_radians, double end_radians,
                  double *x, double *y,
	          int num_pts_in_full_circle) 
{
  int num,j ;
  double theta,delta ;

  //  printf("start = %lf  end = %lf\n",start_radians,end_radians) ;

  delta = end_radians - start_radians ;
  if (delta < 0) return 0 ;
  if (delta > 2*M_PI) return 0 ;

  num = (int)(num_pts_in_full_circle*delta/(2*M_PI)) ;

  //  printf("delta = %lf  num = %d\n",delta,num) ;

  if (num < 1) { num = 1 ; }
  else if (num > 500) { num = 500 ; }

  //  printf("new num = %d\n",num) ;

  j = 0 ; 
  while (j <= num) {
    theta = start_radians + j*delta/num;

    // printf("j = %d  theta = %lf\n",j,theta) ;

    x[j] = xcenter + radius*cos(theta) ;
    y[j] = ycenter + radius*sin(theta) ;

    j++ ;
  }
  x[j] = xcenter ;y[j] = ycenter ; j++ ;

  return j ;
}



int G_sector(double xcenter, double ycenter, double radius, 
             double start_radians, double end_radians)
{
  int num_pts_in_full_circle, n ;
  double x[500+3],y[500+3] ;

  num_pts_in_full_circle = 500 ;

  n = sector(xcenter,ycenter,radius,start_radians,end_radians,
             x,y,num_pts_in_full_circle) ;

  // normally, n == num_pts_in_full_circle + 1 

  if (n <= 0) {
    printf("Sector flaw\n") ;
  } else {
    G_polygon(x,y,n) ;
  }

  return 1 ;
}




int G_fill_sector(double xcenter, double ycenter, double radius, 
                  double start_radians, double end_radians)
{
  int num_pts_in_full_circle, n ;
  double x[500+3],y[500+3] ;

  num_pts_in_full_circle = 500 ;

  n = sector(xcenter,ycenter,radius,start_radians,end_radians,
             x,y,num_pts_in_full_circle) ;

  // normally, n == num_pts_in_full_circle + 1 

  if (n <= 0) {
    printf("Sector flaw\n") ;
  } else {
    G_fill_polygon(x,y,n) ;
  }

  return 1 ;
}



/////////////////////////////////////////////////////////////////

static double mouse_values[2] ;


int G_wait_mouse()
// save off mouse click in reserved variables
{
  G_wait_click(mouse_values) ;
  return 1 ;  
}



double G_x_mouse()
// return x coordinate of last G_wait_mouse
{
  return mouse_values[0] ;
}



double G_y_mouse()
// return y coordinate of last G_wait_mouse
{
  return mouse_values[1] ;
}




//====================================================================
// Time :  


int get_timeI (int *hms)
// hms had better be at least 3 ints long
// returns 1 for success, 0 for failure
// which is the opposite of what the standard
// gettimeofday returns
{
 int failure ;
 struct timeval Tp, *tp ;
 struct timezone Tzp, *tzp ;
 time_t  numseconds, *clock ;
 struct tm *x ;

 tp = &Tp ;
 tzp = &Tzp ;
 failure = gettimeofday(tp,tzp) ; // 1 for fail, 0 for success
 numseconds = tp-> tv_sec ;
 clock = &numseconds ;

 x =  localtime(clock) ; 

 hms[0] =     x->tm_hour ;
 hms[1] =     x->tm_min ;
 hms[2] =     x->tm_sec ;

 return (1 - failure) ;

}




int get_timeD (double *hms)
// hms had better be at least 3 doubles long
// returns 1 for success, 0 for failure
// which is the opposite of what the standard
// gettimeofday returns
{
 int ihms[3] ;
 int s ;

 s = get_timeI (ihms) ;

 hms[0] = ihms[0] ;
 hms[1] = ihms[1] ;
 hms[2] = ihms[2] ;

 return s ;
}



int get_time (double *hms)
// synonym for get_timeD
{
  int s ;
  s = get_timeD(hms) ;
  return s ;
}


int G_get_time (double *hms)
// another synonym for get_timeD
{
  int s ;
  s = get_timeD(hms) ;
  return s ;
}




/////////////////////////////////////////////////////////////////
// bmp file support
/////////////////////////////////////////////////////////////////




static unsigned char bmp_header[54] = {
0x42,0x4D,
0x00,0x00,0x00,0x00, // size of bmp file-overwrite
0x00,0x00,
0x00,0x00,
0x36,0x00,0x00,0x00,
0x28,0x00,0x00,0x00,
0x00,0x00,0x00,0x00, // width of bitmap in pixels-overwrite
0x00,0x00,0x00,0x00, // height of bitmap in pixles-overwrite
0x01,0x00,
0x18,0x00,
0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00, // size of raw bmp data after this bmp_header
0x13,0x0B,0x00,0x00,
0x13,0x0B,0x00,0x00,
0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,
} ;




static void store_int_as_little_endian (int n, int p)
{
  unsigned char h ;

  h = n % 256 ;  bmp_header[p] = h ; 

  p++ ;
  n = n / 256 ;
  h = n % 256 ;  bmp_header[p] = h ; 

  p++ ;
  n = n / 256 ;
  h = n % 256 ;  bmp_header[p] = h ; 

  p++ ;
  n = n / 256 ;
  h = n % 256 ;  bmp_header[p] = h ; 

}


static int get_int_from_little_endian (int p)
{
  int a,b,c,d,r ;

  a = bmp_header[p++] ;
  b = bmp_header[p++] ;
  c = bmp_header[p++] ;
  d = bmp_header[p++] ;

  // probably all of the mask anding is not necessary
  r = ((d << 24) & 0xff000000) |
      ((c << 16) & 0x00ff0000) |
      ((b <<  8) & 0x0000ff00) |
              (a & 0x000000ff) ;


  return r ;
}







int pixel_to_byte_rgb (int pix, char Brgb[3]) 
// Always return 1
{
  Brgb[0] = (char) ((pix >> 16) & 0xff) ;
  Brgb[1] = (char) ((pix >>  8) & 0xff) ;
  Brgb[2] = (char) ((pix      ) & 0xff) ;

  return 1 ;
}




int G_save_to_bmp_file (char *fname)
// return 1 if successful, otherwise return 0 
// (probably because the file could not be opened)
{

  //==================================  
  // X11 stuff
  XImage *pxim ;
  pxim = XGetImage (XxDisplay, XxDrawable, 0,0, Xx_Pix_width, Xx_Pix_height,
                      AllPlanes, ZPixmap) ;
  //==================================


  FILE *f ;
  int s ;

  f = fopen(fname,"w") ;
  if (f == NULL) {
    printf("G_save_to_bmp_file : can't open file %s\n",fname) ;
    return 0 ;
  }


  
  int bmp_header_size = 54 ;
  int width_of_bitmap_in_pixels = Xx_Pix_width ;
  int height_of_bitmap_in_pixels = Xx_Pix_height ;
  int rowsize ;
  rowsize = 3*width_of_bitmap_in_pixels ; // 3 bytes per pixel
  rowsize = ((rowsize+3)/4)*4 ; // 4 byte boundary

  int size_of_raw_bmp_data = rowsize*height_of_bitmap_in_pixels ;
  int size_of_bmp_file = size_of_raw_bmp_data + bmp_header_size ;

  // modify bmp_header as appropriate
  store_int_as_little_endian (size_of_bmp_file,0x02) ;
  store_int_as_little_endian (width_of_bitmap_in_pixels,0x12) ;
  store_int_as_little_endian (height_of_bitmap_in_pixels,0x16) ;
  store_int_as_little_endian (size_of_raw_bmp_data,0x22) ;


  // output the bmp_header
  int i,c,x,y,count ;
  for (i = 0 ; i < 54 ; i++) {
    c = bmp_header[i] ;  fputc(c,f) ;
  }


  // output the color data
  //  Pixel pix ;
  int p ;
  
  char Brgb[3] ;

  for (y = 0 ; y < height_of_bitmap_in_pixels ; y++) {
    count = 0 ; 
    for (x = 0 ; x < width_of_bitmap_in_pixels ; x++) {


      
      // pix = G_get_pixel(x,y) ;
      // pixel_to_byte_rgb(pix, Brgb) ;
      p = XGetPixel(pxim,x, Xx_Pix_height - 1 - y) ; // X11 stuff
      pixel_to_byte_rgb(p, Brgb) ;
      


      c = Brgb[2] ;   fputc(c,f) ; count++ ; // blue first
      c = Brgb[1] ;   fputc(c,f) ; count++ ;
      c = Brgb[0] ;   fputc(c,f) ; count++ ;
    }
    while (count < rowsize) {
      c = 0 ; fputc(c,f) ;
      count++ ;
    }
  }


  fclose(f) ;


  //==================================  
  // X11 stuff
  XDestroyImage(pxim) ; // lack of this was causing mem leaks when
  // many images were being saved for movies
  //==================================  
  
  return 1 ;

}







int get_dimensions_of_bmp_file (char filename[], int dimensions[2])
// return 0 if failure, else return 1
{

  FILE *f ;
  f = fopen(filename,"r") ;
  if (f == NULL) {
    printf("G_display_bmp_file error : can't open file, %s\n",filename) ;
    return 0 ;
  }


  int bmp_header_size = 54 ;
  // header should be 54 bytes...read them
  int i,c ;
  for (i = 0 ; i < bmp_header_size ; i++) {
    c = fgetc(f) ;
    if (c == EOF) return 0 ;
    bmp_header[i] = (unsigned char) c ;
  }

  // check for the proper ID code
  if (bmp_header[0] != 0x42) return 0 ;
  if (bmp_header[1] != 0x4D) return 0 ;


  int size_of_bmp_file = get_int_from_little_endian (0x02) ;
  int width_of_bitmap_in_pixels = get_int_from_little_endian (0x12) ;
  int height_of_bitmap_in_pixels = get_int_from_little_endian (0x16) ;


  
  dimensions[0] = width_of_bitmap_in_pixels ;
  dimensions[1] = height_of_bitmap_in_pixels ;  
  fclose(f) ;
  return 1 ;
}






int G_display_bmp_file (char filename[], int xoffset, int yoffset)
// return 0 if failure, else return 1  
{

  FILE *f ;
  f = fopen(filename,"r") ;
  if (f == NULL) {
    printf("G_display_bmp_file error : can't open file, %s\n",filename) ;
    return 0 ;
  }


  

  int bmp_header_size = 54 ;
  // header should be 54 bytes...read them
  int i,c ;
  for (i = 0 ; i < bmp_header_size ; i++) {
    c = fgetc(f) ;
    if (c == EOF) return 0 ;
    bmp_header[i] = (unsigned char) c ;
  }

  // check for the proper ID code
  if (bmp_header[0] != 0x42) return 0 ;
  if (bmp_header[1] != 0x4D) return 0 ;


  int size_of_bmp_file = get_int_from_little_endian (0x02) ;
  int width_of_bitmap_in_pixels = get_int_from_little_endian (0x12) ;
  int height_of_bitmap_in_pixels = get_int_from_little_endian (0x16) ;
  int size_of_raw_bmp_data = get_int_from_little_endian (0x22) ;


  /////////////////////////////////////////////////////////

  int rowsize ;
  rowsize = 3*width_of_bitmap_in_pixels ; // 3 bytes per pixel
  rowsize = ((rowsize+3)/4)*4 ; // 4 byte boundary

  int computed_size_of_raw_bmp_data = rowsize*height_of_bitmap_in_pixels ;
  int computed_size_of_bmp_file = size_of_raw_bmp_data + bmp_header_size ;

  // do some checks
  if (computed_size_of_raw_bmp_data != size_of_raw_bmp_data ) {
    return 0 ;
  }

  if (computed_size_of_bmp_file != size_of_bmp_file ) {
    return 0 ;
  }


  
  // input the color data
  int y,count,x ;
  unsigned char r,g,b ;

  for (y = 0 ; y < height_of_bitmap_in_pixels ; y++) {
    count = 0 ; 
    for (x = 0 ; x < width_of_bitmap_in_pixels ; x++) {
      c = fgetc(f) ;
      if (c == EOF) return 0 ;
      b = (unsigned char) c ;
      count++ ;

      c = fgetc(f) ;
      if (c == EOF) return 0 ;
      g = (unsigned char) c ;
      count++ ;

      c = fgetc(f) ;
      if (c == EOF) return 0 ;
      r = (unsigned char) c ;
      count++ ;


      Gi_rgb(r,g,b) ;
      G_point(x+xoffset,y+yoffset) ;
      
    }
    while (count < rowsize) {
      c = fgetc(f) ;
      if (c == EOF) return 0 ;
      count++ ;
    }
  }

  c = fgetc(f) ;
  if (c != EOF) return 0 ;
  
  fclose(f) ;

  return 1 ;
}





#endif

