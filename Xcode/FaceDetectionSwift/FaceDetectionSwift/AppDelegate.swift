//
//  AppDelegate.swift
//  FaceDetectionSwift
//
//  Created by DE4ME on 21.09.2022.
//

import Cocoa;


@main
class AppDelegate: NSObject, NSApplicationDelegate {
    
    @objc func canOpen() -> Bool {
        switch NSApp.keyWindow?.contentViewController {
        case let controller as vMainViewController:
            return controller.canOpen;
        default:
            return false;
        }
    }

    func applicationDidFinishLaunching(_ aNotification: Notification) {
        // Insert code here to initialize your application
    }

    func applicationWillTerminate(_ aNotification: Notification) {
        // Insert code here to tear down your application
    }
    
    func applicationShouldTerminateAfterLastWindowClosed(_ sender: NSApplication) -> Bool {
        true;
    }

    func application(_ sender: NSApplication, openFile filename: String) -> Bool {
        guard self.canOpen() else {
            return false;
        }
        sender.keyWindow?.contentViewController?.representedObject = URL(fileURLWithPath:filename);
        return true;
    }
    
}

