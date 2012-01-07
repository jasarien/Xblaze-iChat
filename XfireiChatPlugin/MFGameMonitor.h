/*******************************************************************
	FILE:		MFGameMonitor.h
	
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

#import <Cocoa/Cocoa.h>

extern NSString *kMFGameDidLaunch;
extern NSString *kMFGameDidExit;

@interface MFGameMonitor : NSObject
{
	NSMutableArray *_runningGames;
}

+ (id)sharedMonitor;

// array of dictionaries as returned by an MFGameRegistry
- (NSArray *)runningGames;

@end
