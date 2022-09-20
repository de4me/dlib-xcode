//
//  DLIBWrapper.mm
//  dlib
//
//  Created by DE4ME on 26.07.2022.
//  


#import "DLIBWrapper.h"


#import "image_processing.h"
#import "image_io.h"
#import "image_processing/frontal_face_detector.h"
#import "DLIBFace+Internal.h"
#import "DLIBUtils+Internal.h"


using namespace dlib;


@interface DLIBWrapper ()

@end


@implementation DLIBWrapper
{
    shape_predictor _sp;
    frontal_face_detector _detector;
}

+ (NSURL* _Nullable)defaultShapePredictorURL
{
    NSBundle* bundle = [NSBundle bundleForClass: self.class];
    NSURL* url = [bundle URLForResource:@"shape_predictor_68_face_landmarks" withExtension:@"dat"];
    return url;
}

+ (instancetype)wrapperDefault
{
    NSURL* url = self.defaultShapePredictorURL;
    if (url) {
        return [self wrapperWithShapePredictorURL:url];
    }
    return NULL;
}

+ (instancetype)wrapperWithShapePredictorURL:(NSURL*) url
{
    return [[DLIBWrapper new] initWithShapePredictorURL:url];
}

- (instancetype)init
{
    self = [super init];
    if (self) {
        _detector = get_frontal_face_detector();
    }
    return self;
}

- (instancetype)initWithShapePredictorURL:(NSURL *)url
{
    self = [self init];
    if (self) {
        [self prepareWithShapePredictorURL:url];
    }
    return self;
}

- (void)prepareWithShapePredictorURL:(NSURL *)url
{
    std::string path = url.path.UTF8String;
    dlib::deserialize(path) >> _sp;
}

- (NSArray<DLIBFace *> *)detectFacesFromBufferBGRA: (const UInt8*)buffer width:(NSInteger) width height:(NSInteger) height
{
    NSMutableArray<DLIBFace*>* result = [NSMutableArray new];
    dlib::array2d<dlib::bgr_pixel> img(height, width);
    long position = 0;
    while (img.move_next()) {
        dlib::bgr_pixel& pixel = img.element();
        // assuming rgba format here
        long bufferLocation = position * 4; //(row * width + column) * 4;
        char b = buffer[bufferLocation];
        char g = buffer[bufferLocation + 1];
        char r = buffer[bufferLocation + 2];
        //we do not need this
        //char a = baseBuffer[bufferLocation + 3];
        dlib::bgr_pixel newpixel(b, g, r);
        pixel = newpixel;
        position++;
    }
    for (auto face: _detector(img)) {
        DLIB_MAC_RECTANGLE_FLIP(face, height);
        DLIBFace* obj = [[DLIBFace new] initWithRectangle:face];
        [result addObject:obj];
    }
    return result;
}

- (NSArray<DLIBFace *> *)detectFacesAndShapesFromBufferBGRA: (const UInt8*)buffer width:(NSInteger) width height:(NSInteger) height
{
    NSMutableArray<DLIBFace*>* result = [NSMutableArray new];
    dlib::array2d<dlib::bgr_pixel> img(height, width);
    long position = 0;
    while (img.move_next()) {
        dlib::bgr_pixel& pixel = img.element();
        // assuming rgba format here
        long bufferLocation = position * 4; //(row * width + column) * 4;
        char b = buffer[bufferLocation];
        char g = buffer[bufferLocation + 1];
        char r = buffer[bufferLocation + 2];
        //we do not need this
        //char a = baseBuffer[bufferLocation + 3];
        dlib::bgr_pixel newpixel(b, g, r);
        pixel = newpixel;
        position++;
    }
    for (auto face: _detector(img)) {
        full_object_detection shape = _sp(img, face);
        DLIB_MAC_RECTANGLE_FLIP(face, height);
        DLIB_MAC_SHAPE_FLIP(shape, height);
        DLIBFace* obj = [[DLIBFace new] initWithRectangle:face shape:shape];
        [result addObject:obj];
    }
    return result;
}

- (NSArray<DLIBFace *> *)detectShapesFromBufferBGRA: (const UInt8*)buffer width:(NSInteger) width height:(NSInteger) height withFrames:(NSArray<NSValue*>*) frames
{
    NSMutableArray<DLIBFace*>* result = [NSMutableArray new];
    dlib::array2d<dlib::bgr_pixel> img(height, width);
    long position = 0;
    while (img.move_next()) {
        dlib::bgr_pixel& pixel = img.element();
        // assuming rgba format here
        long bufferLocation = position * 4; //(row * width + column) * 4;
        char b = buffer[bufferLocation];
        char g = buffer[bufferLocation + 1];
        char r = buffer[bufferLocation + 2];
        //we do not need this
        //char a = baseBuffer[bufferLocation + 3];
        dlib::bgr_pixel newpixel(b, g, r);
        pixel = newpixel;
        position++;
    }
    for (NSValue* value in frames) {
        NSRect rect = value.rectValue;
        dlib::rectangle face = DLIBRectangleFromNSRect(rect);
        DLIB_MAC_RECTANGLE_FLIP(face, height);
        auto shape = _sp(img, face);
        DLIB_MAC_SHAPE_FLIP(shape, height);
        DLIBFace* obj = [[DLIBFace new] initWithFrame:rect shape:shape];
        [result addObject:obj];
    }
    return result;
}

- (NSArray<DLIBFace *> *)detectFacesAndShapesFromImageBGRA: (CGImageRef)image
{
    CFDataRef data = CGDataProviderCopyData(CGImageGetDataProvider(image));
    if (data != NULL) {
        NSInteger width = CGImageGetWidth(image);
        NSInteger height = CGImageGetHeight(image);
        const UInt8* baseBuffer = CFDataGetBytePtr(data);
        if (baseBuffer != NULL) {
            NSArray<DLIBFace*>* result = [self detectFacesAndShapesFromBufferBGRA: baseBuffer width:width height:height];
            CFRelease(data);
            return result;
        }
        CFRelease(data);
    }
    return @[];
}

- (NSArray<DLIBFace *> *)detectShapesFromImageBGRA: (CGImageRef)image withFrames:(NSArray<NSValue*>*) frames
{
    CFDataRef data = CGDataProviderCopyData(CGImageGetDataProvider(image));
    if (data != NULL) {
        NSInteger width = CGImageGetWidth(image);
        NSInteger height = CGImageGetHeight(image);
        const UInt8* baseBuffer = CFDataGetBytePtr(data);
        if (baseBuffer != NULL) {
            NSArray<DLIBFace*>* result = [self detectShapesFromBufferBGRA: baseBuffer width:width height:height withFrames: frames];
            CFRelease(data);
            return result;
        }
        CFRelease(data);
    }
    return @[];
}

- (NSArray<DLIBFace *> *)detectFacesAndShapesFromPixelBufferBGRA: (CVPixelBufferRef)pixelBuffer
{
    if (CVPixelBufferLockBaseAddress(pixelBuffer, kCVPixelBufferLock_ReadOnly) == kCVReturnSuccess ) {
        NSInteger width = CVPixelBufferGetWidth(pixelBuffer);
        NSInteger height = CVPixelBufferGetHeight(pixelBuffer);
        const UInt8* baseBuffer = (const UInt8 *)CVPixelBufferGetBaseAddress(pixelBuffer);
        if (baseBuffer != NULL) {
            NSArray<DLIBFace*>* result = [self detectFacesAndShapesFromBufferBGRA: baseBuffer width:width height:height];
            CVPixelBufferUnlockBaseAddress(pixelBuffer, kCVPixelBufferLock_ReadOnly);
            return result;
        }
        CVPixelBufferUnlockBaseAddress(pixelBuffer, kCVPixelBufferLock_ReadOnly);
    }
    return @[];
}

- (NSArray<DLIBFace *> *)detectShapesFromPixelBufferBGRA: (CVPixelBufferRef)pixelBuffer  withFrames:(NSArray<NSValue*>*) frames
{
    if (CVPixelBufferLockBaseAddress(pixelBuffer, kCVPixelBufferLock_ReadOnly) == kCVReturnSuccess ) {
        NSInteger width = CVPixelBufferGetWidth(pixelBuffer);
        NSInteger height = CVPixelBufferGetHeight(pixelBuffer);
        const UInt8* baseBuffer = (const UInt8 *)CVPixelBufferGetBaseAddress(pixelBuffer);
        if (baseBuffer != NULL) {
            NSArray<DLIBFace*>* result = [self detectShapesFromBufferBGRA: baseBuffer width:width height:height withFrames:frames];
            CVPixelBufferUnlockBaseAddress(pixelBuffer, kCVPixelBufferLock_ReadOnly);
            return result;
        }
        CVPixelBufferUnlockBaseAddress(pixelBuffer, kCVPixelBufferLock_ReadOnly);
    }
    return @[];
}

@end
