//
//  ViewController.swift
//  FaceDetectionSwift
//
//  Created by DE4ME on 21.09.2022.
//

import Cocoa;
import ApplicationServices;
import AVFoundation;
import dlib;


class vMainViewController: NSViewController {
    
    @IBOutlet var playButton: NSButton!;
    @IBOutlet var videoPreviewView: DLIBFacePreview!;
    
    //MARK: VAR
    
    private var videoProcessor: cVideoProcessor!;

    var canOpen: Bool = true{
        didSet {
            self.playButton.isEnabled = self.canOpen;
        }
    }
    
    override var representedObject: Any? {
        didSet {
            switch self.representedObject {
            case let url as URL:
                self.videoProcessor?.stop();
                self.videoProcessor = cVideoProcessor(url: url);
                self.videoProcessor.delegate = self;
                self.videoProcessor.prepare();
                self.playButton.isEnabled = true;
                self.view.window?.title = url.deletingPathExtension().lastPathComponent;
            default:
                break;
            }
        }
    }
    
    //MARK: OVERRIDE

    deinit{
#if DEBUG
        print(#function, NSStringFromClass(type(of: self)));
#endif
        self.videoProcessor?.stop();
    }

    override func viewDidLoad() {
        super.viewDidLoad();
        // Do any additional setup after loading the view.
        self.playButton.isEnabled = false;
    }
    
    override func viewDidDisappear() {
        super.viewDidDisappear();
        self.videoProcessor?.stop();
    }
    
    //MARK: ACTION
    
    @objc func terminate(_ sender:Any?) {
        print(#function);
    }

    @objc func openDocument(_ sender:Any?) {
        guard let window = self.view.window else {
            return;
        }
        let panel = NSOpenPanel();
        panel.canChooseDirectories = false;
        panel.canChooseFiles = true;
        panel.allowsMultipleSelection = false;
        panel.isExtensionHidden = false;
        if #available(macOS 12, *) {
            panel.allowedContentTypes = [.movie, .video];
        } else if #available(macOS 11, *) {
            panel.allowedFileTypes = [UTType.movie.identifier, UTType.video.identifier];
        } else {
            panel.allowedFileTypes = [kUTTypeMovie as String, kUTTypeVideo as String];
        }
        panel.beginSheetModal(for: window) { responce in
            guard responce == .OK,
                  let url = panel.url
            else {
                return;
            }
            NSDocumentController.shared.noteNewRecentDocumentURL(url);
            self.representedObject = url;
        }
    }
    
    @IBAction func playClick(_ sender: Any) {
        guard let processor = self.videoProcessor else {
            return;
        }
        self.canOpen = false;
        processor.run();
    }
    
}

//MARK: - VideoProcessorDelegate

extension vMainViewController: VideoProcessorDelegate {
    
    func displayImage(_ image: NSImage, faces: [DLIBFace]?) {
        DispatchQueue.main.async {
            self.videoPreviewView.image = image;
            self.videoPreviewView.faces = faces;
            self.videoPreviewView.setNeedsDisplay(.infinite);
        }
    }
    
    func didFinifshRunning() {
        DispatchQueue.main.async {
            self.canOpen = true;
        }
    }
    
}


