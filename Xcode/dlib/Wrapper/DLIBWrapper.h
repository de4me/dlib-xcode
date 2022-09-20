//
//  DLIBWrapper.h
//  dlib
//
//  Created by DE4ME on 26.07.2022.
//


#import <Foundation/Foundation.h>
#import <CoreMedia/CoreMedia.h>
#import <CoreGraphics/CoreGraphics.h>
#import <dlib/DLIBFace.h>


NS_ASSUME_NONNULL_BEGIN

@interface DLIBWrapper : NSObject

@property (class, readonly) NSURL* _Nullable defaultShapePredictorURL;

+ (instancetype _Nullable)wrapperDefault;

- (instancetype)init NS_UNAVAILABLE;
- (instancetype)initWithShapePredictorURL:(NSURL*) url;

- (NSArray<DLIBFace *> *)detectFacesFromBufferBGRA: (const UInt8*)buffer width:(NSInteger) width height:(NSInteger) height;
- (NSArray<DLIBFace *> *)detectFacesAndShapesFromBufferBGRA: (const UInt8*)buffer width:(NSInteger) width height:(NSInteger) height;
- (NSArray<DLIBFace *> *)detectShapesFromBufferBGRA: (const UInt8*)buffer width:(NSInteger) width height:(NSInteger) height withFrames:(NSArray<NSValue*>*) frames;

- (NSArray<DLIBFace *> *)detectFacesAndShapesFromImageBGRA: (CGImageRef)image;
- (NSArray<DLIBFace *> *)detectFacesAndShapesFromPixelBufferBGRA: (CVPixelBufferRef)pixelBuffer;

- (NSArray<DLIBFace *> *)detectShapesFromImageBGRA: (CGImageRef)image withFrames:(NSArray<NSValue*>*) frames;
- (NSArray<DLIBFace *> *)detectShapesFromPixelBufferBGRA: (CVPixelBufferRef)pixelBuffer  withFrames:(NSArray<NSValue*>*) frames;

@end

NS_ASSUME_NONNULL_END
