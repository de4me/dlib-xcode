//
//  DLIBFace+Internal.h
//  dlib
//
//  Created by DE4ME on 26.07.2022.
//


#import <dlib/DLIBFace.h>


#import "image_processing.h"


NS_ASSUME_NONNULL_BEGIN

@interface DLIBFace()

- (instancetype)initWithFrame:(NSRect)frame;
- (instancetype)initWithRectangle:(dlib::rectangle)frame;

- (instancetype)initWithRectangle:(dlib::rectangle)frame shape:(dlib::full_object_detection)shape;
- (instancetype)initWithFrame:(NSRect)frame shape:(dlib::full_object_detection)shape;

@end

NS_ASSUME_NONNULL_END
