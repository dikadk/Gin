#if defined JUCE_IOS

#import <Foundation/Foundation.h>

inline NSString* juceStringToNS (const juce::String& s)
{
    return [NSString stringWithUTF8String:s.toUTF8()];
}

@interface FileSystemWatcherImpl : NSObject {
    gin::FileSystemWatcher* owner;
    NSString* folder;
}

- (instancetype)initWithOwner:(gin::FileSystemWatcher*)o folder:(NSString*)f;
- (void)setupDispatchSource;
- (void)stopWatching;
@end

@implementation FileSystemWatcherImpl

- (instancetype)initWithOwner:(gin::FileSystemWatcher*)o folder:(NSString*)f
{
    self = [super init];

    return self;
}

- (void)setupDispatchSource
{
    // Implement the code to set up file system monitoring here
}

- (void)stopWatching
{
    // Implement the code to stop file system monitoring here
}

@end

//--------------------------------------------------------------------------

namespace gin
{

    class FileSystemWatcher::Impl
    {
    public:
        FileSystemWatcherImpl* impl;

        Impl (FileSystemWatcher* o, const juce::File& f) : folder (f)
        {
            auto filePath = juceStringToNS (f.getFullPathName()); // Corrected conversion
            impl = [[FileSystemWatcherImpl alloc] initWithOwner:o folder:filePath];
            [impl setupDispatchSource];
        }

        ~Impl()
        {
            [impl stopWatching];
            [impl release];
        }

        juce::File folder;
    };

    //---------------------------------------------------------------------------

    FileSystemWatcher::FileSystemWatcher()
    {
    }

    FileSystemWatcher::~FileSystemWatcher()
    {
    }

    void FileSystemWatcher::addFolder (const juce::File& folder)
    {
        // You can only listen to folders that exist
        jassert (folder.isDirectory());

        if (!getWatchedFolders().contains (folder))
            watched.add (new Impl (this, folder));
    }

    void FileSystemWatcher::removeFolder (const juce::File& folder)
    {
        for (int i = watched.size(); --i >= 0;)
        {
            if (juce::File (watched[i]->folder) == folder)
            {
                watched.remove (i);
                break;
            }
        }
    }

    void FileSystemWatcher::removeAllFolders()
    {
        watched.clear();
    }

    void FileSystemWatcher::addListener (Listener* newListener)
    {
        listeners.add (newListener);
    }

    void FileSystemWatcher::removeListener (Listener* listener)
    {
        listeners.remove (listener);
    }

    void FileSystemWatcher::folderChanged (const juce::File& folder)
    {
        listeners.call (&FileSystemWatcher::Listener::folderChanged, folder);
    }

    void FileSystemWatcher::fileChanged (const juce::File& file, FileSystemEvent fsEvent)
    {
        listeners.call (&FileSystemWatcher::Listener::fileChanged, file, fsEvent);
    }

    juce::Array<juce::File> FileSystemWatcher::getWatchedFolders()
    {
        juce::Array<juce::File> res;

        for (auto w : watched)
            res.add (juce::File (w->folder));

        return res;
    }

} //namespace gin;

#endif
