//
//  cVideoProcessor.swift
//  BlinkCounter
//
//  Created by DE4ME on 26.07.2022.
//

import Cocoa;
import AVFoundation;
import dlib;


enum VideoProcessorError: Error {
    case readerInitializationFailed
    case firstFrameReadFailed
    case objectTrackingFailed
    case rectangleDetectionFailed
}


protocol VideoProcessorDelegate: AnyObject {
    func displayImage(_ image: NSImage, faces: [DLIBFace]?);
    func didFinifshRunning();
}


class cVideoProcessor {
    
    //MARK: CONST
    
    private let playbackQueue = DispatchQueue(label: "com.de4me.VideoProcessor.Playback", qos: .userInteractive);
    private let workQueue = DispatchQueue(label: "com.de4me.VideoProcessor.Work", qos: .userInitiated);
    private let context = CIContext();
    private let colorSpace = CGColorSpaceCreateDeviceRGB();
    private var dlibWrapper: DLIBWrapper?;
    
    let videoAsset: AVAsset;
    
    //MARK: VAR
    
    private var lastWork: DispatchWorkItem?;
    private var cancelRequested = false;
    private var running = false;
    
    var skipFrames: Bool = true;
    weak var delegate: VideoProcessorDelegate?;
    
    //MARK: OVERRIDE
    
#if DEBUG
    deinit{
        print(#function, NSStringFromClass(type(of: self)));
    }
#endif
    
    init(url: URL) {
        self.videoAsset = AVAsset(url: url);
        self.dlibWrapper = DLIBWrapper.wrapperDefault();
    }
    
    //MARK: FUNC
    
    private func processFrame(_ frame: CVPixelBuffer, withAffineTransform transform: CGAffineTransform) {
        let work = DispatchWorkItem {
            autoreleasepool {
                let width = CVPixelBufferGetWidth(frame);
                let heigth = CVPixelBufferGetHeight(frame);
                let length = max(width, heigth);
                let ciImage: CIImage;
                if length > 1280 {
                    let scale = 1280.0 / CGFloat(length);
                    let transform = transform.concatenating(.init(scaleX: scale, y: scale));
                    ciImage = CIImage(cvPixelBuffer: frame).transformed(by: transform);
                } else {
                    ciImage = CIImage(cvPixelBuffer: frame).transformed(by: transform);
                }
                guard let cgImage = self.context.createCGImage(ciImage, from: ciImage.extent, format: .BGRA8, colorSpace: self.colorSpace) else {
                    return;
                }
                let size = NSSize(width: cgImage.width, height: cgImage.height);
                let image = NSImage(cgImage: cgImage, size: size);
                let faces = self.dlibWrapper?.detectFacesAndShapes(fromImageBGRA: cgImage);
                self.delegate?.displayImage(image, faces: faces);
            }
        }
        if self.skipFrames {
            self.lastWork?.cancel();
        }
        self.lastWork = work;
        self.workQueue.async(execute: work);
    }
    
    func prepare() {
        self.workQueue.async {
            do {
                guard let videoReader = cVideoReader(videoAsset: self.videoAsset) else {
                    throw VideoProcessorError.readerInitializationFailed
                }
                guard let firstFrame = videoReader.nextFrame() else {
                    throw VideoProcessorError.firstFrameReadFailed
                }
                self.processFrame(firstFrame, withAffineTransform: videoReader.affineTransform);
            }
            catch {
                print(#function, error);
            }
        }
    }
    
    func run() {
        guard !self.running else {
            return;
        }
        self.playbackQueue.async {
            do {
                guard let videoReader = cVideoReader(videoAsset: self.videoAsset) else {
                    throw VideoProcessorError.readerInitializationFailed;
                }
                var frame = videoReader.nextFrame();
                guard frame != nil else {
                    throw VideoProcessorError.firstFrameReadFailed;
                }
                self.cancelRequested = false;
                self.running = true;
                while self.cancelRequested == false, frame != nil {
                    self.processFrame(frame!, withAffineTransform: videoReader.affineTransform);
                    frame = videoReader.nextFrame();
                    usleep(useconds_t(videoReader.frameRateInMilliseconds*1000));
                }
                self.lastWork = nil;
                self.running = false;
                self.workQueue.async {
                    self.delegate?.didFinifshRunning();
                }
            }
            catch {
                print(#function, error);
            }
        }
    }
    
    func stop() {
        self.cancelRequested = true;
    }
    
}
