//
//  DLIBFace.m
//  dlib
//
//  Created by DE4ME on 26.07.2022.
//


#import "DLIBFace+Internal.h"
#import "DLIBUtils+Internal.h"


#import "image_io.h"


using namespace dlib;


@implementation DLIBFace
{
    NSRect _frame;
    NSArray<NSValue*>* _shape;
}

- (instancetype)initWithRectangle:(rectangle)frame
{
    NSRect rect = DLIBNSRectFromRectangle(frame);
    return [self initWithFrame:rect];
}

- (instancetype)initWithFrame:(NSRect)frame
{
    self = [super init];
    if (self) {
        _frame = frame;
        _shape = @[];
    }
    return self;
}

- (instancetype)initWithRectangle:(rectangle)frame shape:(full_object_detection)shape
{
    NSRect rect = DLIBNSRectFromRectangle(frame);
    return [self initWithFrame:rect shape:shape];
}

- (instancetype)initWithFrame:(NSRect)frame shape:(full_object_detection)shape
{
    self = [super init];
    if (self) {
        _frame = frame;
        NSMutableArray<NSValue*>* points = [NSMutableArray new];
        for (unsigned long i = 0; i<shape.num_parts(); i++) {
            point point = shape.part(i);
            NSPoint pt = NSMakePoint(point.x() , point.y());
            NSValue* value = [NSValue valueWithPoint:pt];
            [points addObject:value];
        }
        _shape = points;
    }
    return self;
}

- (NSRect)frame
{
    return _frame;
}

- (NSArray<NSValue*>*) shape
{
    return _shape;
}

@end
