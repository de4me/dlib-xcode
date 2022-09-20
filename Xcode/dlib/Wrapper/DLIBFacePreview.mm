//
//  DLIBFacePreview.m
//  dlib
//
//  Created by DE4ME on 29.07.2022.
//


#import "DLIBFacePreview.h"
#import "DLIBUtils+Internal.h"


@implementation DLIBFacePreview


- (instancetype)init
{
    self = [super init];
    if (self) {
        [self prepare];
    }
    return self;
}

- (instancetype)initWithFrame:(NSRect)frameRect
{
    self = [super initWithFrame:frameRect];
    if (self) {
        [self prepare];
    }
    return self;
}

- (instancetype)initWithCoder:(NSCoder *)coder
{
    self = [super initWithCoder:coder];
    if (self) {
        [self prepare];
    }
    return self;
}

- (void)prepare
{
    self.frameColor = NSColor.blueColor;
    self.shapeColor = NSColor.greenColor;
    self.drawFrame = TRUE;
    self.drawShape = TRUE;
}

- (void)drawRect:(NSRect)dirtyRect
{
    [super drawRect:dirtyRect];
    NSImage* image = self.image;
    if (image) {
        CGFloat scale = DLIBNSRectScaleForSize(self.bounds, image.size);
        NSRect rect = DLIBNSRectFitToSize(self.bounds, image.size, scale);
        NSRect source_rect = NSMakeRect(0, 0, image.size.width, image.size.height);
        [image drawInRect:rect fromRect:source_rect operation:NSCompositingOperationCopy fraction:1];
        CGAffineTransform transform = CGAffineTransformMake(scale, 0, 0, scale, rect.origin.x, rect.origin.y);
        NSArray<DLIBFace*>* faces = self.faces;
        if (faces && (self.drawFrame || self.drawShape)) {
            for (DLIBFace* face in faces) {
                if (self.drawFrame) {
                    NSRect frame = CGRectApplyAffineTransform(face.frame, transform);
                    NSBezierPath* path = [NSBezierPath bezierPathWithRect:frame];
                    [self.frameColor setStroke];
                    [path stroke];
                }
                if (self.drawShape) {
                    NSBezierPath* path = [NSBezierPath new];
                    for (NSValue* value in face.shape) {
                        NSPoint point = value.pointValue;
                        NSRect point_rect = NSMakeRect(point.x * scale - 1 + rect.origin.x, point.y * scale - 1 + rect.origin.y, 3, 3);
                        [path appendBezierPathWithOvalInRect:point_rect];
                    }
                    [self.shapeColor setFill];
                    [path fill];
                }
            }
        }
    }
}

@end
