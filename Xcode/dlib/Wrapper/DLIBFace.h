//
//  DLIBFace.h
//  dlib
//
//  Created by DE4ME on 26.07.2022.
//


#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>


NS_ASSUME_NONNULL_BEGIN

@interface DLIBFace: NSObject

@property (readonly, nonatomic) NSRect frame;
@property (readonly, nonatomic) NSArray<NSValue*>* shape;

- (instancetype)init NS_UNAVAILABLE;

@end

NS_ASSUME_NONNULL_END
