//
//  DLIBFacePreview.h
//  dlib
//
//  Created by DE4ME on 29.07.2022.
//


#import <AppKit/AppKit.h>
#import <dlib/DLIBFace.h>


NS_ASSUME_NONNULL_BEGIN

IB_DESIGNABLE
@interface DLIBFacePreview : NSView

@property IBInspectable (nonatomic) NSColor* frameColor;
@property IBInspectable (nonatomic) NSColor* shapeColor;

@property IBInspectable (nonatomic) BOOL drawFrame;
@property IBInspectable (nonatomic) BOOL drawShape;

@property (nonatomic) NSImage* _Nullable image;
@property (nonatomic) NSArray<DLIBFace*>* _Nullable faces;

@end

NS_ASSUME_NONNULL_END
