//
//  DLIBUtils.h
//  dlib
//
//  Created by DE4ME on 28.07.2022.
//


#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>


#ifdef __cplusplus
extern "C" {
#endif

CGPoint DLIBMiddlePoint(CGPoint pt1, CGPoint pt2);
CGFloat DLIBDistanceBetweenPoints(CGPoint pt1, CGPoint pt2);

NSRect DLIBNSRectFitToSize(NSRect rect, NSSize size, CGFloat scale);
CGFloat DLIBNSRectScaleForSize(NSRect rect, NSSize size);

#ifdef __cplusplus
}
#endif
