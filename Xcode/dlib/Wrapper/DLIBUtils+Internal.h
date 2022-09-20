//
//  DLIBUtils+Internal.h
//  dlib
//
//  Created by DE4ME on 28.07.2022.
//


#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>


#import "DLIBUtils.h"
#import "geometry.h"
#import "image_processing.h"


#if TARGET_OS_MAC
  #define DLIB_MAC_RECTANGLE_FLIP(a,h) DLIBRectangleFlip(a, h)
  #define DLIB_MAC_SHAPE_FLIP(a,h) DLIBShapeFlip(a, h)
#else
  #define DLIB_MAC_RECTANGLE_FLIP(a,h)
  #define DLIB_MAC_SHAPE_FLIP(a,h)
#endif


inline void DLIBRectangleFlip(dlib::rectangle &rect, long height)
{
    long top = rect.top();
    long bottom = rect.bottom();
    rect.set_bottom(height - top);
    rect.set_top(height - bottom);
}

inline void DLIBShapeFlip(dlib::full_object_detection &shape, long height)
{
    for (unsigned long i=0; i<shape.num_parts(); i++) {
        dlib::point point = shape.part(i);
        shape.part(i) = dlib::point(point.x(), height - point.y());
    }
}

inline dlib::rectangle DLIBRectangleFromNSRect(NSRect rect)
{
#if TARGET_OS_MAC
    long left = rect.origin.x;
    long bottom = rect.origin.y;
    long right = left + rect.size.width;
    long top = bottom + rect.size.height;
    dlib::rectangle dlibRect(left, top, right, bottom);
    return dlibRect;
#else
    long left = rect.origin.x;
    long top = rect.origin.y;
    long right = left + rect.size.width;
    long bottom = top + rect.size.height;
    dlib::rectangle dlibRect(left, top, right, bottom);
    return dlibRect;
#endif
}

inline NSRect DLIBNSRectFromRectangle(dlib::rectangle rect)
{
#if TARGET_OS_MAC
    return NSMakeRect(rect.left(), rect.bottom(), rect.width(), rect.top()-rect.bottom()+1);
#else
    return NSMakeRect(rect.left(), rect.top(), rect.width(), rect.height());
#endif
}
