/*******************************************************************
	FILE:		MFGameMonitor.m
	
	COPYRIGHT:
		Copyright 2007-2008, the MacFire.org team.
		Use of this software is governed by the license terms
		indicated in the License.txt file (a BSD license).
	
	DESCRIPTION:
		Monitors when applications are launched and when they exit
		and searches for known games.  It posts notifications for
		when known games start and quit.
	
	HISTORY:
		2008 04 06  Changed copyright to BSD license.
		2007 12 16  Created.
*******************************************************************/

#import "MFGameMonitor.h"
#import "MFGAmeRegistry.h"

NSString *kMFGameDidLaunch = @"MFGameDidLaunch";
NSString *kMFGameDidExit = @"MFGameDidExit";

static MFGameMonitor *gSharedMonitor = nil;

@interface MFGameMonitor (Private)
- (void)startMonitoring;
- (void)workspaceAppDidLaunch:(NSNotification *)aNote;
- (void)workspaceAppDidExit:(NSNotification *)aNote;
@end


@implementation MFGameMonitor

+ (id)sharedMonitor
{
	if( gSharedMonitor == nil )
	{
		gSharedMonitor = [[MFGameMonitor alloc] init];
	}
	
	return gSharedMonitor;
}

- (id)init
{
	self = [super init];
	if( self )
	{
		_runningGames = [[NSMutableArray alloc] init];
		
		[self startMonitoring];
	}
	return self;
}

- (void)dealloc
{
	[[[NSWorkspace sharedWorkspace] notificationCenter] removeObserver:self];
	[_runningGames release];
	_runningGames = nil;
	[super dealloc];
}

- (NSArray *)runningGames
{
	return [NSArray arrayWithArray:_runningGames];
}

// check all currently running apps to make sure we catch everything that's running
- (void)startMonitoring
{
	NSWorkspace *workspace = [NSWorkspace sharedWorkspace];
	NSArray *runningApps = [workspace launchedApplications];
	NSEnumerator *enumer = [runningApps objectEnumerator];
	NSDictionary *theApp;
	NSDictionary *gameInfo;
	
	while( (theApp = [enumer nextObject]) != nil )
	{
		gameInfo = [MFGameRegistry infoForMacApplication:theApp];
		if( gameInfo )
		{
			[_runningGames addObject:gameInfo];
			[[NSNotificationCenter defaultCenter] postNotificationName:kMFGameDidLaunch object:self userInfo:gameInfo];
		}
	}
	
	[[workspace notificationCenter] addObserver:self
		selector:@selector(workspaceAppDidLaunch:)
		name:NSWorkspaceDidLaunchApplicationNotification
		object:nil];
	[[workspace notificationCenter] addObserver:self
		selector:@selector(workspaceAppDidExit:)
		name:NSWorkspaceDidTerminateApplicationNotification
		object:nil];
}

// Intercept any app launch
// determine if it's a game we recognize
// then route kMFGameDidLaunch
- (void)workspaceAppDidLaunch:(NSNotification *)aNote
{
	NSDictionary *gameInfo = [MFGameRegistry infoForMacApplication:[aNote userInfo]];
	if( gameInfo )
	{
		// it's a valid game, add it to our list
		[_runningGames addObject:gameInfo];
		[[NSNotificationCenter defaultCenter] postNotificationName:kMFGameDidLaunch object:self userInfo:gameInfo];
		
		// TODO: add ability to monitor specific apps closely
		// need custom classes to do that.
		//NSString *monitorClassName = [gameInfo objectForKey:kMFGameRegistryMonitorClassKey];
		//if( monitorClassName )
		//{
		//	Class c = NSClassFromString(monitorClassName);
		//	if( c )
		//	{
		//		id mon = [[c alloc] init];
		//		[_runningMonitors addObject:mon];
		//	}
		//}
	}
}

// Intercept any app exit
// determine if it's a game we recognize
// then route kMFGameDidExit
- (void)workspaceAppDidExit:(NSNotification *)aNote
{
	NSDictionary *gameInfo = [MFGameRegistry infoForMacApplication:[aNote userInfo]];
	
	// it's a valid game, remove it from our list
	if( gameInfo && [_runningGames containsObject:gameInfo] )
	{
		[_runningGames removeObject:gameInfo];
		[[NSNotificationCenter defaultCenter] postNotificationName:kMFGameDidExit object:self userInfo:gameInfo];
	}
}

@end
