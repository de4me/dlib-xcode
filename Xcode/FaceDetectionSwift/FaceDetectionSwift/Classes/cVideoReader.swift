//
//  cVideoReader.swift
//  BlinkCounter
//
//  Created by DE4ME on 26.07.2022.
//

import Cocoa;
import AVFoundation;


class cVideoReader {
    
    //MARK: VAR
    
    private var videoAsset: AVAsset!;
    private var videoTrack: AVAssetTrack!;
    private var assetReader: AVAssetReader!;
    private var videoAssetReaderOutput: AVAssetReaderTrackOutput!;
    
    //MARK: GET
    
    var frameRateInMilliseconds: Float {
        1000.0 / self.videoTrack.nominalFrameRate;
    }
    
    var affineTransform: CGAffineTransform {
        self.videoTrack.preferredTransform.inverted();
    }
    
    var orientation: CGImagePropertyOrientation {
        let transform = self.affineTransform;
        let angleInDegrees = atan2(transform.b, transform.a) * CGFloat(180) / CGFloat.pi;
        switch angleInDegrees {
        case 0:
            return .up; // Recording button is on the right
        case 180:
            return .down; // abs(180) degree rotation recording button is on the right
        case -180:
            return .down; // abs(180) degree rotation recording button is on the right
        case 90:
            return .left; // 90 degree CW rotation recording button is on the top
        case -90:
            return .right; // 90 degree CCW rotation recording button is on the bottom
        default:
            return .up;
        }
    }
    
    //MARK: OVERRIDE
    
#if DEBUG
    deinit{
        print(#function, NSStringFromClass(type(of: self)));
    }
#endif
    
    init?(videoAsset: AVAsset) {
        self.videoAsset = videoAsset;
        guard let track = self.videoAsset.tracks(withMediaType: AVMediaType.video).first else {
            return nil;
        }
        self.videoTrack = track;
        guard self.restartReading() else {
            return nil;
        }
    }
    
    //MARK: FUNC
    
    func restartReading() -> Bool {
        do {
            self.assetReader = try AVAssetReader(asset: self.videoAsset);
            self.videoAssetReaderOutput = AVAssetReaderTrackOutput(track: self.videoTrack, outputSettings: [kCVPixelBufferPixelFormatTypeKey as String: kCVPixelFormatType_32BGRA]);
            self.videoAssetReaderOutput.alwaysCopiesSampleData = true;
            guard self.assetReader.canAdd(self.videoAssetReaderOutput) else {
                return false;
            }
            self.assetReader.add(self.videoAssetReaderOutput);
            return self.assetReader.startReading();
        } catch {
            print(#function, error);
            return false;
        }
    }

    func nextFrame() -> CVImageBuffer? {
        guard let sampleBuffer = self.videoAssetReaderOutput.copyNextSampleBuffer() else {
            return nil
        }
        return CMSampleBufferGetImageBuffer(sampleBuffer);
    }
    
}
