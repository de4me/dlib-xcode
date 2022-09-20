//
//  DLIBUtils.mm
//  dlib
//
//  Created by DE4ME on 28.07.2022.
//


#import "DLIBUtils+Internal.h"
#import "filtering.h"


using namespace dlib;


CGFloat DLIBDistanceBetweenPoints(CGPoint pt1, CGPoint pt2)
{
    return sqrt(pow(pt2.x-pt1.x,2)+pow(pt2.y-pt1.y,2));
}

CGPoint DLIBMiddlePoint(CGPoint pt1, CGPoint pt2)
{
    return CGPointMake((pt1.x + pt2.x)/2, (pt1.y + pt2.y)/2);
}

NSRect DLIBNSRectFitToSize(NSRect rect, NSSize size, CGFloat scale)
{
    CGFloat width = size.width * scale;
    CGFloat height = size.height * scale;
    NSRect result = NSMakeRect(rect.origin.x + (rect.size.width - width) / 2, rect.origin.y + (rect.size.height - height) / 2, width, height);
    return result;
}

CGFloat DLIBNSRectScaleForSize(NSRect rect, NSSize size)
{
    CGFloat scale_x = rect.size.width / size.width;
    CGFloat scale_y = rect.size.height / size.height;
    CGFloat scale = MIN(scale_x, scale_y);
    return scale;
}
